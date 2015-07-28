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
#include "IService.h"
#include "IBusObject.h"
#include "IAboutData.h"
#include "AllJoynProvider.h"

namespace DeviceProviders
{
    ref class AllJoynBusObject;
    ref class AllJoynAboutData;

    ref class AllJoynService : public IService
    {
        DEBUG_LIFETIME_DECL(AllJoynService);

    internal:
        AllJoynService(AllJoynProvider ^ provider, std::string serviceName, alljoyn_sessionport port);

        // Note that it is possible to add a BusObject to a service after the initial About broadcast,
        // so this function may be called multiple times with new about info.
        void Initialize(alljoyn_msgarg aboutDataArg, alljoyn_msgarg objectDescriptionArg);

        bool JoinSession();
        void LeaveSession();
        AllJoynBusObject^ GetOrCreateBusObject(const std::string& path);

        inline AllJoynProvider ^ GetProvider() const { return m_provider; }
        inline alljoyn_busattachment GetBusAttachment() const { return m_provider->GetBusAttachment(); }
        inline const std::string& GetName() const { return m_name; }
        inline uint32 GetSessionId() const { return m_sessionId; }

    public:
        virtual ~AllJoynService();

        virtual property Windows::Foundation::Collections::IVector<IBusObject ^>^ Objects
        {
            Windows::Foundation::Collections::IVector<IBusObject ^>^ get();
        }
        virtual property Platform::String ^ Name
        {
            Platform::String ^ get();
        }
        virtual property uint16 SessionPort
        {
            uint16 get();
        }
        virtual property uint32 SessionId
        {
            uint32 get();
        }
        virtual property IAboutData^ AboutData
        {
            IAboutData^ get();
        }
        virtual property IProvider^ Provider
        {
            inline IProvider^ get() { return m_provider; }
        }

        virtual AllJoynStatus^ Ping();
        virtual bool ImplementsInterface(Platform::String^ interfaceName);
        virtual IBusObject^ GetBusObject(Platform::String^ path);
        virtual Windows::Foundation::Collections::IVector<IBusObject^>^ GetBusObjectsWhichImplementInterface(Platform::String^ interfaceName);

    private:
        static void AJ_CALL SessionLost(const void* context, alljoyn_sessionid sessionId, alljoyn_sessionlostreason reason);

        Platform::WeakReference m_weakThis;
        std::map<std::string, Platform::WeakReference> m_objectsMap;
        CSLock m_objectsLock;

        AllJoynAboutData^ m_aboutData;
        CSLock m_aboutDataLock;

        AllJoynProvider ^ m_provider;
        std::string m_name;
        alljoyn_sessionport m_sessionPort;
        alljoyn_sessionid m_sessionId;
        alljoyn_sessionlistener m_sessionListener;
        alljoyn_msgarg m_aboutDataArg;
        alljoyn_msgarg m_objectDescriptionArg;
        alljoyn_aboutobjectdescription m_objectDescription;

        static DWORD s_pingTimeout;
    };
}