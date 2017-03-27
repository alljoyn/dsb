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

#include "AdapterConstants.h"
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
        std::map<std::string, DeviceMethod *> m_deviceMethods;
        DWORD m_indexForMethod;

        // list of Signals
        std::map<int, DeviceSignal *> m_deviceSignals;
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