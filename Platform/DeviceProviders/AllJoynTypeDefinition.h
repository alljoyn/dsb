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

#include "pch.h"
#include "ITypeDefinition.h"
#include "collection.h"

namespace DeviceProviders
{
    public ref class AllJoynTypeDefinition sealed : public ITypeDefinition
    {
        DEBUG_LIFETIME_DECL(AllJoynTypeDefinition);

    public:
        virtual property TypeId Type
        {
            inline TypeId get() { return m_type; }
        }

        virtual property Windows::Foundation::Collections::IVectorView<ITypeDefinition ^> ^ Fields
        {
            inline Windows::Foundation::Collections::IVectorView<ITypeDefinition ^> ^ get()
            {
                if (m_fields)
                {
                    return m_fields->GetView();
                }

                return nullptr;
            }
        }

        virtual property ITypeDefinition ^ KeyType
        {
            inline ITypeDefinition ^ get() { return m_keyType; }
        }

        virtual  property ITypeDefinition ^ ValueType
        {
            inline ITypeDefinition ^ get() { return m_valueType; }
        }

        static Windows::Foundation::Collections::IVector<ITypeDefinition ^>^ CreateTypeDefintions(Platform::String^ signature);

    internal:
        static Platform::Collections::Vector<ITypeDefinition ^>^ CreateTypeDefintions(const std::string& signature);
        static Platform::Collections::Vector<ParameterInfo ^>^ CreateParameterInfo(const std::string& signature, const std::vector<std::string>& argNames);
        static std::string GetAllJoynSignatureString(ITypeDefinition^ typeDefinition);

    private:
        AllJoynTypeDefinition(const std::string& signature);
        void PopulateFields(const std::string& structSignature);

        TypeId m_type;
        ITypeDefinition ^ m_keyType;
        ITypeDefinition ^ m_valueType;
        Platform::Collections::Vector<ITypeDefinition ^> ^ m_fields;
        std::string m_allJoynSignature;
    };
}