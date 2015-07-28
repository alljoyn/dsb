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

#include "AdapterDefinitions.h"
#include "ZWaveAdapterValue.h"

#include "value_classes\ValueID.h"

#include <map>
#include <string>
#include <functional>

namespace AdapterLib
{
    const std::wstring ValueName = L"value";

    ref class ZWaveAdapterProperty : BridgeRT::IAdapterProperty
    {
    public:
        //
        // Generic for DSB objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return m_name; }
        }
        
        virtual property Platform::String^ InterfaceHint
        {
            Platform::String^ get() { return m_InterfaceHint; }
        }
        
        // Attributes
        virtual property BridgeRT::IAdapterValueVector^ Attributes
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(m_attributes);
            }
        }

    internal:
        friend ref class ZWaveAdapterDevice;
        friend ref class ZWaveAdapter;

        ZWaveAdapterProperty(const OpenZWave::ValueID & value);

        void UpdateValue();
        void Initialize();
        uint32 SetValue(Platform::Object^ data);
        ZWaveAdapterValue^ GetAttributeByName(Platform::String^ name);

    private:
        void GetAttributes();
        std::string EncodePropertyName(const std::string &name);

    private:
        Platform::String^ m_name;
        Platform::String^ m_InterfaceHint;

        std::vector<BridgeRT::IAdapterValue^> m_attributes;

        OpenZWave::ValueID m_valueId;
    };
}