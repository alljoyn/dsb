//
// Device System Bridge MockAdapter
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

#include "pch.h"
#include "MockAdapter.h"
#include "MockAdapterDevice.h"
#include "MockDevices.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;

using namespace BridgeRT;

namespace AdapterLib
{
    //
    // Mock devices information
    //
    MOCK_DEVICE_DESCRIPTOR MockDevices[] =
    {
        // Device #1 - A light bulb
        {
            1,                        // Device ID
            L"Mock BACnet Switch",    // Device name
            L"Microsoft",             // Vendor
            L"2 X Switch",            // Model
            L"001-001-001",           // Serial
            L"1.1.1.1",               // Version
            L"2 Gang Switch Pack",    // Description

            // Properties
            {
                // Property  #1
                {
                    1,                          // Property ID
                    OBJECT_BINARY_OUTPUT,       // Property type
                    L"Switch 1",                // Name
                    AdapterPropertyAccessRW,        // Access
                    UNITS_NO_UNITS,             // Units

                    // Current value
                    {
                        PropertyType::Boolean,  // ValueType
                        L"Room 1 Back Light",   // Name
                        {                       // Initial value
                            {0},    // SimpleType
                            {},     // Array of bytes
                            {L""}   // String
                        }
                    }
                },

                // Property  #2
                {
                    2,                          // Property ID
                    OBJECT_BINARY_OUTPUT,       // Property type
                    L"Switch 2",                // Name
                    AdapterPropertyAccessRW,        // Access
                    UNITS_NO_UNITS,             // Units

                    // Current value
                    {
                        PropertyType::Boolean,  // ValueType
                        L"Room 1 Main Light",   // Name
                        {                       // Initial value
                            {0},    // SimpleType
                            {},     // Array of bytes
                            {L""}   // String
                        },
                    }
                },

                { LAST_DESCRIPTOR_ID } // Last property

            } // Properties[]
        },

        // Device #2 - A dimmable light bulb
        {
            2,                                  // Device ID
            L"Mock BACnet Dimmable Switch",     // Device name
            L"Microsoft",                       // Vendor
            L"Dim Control 725",                 // Model
            L"001-002-001",                     // Serial
            L"1.1.2.1",                         // Version
            L"Room2, Dimmable Light Switch",    // Description

            // Properties
            {
                // Property  #1
                {
                    1,                          // Property ID
                    OBJECT_BINARY_OUTPUT,       // Property type
                    L"Switch",                  // Name
                    AdapterPropertyAccessRW,        // Access
                    UNITS_NO_UNITS,             // Units

                    // Current value
                    {
                        PropertyType::Boolean,  // ValueType
                        L"Living Room Light",   // Name
                        {                       // Initial value
                            {0},    // SimpleType
                            {},     // Array of bytes
                            {L""}   // String
                        },
                    }
                },

                // Property  #2
                {
                    2,                          // Property ID
                    OBJECT_ANALOG_OUTPUT,       // Property type
                    L"Dim Control",             // Name
                    AdapterPropertyAccessRW,        // Access
                    UNITS_PERCENT,              // Units

                    // Current Value
                    {
                        PropertyType::UInt32,   // ValueType
                        L"Living Room Dimmer",  // Name
                        {                       // Initial value
                            {50},   // SimpleType
                            {},     // Array of bytes
                            {L""}   // WCHAR string
                        },
                    }
                },

                { LAST_DESCRIPTOR_ID } // Last property

            } // Properties[]
        },

        // Device #3 - A temperature sensor
        {
            3,                                  // Device ID
            L"Mock BACnet Temperature Sensor",  // Device name
            L"Microsoft",                       // Vendor
            L"Temperature Sensor 155",          // Model
            L"001-003-001",                     // Serial
            L"1.1.3.1",                         // Version
            L"Temperature Sensor",              // Description

            // Properties
            {
                // Property  #1
                {
                    1,                          // Property ID
                    OBJECT_ANALOG_INPUT,        // Property type
                    L"Temperature",             // Name
                    AdapterPropertyAccessRO,        // Access
                    UNITS_DEGREES_FAHRENHEIT,   // Units

                    // Current Value
                    {
                        PropertyType::Double,       // ValueType
                        L"Living Room Temperature", // Name
                        {                           // Initial value
                            {62.5}, // SimpleType
                            {},     // Array of bytes
                            {L""}   // WCHAR string
                        },
                    }
                },

                { LAST_DESCRIPTOR_ID } // Last endpoint
            }
        },

    };
    ULONG MockDevicesCount = ARRAYSIZE(MockDevices);


    String^ ToString(ADAPTER_MOCK_UNITS PropertyUnits)
    {
        String^ unitsStr = ref new String(L"Unsupported");

        switch (PropertyUnits)
        {
        case UNITS_NO_UNITS:
            unitsStr = L"UNITS_NO_UNITS";
            break;
        case UNITS_PERCENT:
            unitsStr = L"UNITS_PERCENT";
            break;
        case UNITS_DEGREES_FAHRENHEIT:
            unitsStr = L"UNITS_DEGREES_FAHRENHEIT";
            break;
        }

        return unitsStr;
    }

    
    String^ ToString(ADAPTER_MOCK_PROPERTY_ACCESS PropertyAccess)
    {
        String^ accessStr = ref new String(L"Unsupported");

        switch (PropertyAccess)
        {
        case AdapterPropertyAccessInvalid:
            accessStr = L"Invalid Access Code";
            break;
        case AdapterPropertyAccessRO:
            accessStr = L"RO";
            break;
        case AdapterPropertyAccessRW:
            accessStr = L"RW";
            break;
        }

        return accessStr;
    }


    String^ ToString(ADAPTER_MOCK_PROPERTY_TYPE PropertyType)
    {
        String^ typeStr = ref new String(L"Unsupported");

        switch (PropertyType)
        {
        case OBJECT_ANALOG_INPUT:
            typeStr = L"OBJECT_ANALOG_INPUT";
            break;
        case OBJECT_ANALOG_OUTPUT:
            typeStr = L"OBJECT_ANALOG_OUTPUT";
            break;
        case OBJECT_ANALOG_VALUE:
            typeStr = L"OBJECT_ANALOG_VALUE";
            break;
        case OBJECT_BINARY_INPUT:
            typeStr = L"OBJECT_BINARY_INPUT";
            break;
        case OBJECT_BINARY_OUTPUT:
            typeStr = L"OBJECT_BINARY_OUTPUT";
            break;
        case OBJECT_BINARY_VALUE:
            typeStr = L"OBJECT_BINARY_VALUE";
            break;
        }

        return typeStr;
    }


    String^ ToString(ADAPTER_MOCK_PROPERTY_ATTRIBUTE_ID PropertyAttributeId)
    {
        String^ attrIdStr = ref new String(L"Unsupported");

        switch (PropertyAttributeId)
        {
        case PROP_NAME:
            attrIdStr = L"Object_Name";
            break;
        case PROP_TYPE:
            attrIdStr = L"Object_Type";
            break;
        case PROP_ID:
            attrIdStr = L"Object_Identifier";
            break;
        case PROP_ACCESS:
            attrIdStr = L"Access";
            break;
        case PROP_UNITS:
            attrIdStr = L"Units";
            break;
        case PROP_PRESENT_VALUE:
            attrIdStr = L"Present_Value";
            break;
        }

        return attrIdStr;
    }


} // AdapterLib
