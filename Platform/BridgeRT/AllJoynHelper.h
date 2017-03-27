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

namespace BridgeRT
{
    static const int WAIT_TIMEOUT_FOR_ADAPTER_OPERATION = 20000;  // ms

    class DeviceMain;

    class AllJoynHelper
    {
    public:
        AllJoynHelper();

        static QStatus SetMsgArg(_In_ IAdapterValue ^adapterValue, _Inout_ alljoyn_msgarg msgArg);

        template<typename T>
        static QStatus SetMsgArg(_Inout_ alljoyn_msgarg msgArg, _In_ const std::string& ajSignature, _In_ Platform::Array<T>^ arrayArg);

        static QStatus SetMsgArgFromAdapterObject(_In_ IAdapterValue ^adapterValue, _Inout_ alljoyn_msgarg msgArg, _In_ DeviceMain *deviceMain);

        template<typename T>
        static QStatus GetArrayFromMsgArg(_In_ alljoyn_msgarg msgArg, _In_ const std::string& ajSignature, _Out_ Platform::Array<T>^* arrayArg);

        static QStatus GetAdapterValue(_Inout_ IAdapterValue ^adapterValue, _In_ alljoyn_msgarg msgArg);
        static QStatus GetAdapterObject(_Inout_ IAdapterValue ^adapterValue, _In_ alljoyn_msgarg msgArg, _In_ DeviceMain *deviceMain);
        static QStatus GetSignature(_In_ Windows::Foundation::PropertyType propertyType, _Out_ std::string &signature);

        static void EncodeBusObjectName(_In_ Platform::String ^inString, _Inout_ std::string &builtName);
        static void EncodePropertyOrMethodOrSignalName(_In_ Platform::String ^inString, _Inout_ std::string &builtName);
        static void EncodeStringForInterfaceName(_In_ Platform::String ^inString, _Out_ std::string &encodeString);
        static void EncodeStringForServiceName(_In_ Platform::String ^inString, _Out_ std::string &encodeString);
        static void EncodeStringForRootServiceName(_In_ Platform::String ^inString, _Out_ std::string &encodeString);
        static void EncodeStringForAppName(_In_ Platform::String ^inString, _Out_ std::string &encodeString);
        static std::string TrimChar(const std::string& inString, char ch);
    };
}
