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
    class PropertyInterface;

    class AllJoynProperty
    {
    public:
        AllJoynProperty();
        virtual ~AllJoynProperty();

        QStatus Create(_In_ IAdapterValue ^adapterValue, _In_ PropertyInterface *parent);
        bool IsSameType(_In_ IAdapterValue ^adapterValue);

        inline std::string *GetName()
        {
            return &m_exposedName;
        }
        inline std::string *GetSignature()
        {
            return &m_signature;
        }

    private:
        QStatus SetName(_In_ Platform::String^ name);

        PropertyInterface *m_parent;
        Platform::String ^m_originalName;
        Platform::TypeCode m_dsbType;
        Windows::Foundation::PropertyType m_dsbSubType;

        std::string m_signature;
        std::string m_exposedName;
    };
}

