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

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Storage;
using BridgeRT;

namespace AdapterLib
{
    public sealed class Adapter : IAdapter
    {
        private const uint ERROR_SUCCESS = 0;
        private const uint ERROR_INVALID_HANDLE = 6;

        // Device Arrival and Device Removal Signal Indices
        private const int DEVICE_ARRIVAL_SIGNAL_INDEX = 0;
        private const int DEVICE_ARRIVAL_SIGNAL_PARAM_INDEX = 0;
        private const int DEVICE_REMOVAL_SIGNAL_INDEX = 1;
        private const int DEVICE_REMOVAL_SIGNAL_PARAM_INDEX = 0;

        // Modbus Gateway
        private const string CONFIG_FILENAME = "ModbusConfig.xml";
        private ModbusGateway m_gateway;

        // My Modbus Specific Error Code
        private const uint ERROR_MODBUS_GETCONFIG_FAILED= 0xFF01;
        private const uint ERROR_MODBUS_SETCONFIG_FAILED = 0xFF02;
        private const uint ERROR_MODBUS_GATEWAY_DISCONNECT = 0xFF03;
        private const uint ERROR_MODBUS_INI_FAILED = 0xFF04;

        public string Vendor { get; }

        public string AdapterName { get; }

        public string Version { get; }

        public string ExposedAdapterPrefix { get; }

        public string ExposedApplicationName { get; }

        public Guid ExposedApplicationGuid { get; }

        public IList<IAdapterSignal> Signals { get; }

        public Adapter()
        {
            byte[] ConfigurationDataPtr;
            if (GetConfiguration(out ConfigurationDataPtr) == ERROR_SUCCESS)
            {
                var temp = System.Text.Encoding.UTF8.GetString(ConfigurationDataPtr);
                m_gateway = new ModbusGateway(System.Text.Encoding.UTF8.GetString(ConfigurationDataPtr));  // The Designated IP address of the Modbus Gateway
            }

            Windows.ApplicationModel.Package package = Windows.ApplicationModel.Package.Current;
            Windows.ApplicationModel.PackageId packageId = package.Id;
            Windows.ApplicationModel.PackageVersion versionFromPkg = packageId.Version;

            this.Vendor = "Microsoft";
            this.AdapterName = "Modbus Device System Bridge";

            // the adapter prefix must be something like "com.mycompany" (only alpha num and dots)
            // it is used by the Device System Bridge as root string for all services and interfaces it exposes
            this.ExposedAdapterPrefix = "com." + this.Vendor.ToLower();
            this.ExposedApplicationGuid = Guid.Parse("{0xd3982475,0x38eb,0x4e53,{0xb8,0xd8,0x54,0x04,0xd8,0x78,0x88,0x1a}}");

            if (null != package && null != packageId)
            {
                this.ExposedApplicationName = packageId.Name;
                this.Version = versionFromPkg.Major.ToString() + "." +
                               versionFromPkg.Minor.ToString() + "." +
                               versionFromPkg.Revision.ToString() + "." +
                               versionFromPkg.Build.ToString();
            }
            else
            {
                this.ExposedApplicationName = "ModbusDeviceSystemBridge";
                this.Version = "0.0.0.0";
            }

            try
            {
                this.Signals = new List<IAdapterSignal>();
                this.devices = new List<IAdapterDevice>();
                this.signalListeners = new Dictionary<int, IList<SIGNAL_LISTENER_ENTRY>>();

                //Create Adapter Signals
                this.createSignals();
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        public uint SetConfiguration([ReadOnlyArray] byte[] ConfigurationData)
        {
            try
            {
                XDocument doc = XDocument.Load(CONFIG_FILENAME);
                XElement value = doc.Root.Descendants("IP").SingleOrDefault();
                value.SetValue(System.Text.Encoding.UTF8.GetString(ConfigurationData));

                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.LoadXml(doc.Declaration.ToString()+doc.ToString());

                Task task = new Task(async () =>
                {
                    StorageFolder storageFolder = await StorageFolder.GetFolderFromPathAsync(ApplicationData.Current.LocalFolder.Path);
                    StorageFile storageFile = await storageFolder.CreateFileAsync(CONFIG_FILENAME, CreationCollisionOption.ReplaceExisting);
                    Stream stream = await storageFile.OpenStreamForWriteAsync();
                    xmlDoc.Save(stream);
                });
                task.Start();
            }
            catch { }
            return ERROR_MODBUS_SETCONFIG_FAILED;
        }

        public uint GetConfiguration(out byte[] ConfigurationDataPtr)
        {
            ConfigurationDataPtr = null;

            try
            {
                XmlReader reader = XmlReader.Create(CONFIG_FILENAME);
                while (reader.Read())
                {
                    if (reader.Name == "IP")
                    {
                        reader.Read();
                        ConfigurationDataPtr = System.Text.Encoding.UTF8.GetBytes(reader.Value);
                        reader.Dispose();
                        return ERROR_SUCCESS;
                    }
                }
                reader.Dispose();
            }
            catch { }
            return ERROR_MODBUS_GETCONFIG_FAILED;
        }

        public uint Initialize()
        {
            uint status;

            //Initialize a Modbus Gateway
            m_gateway.InitializeAsync().Wait();

            if (m_gateway.isConnected)
            {
                //Initialize a Modbus Sensor Device
                ModbusSensorDevice newDevice = new ModbusSensorDevice(m_gateway, "Modbus Sensor", this.Vendor, "T/H/CO2 Sensor", this.Version, "150515-0057", "Modbus T/H/CO2 Sensor");
                status = newDevice.Initialize();

                if (status == ERROR_SUCCESS)
                {
                    devices.Add(newDevice);
                }
                return ERROR_SUCCESS;
            }

            // Initialization Failed
            return ERROR_MODBUS_INI_FAILED;
        }

        public uint Shutdown()
        {
            if (null != m_gateway)
            {
                m_gateway.Disconnect();
            }
            return ERROR_SUCCESS;
        }

        public uint EnumDevices(
            ENUM_DEVICES_OPTIONS Options,
            out IList<IAdapterDevice> DeviceListPtr,
            out IAdapterIoRequest RequestPtr)
        {
            RequestPtr = null;

            try
            {
                DeviceListPtr = new List<IAdapterDevice>(this.devices);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }

            return ERROR_SUCCESS;
        }

        public uint GetProperty(
            IAdapterProperty Property,
            out IAdapterIoRequest RequestPtr)
        {
            RequestPtr = null;

            return ERROR_SUCCESS;
        }

        public uint SetProperty(
            IAdapterProperty Property,
            out IAdapterIoRequest RequestPtr)
        {
            RequestPtr = null;

            return ERROR_SUCCESS;
        }

        private void tempValueChangeEventHandler()
        {
            IAdapterSignal covSignal = devices.ElementAt(0).Signals.ElementAt(0);
            NotifySignalListener(covSignal);
        }

        double previousTempValue = -100.0;
        public uint GetPropertyValue(
            IAdapterProperty Property,
            string AttributeName,
            out IAdapterValue ValuePtr,
            out IAdapterIoRequest RequestPtr)
        {
            ValuePtr = null;
            RequestPtr = null;

            // find corresponding attribute
            foreach (var attribute in Property.Attributes)
            {
                if (attribute.Value.Name == AttributeName)
                {
                    //Read from Modbus Device
                    ((ModbusSensorAttribute)attribute).ReadAsync().Wait();
                    ValuePtr = attribute.Value;

                    //Send signals if needed
                    if (AttributeName == "Temperature" && previousTempValue != (double)ValuePtr.Data)
                    {
                        if (previousTempValue != -100.0)
                        {
                            tempValueChangeEventHandler();
                        }
                        previousTempValue = (double)ValuePtr.Data;
                    }
                    return ERROR_SUCCESS;
                }
            }

            return ERROR_INVALID_HANDLE;
        }

        public uint SetPropertyValue(
            IAdapterProperty Property,
            IAdapterValue Value,
            out IAdapterIoRequest RequestPtr)
        {
            RequestPtr = null;

            // find corresponding attribute
            foreach (var attribute in Property.Attributes)
            {
                if (attribute.Value.Name == Value.Name)
                {
                    //Write to Modbus Device
                    ((ModbusSensorAttribute)attribute).WriteAsync(Value.Data).Wait();
                    attribute.Value.Data = Value.Data;
                    return ERROR_SUCCESS;
                }
            }

            return ERROR_INVALID_HANDLE;
        }

        public uint CallMethod(
            IAdapterMethod Method,
            out IAdapterIoRequest RequestPtr)
        {
            RequestPtr = null;
            ((ModbusSensorMethod)Method).SetValue();

            return ERROR_SUCCESS;
        }

        public uint RegisterSignalListener(
            IAdapterSignal Signal,
            IAdapterSignalListener Listener,
            object ListenerContext)
        {
            if (Signal == null || Listener == null)
            {
                return ERROR_INVALID_HANDLE;
            }

            int signalHashCode = Signal.GetHashCode();

            SIGNAL_LISTENER_ENTRY newEntry;
            newEntry.Signal = Signal;
            newEntry.Listener = Listener;
            newEntry.Context = ListenerContext;

            lock (this.signalListeners)
            {
                if (this.signalListeners.ContainsKey(signalHashCode))
                {
                    this.signalListeners[signalHashCode].Add(newEntry);
                }
                else
                {
                    IList<SIGNAL_LISTENER_ENTRY> newEntryList;

                    try
                    {
                        newEntryList = new List<SIGNAL_LISTENER_ENTRY>();
                    }
                    catch (OutOfMemoryException ex)
                    {
                        throw;
                    }

                    newEntryList.Add(newEntry);
                    this.signalListeners.Add(signalHashCode, newEntryList);
                }
            }

            return ERROR_SUCCESS;
        }

        public uint UnregisterSignalListener(
            IAdapterSignal Signal,
            IAdapterSignalListener Listener)
        {
            return ERROR_SUCCESS;
        }

        public uint NotifySignalListener(IAdapterSignal Signal)
        {
            if (Signal == null)
            {
                return ERROR_INVALID_HANDLE;
            }

            int signalHashCode = Signal.GetHashCode();

            lock (this.signalListeners)
            {
                IList<SIGNAL_LISTENER_ENTRY> listenerList = this.signalListeners[signalHashCode];
                foreach (SIGNAL_LISTENER_ENTRY entry in listenerList)
                {
                    IAdapterSignalListener listener = entry.Listener;
                    object listenerContext = entry.Context;
                    listener.AdapterSignalHandler(Signal, listenerContext);
                }
            }

            return ERROR_SUCCESS;
        }

        public uint NotifyDeviceArrival(IAdapterDevice Device)
        {
            if (Device == null)
            {
                return ERROR_INVALID_HANDLE;
            }

            IAdapterSignal deviceArrivalSignal = this.Signals[DEVICE_ARRIVAL_SIGNAL_INDEX];
            IAdapterValue signalParam = deviceArrivalSignal.Params[DEVICE_ARRIVAL_SIGNAL_PARAM_INDEX];
            signalParam.Data = Device;
            this.NotifySignalListener(deviceArrivalSignal);

            return ERROR_SUCCESS;
        }

        public uint NotifyDeviceRemoval(IAdapterDevice Device)
        {
            if (Device == null)
            {
                return ERROR_INVALID_HANDLE;
            }

            IAdapterSignal deviceRemovalSignal = this.Signals[DEVICE_REMOVAL_SIGNAL_INDEX];
            IAdapterValue signalParam = deviceRemovalSignal.Params[DEVICE_REMOVAL_SIGNAL_PARAM_INDEX];
            signalParam.Data = Device;
            this.NotifySignalListener(deviceRemovalSignal);

            return ERROR_SUCCESS;
        }

        private void createSignals()
        {
            try
            {
                // Device Arrival Signal
                AdapterSignal deviceArrivalSignal = new AdapterSignal(Constants.DEVICE_ARRIVAL_SIGNAL);
                AdapterValue deviceHandle_arrival = new AdapterValue(
                                                            Constants.DEVICE_ARRIVAL__DEVICE_HANDLE,
                                                            null);
                deviceArrivalSignal.Params.Add(deviceHandle_arrival);

                // Device Removal Signal
                AdapterSignal deviceRemovalSignal = new AdapterSignal(Constants.DEVICE_REMOVAL_SIGNAL);
                AdapterValue deviceHandle_removal = new AdapterValue(
                                                            Constants.DEVICE_REMOVAL__DEVICE_HANDLE,
                                                            null);
                deviceRemovalSignal.Params.Add(deviceHandle_removal);

                // Add Signals to the Adapter Signals
                this.Signals.Add(deviceArrivalSignal);
                this.Signals.Add(deviceRemovalSignal);
            }
            catch (OutOfMemoryException ex)
            {
                throw;
            }
        }

        private struct SIGNAL_LISTENER_ENTRY
        {
            // The signal object
            internal IAdapterSignal Signal;

            // The listener object
            internal IAdapterSignalListener Listener;

            //
            // The listener context that will be
            // passed to the signal handler
            //
            internal object Context;
        }

        // List of Devices
        private IList<IAdapterDevice> devices;

        // A map of signal handle (object's hash code) and related listener entry
        private Dictionary<int, IList<SIGNAL_LISTENER_ENTRY>> signalListeners;
    }
}