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

namespace DeviceProviders
{
    ref class AllJoynService;
    ref class AllJoynSessionImplementation;
    interface class IBusObject;

    public ref class AllJoynSession sealed
    {
    internal:
        AllJoynSession(AllJoynSessionImplementation^ sessionImpl);
        inline AllJoynSessionImplementation^ GetImplementation() { return m_sessionImpl; }
    public:
        virtual ~AllJoynSession();

    public:
        property alljoyn_sessionid SessionId { alljoyn_sessionid get(); }
        property int32 UsageCount { int32 get(); }
        property Windows::Foundation::Collections::IVector<IBusObject^>^ Objects
        {
            Windows::Foundation::Collections::IVector<IBusObject^>^ get();
        }
        IBusObject ^ GetBusObject(Platform::String ^ path);
        Windows::Foundation::Collections::IVector<IBusObject^>^ GetBusObjectsWhichImplementInterface(Platform::String ^ interfaceName);

    private:
        CSLock m_lock;
        AllJoynSessionImplementation^ m_sessionImpl;
    };
}