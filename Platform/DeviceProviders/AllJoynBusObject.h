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

#include "pch.h"
#include "IBusObject.h"
#include "AllJoynService.h"
#include "AllJoynSessionImplementation.h"

namespace DeviceProviders
{
    ref class AllJoynBusObject : public IBusObject
    {
        DEBUG_LIFETIME_DECL(AllJoynBusObject);

    internal:
        AllJoynBusObject(AllJoynSessionImplementation^ session, const std::string& path, const std::vector<const char*> interfaceNames);
        AllJoynBusObject(AllJoynSessionImplementation^ session, const std::string& path, alljoyn_proxybusobject proxyBusObject);
        void Shutdown();

        inline const std::string& GetPath() const { return m_path; }
        inline AllJoynService^ GetService() const { return m_session->GetService(); }
        inline AllJoynSessionImplementation^ GetSession() const { return m_session; }
        inline alljoyn_proxybusobject GetProxyBusObject() const { return m_proxyBusObject; }
        inline alljoyn_busattachment GetBusAttachment() const { return GetService()->GetBusAttachment(); }

    public:
        virtual ~AllJoynBusObject();

        virtual property Windows::Foundation::Collections::IVector<IInterface ^>^ Interfaces
        {
            Windows::Foundation::Collections::IVector<IInterface ^>^ get();
        }
        virtual property Windows::Foundation::Collections::IVector<IBusObject ^>^ ChildObjects
        {
            Windows::Foundation::Collections::IVector<IBusObject ^>^ get();
        }
        virtual property Platform::String ^ Path
        {
            Platform::String ^ get();
        }
        virtual property IService^ Service
        {
            inline IService^ get() { return m_session->GetService(); }
        }

        virtual IInterface^ GetInterface(Platform::String^ interfaceName);
        virtual IBusObject^ GetChild(Platform::String^ fullPath);

    private:
        AllJoynBusObject^ GetChildIfCreated(const std::string& fullPath);

        std::atomic<bool> m_active;
        AllJoynSessionImplementation^ m_session;
        alljoyn_proxybusobject m_proxyBusObject;
        std::string m_path;
        bool m_introspectedSuccessfully;

        bool AllJoynBusObject::Introspect();

        std::map<std::string, Platform::WeakReference> m_interfaces;
        std::map<std::string, Platform::WeakReference> m_childObjects;
        CSLock m_lock;
    };
}