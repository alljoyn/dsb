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

namespace AdapterLib
{
    //
    // Mock adapter methods and parameters names
    //

    // Methods return value name
    static Platform::String^ DSB_METHOD_RETURN_VALUE_NAME = ref new Platform::String(L"Return_Value");

    // Reset method
    static Platform::String^ DEVICE_RESET_METHOD = ref new Platform::String(L"Device_Reset");
        static Platform::String^ DEVICE_RESET__PROPERTY_HANDLE = ref new Platform::String(L"Property_Handle");

    //
    // Mock adapter signals and parameters names
    //

    // Heartbeat signal
    static Platform::String^ HEARTBEAT_SIGNAL = ref new Platform::String(L"HeartbeatSignal");
}