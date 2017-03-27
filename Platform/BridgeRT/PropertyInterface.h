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

        QStatus Create(_In_ IAdapterProperty ^adapterProperty, _In_ std::string &name, _In_ BridgeDevice ^device);
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
        // alljoyn related
        alljoyn_interfacedescription m_interfaceDescription;
        std::string m_interfaceName;
        std::vector<AllJoynProperty *> m_AJProperties;

        DWORD m_indexForAJProperty;
    };
}
