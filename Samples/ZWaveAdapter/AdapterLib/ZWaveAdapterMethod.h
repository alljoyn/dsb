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

#include "AdapterDefinitions.h"

#include "value_classes\ValueID.h"

#include <vector>

namespace AdapterLib
{
    ref class ZWaveAdapterMethod : BridgeRT::IAdapterMethod
    {
    public:
        // Object name
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return m_name; }
        }

        // Method description
        virtual property Platform::String^ Description
        {
            Platform::String^ get() { return m_description; }
        }

        // The input parameters
        virtual property BridgeRT::IAdapterValueVector^ InputParams
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(m_inParams);
            }
            void set(BridgeRT::IAdapterValueVector^ Params) {}
        }

        // The output parameters
        virtual property BridgeRT::IAdapterValueVector^ OutputParams
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(m_outParams);
            }
        }

        // The return value
        virtual property int32 HResult
        {
            int32 get() { return m_result; }
        }

    internal:
        friend ref class ZWaveAdapterDevice;
        friend ref class ZWaveAdapter;

        ZWaveAdapterMethod(const OpenZWave::ValueID & value);

        void Initialize();
        void Execute();

    private:
        Platform::String^ m_name;

        // Method information
        Platform::String^ m_description;

        // Method parameters
        std::vector<BridgeRT::IAdapterValue^> m_inParams;
        std::vector<BridgeRT::IAdapterValue^> m_outParams;
        int32 m_result;

        OpenZWave::ValueID m_valueId;
    };
}