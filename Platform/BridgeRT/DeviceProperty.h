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

namespace BridgeRT
{
    ref class BridgeDevice;
    class PropertyInterface;
    class AllJoynProperty;

    typedef struct
    {
        AllJoynProperty *ajProperty;
        IAdapterValue ^adapterValue;
    } AJpropertyAdapterValuePair;

    class DeviceProperty
    {
    public:
        DeviceProperty();
        virtual ~DeviceProperty();

        QStatus Initialize(_In_ IAdapterProperty ^deviceProperty, _In_ PropertyInterface *propertyInterface, _In_ BridgeDevice ^parent);
        void Shutdown();
        void EmitSignalCOV(_In_ IAdapterValue ^newValue, const std::vector<alljoyn_sessionid>& sessionIds);

        inline std::string *GetPathName()
        {
            return &m_AJBusObjectPath;
        }
        inline alljoyn_busobject GetBusObject()
        {
            return m_AJBusObject;
        }
        inline PropertyInterface *GetPropertyInterface()
        {
            return m_propertyInterface;
        }
        inline IAdapterProperty ^GetAdapterProperty()
        {
            return m_deviceProperty;
        }

    private:
        QStatus PairAjProperties();

        static QStatus AJ_CALL GetProperty(_In_ const void* context, _In_z_ const char* ifcName, _In_z_ const char* propName, _Out_ alljoyn_msgarg val);
        static QStatus AJ_CALL SetProperty(_In_ const void* context, _In_z_ const char* ifcName, _In_z_ const char* propName, _In_ alljoyn_msgarg val);
        
        // parent class
        BridgeDevice ^m_parent;

        // device property
        IAdapterProperty ^m_deviceProperty;

        // AllJoyn related
        alljoyn_busobject m_AJBusObject;
        PropertyInterface *m_propertyInterface;
        bool m_registeredOnAllJoyn;

        // store association of each bus interface property exposed on AllJoyn by this bus object 
        // with its corresponding device instance (adapter value)
        std::map<std::string, AJpropertyAdapterValuePair> m_AJpropertyAdapterValuePairs;

        std::string m_AJBusObjectPath;
    };
}

