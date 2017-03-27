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

namespace BackgroundHost { namespace Headed { namespace Views
{
    using namespace ::Platform;
    using namespace ::Windows::UI::Xaml;
    using namespace ::Windows::UI::Xaml::Data;
    using namespace ::Windows::UI::Xaml::Interop;

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BooleanValueConverter : public DependencyObject, public IValueConverter
    {
    public:
        virtual Object^ Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language)
        {
            auto boolValue = dynamic_cast<IBox<bool>^>(value);
            return (boolValue && boolValue->Value) ? this->GetTrueValue() : this->GetFalseValue();
        }

        virtual Object^ ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language)
        {
            throw ref new NotImplementedException();
        }

    protected:
        virtual Object^ GetFalseValue() { return nullptr; }
        virtual Object^ GetTrueValue() { return nullptr; }
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BooleanToVisibilityConverter sealed : public BooleanValueConverter
    {
    public:
        BooleanToVisibilityConverter()
        {
            this->FalseValue = Visibility::Collapsed;
            this->TrueValue = Visibility::Visible;
        }

    public:
        property Visibility FalseValue;
        property Visibility TrueValue;

    protected:
        virtual Object^ GetFalseValue() override { return this->FalseValue; }
        virtual Object^ GetTrueValue() override { return this->TrueValue; }
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BooleanToDoubleConverter sealed : public BooleanValueConverter
    {
    public:
        BooleanToDoubleConverter()
        {
            this->FalseValue = 0.0;
            this->TrueValue = 1.0;
        }

    public:
        property double FalseValue;
        property double TrueValue;

    protected:
        virtual Object^ GetFalseValue() override { return this->FalseValue; }
        virtual Object^ GetTrueValue() override { return this->TrueValue; }
    };
}}}