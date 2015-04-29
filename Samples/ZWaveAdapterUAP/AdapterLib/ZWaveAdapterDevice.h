// Copyright (c) 2015, Microsoft Corporation
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

#pragma once

#include "AdapterDefinitions.h"

#include "value_classes\ValueID.h"

namespace AdapterLib
{
    ref class ZWaveAdapterDevice : BridgeRT::IAdapterDevice
    {
    public:
        //
        // Generic for DSB objects
        //
        virtual property Platform::String^ Name
        {
            Platform::String^ get() { return m_name; }
        }

        //
        // Device information
        //
        virtual property Platform::String^ Vendor
        {
            Platform::String^ get() { return m_vendor; }
        }
        virtual property Platform::String^ Model
        {
            Platform::String^ get() { return m_model; }
        }
        virtual property Platform::String^ Version
        {
            Platform::String^ get() { return m_version; }
        }
        virtual property Platform::String^ FirmwareVersion
        {
            Platform::String^ get() { return m_firmwareVersion; }
        }
        virtual property Platform::String^ SerialNumber
        {
            Platform::String^ get() { return m_serialNumber; }
        }
        virtual property Platform::String^ Description
        {
            Platform::String^ get() { return m_description; }
        }

        // Device properties
        virtual property BridgeRT::IAdapterPropertyVector^ Properties
        {
            BridgeRT::IAdapterPropertyVector^ get()
            {
                return ref new BridgeRT::AdapterPropertyVector(m_properties);
            }
        }

        // Device methods
        virtual property BridgeRT::IAdapterMethodVector^ Methods
        {
            BridgeRT::IAdapterMethodVector^ get() 
            { 
                return ref new BridgeRT::AdapterMethodVector(m_methods);
            }
        }

        // Device signals
        virtual property BridgeRT::IAdapterSignalVector^ Signals
        {
            BridgeRT::IAdapterSignalVector^ get()
            {
                return ref new BridgeRT::AdapterSignalVector(m_signals);
            }
        }
    
    internal:
        friend ref class ZWaveAdapter;

        ZWaveAdapterDevice(uint32 homeId, uint8 nodeId);

        void Initialize();

        void AddPropertyValue(const OpenZWave::ValueID& value);
        void UpdatePropertyValue(const OpenZWave::ValueID& value);

        void RemovePropertyValue(const OpenZWave::ValueID& value);
        
    private:
        std::vector<BridgeRT::IAdapterProperty^>::iterator GetProperty(const OpenZWave::ValueID& value);
        BridgeRT::IAdapterSignal^ GetSignal(Platform::String^ name);

        void BuildSignals();

    private:
        Platform::String^ m_name = nullptr;

        // Device information
        Platform::String^ m_vendor;
        Platform::String^ m_model;
        Platform::String^ m_version;
        Platform::String^ m_firmwareVersion;
        Platform::String^ m_serialNumber;
        Platform::String^ m_description;

        // Device properties 
        std::vector<BridgeRT::IAdapterProperty^> m_properties;

        // Device methods 
        std::vector<BridgeRT::IAdapterMethod^> m_methods;

        // Device signals 
        std::vector<BridgeRT::IAdapterSignal^> m_signals;

        uint32 m_homeId;
        uint8 m_nodeId;
    };
}