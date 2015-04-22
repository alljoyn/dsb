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

#include "AdapterDefinitions.h"
#include "MockDevices.h"

namespace AdapterLib
{
    //
    // MockAdapterValue.
    // Description:
    //  The class that implements BridgeRT::IAdapterValue.
    //
    ref class MockAdapterValue : BridgeRT::IAdapterValue
    {
    public:
        //
        // Generic for adapter objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return this->name; }
        }
        virtual property Platform::Object^ Parent
        {
            Platform::Object^ get() { return this->parent; }
        }

        // Data
        virtual property Platform::Object^ Data
        {
            Platform::Object^ get() { return this->data; }
            void set(Platform::Object^ NewData) { this->data = NewData; }
        }

        uint32 Set(BridgeRT::IAdapterValue^ Other);

    internal:
        MockAdapterValue(
            Platform::String^ ObjectName, 
            Platform::Object^ ParentObject, 
            Platform::Object^ DefaultData = nullptr // For signature initialization
            );
        MockAdapterValue(const MockAdapterValue^ Other);

    private:
        // Generic
        Platform::String^ name;
        Platform::Object^ parent;

        Platform::Object^ data;
    };


    //
    // MockAdapterProperty.
    // Description:
    //  The class that implements BridgeRT::IAdapterProperty.
    //
    struct MOCK_PROPERTY_DESCRIPTOR;
    ref class MockAdapterProperty : BridgeRT::IAdapterProperty
    {
    public:
        //
        // Generic for adapter objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return this->name; }
        }
        virtual property Platform::Object^ Parent
        {
            Platform::Object^ get() { return this->parent; }
        }

        // Attributes
        virtual property BridgeRT::IAdapterValueVector^ Attributes
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(this->attributes);
            }
        }

    internal:
        MockAdapterProperty::MockAdapterProperty(Platform::String^ Name, Platform::Object^ ParentObject);
        MockAdapterProperty(const MOCK_PROPERTY_DESCRIPTOR* MockPropertyDescPtr, Platform::Object^ ParentObject);
        MockAdapterProperty(const MockAdapterProperty^ Other);

        uint32 Set(BridgeRT::IAdapterProperty^ Other);

        MockAdapterValue^ GetAttributeByName(Platform::String^ Name);

        uint32 CheckAccess(uint32 DesiredAccess);

        uint32 Reset();

        void NotifyCovSignal(BridgeRT::IAdapterValue^ Attribute);

    private:
        // Generic
        Platform::String^ name;
        Platform::Object^ parent;

        const MOCK_PROPERTY_DESCRIPTOR* mockDescPtr;

        std::vector<BridgeRT::IAdapterValue^> attributes;
    };


    //
    // MockAdapterMethod.
    // Description:
    //  The class that implements BridgeRT::IAdapterDevice.
    //
    ref class MockAdapterMethod :public BridgeRT::IAdapterMethod
    {
    public:
        // Object name
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return this->name; }
        }

        // Get the object's parent
        virtual property Platform::Object^ Parent
        {
            Platform::Object^ get() { return this->parent; }
        }

        // Method description
        virtual property Platform::String^ Description
        {
            Platform::String^ get() { return this->description; }
        }

        // The input parameters
        virtual property BridgeRT::IAdapterValueVector^ InputParams
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(this->inParams);
            }
            void set(BridgeRT::IAdapterValueVector^ Params);
        }

        // The output parameters
        virtual property BridgeRT::IAdapterValueVector^ OutputParams
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(this->outParams);
            }
        }

        // The return value
        virtual property int32 HResult
        {
            int32 get() { return this->result; }
        }

    internal:
        MockAdapterMethod(Platform::String^ ObjectName, Platform::Object^ ParentObject);
        MockAdapterMethod(const MockAdapterMethod^ Other);

        // Adding parameters
        void addInputParam(BridgeRT::IAdapterValue^ InParameter)
        {
            this->inParams.push_back(InParameter);
        }
        void addOutputParam(BridgeRT::IAdapterValue^ OutParameter)
        {
            this->outParams.push_back(OutParameter);
        }

        void setResult(HRESULT Hr);

    private:
        // Generic
        Platform::String^ name;
        Platform::Object^ parent;

        // Method information
        Platform::String^ description;

        // Method parameters
        std::vector<BridgeRT::IAdapterValue^> inParams;
        std::vector<BridgeRT::IAdapterValue^> outParams;
        int32 result;
    };


    //
    // MockAdapterSignal.
    // Description:
    //  The class that implements BridgeRT::IAdapterSignal.
    //
    ref class MockAdapterSignal : BridgeRT::IAdapterSignal
    {
    public:
        //
        // Generic for adapter objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return this->name; }
        }
        virtual property Platform::Object^ Parent
        {
            Platform::Object^ get() { return this->parent; }
        }

        // Signal parameters
        virtual property BridgeRT::IAdapterValueVector^ Params
        {
            BridgeRT::IAdapterValueVector^ get()
            {
                return ref new BridgeRT::AdapterValueVector(this->params);
            }
        }

    internal:
        MockAdapterSignal(Platform::String^ ObjectName, Platform::Object^ ParentObject)
            : name(ObjectName)
            , parent(ParentObject)
        {
        }

        // Adding parameters
        MockAdapterSignal^ operator += (BridgeRT::IAdapterValue^ Parameter)
        {
            this->params.push_back(Parameter);
            return this;
        }

    private:
        // Generic
        Platform::String^ name;
        Platform::Object^ parent;

        std::vector<BridgeRT::IAdapterValue^> params;
    };


    //
    // MockAdapterDevice.
    // Description:
    //  The class that implements BridgeRT::IAdapterDevice.
    //
    struct MOCK_DEVICE_DESCRIPTOR;
    ref class MockAdapterDevice : BridgeRT::IAdapterDevice
    {
    public:
        //
        // Generic for adapter objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return this->name; }
        }
        virtual property Platform::Object^ Parent
        {
            Platform::Object^ get() { return this->parent; }
        }

        //
        // Device information
        //
        virtual property Platform::String^ Vendor
        {
            Platform::String^ get() { return this->vendor; }
        }
        virtual property Platform::String^ Model
        {
            Platform::String^ get() { return this->model; }
        }
        virtual property Platform::String^ Version
        {
            Platform::String^ get() { return this->version; }
        }
        virtual property Platform::String^ FirmwareVersion
        {
            Platform::String^ get() { return this->firmwareVersion; }
        }
        virtual property Platform::String^ ID
        {
            Platform::String^ get() { return this->serialNumber; }
        }
        virtual property Platform::String^ SerialNumber
        {
            Platform::String^ get() { return this->serialNumber; }
        }
        virtual property Platform::String^ Description
        {
            Platform::String^ get() { return this->description; }
        }

        // Device properties
        virtual property BridgeRT::IAdapterPropertyVector^ Properties
        {
            BridgeRT::IAdapterPropertyVector^ get()
            {
                return ref new BridgeRT::AdapterPropertyVector(this->properties);
            }
        }

        // Device methods
        virtual property BridgeRT::IAdapterMethodVector^ Methods
        {
            BridgeRT::IAdapterMethodVector^ get();
        }

        // Device signals
        virtual property BridgeRT::IAdapterSignalVector^ Signals
        {
            BridgeRT::IAdapterSignalVector^ get()
            {
                return ref new BridgeRT::AdapterSignalVector(this->signals);
            }
        }

    internal:
        MockAdapterDevice(Platform::String^ Name, Platform::Object^ ParentObject);
        MockAdapterDevice(const MOCK_DEVICE_DESCRIPTOR* MockDeviceDescPtr, Platform::Object^ ParentObject);

        void SendSignal(
            _In_ Platform::String^ SignalName,
            _In_opt_ BridgeRT::IAdapterProperty^ Property,
            _In_opt_ BridgeRT::IAdapterValue^ Attribute
            );

        uint32 MethodDispatch(
                _In_ BridgeRT::IAdapterMethod^ Method,
                _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
                );

    private:
        void createMethods();
        void createSignals();
    
        void methodReset(_Inout_ BridgeRT::IAdapterMethod^ Method);

    private:
        // Generic
        Platform::String^ name;
        Platform::Object^ parent;

        // Device information
        Platform::String^ vendor;
        Platform::String^ model;
        Platform::String^ version;
        Platform::String^ firmwareVersion;
        Platform::String^ serialNumber;
        Platform::String^ description;

        // Sync object
        DsbCommon::CSLock lock;

        // Device properties 
        std::vector<BridgeRT::IAdapterProperty^> properties;

        // Device methods
        std::vector<BridgeRT::IAdapterMethod^> methods;

        // Device signals 
        std::vector<BridgeRT::IAdapterSignal^> signals;
    };

} // namespace AdapterLib