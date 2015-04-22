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

#include <collection.h>
#include <exception>
#include <string>
#include <ppltasks.h>

#define QCC_OS_GROUP_WINDOWS

#include <alljoyn_c/Init.h>
#include <alljoyn_c/dbusstddefines.h>
#include <alljoyn_c/BusAttachment.h>
#include <alljoyn_c/Session.h>
#include <alljoyn_c/BusListener.h>
#include <alljoyn_c/SessionPortListener.h>
#include <alljoyn_c/InterfaceDescription.h>

#include "Misc.h"
#include "IAdapter.h"

#include <windows.h>
#include <tchar.h>
