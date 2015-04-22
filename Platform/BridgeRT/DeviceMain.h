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

#include "BridgeDevice.h"

namespace BridgeRT
{
    class DeviceMethod;
    class DeviceSignal;

    class DeviceMain
    {
    public:
        DeviceMain();
        virtual ~DeviceMain();

        QStatus Initialize(_In_ BridgeDevice ^parent);
        bool IsMethodNameUnique(_In_ std::string name);
        bool IsSignalNameUnique(_In_ std::string name);
        void HandleSignal(IAdapterSignal ^adapterSignal);

        inline alljoyn_busobject GetBusObject()
        {
            return m_AJBusObject;
        }
        inline alljoyn_interfacedescription GetInterfaceDescription()
        {
            return m_interfaceDescription;
        }
        inline DWORD GetIndexForMethod()
        {
            return m_indexForMethod++;
        }
        inline DWORD GetIndexForSignal()
        {
            return m_indexForSignal++;
        }
        inline IAdapterProperty ^GetAdapterProperty(_In_ std::string busObjectPath)
        {
            return m_parent->GetAdapterProperty(busObjectPath);
        }
        inline std::string GetBusObjectPath(_In_ IAdapterProperty ^adapterProperty)
        {
            return m_parent->GetBusObjectPath(adapterProperty);
        }

    private:
        void Shutdown();
        QStatus CreateMethodsAndSignals();

        static DeviceMain *GetInstance(_In_ alljoyn_busobject busObject);
        static void AJ_CALL AJMethod(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message msg);

        // list of device method
        std::map<std::string, DeviceMethod *>	m_deviceMethods;
        DWORD m_indexForMethod;

        // list of Signals
        std::map<int, DeviceSignal *>	m_deviceSignals;
        DWORD m_indexForSignal;

        // alljoyn related
        alljoyn_busobject m_AJBusObject;
        alljoyn_interfacedescription m_interfaceDescription;

        bool m_registeredOnAllJoyn;
        std::string m_busObjectPath;
        std::string m_interfaceName;

        // parent object
        BridgeDevice ^m_parent;
    };
}

