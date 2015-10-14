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
#include "AllJoynService.h"
#include "AllJoynBusObject.h"
#include "AllJoynProvider.h"
#include "AllJoynHelpers.h"
#include "AllJoynAboutData.h"
#include "AllJoynStatus.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;
using namespace Platform;
using namespace std;

namespace DeviceProviders
{
    DWORD AllJoynService::s_pingTimeout = 1000;

    AllJoynService::AllJoynService(AllJoynProvider ^ provider, std::string serviceName, alljoyn_sessionport port)
        : m_provider(provider)
        , m_name(serviceName)
        , m_sessionPort(port)
        , m_sessionId(0)
        , m_sessionListener(nullptr)
        , m_sessionUserCount(0)
        , m_aboutDataArg(nullptr)
        , m_objectDescriptionArg(nullptr)
        , m_objectDescription(nullptr)
        , m_weakThis(this)
        , m_active(false)
    {
        DEBUG_LIFETIME_IMPL(AllJoynService);
    }

    AllJoynService::~AllJoynService()
    {
        if (m_active)
        {
            Shutdown();
        }
    }

    void AllJoynService::Initialize(alljoyn_msgarg aboutDataArg, alljoyn_msgarg objectDescriptionArg)
    {
        AutoLock lock(&m_objectsLock, true);

        if (m_aboutDataArg != nullptr)
        {
            alljoyn_msgarg_destroy(m_aboutDataArg);
        }
        if (m_objectDescriptionArg != nullptr)
        {
            alljoyn_msgarg_destroy(m_objectDescriptionArg);
        }
        if (m_objectDescription != nullptr)
        {
            alljoyn_aboutobjectdescription_destroy(m_objectDescription);
        }

        m_aboutDataArg = aboutDataArg;
        m_objectDescriptionArg = objectDescriptionArg;
        m_objectDescription = alljoyn_aboutobjectdescription_create_full(m_objectDescriptionArg);

        size_t pathCount = alljoyn_aboutobjectdescription_getpaths(m_objectDescription, nullptr, 0);
        if (0 != pathCount)
        {
            auto pathArray = vector<const char*>(pathCount);
            alljoyn_aboutobjectdescription_getpaths(m_objectDescription, pathArray.data(), pathCount);

            auto pathSet = set<string>(pathArray.begin(), pathArray.end());
            for (auto& path : pathSet)
            {
                m_objectsMap.insert(make_pair(path, WeakReference(nullptr)));
            }
            for (auto& kvp : m_objectsMap)
            {
                if (pathSet.find(kvp.first) == pathSet.end())
                {
                    m_objectsMap.erase(kvp.first);
                }
            }
        }

        m_active = true;
    }

    void AllJoynService::Shutdown()
    {
        AutoLock lock(&m_objectsLock, true);
        m_active = false;

        for (auto iter : m_objectsMap)
        {
            auto busObject = iter.second.Resolve<AllJoynBusObject>();
            if (busObject != nullptr)
            {
                busObject->Shutdown();
            }
        }
        m_objectsMap.clear();


        if (m_aboutDataArg != nullptr)
        {
            alljoyn_msgarg_destroy(m_aboutDataArg);
            m_aboutDataArg = nullptr;
        }

        if (m_objectDescriptionArg != nullptr)
        {
            alljoyn_msgarg_destroy(m_objectDescriptionArg);
            m_objectDescriptionArg = nullptr;
        }

        if (m_objectDescription != nullptr)
        {
            alljoyn_aboutobjectdescription_destroy(m_objectDescription);
            m_objectDescription = nullptr;
        }

        while (m_sessionUserCount > 0)
        {
            LeaveSession();
        }
    }

    AllJoynStatus^ AllJoynService::JoinSession()
    {
        return this->JoinSession(m_sessionPort);
    }

    AllJoynStatus^ AllJoynService::JoinSession(uint16 sessionPort)
    {
        AutoLock lock(&m_objectsLock, true);

        QStatus status = ER_OK;

        if (m_sessionId == 0 && m_sessionUserCount == 0)
        {
            alljoyn_sessionlistener_callbacks sessionListenerCallbacks = { SessionLost, nullptr, nullptr };
            m_sessionListener = alljoyn_sessionlistener_create(&sessionListenerCallbacks, &m_weakThis);

            alljoyn_sessionopts sessionOpts = alljoyn_sessionopts_create(ALLJOYN_TRAFFIC_TYPE_MESSAGES, QCC_FALSE, ALLJOYN_PROXIMITY_ANY, ALLJOYN_TRANSPORT_ANY);

            status = alljoyn_busattachment_joinsession(m_provider->GetBusAttachment(),
                m_name.c_str(),
                sessionPort,
                m_sessionListener,
                &m_sessionId,
                sessionOpts);

            if (status == ER_OK)
            {
                m_sessionUserCount = 1;
            }
            else
            {
                lock.Unlock();
                m_provider->RemoveService(this, alljoyn_sessionlostreason::ALLJOYN_SESSIONLOST_REMOTE_END_CLOSED_ABRUPTLY);
            }
        }
        else
        {
            ++m_sessionUserCount;
        }
        return ref new AllJoynStatus(status);
    }

    AllJoynStatus^ AllJoynService::LeaveSession()
    {
        AutoLock lock(&m_objectsLock, true);

        QStatus status = ER_OK;

        if (--m_sessionUserCount == 0)
        {
            if (m_sessionId != 0)
            {
                auto status = alljoyn_busattachment_ping(m_provider->GetBusAttachment(), m_name.c_str(), s_pingTimeout);
                if (status == ER_OK)
                {
                    status = alljoyn_busattachment_leavesession(this->GetProvider()->GetBusAttachment(), m_sessionId);
                    m_sessionId = 0;
                }
                else if (m_active)
                {
                    lock.Unlock();
                    m_provider->RemoveService(this, alljoyn_sessionlostreason::ALLJOYN_SESSIONLOST_REMOTE_END_CLOSED_ABRUPTLY);
                    status = ER_ALLJOYN_LEAVESESSION_REPLY_NO_SESSION;
                }
            }
            if (m_sessionListener != nullptr)
            {
                alljoyn_sessionlistener_destroy(m_sessionListener);
                m_sessionListener = nullptr;
            }
        }
        else if (m_sessionUserCount < 0)
        {
            m_sessionUserCount = 0;
        }

        return ref new AllJoynStatus(status);
    }

    void AJ_CALL AllJoynService::SessionLost(const void* context, alljoyn_sessionid sessionId, alljoyn_sessionlostreason reason)
    {
        auto service = static_cast<const WeakReference *>(context)->Resolve<AllJoynService>();
        if (service && sessionId == service->m_sessionId)
        {
            {
                AutoLock lock(&service->m_objectsLock, true);

                service->m_sessionId = 0;
                service->m_sessionUserCount = 0;
            }
            service->GetProvider()->RemoveService(service, reason);
        }
    };

    AllJoynBusObject^ AllJoynService::GetBusObjectIfCreated(const string& path)
    {
        AllJoynBusObject^ busObject = nullptr;

        if (m_active)
        {
            AutoLock lock(&m_objectsLock, true);

            auto iter = m_objectsMap.find(path);
            if (iter != m_objectsMap.end())
            {
                busObject = iter->second.Resolve<AllJoynBusObject>();
            }
        }
        return busObject;
    }

    AllJoynBusObject^ AllJoynService::GetOrCreateBusObject(const string& path)
    {
        AllJoynBusObject^ busObject = nullptr;

        if (m_active)
        {
            AutoLock lock(&m_objectsLock, true);

            busObject = GetBusObjectIfCreated(path);

            if (busObject == nullptr && m_objectsMap.find(path) != m_objectsMap.end())
            {
                if (m_sessionId != 0 || JoinSession()->IsSuccess)
                {
                    size_t interfaceCount = 0;
                    interfaceCount = alljoyn_aboutobjectdescription_getinterfaces(m_objectDescription, path.data(), nullptr, 0);
                    vector<const char*> interfacesArray;

                    if (0 != interfaceCount)
                    {
                        interfacesArray = vector<const char*>(interfaceCount);
                        alljoyn_aboutobjectdescription_getinterfaces(m_objectDescription, path.data(), interfacesArray.data(), interfaceCount);
                    }

                    busObject = ref new AllJoynBusObject(this, path, interfacesArray.data(), interfaceCount);
                    m_objectsMap[path] = WeakReference(busObject);
                }
            }
        }
        return busObject;
    }

    IVector<IBusObject^> ^ AllJoynService::Objects::get()
    {
        if (!m_active)
        {
            return nullptr;
        }

        auto busObjects = ref new Vector<IBusObject^>();
        AutoLock lock(&m_objectsLock, true);

        for (auto& kvp : m_objectsMap)
        {
            auto busObject = GetOrCreateBusObject(kvp.first);
            if (busObject)
            {
                busObjects->Append(busObject);
            }
            else
            {
                break;
            }
        }

        return busObjects;
    }

    String ^ AllJoynService::Name::get()
    {
        return AllJoynHelpers::MultibyteToPlatformString(m_name.c_str());
    }

    IAboutData ^ AllJoynService::AboutData::get()
    {
        AutoLock lock(&m_objectsLock, true);

        if (!m_aboutData && m_aboutDataArg)
        {
            m_aboutData = ref new AllJoynAboutData(this, m_aboutDataArg);
        }

        return m_aboutData;
    }

    AllJoynStatus^ AllJoynService::Ping()
    {
        auto status = alljoyn_busattachment_ping(m_provider->GetBusAttachment(), m_name.c_str(), s_pingTimeout);
        if (status != ER_OK)
        {
            {
                AutoLock lock(&m_objectsLock, true);
                m_sessionUserCount = 0;
            }

            m_provider->RemoveService(this, alljoyn_sessionlostreason::ALLJOYN_SESSIONLOST_REMOTE_END_CLOSED_ABRUPTLY);
        }
        return ref new AllJoynStatus(status);
    }

    bool AllJoynService::ImplementsInterface(Platform::String^ interfaceName)
    {
        bool implementsInterface = false;
        if (m_active)
        {
            implementsInterface = alljoyn_aboutobjectdescription_hasinterface(
                m_objectDescription,
                AllJoynHelpers::PlatformToMultibyteStandardString(interfaceName).data());
        }
        return implementsInterface;
    }

    IBusObject^ AllJoynService::GetBusObject(String^ fullpath)
    {
        auto path = AllJoynHelpers::PlatformToMultibyteStandardString(fullpath);
        IBusObject^ busObject = GetOrCreateBusObject(path);

        if (busObject == nullptr)
        {
            // find an ancestor of the provided path if we know of one
            auto iter = std::find_if(m_objectsMap.begin(), m_objectsMap.end(), [&path](pair<string, WeakReference> kvp) -> bool
            {
                return path.substr(0, kvp.first.length()) == kvp.first;
            });

            if (iter != m_objectsMap.end())
            {
                auto ancestor = GetOrCreateBusObject(iter->first);
                busObject = ancestor->GetChild(fullpath);
            }
        }
        return busObject;
    }

    IVector<IBusObject^>^ AllJoynService::GetBusObjectsWhichImplementInterface(Platform::String^ interfaceName)
    {
        if (!m_active)
        {
            return nullptr;
        }

        auto busObjects = ref new Vector<IBusObject^>();

        string interfaceNameString = AllJoynHelpers::PlatformToMultibyteStandardString(interfaceName);

        AutoLock lock(&m_objectsLock, true);

        // get number of bus object path
        size_t pathCount = alljoyn_aboutobjectdescription_getinterfacepaths(m_objectDescription, interfaceNameString.data(), nullptr, 0);
        if (0 != pathCount)
        {
            auto pathArray = vector<const char*>(pathCount);
            alljoyn_aboutobjectdescription_getinterfacepaths(m_objectDescription, interfaceNameString.data(), pathArray.data(), pathCount);

            for (auto& path : pathArray)
            {
                busObjects->Append(GetOrCreateBusObject(path));
            }
        }
        return busObjects;
    }
}