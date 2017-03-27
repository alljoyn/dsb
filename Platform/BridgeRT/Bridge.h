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

#include "IAdapter.h"
#include "AdapterConstants.h"
#include "ConfigManager.h"
#include "BridgeConfig.h"

namespace BridgeRT
{
    ref class BridgeDevice;

    struct SControlEvents
    {
        HANDLE hShutdownEvt;   // Event=WAIT_OBJECT_0
        HANDLE hResetEvt;      // Event=WAIT_OBJECT_0+1
    };

    public ref class DsbBridge sealed : IAdapterSignalListener
    {
    public:
        static DsbBridge^ SingleInstance();

    public:
        DsbBridge(_In_ IAdapter^ adapter);
        virtual ~DsbBridge();


        int32 Initialize();
        int32 Shutdown();

        // IAdapterSignalListener implementation
        virtual void AdapterSignalHandler(
            _In_ IAdapterSignal^ Signal,
            _In_opt_ Platform::Object^ Context
            );

    internal:
        inline IAdapter^ GetAdapter()
        {
            return m_adapter;
        }
        inline ConfigManager *GetConfigManager()
        {
            return &m_configManager;
        }
        QStatus		InitializeDevices(_In_ bool isUpdate = false);
        inline std::map<int, BridgeDevice ^> &GetDeviceList()
        {
            return m_deviceList;
        }

    private:
        int32		registerAdapterSignalHandlers(bool IsRegister);
        HRESULT     InitializeAdapter();

        QStatus		CreateDevice(_In_ IAdapterDevice ^device);
        QStatus		UpdateDevice(_In_ IAdapterDevice ^device, _In_ bool exposedOnAllJoynBus);

        // indicate if alljoyn has been initialized
        bool m_alljoynInitialized;

        // underlying adapter
        IAdapter^ m_adapter;

        // CSP / configuration
        ConfigManager m_configManager;

        // device representations
        std::map<int, BridgeDevice ^>	m_deviceList;

        // Synchronization object
        std::recursive_mutex m_bridgeLock;

    internal:
        // The following is needed to synchronize access to the configuration file.
        // It is unlikely, but possible that a device is added to the config file
        // at the same time the CSP attempts to read it.
        friend CspBridge;
        std::recursive_mutex& GetLock();

        //called from background MonitorThread(void* pContext);
        int32 Reset();

        int32 InitializeInternal();
        int32 ShutdownInternal();

        SControlEvents m_ctrlEvents;
        HANDLE m_hThread;
        DWORD m_threadId;
        Windows::System::Threading::ThreadPool^ m_threadPool;

    };
}