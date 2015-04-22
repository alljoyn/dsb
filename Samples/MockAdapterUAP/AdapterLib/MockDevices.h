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

#pragma once

namespace AdapterLib
{
    //
    // Mock devices configuration information
    //
    #define MOCK_VALUE_MAX_SIZE         ULONG(128)
    #define MOCK_PROPERTY_MAX_VALUES    ULONG(10)
    #define MOCK_DEVICE_MAX_PROPERTIES  ULONG(10)

    // Last descriptor mark
    #define LAST_DESCRIPTOR_ID  ULONG(-1)

    //
    // ADAPTER_MOCK_UNITS.
    // Description:
    //  The allowed access to an Adapter property
    //
    enum ADAPTER_MOCK_UNITS : uint32
    {
        UNITS_NO_UNITS = 0,
        UNITS_PERCENT,
        UNITS_DEGREES_FAHRENHEIT,
    };
    Platform::String^ ToString(ADAPTER_MOCK_UNITS PropertyUnits);


    //
    // ADAPTER_MOCK_PROPERTY_ACCESS.
    // Description:
    //  The allowed access to an Adapter property
    //
    enum ADAPTER_MOCK_PROPERTY_ACCESS : uint32
    {
        AdapterPropertyAccessInvalid = 0,
        AdapterPropertyAccessRO,
        AdapterPropertyAccessRW
    };
    Platform::String^ ToString(ADAPTER_MOCK_PROPERTY_ACCESS PropertyAccess);


    //
    // ADAPTER_MOCK_PROPERTY_TYPE.
    // Description:
    //  The allowed access to an Adapter property
    //
    enum ADAPTER_MOCK_PROPERTY_TYPE : uint32
    {
        OBJECT_ANALOG_INPUT = 0,
        OBJECT_ANALOG_OUTPUT,
        OBJECT_ANALOG_VALUE,
        OBJECT_BINARY_INPUT,
        OBJECT_BINARY_OUTPUT,
        OBJECT_BINARY_VALUE,
    };
    Platform::String^ ToString(ADAPTER_MOCK_PROPERTY_TYPE PropertyType);


    // Attribute IDs
    enum ADAPTER_MOCK_PROPERTY_ATTRIBUTE_ID : uint32
    {
        PROP_NAME = 0,
        PROP_TYPE,
        PROP_ID,
        PROP_ACCESS,
        PROP_UNITS,
        PROP_PRESENT_VALUE,

        PROP__MAX
    };
    Platform::String^ ToString(ADAPTER_MOCK_PROPERTY_ATTRIBUTE_ID PropertyAttributeId);


    //
    // MOCK_VALUE_DESCRIPTOR
    // Description:
    //      A mock value descriptor.
    //
    struct MOCK_VALUE_DESCRIPTOR
    {
        // Value type
        Windows::Foundation::PropertyType   ValueType;

        // The attribute name
        Platform::String^   Name;

        //
        // Initial value 
        // (union like, but easier to statically initialize)
        //
        struct
        {
            double              AsSimpleType;

            BYTE                AsBytes[MOCK_VALUE_MAX_SIZE];

            Platform::String^   AsString;

        } InitialValue;
    };


    //
    // MOCK_PROPERTY_DESCRIPTOR
    // Description:
    //      A mock property descriptor.
    //
    struct MOCK_PROPERTY_DESCRIPTOR
    {
        // The property ID
        ULONG                       Id;

        // The property type
        ADAPTER_MOCK_PROPERTY_TYPE      PropertyType;

        // The property name
        Platform::String^           Name;

        // The property access
        ADAPTER_MOCK_PROPERTY_ACCESS    PropertyAccess;

        // Value units
        ADAPTER_MOCK_UNITS              Units;

        // The current value
        MOCK_VALUE_DESCRIPTOR       CurrentValue;
    };


    //
    // MOCK_DEVICE_DESCRIPTOR
    // Description:
    //      A mock device descriptor.
    //
    struct MOCK_DEVICE_DESCRIPTOR
    {
        // The device ID
        ULONG                       Id;

        // The device name
        Platform::String^           Name;

        // The device manufacturer name
        Platform::String^           VendorName;

        // The device model name
        Platform::String^           Model;

        // The device serial number
        Platform::String^           SerialNumer;

        // The device version number
        Platform::String^           Version;

        // The specific device description
        Platform::String^           Description;

        // Array of property descriptors
        MOCK_PROPERTY_DESCRIPTOR    PropertyDescriptors[MOCK_DEVICE_MAX_PROPERTIES];
    };


    // Mock devices information
    extern ULONG MockDevicesCount;
    extern MOCK_DEVICE_DESCRIPTOR MockDevices[];

} // AdapterLib
