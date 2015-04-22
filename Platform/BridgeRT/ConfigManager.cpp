//
// Device System Bridge BridgeRT
//
// Copyright (c) Microsoft Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "pch.h"

#include "Bridge.h"
#include "DsbServiceNames.h"
#include "CspAdapter.h"
#include "CspBridge.h"
#include "AllJoynAbout.h"
#include "AllJoynFileTransfer.h"
#include "BridgeConfig.h"

#include "ConfigManager.h"

using namespace BridgeRT;
using namespace DsbCommon;

static const uint32_t SESSION_LINK_TIMEOUT = 30;        // seconds
const StringReference BRIDGE_CONFIG_FILE(L"BridgeConfig.xml");

ConfigManager::ConfigManager()
    : m_adapter(nullptr),
    m_AJBusAttachment(NULL),
    m_AJBusListener(NULL),
    m_parent(nullptr)
{
}

ConfigManager::~ConfigManager()
{
    Shutdown();
}

int32 ConfigManager::Initialize(DsbBridge ^bridge)
{
    int32 hr = S_OK;
    AutoLock bridgeLocker(&DsbBridge::SingleInstance()->GetLock(), true);

    if (nullptr == bridge)
    {
        hr = E_INVALIDARG;
        goto Leave;
    }
    m_parent = bridge;

    hr = m_bridgeConfig.Init(BRIDGE_CONFIG_FILE);
    if (FAILED(hr))
    {
        goto Leave;
    }

Leave:
    return hr;
}

int32 ConfigManager::Shutdown()
{
    int32 hr = S_OK;
    AutoLock bridgeLocker(&DsbBridge::SingleInstance()->GetLock(), true);

    ShutdownAllJoyn();
    m_adapter = nullptr;
    m_parent = nullptr;

    return hr;
}

QStatus ConfigManager::ConnectToAllJoyn(_In_ IAdapter^ adapter)
{
    QStatus status = ER_OK;
    alljoyn_buslistener_callbacks busListenerCallbacks = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    alljoyn_sessionportlistener_callbacks sessionPortListenerCallbacks = { ConfigManager::AcceptSessionJoinerCallback, ConfigManager::SessionJoined };
    alljoyn_sessionlistener_callbacks sessionListenerCallbacks = { NULL, NULL, (alljoyn_sessionlistener_sessionmemberremoved_ptr)ConfigManager::MemberRemoved };
    alljoyn_sessionopts opts = NULL;
    alljoyn_sessionport sp = DSB_SERVICE_PORT;

    // sanity check
    if (nullptr == adapter)
    {
        status = ER_BAD_ARG_1;
        goto Leave;
    }

    // verify if already connected
    if (NULL != m_AJBusAttachment)
    {
        goto Leave;
    }

    // save away adapter
    m_adapter = adapter;

    // create the bus attachment
    m_AJBusAttachment = alljoyn_busattachment_create(DSB_APP_NAME, QCC_TRUE);
    if (NULL == m_AJBusAttachment)
    {
        status = ER_OUT_OF_MEMORY;
        goto Leave;
    }

    // create the bus listener
    m_AJBusListener = alljoyn_buslistener_create(&busListenerCallbacks, NULL);
    if (NULL == m_AJBusListener)
    {
        status = ER_OUT_OF_MEMORY;
        goto Leave;
    }

    // introduce the bus attachment and the listener
    alljoyn_busattachment_registerbuslistener(m_AJBusAttachment, m_AJBusListener);

    // start the bus attachment
    status = alljoyn_busattachment_start(m_AJBusAttachment);
    if (ER_OK != status)
    {
        goto Leave;
    }

    // initialize about service
    status = m_about.Initialize(m_AJBusAttachment);
    if (ER_OK != status)
    {
        goto Leave;
    }

    // set adapter info in about
    m_about.SetManufacturer(m_adapter->Vendor->Data());
    m_about.SetDeviceName(m_adapter->AdapterName->Data());
    m_about.SetVersion(m_adapter->Version->Data());

    status = InitializeCSPBusObjects();
    if (ER_OK != status)
    {
        goto Leave;
    }

    // connect the bus attachment
    status = alljoyn_busattachment_connect(m_AJBusAttachment, NULL);
    if (ER_OK != status)
    {
        goto Leave;
    }

    /*
    * Advertise this service on the bus.
    * There are three steps to advertising this service on the bus.
    * 1) Request a well-known name that will be used by the client to discover
    *    this service.
    * 2) Create a session.
    * 3) Advertise the well-known name.
    */
    status = alljoyn_busattachment_requestname(m_AJBusAttachment, DSB_SERVICE_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE);
    if (ER_OK != status)
    {
        goto Leave;
    }

    // callback will get this class as context
    m_sessionListener = alljoyn_sessionlistener_create(&sessionListenerCallbacks, this);
    if (NULL == m_sessionListener)
    {
        status = ER_OUT_OF_MEMORY;
        goto Leave;
    }

    m_sessionPortListener = alljoyn_sessionportlistener_create(&sessionPortListenerCallbacks, this);
    if (NULL == m_sessionPortListener)
    {
        status = ER_OUT_OF_MEMORY;
        goto Leave;
    }

    opts = alljoyn_sessionopts_create(ALLJOYN_TRAFFIC_TYPE_MESSAGES, QCC_TRUE, ALLJOYN_PROXIMITY_ANY, ALLJOYN_TRANSPORT_ANY);
    if (NULL == opts)
    {
        status = ER_OUT_OF_MEMORY;
        goto Leave;
    }

    status = alljoyn_busattachment_bindsessionport(m_AJBusAttachment, &sp, opts, m_sessionPortListener);
    if (ER_OK != status)
    {
        goto Leave;
    }

    status = alljoyn_busattachment_advertisename(m_AJBusAttachment, DSB_SERVICE_NAME, alljoyn_sessionopts_get_transports(opts));
    if (ER_OK != status)
    {
        goto Leave;
    }

    // announce
    m_about.Announce();

Leave:
    if (NULL != opts)
    {
        alljoyn_sessionopts_destroy(opts);
    }

    if (ER_OK != status)
    {
        ShutdownAllJoyn();
    }

    return status;
}

QStatus ConfigManager::ShutdownAllJoyn()
{
    QStatus status = ER_OK;

    // verify if already shutdown
    if (NULL == m_AJBusAttachment)
    {
        goto Leave;
    }

    // note that destruction of all alljoyn bus objects (CSP, about and device related)
    // and interfaces must be performed before bus attachment destruction
    // cancel advertised name and session port binding
    alljoyn_busattachment_canceladvertisename(this->m_AJBusAttachment, DSB_SERVICE_NAME, ALLJOYN_TRANSPORT_ANY);
    alljoyn_busattachment_unbindsessionport(this->m_AJBusAttachment, DSB_SERVICE_PORT);

    if (NULL != this->m_sessionPortListener)
    {
        alljoyn_sessionportlistener_destroy(this->m_sessionPortListener);
        this->m_sessionPortListener = NULL;
    }

    alljoyn_busattachment_releasename(this->m_AJBusAttachment, DSB_SERVICE_NAME);
    alljoyn_busattachment_disconnect(this->m_AJBusAttachment, nullptr);

    // destroy CSP interfaces
    m_adapterCSP.Destroy();
    m_bridgeCSP.Destroy();

    // remove authentication handler
    m_authHandler.ShutDown();

    // shutdown about
    m_about.ShutDown();

    alljoyn_busattachment_stop(this->m_AJBusAttachment);

    // destroy bus listener and session port listener
    if (NULL != this->m_AJBusListener)
    {
        alljoyn_busattachment_unregisterbuslistener(this->m_AJBusAttachment, this->m_AJBusListener);
        alljoyn_buslistener_destroy(this->m_AJBusListener);
        this->m_AJBusListener = NULL;
    }

    alljoyn_busattachment_destroy(this->m_AJBusAttachment);
    this->m_AJBusAttachment = NULL;

Leave:
    return status;
}

QStatus ConfigManager::InitializeCSPBusObjects()
{
    QStatus status = ER_OK;
    AutoLock bridgeLocker(&DsbBridge::SingleInstance()->GetLock(), true);

    // init CSP related bus objects if this isn't an update
    //-------------------------------------------------------
    if (IsConfigurationAccessSecured())
    {
		status = m_authHandler.InitializeWithKeyXAuthentication(m_AJBusAttachment, m_bridgeConfig.BridgeKeyX());
        if (ER_OK != status)
        {
            goto Leave;
        }
    }

    status = m_adapterCSP.Initialize(&m_AJBusAttachment, this);
    if (ER_OK != status)
    {
        goto Leave;
    }

    // add bus object description in about service
    status = m_about.AddObject(m_adapterCSP.GetBusObject(), m_adapterCSP.GetInterface());
    if (ER_OK != status)
    {
        goto Leave;
    }

    status = m_bridgeCSP.Initialize(&m_AJBusAttachment, this, m_bridgeConfig);
    if (ER_OK != status)
    {
        goto Leave;
    }
    // add bus object description in about service
    status = m_about.AddObject(m_bridgeCSP.GetBusObject(), m_bridgeCSP.GetInterface());
    if (ER_OK != status)
    {
        goto Leave;
    }

Leave:
    return status;
}

bool BridgeRT::ConfigManager::IsConfigurationAccessSecured()
{
	if (!m_bridgeConfig.BridgeKeyX()->IsEmpty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BridgeRT::ConfigManager::IsDeviceAccessSecured()
{
	if (!m_bridgeConfig.DeviceKeyX()->IsEmpty() ||
        (!m_bridgeConfig.DeviceUsername()->IsEmpty() &&
		 !m_bridgeConfig.DevicePassword()->IsEmpty()) ||
        (!m_bridgeConfig.DeviceEcdheEcdsaPrivateKey()->IsEmpty() &&
         !m_bridgeConfig.DeviceEcdheEcdsaCertChain()->IsEmpty()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

AllJoynFileTransfer* ConfigManager::GetAllJoynFileTransferInstance(_In_ alljoyn_busobject busObject)
{
    AllJoynFileTransfer *objectInstance = nullptr;

    if (m_bridgeCSP.GetBusObject() == busObject)
    {
        objectInstance = reinterpret_cast<AllJoynFileTransfer *> (&m_bridgeCSP);
    }
    else if (m_adapterCSP.GetBusObject() == busObject)
    {
        objectInstance = reinterpret_cast<AllJoynFileTransfer *> (&m_adapterCSP);
    }

    return objectInstance;
}

HRESULT ConfigManager::SetDeviceConfig(_In_ std::wstring &tempFileName, _Out_ HANDLE *finalEvent)
{
    QStatus updateStatus = ER_OK;
    HRESULT hr = S_OK;
    String^ bridgeConfigFileName = ref new String(tempFileName.c_str());
    BridgeConfig newConfig;

    String^ keyX = m_bridgeConfig.BridgeKeyX();

    String^ deviceKeyX = m_bridgeConfig.DeviceKeyX();
    String^ deviceUserName = m_bridgeConfig.DeviceUsername();
    String^ devicePassword = m_bridgeConfig.DevicePassword();
    String^ deviceEcdheEcdsaPrivateKey = m_bridgeConfig.DeviceEcdheEcdsaPrivateKey();
    String^ deviceEcdheEcdsaCertChain = m_bridgeConfig.DeviceEcdheEcdsaCertChain();

    // default final event to nothing
    if (nullptr != finalEvent)
    {
        *finalEvent = NULL;
    }

    // Try to load the new configuration file
    hr = newConfig.Init(bridgeConfigFileName);
    if (FAILED(hr))
    {
        goto Leave;
    }

    // Try to merge the new configuration file with the original configuration file.
    // On failure, reload the in-memory version from file
    hr = m_bridgeConfig.MergeFrom(newConfig);
    if (FAILED(hr))
    {
        m_bridgeConfig.Init(BRIDGE_CONFIG_FILE);
        goto Leave;
    }

    // Update the bridge configuration file with the new data values
    hr = m_bridgeConfig.ToFile();
    if (FAILED(hr))
    {
        goto Leave;
    }

    // Now update what is exposed on AllJoyn 
    if (nullptr != m_parent)
    {
        AutoLock bridgeLocker(&DsbBridge::SingleInstance()->GetLock(), true);
        updateStatus = m_parent->InitializeDevices(true);
        if (updateStatus != ER_OK)
        {
            hr = HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED);
        }
    }

    // If one of the authentication methods has changed, signal a reset request
    if (keyX != m_bridgeConfig.BridgeKeyX() ||
        deviceKeyX != m_bridgeConfig.DeviceKeyX() ||
        deviceUserName != m_bridgeConfig.DeviceUsername() ||
        devicePassword != m_bridgeConfig.DevicePassword() ||
        deviceEcdheEcdsaPrivateKey != m_bridgeConfig.DeviceEcdheEcdsaPrivateKey() ||
        deviceEcdheEcdsaCertChain != m_bridgeConfig.DeviceEcdheEcdsaCertChain())
    {
        if (nullptr != finalEvent)
        {
            // set post write file event to reset event
            *finalEvent = this->m_parent->m_ctrlEvents.hResetEvt;
        }
    }

Leave:
    return hr;
}


HRESULT ConfigManager::SetAdapterConfig(_In_ Array<BYTE>^ adapterConfig)
{
    m_adapter->SetConfiguration(adapterConfig);
    return S_OK;
}


HRESULT ConfigManager::GetAdapterConfig(_Out_ Array<BYTE>^* pAdapterConfig)
{
    m_adapter->GetConfiguration(pAdapterConfig);
    return S_OK;
}

bool ConfigManager::GetObjectConfigItem(_In_ IAdapterDevice ^device, _Out_ DsbObjectConfig& objConfigItem)
{
    bool bConfigItemAdded = false;
    // Select this object from the object configuration file
    HRESULT hr = m_bridgeConfig.FindObject(device->SerialNumber, objConfigItem);
    if (FAILED(hr))
    {
        // Unexpected error searching for device configuration
        throw ref new Exception(hr);
    }

    // If the object wasn't found, then try to add it to the configuration file, defaulted to false
    // If it can't be added, we will still not expose it
    if (hr == S_FALSE)
    {
        // Try to add the object to the in memory configuration.  (Try is here to handle potential exceptions from wstring)
        try
        {
            objConfigItem.bVisible = m_bridgeConfig.DefaultVisibility();
            objConfigItem.id = device->SerialNumber;
            objConfigItem.description = device->Model;
            hr = m_bridgeConfig.AddObject(objConfigItem);
            bConfigItemAdded = true;
        }

        catch (...)
        {
            throw;
        }
    }

    // Return whether or not this is a new item (indicates that the in-memory XML file needs to be saved back to disk)
    // Save is not automatically done here to allow caller a chance to decide when it is appropriate to perform update)
    return bConfigItemAdded;
}

void ConfigManager::ToFile()
{
    // Try to update configuration file, do not fail on write error
    (void)m_bridgeConfig.ToFile();
}

QCC_BOOL AJ_CALL ConfigManager::AcceptSessionJoinerCallback(const void * context, alljoyn_sessionport sessionPort, const char * joiner, const alljoyn_sessionopts opts)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(joiner);
    UNREFERENCED_PARAMETER(opts);

    if (DSB_SERVICE_PORT != sessionPort)
    {
        return QCC_FALSE;
    }
    else
    {
        return QCC_TRUE;
    }
}

void ConfigManager::SessionJoined(_In_ const void *context, _In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_ const char *joiner)
{
    QStatus status = ER_OK;
    const ConfigManager *configManager = nullptr;
    uint32_t timeOut = SESSION_LINK_TIMEOUT;

    UNREFERENCED_PARAMETER(sessionPort);
    UNREFERENCED_PARAMETER(joiner);

    configManager = reinterpret_cast<const ConfigManager *> (context);
    if (nullptr == configManager)
    {
        goto leave;
    }

    // Enable concurrent callbacks since some of the calls below could block
    alljoyn_busattachment_enableconcurrentcallbacks(configManager->m_AJBusAttachment);

    // set session listener and time-out
    status = alljoyn_busattachment_setsessionlistener(configManager->m_AJBusAttachment, id, configManager->m_sessionListener);
    if (status != ER_OK)
    {
        goto leave;
    }
    status = alljoyn_busattachment_setlinktimeout(configManager->m_AJBusAttachment, id, &timeOut);
    if (status != ER_OK)
    {
        goto leave;
    }

leave:
    return;
}

void ConfigManager::MemberRemoved(_In_  void* context, _In_ alljoyn_sessionid sessionid, _In_ const char* uniqueName)
{
    ConfigManager *configManager = nullptr;

    UNREFERENCED_PARAMETER(sessionid);
    UNREFERENCED_PARAMETER(uniqueName);

    // get config manager instance
    configManager = reinterpret_cast<ConfigManager *> (context);
    if (nullptr == configManager)
    {
        goto leave;
    }

    // reset access
    configManager->m_authHandler.ResetAccess(uniqueName);

    // end CSP file transfer    
    configManager->m_adapterCSP.EndTransfer();
    configManager->m_bridgeCSP.EndTransfer();

leave:
    return;
}
