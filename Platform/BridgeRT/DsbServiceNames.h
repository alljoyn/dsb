//
// Copyright (c) 2015, Microsoft Corporation
// 
// Permission to use, copy, modify, and/or distribute this software for any 
// purpose with or without fee is hereby granted, provided that the above 
// copyright notice and this permission notice appear in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
// IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

#pragma once

#include <alljoyn_c/Session.h>

static const char* DSB_DEVICENOTIFICATION_SIGNAL = "DeviceNotificationSignal";
static const char* DSB_SENDMSGTODEVICE_METHOD = "SendMessageToDeviceSynchronous";

static const char* DSB_SERVICE_NAME = "com.microsoft.DeviceSystemBridge";
static const alljoyn_sessionport DSB_SERVICE_PORT = 27;

static const std::string DEFAULT_ROOT_SERVICE_NAME = "com.microsoft";
static const std::string BRIDGE_DEVICE_NAME = "device";

// {EF116A26-9888-47C2-AE85-B77142F24EFA}
static const uint8_t DSB_APP_GUID[] = { 0xef, 0x11, 0x6a, 0x26, 0x98, 0x88, 0x47, 0xc2, 0xae, 0x85, 0xb7, 0x71, 0x42, 0xf2, 0x4e, 0xfa };
static const char* DSB_APP_NAME = "DSBService";
