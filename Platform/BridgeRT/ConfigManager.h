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

#pragma once

#include "CspAdapter.h"
#include "CspBridge.h"
#include "AllJoynAbout.h"
#include "BridgeAuthHandler.h"

class BridgeConfig;

namespace BridgeRT
{
    class AllJoynFileTransfer;
    ref class DsbBridge;

    class ConfigManager
    {
    public:
        ConfigManager();
        virtual ~ConfigManager();

        int32 Initialize(DsbBridge ^bridge);
        QStatus	ConnectToAllJoyn(_In_ IAdapter^ adapter);
        int32 Shutdown();

        AllJoynFileTransfer *GetAllJoynFileTransferInstance(_In_ alljoyn_busobject busObject);
        HRESULT SetAdapterConfig(_In_ Array<BYTE>^ adapterConfig);
        HRESULT GetAdapterConfig(_Out_ Array<BYTE>^* pAdapterConfig);
        HRESULT SetDeviceConfig(_In_ std::wstring &tempFileName, _Out_ HANDLE *finalEvent);
        
		bool IsConfigurationAccessSecured();
		bool IsDeviceAccessSecured();

        bool GetObjectConfigItem(_In_ IAdapterDevice ^device, _Out_ DsbObjectConfig& configItem);
        void ToFile();

        inline BridgeConfig *GetBridgeConfig()
        {
            return &m_bridgeConfig;
        }

    private:
        QStatus	ShutdownAllJoyn();
        QStatus	InitializeCSPBusObjects();

        // callback for session listener and session port listener
        static QCC_BOOL AJ_CALL AcceptSessionJoinerCallback(_In_ const void* context, _In_ alljoyn_sessionport sessionPort, _In_z_ const char* joiner, _In_ const alljoyn_sessionopts opts);
        static void AJ_CALL SessionJoined(_In_ const void *context, _In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_z_ const char *joiner);
        static void AJ_CALL MemberRemoved(_In_ void* context, _In_ alljoyn_sessionid sessionid, _In_z_ const char* uniqueName);

        // parent
        DsbBridge ^m_parent;

        // adapter
        IAdapter^ m_adapter;

        // AllJoyn related data
        alljoyn_busattachment m_AJBusAttachment;
        alljoyn_buslistener	m_AJBusListener;
        alljoyn_sessionlistener m_sessionListener;
        alljoyn_sessionportlistener	m_sessionPortListener;

        // authentication management
        BridgeAuthHandler m_authHandler;
        
        // Configuration Data File
        BridgeConfig m_bridgeConfig;

        // about service
        AllJoynAbout m_about;

        // DSB CSP related
        //---------------------
        CspAdapter m_adapterCSP;
        CspBridge m_bridgeCSP;
    };
}

