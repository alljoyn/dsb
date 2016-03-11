// Copyright (c) 2016, Microsoft Corporation
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

﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BridgeRT;

namespace AdapterLib
{
    //
    // AdapterValue.
    // Description:
    // The class that implements IAdapterValue from BridgeRT.
    //
    class AdapterValue : IAdapterValue
    {
        // public properties
        public string Name { get; }
        public object Data { get; set; }

        internal AdapterValue(string ObjectName, object DefaultData)
        {
            this.Name = ObjectName;
            this.Data = DefaultData;
        }

        internal AdapterValue(AdapterValue Other)
        {
            this.Name = Other.Name;
            this.Data = Other.Data;
        }
    }

    //
    // ModbusSensorProperty.
    // Description:
    // The class that implements IAdapterProperty from BridgeRT.
    //
    class ModbusSensorProperty : IAdapterProperty
    {
        // public properties
        public string Name { get; }
        public string InterfaceHint { get; }
        public IList<IAdapterAttribute> Attributes { get; }

        private ModbusSensorDevice m_device;

        internal ModbusSensorProperty(ModbusSensorDevice ParentDevice, string ObjectName, string IfHint)
        {
            this.m_device = ParentDevice;
            this.Name = ObjectName;
            this.InterfaceHint = IfHint;

            try
            {
                this.Attributes = new List<IAdapterAttribute>();
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal ModbusSensorProperty(ModbusSensorProperty Other)
        {
            this.m_device = Other.m_device;
            this.Name = Other.Name;
            this.InterfaceHint = Other.InterfaceHint;

            try
            {
                this.Attributes = new List<IAdapterAttribute>(Other.Attributes);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }
    }

    //
    // ModbusSensorAttribute.
    // Description:
    // The class that implements IAdapterAttribute from BridgeRT.
    //
    class ModbusSensorAttribute : IAdapterAttribute
    {
        // public properties
        public IAdapterValue Value { get; }
        public E_ACCESS_TYPE Access { get; set; }
        public IDictionary<string, string> Annotations { get; }
        public SignalBehavior COVBehavior { get; set; }

        private ModbusSensorDevice m_device;
        private ModbusRequest m_request = null;
        private ModbusResponse m_response = null;

        internal ModbusSensorAttribute(ModbusSensorDevice ParentDevice, string ObjectName, object DefaultData, E_ACCESS_TYPE access = E_ACCESS_TYPE.ACCESS_READ)
        {
            try
            {
                this.m_device = ParentDevice;
                this.Value = new AdapterValue(ObjectName, DefaultData);
                this.Annotations = new Dictionary<string, string>();
                this.Access = access;
                this.COVBehavior = SignalBehavior.Never;
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal ModbusSensorAttribute(ModbusSensorAttribute Other)
        {
            this.m_device = Other.m_device;
            this.Value = Other.Value;
            this.Annotations = Other.Annotations;
            this.Access = Other.Access;
            this.COVBehavior = Other.COVBehavior;
        }

        internal async Task<uint> ReadAsync()
        {
            try
            {
                if (!m_device.Gateway.isConnected)
                {
                    m_device.Gateway.ConnectAsync().Wait();
                }

                if (m_device.Gateway.isConnected)
                {
                    ushort index = m_device.AttributeMap[Value.Name];
                    m_request = new ModbusRequest(1, 1, 3, index, 1);
                    m_response = await m_device.Gateway.sendRequestAsync(m_request);

                    switch (index)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            Value.Data = ((short)m_response.Data[0]) / 10.0;
                            break;
                        case 4:
                        case 7:
                        case 5:
                            Value.Data = m_response.Data[0];
                            break;
                        case 6:
                            Value.Data = (short)m_response.Data[0];
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    return 2;
                }
            }
            catch { return 1; }

            return 0;
        }

        internal async Task<uint> WriteAsync(object value)
        {
            try
            {
                if (!m_device.Gateway.isConnected)
                {
                    m_device.Gateway.ConnectAsync().Wait();
                }

                if (m_device.Gateway.isConnected)
                {
                    ushort index = m_device.AttributeMap[Value.Name];
                    short input = 0;

                    switch (index)
                    {
                        case 2:
                        case 3:
                            input = Convert.ToInt16((double)value * 10);
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)input });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 4:
                        case 7:
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)value });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 6:
                            input = Convert.ToInt16(value);
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)input });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 0:
                        case 1:
                        case 5:
                        default:
                            break;
                    }
                }
                else
                {
                    return 2;
                }

            }
            catch { }

            return 0;
        }

    }

    //
    // AdapterMethod.
    // Description:
    // The class that implements IAdapterMethod from BridgeRT.
    //
    class ModbusSensorMethod : IAdapterMethod
    {
        // public properties
        public string Name { get; }

        public string Description { get; }

        public IList<IAdapterValue> InputParams { get; set; }

        public IList<IAdapterValue> OutputParams { get; }

        public int HResult { get; private set; }

        private ModbusSensorDevice m_device;
        private ModbusRequest m_request = null;
        private ModbusResponse m_response = null;
        private string m_attributeName = null;

        internal ModbusSensorMethod(
            ModbusSensorDevice ParentDevice,
            string ObjectName,
            string Description,
            string targetAttributeName,
            int ReturnValue)
        {
            this.m_device = ParentDevice;
            this.Name = ObjectName;
            this.Description = Description;
            this.m_attributeName = targetAttributeName;
            this.HResult = ReturnValue;

            try
            {
                this.InputParams = new List<IAdapterValue>();
                this.OutputParams = new List<IAdapterValue>();
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal ModbusSensorMethod(ModbusSensorMethod Other)
        {

            this.m_device = Other.m_device;
            this.Name = Other.Name;
            this.Description = Other.Description;
            this.HResult = Other.HResult;

            try
            {
                this.InputParams = new List<IAdapterValue>(Other.InputParams);
                this.OutputParams = new List<IAdapterValue>(Other.OutputParams);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal async void SetValue()
        {
            this.HResult = await WriteAsync(InputParams[0].Data);
        }

        internal async Task<int> WriteAsync(object value)
        {
            try
            {
                if (!m_device.Gateway.isConnected)
                {
                    m_device.Gateway.ConnectAsync().Wait();
                }

                if (m_device.Gateway.isConnected)
                {
                    ushort index = m_device.AttributeMap[m_attributeName];
                    short input = 0;

                    switch (index)
                    {
                        case 2:
                        case 3:
                            input = Convert.ToInt16((double)value * 10);
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)input });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 4:
                        case 7:
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)value });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 6:
                            input = Convert.ToInt16(value);
                            m_request = new ModbusRequest(1, 1, 6, index, 1, 2, new ushort[1] { (ushort)input });
                            m_response = await m_device.Gateway.sendRequestAsync(m_request);
                            break;
                        case 0:
                        case 1:
                        case 5:
                        default:
                            break;
                    }
                }
                else
                {
                    return -1;
                }

            }
            catch { }

            return 0;
        }
    }

    //
    // AdapterSignal.
    // Description:
    // The class that implements IAdapterSignal from BridgeRT.
    //
    class AdapterSignal : IAdapterSignal
    {
        // public properties
        public string Name { get; }

        public IList<IAdapterValue> Params { get; }

        internal AdapterSignal(string ObjectName)
        {
            this.Name = ObjectName;

            try
            {
                this.Params = new List<IAdapterValue>();
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal AdapterSignal(AdapterSignal Other)
        {
            this.Name = Other.Name;

            try
            {
                this.Params = new List<IAdapterValue>(Other.Params);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }
    }

    //
    // ModbusSensorDevice.
    // Description:
    // The class that implements IAdapterDevice from BridgeRT.
    //
    class ModbusSensorDevice : IAdapterDevice,
                            IAdapterDeviceLightingService,
                            IAdapterDeviceControlPanel
    {

        private const uint ERROR_SUCCESS = 0;
        private const uint ERROR_INVALID_HANDLE = 6;

        public ModbusGateway Gateway { get; }

        // Object Name
        public string Name { get; }

        // Device information
        public string Vendor { get; }

        public string Model { get; }

        public string Version { get; }

        public string FirmwareVersion { get; }

        public string SerialNumber { get; }

        public string Description { get; }

        // Device properties
        public IList<IAdapterProperty> Properties { get; }

        // Device methods
        public IList<IAdapterMethod> Methods { get; }

        // Device signals
        public IList<IAdapterSignal> Signals { get; }

        // Control Panel Handler
        public IControlPanelHandler ControlPanelHandler
        {
            get
            {
                return null;
            }
        }

        // Lighting Service Handler
        public ILSFHandler LightingServiceHandler
        {
            get
            {
                return null;
            }
        }

        // Icon
        public IAdapterIcon Icon
        {
            get
            {
                return null;
            }
        }

        //AttributeMap: Maps Attribute to respecitve sensor register address
        public Dictionary<string, ushort> AttributeMap = new Dictionary<string, ushort>();


        internal ModbusSensorDevice(
            ModbusGateway GatewayObj,
            string Name,
            string VendorName,
            string Model,
            string Version,
            string SerialNumber,
            string Description)
        {
            this.Gateway = GatewayObj;
            this.Name = Name;
            this.Vendor = VendorName;
            this.Model = Model;
            this.Version = Version;
            this.FirmwareVersion = Version;
            this.SerialNumber = SerialNumber;
            this.Description = Description;

            try
            {
                this.Properties = new List<IAdapterProperty>();
                this.Methods = new List<IAdapterMethod>();
                this.Signals = new List<IAdapterSignal>();

                //Initialize AttributeMap
                AttributeMap.Add("Temperature", 0);
                AttributeMap.Add("Humidity", 1);
                AttributeMap.Add("Temperature Adjustment", 2);
                AttributeMap.Add("Humidity Adjustment", 3);
                AttributeMap.Add("T/H Reading Interval", 4);
                AttributeMap.Add("CO2 Concentration", 5);
                AttributeMap.Add("CO2 Adjustment", 6);
                AttributeMap.Add("CO2 Reading Interval", 7);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal ModbusSensorDevice(ModbusSensorDevice Other)
        {
            this.Gateway = Other.Gateway;
            this.Name = Other.Name;
            this.Vendor = Other.Vendor;
            this.Model = Other.Model;
            this.Version = Other.Version;
            this.FirmwareVersion = Other.FirmwareVersion;
            this.SerialNumber = Other.SerialNumber;
            this.Description = Other.Description;
            this.AttributeMap = Other.AttributeMap;

            try
            {
                this.Properties = new List<IAdapterProperty>(Other.Properties);
                this.Methods = new List<IAdapterMethod>(Other.Methods);
                this.Signals = new List<IAdapterSignal>(Other.Signals);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal void AddChangeOfValueSignal(
            IAdapterProperty Property,
            IAdapterValue Attribute)
        {
            try
            {
                AdapterSignal covSignal = new AdapterSignal(Constants.CHANGE_OF_VALUE_SIGNAL);

                // Property Handle
                AdapterValue propertyHandle = new AdapterValue(
                                                    Constants.COV__PROPERTY_HANDLE,
                                                    Property);

                // Attribute Handle
                AdapterValue attrHandle = new AdapterValue(
                                                    Constants.COV__ATTRIBUTE_HANDLE,
                                                    Attribute);

                covSignal.Params.Add(propertyHandle);
                covSignal.Params.Add(attrHandle);

                this.Signals.Add(covSignal);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        internal uint Initialize()
        {
            // Sensor property.
            ModbusSensorProperty sensorProperty = new ModbusSensorProperty(this, "T/H/CO2 Sensor", "");

            // Current Temperature Reading attribute.
            ModbusSensorAttribute tempAttr = new ModbusSensorAttribute(
                this,
                "Temperature",
                Windows.Foundation.PropertyValue.CreateDouble(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            tempAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(tempAttr);

            // Temperature Adjustment attribute.
            ModbusSensorAttribute tempAdjAttr = new ModbusSensorAttribute(
                this,
                "Temperature Adjustment",
                Windows.Foundation.PropertyValue.CreateDouble(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            tempAdjAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(tempAdjAttr);

            //Set Temperature Adjustment method.
            ModbusSensorMethod tempAdjCommand = new ModbusSensorMethod(
                this,
                "setTempAdj",
                "Sets the value of Temperature Adjustment to adjust the temperature reading. The input value must be between -20.0 to +20.0.",
                "Temperature Adjustment",
                0);
            var tempAdjParam = new AdapterValue("value", Windows.Foundation.PropertyValue.CreateDouble(0));
            tempAdjCommand.InputParams.Add(tempAdjParam);
            this.Methods.Add(tempAdjCommand);

            // Currernt Humidity Reading attribute.
            ModbusSensorAttribute humidAttr = new ModbusSensorAttribute(
                this,
                "Humidity",
                Windows.Foundation.PropertyValue.CreateDouble(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            humidAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(humidAttr);

            // Humidity Adjustment attribute.
            ModbusSensorAttribute humidAdjAttr = new ModbusSensorAttribute(
                this,
                "Humidity Adjustment",
                Windows.Foundation.PropertyValue.CreateDouble(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            humidAdjAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(humidAdjAttr);

            //Set Humidity Adjustment method.
            ModbusSensorMethod humidAdjCommand = new ModbusSensorMethod(
                this,
                "setHumidAdj",
                "Sets the value of Humidity Adjustment to adjust the humidity reading. The input value must be between -20.0 to +20.0.",
                "Humidity Adjustment",
                0);
            var humidAdjParam = new AdapterValue("value", Windows.Foundation.PropertyValue.CreateDouble(0));
            humidAdjCommand.InputParams.Add(humidAdjParam);
            this.Methods.Add(humidAdjCommand);

            // Temperature/Humidity Reading Interval attribute.
            ModbusSensorAttribute THIntervalAttr = new ModbusSensorAttribute(
                this,
                "T/H Reading Interval",
                Windows.Foundation.PropertyValue.CreateUInt16(1),
                E_ACCESS_TYPE.ACCESS_READ
                );
            THIntervalAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(THIntervalAttr);

            //Set Temperature/Humidity Reading Interval method.
            ModbusSensorMethod THIntervalCommand = new ModbusSensorMethod(
                this,
                "setTHInterval",
                "Sets the value of temperature and humidity reading interval. The input value must be between 1 to 10.",
                "T/H Reading Interval",
                0);
            var THIntervalParam = new AdapterValue("value", Windows.Foundation.PropertyValue.CreateUInt16(1));
            THIntervalCommand.InputParams.Add(THIntervalParam);
            this.Methods.Add(THIntervalCommand);


            // Current CO2 Concentration attribute.
            ModbusSensorAttribute CO2Attr = new ModbusSensorAttribute(
                this,
                "CO2 Concentration",
                Windows.Foundation.PropertyValue.CreateUInt16(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            CO2Attr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(CO2Attr);

            // CO2 Adjustment attribute.
            ModbusSensorAttribute CO2AdjAttr = new ModbusSensorAttribute(
                this,
                "CO2 Adjustment",
                Windows.Foundation.PropertyValue.CreateInt16(0),
                E_ACCESS_TYPE.ACCESS_READ
                );
            CO2AdjAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(CO2AdjAttr);

            //Set CO2 Concentration Adjustment method.
            ModbusSensorMethod CO2AdjCommand = new ModbusSensorMethod(
                this,
                "setCO2Adj",
                "Sets the value of CO2 concentration Adjustment to adjust the CO2 concentration reading. The input value must be between -1500 to +1500",
                "CO2 Adjustment",
                0);
            var CO2AdjParam = new AdapterValue("value", Windows.Foundation.PropertyValue.CreateInt16(0));
            CO2AdjCommand.InputParams.Add(CO2AdjParam);
            this.Methods.Add(CO2AdjCommand);

            // CO2 Reading Interval attribute.
            ModbusSensorAttribute CO2IntervalAttr = new ModbusSensorAttribute(
                this,
                "CO2 Reading Interval",
                Windows.Foundation.PropertyValue.CreateUInt16(1),
                E_ACCESS_TYPE.ACCESS_READ
                );
            CO2IntervalAttr.COVBehavior = SignalBehavior.Always;
            sensorProperty.Attributes.Add(CO2IntervalAttr);

            //Set CO2 Reading Interval method.
            ModbusSensorMethod CO2IntervalCommand = new ModbusSensorMethod(
                this,
                "setCO2Interval",
                "Sets the value of temperature and humidity reading interval. The input value must be between 1 to 10.",
                "CO2 Reading Interval",
                0);
            var CO2IntervalParam = new AdapterValue("value", Windows.Foundation.PropertyValue.CreateUInt16(1));
            CO2IntervalCommand.InputParams.Add(CO2IntervalParam);
            this.Methods.Add(CO2IntervalCommand);


            // Create Change of Value Signal for the Temperature Attribute
            AdapterSignal covSignal = new AdapterSignal(Constants.CHANGE_OF_VALUE_SIGNAL);
            AdapterValue propertyHandle = new AdapterValue(Constants.COV__PROPERTY_HANDLE, sensorProperty);
            AdapterValue attrHandle = new AdapterValue(Constants.COV__ATTRIBUTE_HANDLE, tempAttr.Value);
            covSignal.Params.Add(propertyHandle);
            covSignal.Params.Add(attrHandle);


            this.Signals.Add(covSignal);
            this.Properties.Add(sensorProperty);

            return ERROR_SUCCESS;
        }
    }
}
