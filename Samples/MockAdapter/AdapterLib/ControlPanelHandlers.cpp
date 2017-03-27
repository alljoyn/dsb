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

#include "pch.h"
#include "ControlPanelHandlers.h"

using namespace BridgeRT;
using namespace AdapterLib;


//***********************************************************************************************************************************************
//
//  Control Panel Temperature Sensor Constructor.  Finds the present temperature value and exposes to the bridge for use by the
//  ControlPanelSimple class.
//
//***********************************************************************************************************************************************
ControlPanelHandlerTempSensor::ControlPanelHandlerTempSensor(MockAdapterDevice^ myDevice)
    : m_device(myDevice)
    , m_temperatureValue(nullptr)
    , m_temperatureProperty(nullptr)
{
    m_temperatureProperty = dynamic_cast<MockAdapterProperty^>(myDevice->Properties->GetAt(0));
    m_temperatureValue = dynamic_cast<MockAdapterValue^>(m_temperatureProperty->GetAttributeByName(L"Present_Value")->Value);
}

//***********************************************************************************************************************************************
//
//  Destructor
//
//***********************************************************************************************************************************************
ControlPanelHandlerTempSensor::~ControlPanelHandlerTempSensor()
{
}

//***********************************************************************************************************************************************
//
//  Run handler.  Not used by this control panel, but required by IControlPanelSimpleHandler.
//
//***********************************************************************************************************************************************
void ControlPanelHandlerTempSensor::Run(Platform::String^ runArgument)
{
    UNREFERENCED_PARAMETER(runArgument);
}



//***********************************************************************************************************************************************
//
//  Control Panel Dimmer Switch Constructor.  Finds the appropriate values and properties from the device and exposes them to the Bridge
//  for use by the ControlPanelSimple class.
//
//***********************************************************************************************************************************************
ControlPanelHandlerDimmerSwitch::ControlPanelHandlerDimmerSwitch(MockAdapterDevice^ myDevice)
    : m_device(myDevice)
    , m_switchProperty(nullptr)
    , m_dimProperty(nullptr)
{
    m_switchProperty = dynamic_cast<MockAdapterProperty^>(myDevice->Properties->GetAt(0));
    m_switchValue = dynamic_cast<MockAdapterValue^>(m_switchProperty->GetAttributeByName(L"Present_Value")->Value);

    m_dimProperty = dynamic_cast<MockAdapterProperty^>(myDevice->Properties->GetAt(1));
    m_dimValue = dynamic_cast<MockAdapterValue^>(m_dimProperty->GetAttributeByName(L"Present_Value")->Value);

    auto signals = myDevice->Signals;
    m_covSignal = dynamic_cast<MockAdapterSignal^>(signals->GetAt(0));

}

//***********************************************************************************************************************************************
//
//  Destructor
//
//***********************************************************************************************************************************************
ControlPanelHandlerDimmerSwitch::~ControlPanelHandlerDimmerSwitch()
{
}

//***********************************************************************************************************************************************
//
// Handle the Run command from the Control Panel
//
// runCmd       a string-ized integer value representing the % brightness for the dimmable light [0-100]
//
//
//***********************************************************************************************************************************************
void ControlPanelHandlerDimmerSwitch::Run(Platform::String^ runCmd)
{
    // Try to convert string to integer
    int dimlevel = _wtoi(runCmd->Begin());

    // Limit the range
    dimlevel = __min(dimlevel, 100);
    dimlevel = __max(0, dimlevel);

    // Set the value on the device
    MockAdapterValue^ currValue = ref new MockAdapterValue(m_dimValue);
    currValue->Data = dimlevel;
    m_dimValue->Set(currValue);
}
