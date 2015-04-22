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
#include "CspBridge.h"
#include "ConfigManager.h"

using namespace BridgeRT;
using namespace DsbCommon;

CspBridge::CspBridge()
    : AllJoynFileTransfer(),
	m_configManager(nullptr)
{
    m_tempConfigFilePath.clear();
}

CspBridge::~CspBridge()
{
}


//************************************************************************************************
//
// Initialize
//
// Creates an AllJoyn Object for this Bridge that handles Bridge Specific Configuration data.
// The bridge implements the Microsoft standard alljoyn management configuration interface.
//
//************************************************************************************************
QStatus CspBridge::Initialize(_In_ alljoyn_busattachment* bus, _In_ ConfigManager *configManager, _In_ BridgeConfig& bridgeConfig)
{
    QStatus status = ER_OK;

    try
    {
        m_busObjectPath = "/";
        m_busObjectPath += DSB_APP_NAME;
        m_busObjectPath += "/BridgeConfig";
    }
    catch (...)
    {
        status = ER_OUT_OF_MEMORY;
        goto leave;
    }

	m_configManager = configManager;

    try
    {
        StorageFolder^ tempFolder = ApplicationData::Current->LocalFolder;
        String^ tempFilePath = tempFolder->Path + L"\\";
        m_srcConfigFilePath = tempFilePath->Data();
        m_srcConfigFilePath += bridgeConfig.FileName()->Data();
    }
    catch (...)
    {
        status = ER_OUT_OF_MEMORY;
        goto leave;
    }
      
    status = AllJoynFileTransfer::Initialize(bus, m_busObjectPath.c_str(), configManager);

leave:
    return status;
}

/************************************************************************************************
*
* Post File Write
*
* We transfer the bridge configuration file directly.  Not temporary file is necessary
*
************************************************************************************************/
HRESULT CspBridge::PostFileWriteAction(_In_ std::wstring &appRelativeFileName, _Out_ HANDLE *finalEvent)
{
	return m_configManager->SetDeviceConfig(appRelativeFileName, finalEvent);
}

//************************************************************************************************
//
// Pre File Read Handler for Bridge
// 
// For the bridge, transfer the bridge configuration file directly. 
// No temporary file is necessary
//
//************************************************************************************************
HRESULT CspBridge::PreFileReadAction(_Out_ std::wstring &appRelativeFileName)
{
    HRESULT hr = S_OK;
    try
    {
        WCHAR tempString[MAX_GUID_STRING_LEN];
        int guidLength = 0;
        GUID guid = { 0 };
        StorageFolder^ tempFolder = ApplicationData::Current->LocalFolder;
        String^ tempFilePath = tempFolder->Path + L"\\";
      
        // create 'new' temp file name
        hr = CoCreateGuid(&guid);
        if (FAILED(hr))
        {
            goto leave;
        }

        //Convert GUID into String
        guidLength = StringFromGUID2(guid, tempString, MAX_GUID_STRING_LEN);
        if (0 == guidLength)
        {
            hr = E_FAIL;
            goto leave;
        }

        m_tempConfigFilePath = tempFilePath->Data();
        m_tempConfigFilePath += tempString;
        appRelativeFileName = tempString;

        AutoLock bridgeLocker(&DsbBridge::SingleInstance()->GetLock(), true);
        hr = CopyFile2(m_srcConfigFilePath.c_str(), m_tempConfigFilePath.c_str(), nullptr);
        if (FAILED(hr))
        {
            DeleteFile(m_tempConfigFilePath.c_str());
        }
    }
    catch (...)
    {
        hr = E_OUTOFMEMORY;
    }

leave:
    
    return hr;
}

//************************************************************************************************
//
// Post File Read Handler.
//
//************************************************************************************************
HRESULT CspBridge::PostFileReadAction(void)
{
    DeleteFile(m_tempConfigFilePath.c_str());
    return S_OK;
}