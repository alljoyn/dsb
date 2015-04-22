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

#include <vector>

namespace BridgeRT
{
    ref class BridgeDevice;
    class AllJoynProperty;

    class PropertyInterface
    {
    public:
        PropertyInterface();
        virtual ~PropertyInterface();

        QStatus Create(_In_ IAdapterProperty ^adapterProperty, _In_ std::string &name, _In_ BridgeDevice ^parent);
        bool InterfaceMatchWithAdapterProperty(_In_ IAdapterProperty ^adapterProperty);

        inline alljoyn_interfacedescription GetInterfaceDescription()
        {
            return m_interfaceDescription;
        }
        inline std::string *GetInterfaceName()
        {
            return &m_interfaceName;
        }

        bool IsAJPropertyNameUnique(_In_ std::string name);
        inline DWORD GetIndexForAJProperty()
        {
            return m_indexForAJProperty++;
        }

        inline std::vector<AllJoynProperty *> &GetAJProperties()
        {
            return m_AJProperties;
        }

    private:
        // parent class
        BridgeDevice ^m_parent;

        // alljoyn related
        alljoyn_interfacedescription m_interfaceDescription;
        std::string m_interfaceName;
        std::vector<AllJoynProperty *> m_AJProperties;

        DWORD m_indexForAJProperty;
    };
}

