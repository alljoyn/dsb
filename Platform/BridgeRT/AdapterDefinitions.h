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

#include <collection.h>
#include <algorithm>

namespace BridgeRT
{
    //
    // Adapter objects vector/vector interfaces
    //

    interface class IAdapterValue;
    typedef Platform::Collections::Vector<IAdapterValue^> AdapterValueVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterValue^> IAdapterValueVector;
    typedef Windows::Foundation::PropertyType AdapterValueType;

    interface class IAdapterProperty;
    typedef Platform::Collections::Vector<IAdapterProperty^> AdapterPropertyVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterProperty^> IAdapterPropertyVector;

    interface class IAdapterMethod;
    typedef Platform::Collections::Vector<IAdapterMethod^> AdapterMethodVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterMethod^> IAdapterMethodVector;

    interface class IAdapterSignal;
    typedef Platform::Collections::Vector<IAdapterSignal^> AdapterSignalVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterSignal^> IAdapterSignalVector;

    interface class IAdapterDevice;
    typedef Platform::Collections::Vector<IAdapterDevice^> AdapterDeviceVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterDevice^> IAdapterDeviceVector;


    //
    // Adapter level signals and parameters names
    //

    // Device arrival signal
    static Platform::String^ DEVICE_ARRIVAL_SIGNAL = ref new Platform::String(L"Device_Arrival");
    static Platform::String^ DEVICE_ARRIVAL__DEVICE_HANDLE = ref new Platform::String(L"Device_Handle");
    
    // Device removal signal
    static Platform::String^ DEVICE_REMOVAL_SIGNAL = ref new Platform::String(L"Device_Removal");
    static Platform::String^ DEVICE_REMOVAL__DEVICE_HANDLE = ref new Platform::String(L"Device_Handle");

    // Change Of Value signal
    static Platform::String^ CHANGE_OF_VALUE_SIGNAL = ref new Platform::String(L"Change_Of_Value");
    static Platform::String^ COV__PROPERTY_HANDLE = ref new Platform::String(L"Property_Handle");
    static Platform::String^ COV__ATTRIBUTE_HANDLE = ref new Platform::String(L"Attribute_Handle");
}