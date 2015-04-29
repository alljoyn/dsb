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

#include <string>
#include <vector>

namespace AdapterLib
{
    ref class ZWaveAdapterValue : BridgeRT::IAdapterValue
    {
    public:
        //
        // Generic for DSB objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return m_name; }
        }
        
        // Data
        virtual property Platform::Object^ Data
        {
            Platform::Object^ get() { return m_data; }
            void set(Platform::Object^ NewData) { m_data = NewData; }
        }

    internal:
        friend ref class ZWaveAdapterProperty;

        ZWaveAdapterValue(Platform::String^ name, Platform::Object^ data);
        ZWaveAdapterValue(Platform::String^ name, const std::wstring& data);
        ZWaveAdapterValue(Platform::String^ name, const std::string& data);
        ZWaveAdapterValue(Platform::String^ name, int32 data);
        ZWaveAdapterValue(Platform::String^ name, bool data);
        ZWaveAdapterValue(Platform::String^ name, const std::vector<std::string>& data);

        //copy constructor
        ZWaveAdapterValue(const ZWaveAdapterValue^ other);

    private:
        // Generic
        Platform::String^ m_name;

        Platform::Object^ m_data;
    };
}