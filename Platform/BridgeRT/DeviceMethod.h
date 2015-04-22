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
    class DeviceMain;

    class DeviceMethod
    {
    public:
        DeviceMethod();
        virtual ~DeviceMethod();

        QStatus Initialize(_In_ DeviceMain *parent, IAdapterMethod ^adapterMethod);
        uint32 InvokeMethod(_In_ alljoyn_message msg, _Out_ alljoyn_msgarg *outArgs, _Out_ size_t *nbOfArgs);
        inline std::string &GetName()
        {
            return m_exposedName;
        }

    private:
        QStatus SetName(_In_ Platform::String ^name);
        QStatus BuildSignature(_In_ IAdapterValueVector ^valueList, _Inout_ std::string &signature, _Inout_ std::string &parameterNames);

        // device method
        IAdapterMethod ^m_adapterMethod;

        // alljoyn related
        std::string m_exposedName;
        std::string m_inSignature;
        std::string m_outSignature;
        std::string m_parameterNames;

        // parent
        DeviceMain *m_parent;
    };
}

