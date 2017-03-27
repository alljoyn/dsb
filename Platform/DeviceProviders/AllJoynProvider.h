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
#include "IProvider.h"

namespace DeviceProviders
{
    ref class AllJoynService;

    public ref class AllJoynProvider sealed : public IProvider
    {
        DEBUG_LIFETIME_DECL(AllJoynProvider);

    public:
        AllJoynProvider();

        virtual AllJoynStatus^ Start();
        virtual void Shutdown();
        virtual Windows::Foundation::Collections::IVector<IService^>^ GetServicesWhichImplementInterface(Platform::String^ interfaceName);

        virtual property Windows::Foundation::Collections::IVector<IService ^>^ Services
        {
            Windows::Foundation::Collections::IVector<IService ^>^ get();
        }

        virtual event Windows::Foundation::TypedEventHandler<IProvider^, ServiceDroppedEventArgs^>^ ServiceDropped;
        virtual event Windows::Foundation::TypedEventHandler<IProvider^, ServiceJoinedEventArgs^>^ ServiceJoined;

    internal:
        static void AJ_CALL AboutAnnounced(const void* context,
            const char* serviceName,
            uint16_t version,
            alljoyn_sessionport port,
            const alljoyn_msgarg objectDescriptionArg,
            const alljoyn_msgarg aboutDataArg);

        static void AJ_CALL NameOwnerChanged(const void* context, const char* busName, const char* previousOwner, const char* newOwner);

        inline alljoyn_busattachment GetBusAttachment() const { return m_busAttachment; }
        void RemoveService(AllJoynService^ service, alljoyn_sessionlostreason reason);

    private:
        ~AllJoynProvider();

        bool m_allJoynInitialized;
        Platform::WeakReference m_weakThis;

        std::map<std::string, AllJoynService ^> m_servicesMap;
        CSLock m_servicesLock;

        alljoyn_aboutlistener m_aboutListener;
        alljoyn_busattachment m_busAttachment;
        alljoyn_buslistener m_busListener;
    };
}