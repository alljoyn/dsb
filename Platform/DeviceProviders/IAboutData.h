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

#include "IAboutIcon.h"
#include "AllJoynMessageArgVariant.h"

namespace DeviceProviders
{
    public interface class IAboutData
    {
        typedef Windows::Foundation::Collections::IKeyValuePair<Platform::String^, AllJoynMessageArgVariant ^> IStringVariantPair;

        property Platform::String ^ CurrentLanguage
        {
            Platform::String ^ get();
            void set(Platform::String ^ value);
        }

        property Platform::String ^ AppId
        {
            Platform::String ^ get();
        }
        property Platform::String ^ DefaultLanguage
        {
            Platform::String ^ get();
        }
        property Platform::String ^ DeviceName
        {
            Platform::String ^ get();
        }
        property Platform::String ^ DeviceId
        {
            Platform::String ^ get();
        }
        property Platform::String ^ AppName
        {
            Platform::String ^ get();
        }
        property Platform::String ^ Manufacturer
        {
            Platform::String ^ get();
        }
        property Platform::String ^ ModelNumber
        {
            Platform::String ^ get();
        }
        property Platform::String ^ Description
        {
            Platform::String ^ get();
        }
        property Platform::String ^ DateOfManufacture
        {
            Platform::String ^ get();
        }
        property Platform::String ^ SoftwareVersion
        {
            Platform::String ^ get();
        }
        property Platform::String ^ AllJoynSoftwareVersion
        {
            Platform::String ^ get();
        }
        property Platform::String ^ HardwareVersion
        {
            Platform::String ^ get();
        }
        property Platform::String ^ SupportUrl
        {
            Platform::String ^ get();
        }

        property Windows::Foundation::Collections::IVectorView<Platform::String^> ^ SupportedLanguages
        {
            Windows::Foundation::Collections::IVectorView<Platform::String^> ^ get();
        }

        property Windows::Foundation::Collections::IVectorView<IStringVariantPair ^> ^ AnnouncedFields
        {
            Windows::Foundation::Collections::IVectorView<IStringVariantPair ^> ^ get();
        }

        Windows::Foundation::Collections::IVectorView<IStringVariantPair ^> ^ GetAllFields();

        Windows::Foundation::IAsyncOperation<IAboutIcon^>^ GetIconAsync();
    };
}