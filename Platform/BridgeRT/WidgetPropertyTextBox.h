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
#include "WidgetProperty.h"

namespace BridgeRT
{
    ///
    ///  A Widget Text Box.  This widget facilitates the exchange of data from an AllJoyn Control Panel to the Bridge.
    ///  Data for this widget is accumulated, but not passed to a target device immediately, It is expected that a
    ///  Bridge Control Panel will forward this data as part of a seperate button action.  This design minimizes the
    ///  amount of data transferred across the device's bus.  (Otherwise each individual keystroke would get passed)
    ///
    class WidgetPropertyTextBox : public WidgetProperty
    {
    public:
        WidgetPropertyTextBox(_In_ ControlPanel* pControlPanel);
        virtual ~WidgetPropertyTextBox();

        // Public Setter method, used for changing the text box content
        void Set(const char* fixedLabel);

        // Publice Getter method, used for reading the current text box content.
        const char* Get() const { return m_myValue.c_str(); };

    protected:
        // Text Box Value Setter
        virtual QStatus GetValue(_Out_ alljoyn_msgarg val) const;

        // Text Box Value Setter
        virtual QStatus SetValue(_In_ alljoyn_msgarg val);

        // A cached text box value.  This string absorbs all values entered into a text box.
        std::string m_myValue;
    };
}