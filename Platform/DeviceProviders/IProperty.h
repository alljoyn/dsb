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

namespace DeviceProviders
{
    interface class ITypeDefinition;
    interface class IInterface;
    ref class AllJoynStatus;

    public ref struct ReadValueResult sealed
    {
        property AllJoynStatus^ Status;
        property Platform::Object ^ Value;
    };

    public interface class IProperty
    {
        Windows::Foundation::IAsyncOperation<ReadValueResult ^> ^ ReadValueAsync();
        Windows::Foundation::IAsyncOperation<AllJoynStatus^> ^ SetValueAsync(Platform::Object ^ newValue);

        property Platform::String ^ Name
        {
            Platform::String ^ get();
        }

        property ITypeDefinition ^ TypeInfo
        {
            ITypeDefinition ^ get();
        }

        property bool CanRead
        {
            bool get();
        }

        property bool CanWrite
        {
            bool get();
        }

        property IInterface^ Interface
        {
            IInterface^ get();
        }

        property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^> ^ Annotations
        {
            Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^> ^ get();
        }

        event Windows::Foundation::TypedEventHandler<IProperty^, Object^>^ ValueChanged;
    };
}