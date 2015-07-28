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
#include "AllJoynBusObject.h"
#include "AllJoynService.h"
#include "AllJoynInterface.h"
#include "AllJoynHelpers.h"

using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;
using namespace Platform::Details;
using namespace Platform;

namespace DeviceProviders
{

    AllJoynBusObject::AllJoynBusObject(AllJoynService ^ service, const string& path, const char **interfaceNames, size_t interfaceCount)
        : m_service(service)
        , m_proxyBusObject(nullptr)
        , m_path(path)
        , m_introspectedSuccessfully(false)
    {
        DEBUG_LIFETIME_IMPL(AllJoynBusObject);

        for (size_t i = 0; i < interfaceCount; ++i)
        {
            m_interfaces.insert(make_pair<string, WeakReference>(interfaceNames[i], WeakReference(nullptr)));
        }
    }

    AllJoynBusObject::AllJoynBusObject(AllJoynService ^ service, const string& path, alljoyn_proxybusobject proxyBusObject)
        : m_service(service)
        , m_proxyBusObject(proxyBusObject)
        , m_path(path)
        , m_introspectedSuccessfully(false)
    {
        DEBUG_LIFETIME_IMPL(AllJoynBusObject);
    }

    AllJoynBusObject::~AllJoynBusObject()
    {
        {
            AutoLock lock(&m_interfacesLock, true);
            for (auto iter : m_interfaces)
            {
                auto iface = iter.second.Resolve<AllJoynInterface>();
                if (iface != nullptr)
                {
                    delete iface;
                }
            }
            m_interfaces.clear();
        }

        if (nullptr != m_proxyBusObject)
        {
            alljoyn_proxybusobject_destroy(m_proxyBusObject);
        }
    }

    bool AllJoynBusObject::Introspect()
    {
        if (!m_introspectedSuccessfully)
        {
            auto service = this->GetService();
            if (service != nullptr)
            {
                if (m_proxyBusObject == nullptr)
                {
                    m_proxyBusObject = alljoyn_proxybusobject_create(service->GetBusAttachment(),
                        service->GetName().c_str(),
                        m_path.c_str(),
                        service->GetSessionId());
                }

                if (m_proxyBusObject != nullptr)
                {
                    if (ER_OK == alljoyn_proxybusobject_introspectremoteobject(m_proxyBusObject))
                    {
                        m_introspectedSuccessfully = true;
                    }
                }
            }
        }
        return m_introspectedSuccessfully;
    }

    IVector<IInterface ^>^ AllJoynBusObject::Interfaces::get()
    {
        auto interfaces = ref new Vector<IInterface ^>();

        if (!this->Introspect())
        {
            return interfaces;
        }     

        AutoLock lock(&m_interfacesLock, true);

        // First check for interfaces we already know about. Not sure this is necessary. It will only matter if there are interface mention in the
        // About announcement but not returned in alljoyn_proxybusobject_getinterfaces
        for (auto& interfaceNameIterator : m_interfaces)
        {
            auto iface = interfaceNameIterator.second.Resolve<AllJoynInterface>();
            if (iface == nullptr)
            {
                alljoyn_interfacedescription description = alljoyn_proxybusobject_getinterface(m_proxyBusObject, interfaceNameIterator.first.data());
                if (nullptr != description)
                {
                    iface = ref new AllJoynInterface(this, description);                    
                }
            }
            interfaces->Append(iface);
            m_interfaces[interfaceNameIterator.first] = WeakReference(iface);
        }

        size_t interfaceCount = alljoyn_proxybusobject_getinterfaces(m_proxyBusObject, nullptr, 0);

        if (interfaceCount > 0)
        {
            auto interfaceDescriptions = vector<alljoyn_interfacedescription>(interfaceCount);
            alljoyn_proxybusobject_getinterfaces(m_proxyBusObject, interfaceDescriptions.data(), interfaceCount);

            for (auto& description : interfaceDescriptions)
            {
                // if the interface has already been created (and still exists) don't create it again
                string interfaceName = alljoyn_interfacedescription_getname(description);

                auto iter = m_interfaces.find(interfaceName);
                if (iter == m_interfaces.end() || iter->second.Resolve<AllJoynInterface>() == nullptr)
                {
                    auto iface = ref new AllJoynInterface(this, description);
                    interfaces->Append(iface);
                    m_interfaces[interfaceName] = WeakReference(iface);
                }
            }
        }
        return interfaces;
    }

    IVector<IBusObject ^>^ AllJoynBusObject::ChildObjects::get()
    {
        auto childObjects = ref new Vector<IBusObject ^>();

        if (!this->Introspect())
        {
            return childObjects;
        }

        size_t childCount = alljoyn_proxybusobject_getchildren(m_proxyBusObject, nullptr, 0);

        if (childCount > 0)
        {
            auto children = vector<alljoyn_proxybusobject>(childCount);
            alljoyn_proxybusobject_getchildren(m_proxyBusObject, children.data(), childCount);

            for (size_t i = 0; i < childCount; ++i)
            {
                string path = alljoyn_proxybusobject_getpath(children[i]);

                // Check if we have already created this bus object based on the About announcement
                auto busObject = m_service->GetOrCreateBusObject(path);
                childObjects->Append(busObject ? busObject : ref new AllJoynBusObject(m_service, path, children[i]));
            }
        }
        return childObjects;
    }

    String ^ AllJoynBusObject::Path::get()
    {
        return AllJoynHelpers::MultibyteToPlatformString(m_path.c_str());
    }

    IInterface^ AllJoynBusObject::GetInterface(Platform::String^ interfaceName)
    {
        if (!this->Introspect())
        {
            return nullptr;
        }

        string name = AllJoynHelpers::PlatformToMultibyteStandardString(interfaceName);

        AutoLock lock(&m_interfacesLock, true);

        auto iter = m_interfaces.find(name);
        if (iter != m_interfaces.end() && iter->second.Resolve<IInterface>() != nullptr)
        {
            return iter->second.Resolve<IInterface>();
        }

        auto interfaceDescription = alljoyn_proxybusobject_getinterface(m_proxyBusObject, name.data());
        if (interfaceDescription != nullptr)
        {
            IInterface^ iface = ref new AllJoynInterface(this, interfaceDescription);
            m_interfaces[name] = WeakReference(iface);
            return iface;
        }

        return nullptr;
    }
}