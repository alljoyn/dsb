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

#pragma once

#include "pch.h"
#include "IBusObject.h"
#include "AllJoynService.h"

namespace DeviceProviders
{
    ref class AllJoynBusObject : public IBusObject
    {
        DEBUG_LIFETIME_DECL(AllJoynBusObject);

    internal:
        AllJoynBusObject(AllJoynService ^ service, const std::string& path, const char **interfaceNames, size_t interfaceCount);
        AllJoynBusObject(AllJoynService ^ service, const std::string& path, alljoyn_proxybusobject proxyBusObject);
        void Shutdown();

        inline const std::string& GetPath() const { return m_path; }
        inline AllJoynService^ GetService() const { return m_service; }
        inline alljoyn_proxybusobject GetProxyBusObject() const { return m_proxyBusObject; }
        inline alljoyn_busattachment GetBusAttachment() const { return m_service->GetBusAttachment(); }

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
            inline IService^ get() { return m_service; }
        }

        virtual IInterface^ GetInterface(Platform::String^ interfaceName);
        virtual IBusObject^ GetChild(Platform::String^ fullPath);

    private:
        AllJoynBusObject^ GetChildIfCreated(const std::string& fullPath);

        std::atomic<bool> m_active;
        AllJoynService ^ m_service;
        alljoyn_proxybusobject m_proxyBusObject;
        std::string m_path;
        bool m_introspectedSuccessfully;

        bool AllJoynBusObject::Introspect();

        std::map<std::string, Platform::WeakReference> m_interfaces;
        std::map<std::string, Platform::WeakReference> m_childObjects;
        CSLock m_lock;
    };
}