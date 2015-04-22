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

#include "IAdapter.h"
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
        DsbCommon::CSLock m_bridgeLock;

    internal:
        // The following is needed to synchronize access to the configuration file.
        // It is unlikely, but possible that a device is added to the config file 
        // at the same time the CSP attempts to read it.
        friend CspBridge;
        DsbCommon::CSLock& GetLock();

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
