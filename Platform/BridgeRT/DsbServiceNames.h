//
// Device System Bridge BridgeRT
//
// Copyright (c) Microsoft Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
