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
#include "IProperty.h"
#include "AllJoynInterface.h"

namespace DeviceProviders
{
    interface class ITypeDefinition;

    ref class AllJoynProperty : public IProperty
    {
        DEBUG_LIFETIME_DECL(AllJoynProperty);

    internal:
        AllJoynProperty(_In_ AllJoynInterface ^ parent, const alljoyn_interfacedescription_property& propertyDescription);
        inline const std::string& GetName() const { return m_name; }
		void Shutdown();

        static void AJ_CALL OnPropertyChanged(_In_ alljoyn_proxybusobject busObject,
            _In_ const char* interfaceName,
            _In_ alljoyn_msgarg changed,
            _In_ alljoyn_msgarg invalidated,
            _In_ void *context);

    public:
        virtual ~AllJoynProperty();

        virtual Windows::Foundation::IAsyncOperation<ReadValueResult ^> ^ ReadValueAsync();
        virtual Windows::Foundation::IAsyncOperation<AllJoynStatus ^> ^ SetValueAsync(Platform::Object ^ newValue);

        virtual property Platform::String ^ Name
        {
            Platform::String ^ get();
        }

        virtual property ITypeDefinition ^ TypeInfo
        {
            inline ITypeDefinition ^ get() { return m_typeInfo; }
        }

        virtual property bool CanRead
        {
            inline bool get() { return m_canRead; }
        }

        virtual property bool CanWrite
        {
            inline bool get() { return m_canWrite; }
        }

        virtual property IInterface^ Interface
        {
            inline IInterface^ get() { return m_interface; }
        }

        virtual property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^> ^ Annotations
        {
            Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^> ^ get() { return m_annotations; }
        }

        virtual event Windows::Foundation::TypedEventHandler<IProperty^, Object^>^ ValueChanged;

    private:
		std::atomic<bool> m_active;
        Platform::WeakReference m_weakThis;
        AllJoynInterface ^ m_interface;
        ITypeDefinition ^ m_typeInfo;
        std::string m_signature;
        std::string m_name;
        bool m_canRead;
        bool m_canWrite;
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^> ^ m_annotations;
    };
}