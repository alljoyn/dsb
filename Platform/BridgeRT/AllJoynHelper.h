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

namespace BridgeRT
{
    static const int WAIT_TIMEOUT_FOR_ADAPTER_OPERATION = 20000;  // ms

    class DeviceMain;

    class AllJoynHelper
    {
    public:
        AllJoynHelper();

        static QStatus SetMsgArg(_In_ IAdapterValue ^adapterValue, _Inout_ alljoyn_msgarg msgArg);
        static QStatus SetMsgArgFromAdapterObject(_In_ IAdapterValue ^adapterValue, _Inout_ alljoyn_msgarg msgArg, _In_ DeviceMain *deviceMain);
        static QStatus GetAdapterValue(_Inout_ IAdapterValue ^adapterValue, _In_ alljoyn_msgarg msgArg);
        static QStatus GetAdapterObject(_Inout_ IAdapterValue ^adapterValue, _In_ alljoyn_msgarg msgArg, _In_ DeviceMain *deviceMain);
        static QStatus GetSignature(_In_ Windows::Foundation::PropertyType propertyType, _Out_ std::string &signature);

        static void BuildBusObjectName(_In_ Platform::String ^inString, _Inout_ std::string &builtName);
        static void BuildPropertyOrMethodOrSignalName(_In_ Platform::String ^inString, _Inout_ std::string &builtName);
        static void EncodeStringForServiceName(_In_ Platform::String ^inString, _Out_ std::string &encodeString);
    };
}

