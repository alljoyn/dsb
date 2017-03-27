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
#include "Widget.h"

namespace BridgeRT
{
    class WidgetProperty : public Widget
    {
    public:
        WidgetProperty(_In_ ControlPanel* pControlPanel, _In_ uint16_t alljoynWidgetTypeId, _In_ bool bReadOnly = false);
        virtual ~WidgetProperty();

        // The AllJoyn Property Get Callback
        virtual QStatus Get(_In_z_ const char* interfaceName, _In_z_ const char* propName, _Out_ alljoyn_msgarg val) const;

        // The AllJoyn Propertt Set Callback
        virtual QStatus Set(const char* interfaceName, const char* propName, alljoyn_msgarg val);

    protected:
        virtual const char* GetInterfaceName();

        // Every PropertyWidget has a value.  Derived classes override this method to read the current value of the widget and returns it to the AllJoyn Caller.
        virtual QStatus GetValue(_Out_ alljoyn_msgarg val) const = 0;

        // Every PropertyWidget has a value.  Derivced classes override this method to set the current value.  For read-only widgets, use this default.
        virtual QStatus SetValue(_In_ alljoyn_msgarg val);

        // Helper method that adds custom Widget Properties to this AllJoyn widget
        virtual QStatus AddCustomInterfaces(_In_ alljoyn_interfacedescription busInterface);

        // The AllJoyn Widget Type ID of this widget
        uint16_t m_alljoynWidgetTypeId;
    };
}