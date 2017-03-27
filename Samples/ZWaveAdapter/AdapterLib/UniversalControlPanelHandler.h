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

#include "ZWaveAdapterDevice.h"
#include "ZWaveAdapterProperty.h"
#include "ZWaveAdapterValue.h"
#include "ZWaveADapterSignal.h"

namespace AdapterLib
{
    ref class UniversalControlPanelHandler sealed :
        public BridgeRT::IControlPanelHandlerUniversal
    {
    public:
        UniversalControlPanelHandler(AdapterLib::ZWaveAdapterDevice^ device);

        virtual Platform::String^ GetLabel(BridgeRT::IAdapterProperty^ prop);

        virtual BridgeRT::IAdapterValue^ GetValue(BridgeRT::IAdapterProperty^ prop);

        virtual BridgeRT::ControlType GetType(BridgeRT::IAdapterProperty^ prop);

        virtual property Windows::Foundation::Collections::IVector<BridgeRT::IAdapterProperty^>^ ControlledProperties
        {
            Windows::Foundation::Collections::IVector<BridgeRT::IAdapterProperty^>^ get()
            {
                return m_controlledProperties;
            }
        }

        // Return the Change of Value Signal
        virtual property BridgeRT::IAdapterSignal^ ChangeOfValueSignal
        {
            BridgeRT::IAdapterSignal^ get() { return m_covSignal; };
        }

    private:
        // Device to control with this control panel handler
        AdapterLib::ZWaveAdapterDevice^ m_device;

        // Set of all properties for a device that will be controlled.  The order that these properties are added to the vector
        // is the order that those properties will appear on the control panel controller.
        Windows::Foundation::Collections::IVector<BridgeRT::IAdapterProperty^>^ m_controlledProperties;

        // The Dimmable Light's change of value signal
        AdapterLib::ZWaveAdapterSignal^ m_covSignal;
    };
}