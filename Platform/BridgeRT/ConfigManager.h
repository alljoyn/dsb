//
// 
// IN Copyright (c) %s, Microsoft Corporation
// IN 
// IN SPDX-License-Identifier: Apache-2.0 *
// IN 
// IN All rights reserved. This program and the accompanying materials are
// IN made available under the terms of the Apache License, Version 2.0
// IN which accompanies this distribution, and is available at
// IN http://www.apache.org/licenses/LICENSE-2.0
// IN 
// IN Permission to use, copy, modify, and/or distribute this software for
// IN any purpose with or without fee is hereby granted, provided that the
// IN above copyright notice and this permission notice appear in all
// IN copies.
// IN 
// IN THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// IN WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// IN WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// IN AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// IN DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// IN PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// IN TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// IN PERFORMANCE OF THIS SOFTWARE.
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
        QStatus BuildServiceName();

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
        std::string m_serviceName;

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
