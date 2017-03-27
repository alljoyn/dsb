
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
#include "IAdapter.h"


namespace BridgeRT
{
    class ControlPanel;

    // To handle a device's change of value signal, a WinRT class must be registered with the Adapter.  The Widget and Control Panel classes are implemented
    // using C++, this class converts a WinRT callback to a C++ callback.  Or said another way, when a signal changes, this WinRT signal handler is called,
    // which forwards the call to a standard C++ Control Panel.
    ref class WidgetSignalHandler sealed :
        public IAdapterSignalListener
    {
    public:

        // WiNRT Signal Handler Called by Signal Dispatcher.
        virtual void AdapterSignalHandler(
            _In_ IAdapterSignal^ Signal,
            _In_opt_ Platform::Object^ Context);

    internal:
        // Constructor
        WidgetSignalHandler(ControlPanel* pControlPanel);

        // Control Panel to forward Signal Change to
        ControlPanel* m_pControlPanel;
    };
}