//
// Copyright (c) 2015, Microsoft Corporation
// 
// Permission to use, copy, modify, and/or distribute this software for any 
// purpose with or without fee is hereby granted, provided that the above 
// copyright notice and this permission notice appear in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
// IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

#include "pch.h"

#include "IAdapter.h"
#include "DsbServiceNames.h"
#include "Bridge.h"
#include "BridgeConfig.h"
#include "ConfigManager.h"
#include "BridgeDevice.h"
#include "AllJoynHelper.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

using namespace BridgeRT;
using namespace DsbCommon;
using namespace std;

DsbBridge ^g_TheOneOnlyInstance = nullptr;

DsbBridge^ DsbBridge::SingleInstance()
{
    return g_TheOneOnlyInstance;
}

DsbBridge::DsbBridge(IAdapter^ adapter)
    : m_adapter(adapter),
    m_hThread(NULL),
    m_alljoynInitialized(false)
{
    memset(&m_ctrlEvents, 0, sizeof(m_ctrlEvents));
    g_TheOneOnlyInstance = this;
}

DsbBridge::~DsbBridge()
{
    Shutdown();
    m_adapter = nullptr;
}

DWORD WINAPI MonitorThread(LPVOID pContext)
{
    const size_t RESET_EVT_IDX = offsetof(SControlEvents, hResetEvt) / sizeof(HANDLE);
    const size_t SHUTDOWN_EVT_IDX = offsetof(SControlEvents, hShutdownEvt) / sizeof(HANDLE);
    // Treat the Struct of HANDLEs as an Array of Handles
    HANDLE* pEvents = reinterpret_cast<HANDLE*>(pContext);
    HRESULT hr = S_OK;
    DWORD numWaitables = sizeof(SControlEvents) / sizeof(HANDLE);
    for (;;)
    {
        DWORD waitResult = ::WaitForMultipleObjectsEx(
            numWaitables,
            pEvents,
            FALSE,
            INFINITE,
            FALSE);
        switch (waitResult)
        {
        case SHUTDOWN_EVT_IDX:
            goto Leave;

        case RESET_EVT_IDX:
            hr = (HRESULT)g_TheOneOnlyInstance->Reset();
            if (hr != S_OK)
            {
                goto Leave;
            }
            ResetEvent(pEvents[RESET_EVT_IDX]);
            break;

        default:
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Leave;
        }
    }

Leave:
    return hr;
}

int32
DsbBridge::Initialize()
{
    int32 hr = S_OK;
    AutoLock bridgeLocker(&this->m_bridgeLock, true);

    // initialize AllJoyn
    if (!m_alljoynInitialized)
    {
        QStatus status = alljoyn_init();
        if (ER_OK != status)
        {
            hr = ER_FAIL;
            goto Leave;
        }
        m_alljoynInitialized = true;
    }
    
    // If the background thread has been created, then we have already called Initialize.  
    // Caller must call Shutdown first
    if (m_hThread != NULL)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }

    m_ctrlEvents.hShutdownEvt = ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, SYNCHRONIZE | EVENT_MODIFY_STATE);
    if (m_ctrlEvents.hShutdownEvt == nullptr)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Leave;
    }

    m_ctrlEvents.hResetEvt = ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, SYNCHRONIZE | EVENT_MODIFY_STATE);
    if (m_ctrlEvents.hResetEvt == nullptr)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Leave;
    }

    m_hThread = CreateThread(nullptr, 0, ::MonitorThread, &m_ctrlEvents, 0, &m_threadId);
    if (m_hThread == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Leave;
    }

    hr = this->InitializeInternal();
    if (FAILED(hr))
    {
        goto Leave;
    }

Leave:
    if (FAILED(hr))
    {
        this->Shutdown();
    }

    return hr;
}

int32
DsbBridge::InitializeInternal()
{
    int32 hr = S_OK;
    QStatus status = ER_OK;

    hr = this->m_configManager.Initialize(this);
    if (FAILED(hr))
    {
        goto Leave;
    }

    // initialize adapter
    hr = this->InitializeAdapter();
    if (FAILED(hr))
    {
        goto Leave;
    }

    // connect config manager to AllJoyn
    status = this->m_configManager.ConnectToAllJoyn(m_adapter);
    if (ER_OK != status)
    {
        hr = E_FAIL;
        goto Leave;
    }

    // initialize devices
    hr = InitializeDevices();
    if (FAILED(hr))
    {
        goto Leave;
    }

    // register for signals at IAdapter level
    hr = this->registerAdapterSignalHandlers(true);

Leave:
    return hr;
}


int32
DsbBridge::Shutdown()
{
    int32 hr = S_OK;
    AutoLock bridgeLocker(&this->m_bridgeLock, true);

    if (m_hThread != NULL)
    {
        SetEvent(m_ctrlEvents.hShutdownEvt);
        WaitForSingleObjectEx(m_hThread, 1000, FALSE);
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    if (m_ctrlEvents.hShutdownEvt != nullptr)
    {
        CloseHandle(m_ctrlEvents.hShutdownEvt);
        m_ctrlEvents.hShutdownEvt = nullptr;
    }

    if (m_ctrlEvents.hResetEvt != nullptr)
    {
        CloseHandle(m_ctrlEvents.hResetEvt);
        m_ctrlEvents.hResetEvt = nullptr;
    }

    hr = ShutdownInternal();

    if (m_alljoynInitialized)
    {
        alljoyn_shutdown();
        m_alljoynInitialized = false;
    }
    return hr;
}

int32 DsbBridge::ShutdownInternal()
{
    HRESULT hr = S_OK;

    this->m_configManager.Shutdown();
    if (this->m_adapter != nullptr)
    {
        (void)this->registerAdapterSignalHandlers(false);

        hr = this->m_adapter->Shutdown();
    }
    for (auto val : this->m_deviceList)
    {
        val.second->Shutdown();
    }
    this->m_deviceList.clear();

    return hr;
}

QStatus DsbBridge::InitializeDevices(_In_ bool isUpdate)
{
    bool bUpdateXmlConfig = false;
    QStatus status = ER_OK;
    DWORD dwStatus = ERROR_SUCCESS;
    IAdapterDeviceVector ^devicesList;
    IAdapterIoRequest^ request;

    //enumerate all devices
    dwStatus = this->m_adapter->EnumDevices(
        isUpdate ? ENUM_DEVICES_OPTIONS::CACHE_ONLY : ENUM_DEVICES_OPTIONS::FORCE_REFRESH,
        &devicesList,
        &request);

    if (ERROR_IO_PENDING == dwStatus)
    {
        // wait for completion
        dwStatus = request->Wait(WAIT_TIMEOUT_FOR_ADAPTER_OPERATION);
    }
    if (ERROR_SUCCESS != dwStatus)
    {
        status = ER_FAIL;
        goto Leave;
    }

    {
        // loop through the Devices list and create corresponding AllJoyn bus objects
        AutoLock bridgeLocker(&this->m_bridgeLock, true);
        for (auto device : devicesList)
        {
            DsbObjectConfig objConfigItem;
            try
            {
                bUpdateXmlConfig |= this->m_configManager.GetObjectConfigItem(device, objConfigItem);
            }
            catch (...)
            {
                //TBD - after config xml part is done
                continue;
            }

            if (isUpdate)
            {
                status = UpdateDevice(device, objConfigItem.bVisible);
            }
            else if (objConfigItem.bVisible)
            {
                // only create device that are exposed
                // don't leave if a device cannot be created, just create the others
                status = CreateDevice(device);
            }
        }

        // Save the Bridge Configuration to an XML file
        if (bUpdateXmlConfig)
        {
            m_configManager.ToFile();
        }
    }

Leave:
    return status;
}

QStatus DsbBridge::CreateDevice(IAdapterDevice ^device)
{
    QStatus status = ER_OK;
    BridgeDevice ^newDevice = nullptr;
    AutoLock bridgeLocker(&this->m_bridgeLock, true);

    // create and initialize the device
    newDevice = ref new BridgeDevice();
    if (nullptr == newDevice)
    {
        status = ER_OUT_OF_MEMORY;
        goto leave;
    }

    status = newDevice->Initialize(device);
    if (ER_OK != status)
    {
        goto leave;
    }

    // add device in device list
    m_deviceList.insert(std::make_pair(device->GetHashCode(), newDevice));

leave:
    if (ER_OK != status)
    {
        if (nullptr != newDevice)
        {
            newDevice->Shutdown();
            delete newDevice;
        }
    }

    return status;
}

QStatus DsbBridge::UpdateDevice(IAdapterDevice ^ device, bool exposedOnAllJoynBus)
{
    QStatus status = ER_OK;

    // check if device is already in device list (hence exposed on alljoyn)
    auto deviceIterator = m_deviceList.find(device->GetHashCode());
    if (deviceIterator == m_deviceList.end() &&
        exposedOnAllJoynBus)
    {
        // device doesn't exist (hence isn't exposed on AllJoyn) => create it (creation also expose on alljoyn)
        status = CreateDevice(device);
    }
    else if (deviceIterator != m_deviceList.end() &&
        !exposedOnAllJoynBus)
    {

        // the device exist (hence is exposed on allJoyn) 
        // => remove it from list THEN shut it down and delete 
        BridgeDevice ^tempDevice = deviceIterator->second;
        m_deviceList.erase(deviceIterator);
        tempDevice->Shutdown();
        delete tempDevice;
    }

    return status;
}

_Use_decl_annotations_
void
DsbBridge::AdapterSignalHandler(
    IAdapterSignal^ Signal,
    Platform::Object^ Context
)
{
    DsbObjectConfig objConfigItem;
    IAdapterDevice ^adapterDevice = nullptr;

    UNREFERENCED_PARAMETER(Context);

    if (Signal->Name == DEVICE_ARRIVAL_SIGNAL ||
        Signal->Name == DEVICE_REMOVAL_SIGNAL)
    {
        //look for the device handle
        for (auto param : Signal->Params)
        {
            if (param->Name == DEVICE_ARRIVAL__DEVICE_HANDLE)
            {
                adapterDevice = dynamic_cast<IAdapterDevice^>(param->Data);
                break;
            }
        }
        if (adapterDevice == nullptr)
        {
            goto Leave;
        }
    }

    if (Signal->Name == DEVICE_ARRIVAL_SIGNAL)
    {
        // get config for the device
        try
        {
            bool bUpdateXml = m_configManager.GetObjectConfigItem(adapterDevice, objConfigItem);
            if (bUpdateXml)
            {
                AutoLock bridgeLocker(&this->m_bridgeLock, true);
                m_configManager.ToFile();
            }
        }
        catch (...)
        {
            //TBD - after config xml part is done
        }

        // create the new device and expose it on alljoyn (if required)
        if (objConfigItem.bVisible)
        {
            // only create device that are exposed
            //
            // Note: creation failure is handle inside CreateDevice and there is no more 
            // that can be done to propagate error in this routine => don't check status
            CreateDevice(adapterDevice);
        }
    }
    else if (Signal->Name == DEVICE_REMOVAL_SIGNAL)
    {
        // remove device
        // note that config doesn't need to be updated, if the device come again
        // it will be shown again and config clean up is done by CSP)
        AutoLock bridgeLocker(&this->m_bridgeLock, true);
        UpdateDevice(adapterDevice, false);
    }

Leave:
    return;
}

int32
DsbBridge::registerAdapterSignalHandlers(bool IsRegister)
{
    AutoLock sync(&this->m_bridgeLock, true);

    for (auto signal : this->m_adapter->Signals)
    {
        uint32 status;

        if (IsRegister)
        {
            status = this->m_adapter->RegisterSignalListener(signal, this, nullptr);
        }

        else
        {
            status = this->m_adapter->UnregisterSignalListener(signal, this);
        }

        if (status != ERROR_SUCCESS)
        {
            return HRESULT_FROM_WIN32(status);
        }
    }

    return S_OK;
}

HRESULT
DsbBridge::InitializeAdapter()
{
    HRESULT hr = E_FAIL;

    if (nullptr == this->m_adapter)
    {
        hr = E_NOT_VALID_STATE;
        goto Leave;
    }
    hr = HRESULT_FROM_WIN32(this->m_adapter->Initialize());

Leave:
    return hr;
}

CSLock& DsbBridge::GetLock()
{
    return m_bridgeLock;
}

int32 DsbBridge::Reset()
{
    int32 hr = S_OK;
    AutoLock bridgeLocker(&this->m_bridgeLock, true);

    hr = this->ShutdownInternal();
    if (SUCCEEDED(hr))
    {
        hr = this->InitializeInternal();
        if (FAILED(hr))
        {
            this->ShutdownInternal();
        }
    }

    return hr;
}