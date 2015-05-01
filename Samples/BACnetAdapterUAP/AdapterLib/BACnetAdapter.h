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
#include "BACnetNotification.h"
#include "Misc.h"
#include "Thread.h"
#include "AdapterConfig.h"

namespace AdapterLib
{
    // 
    // Microsoft DSB Adapter information
    //
    static Platform::String^ DSB_BACNET_ADAPTER_VENDOR = ref new Platform::String(L"Microsoft");
    static Platform::String^ DSB_BACNET_ADAPTER_NAME = ref new Platform::String(L"DSB BACnet Adapter");
    static Platform::String^ DSB_BACNET_ADAPTER_VERSION = ref new Platform::String(L"1.0.0.1");


    //
    // BACnetAdapter class.
    // Description:
    // The class that implements the BACnet Adapter as IAdapter.
    //
    ref class BACnetInterface;
    //
    public ref class BACnetAdapter sealed
        : public BridgeRT::IAdapter,
		public IBACnetNotificationListener
    {
    public:
        //
        // Adapter information
        //
        virtual property Platform::String^ Vendor
        {
            Platform::String^ get() { return this->vendor; }
        }
        virtual property Platform::String^ AdapterName
        {
            Platform::String^ get() { return this->adapterName; }
        }
        virtual property Platform::String^ Version
        {
            Platform::String^ get() { return this->version; }
        }

        //
        // Adapter signals
        //
        virtual property BridgeRT::IAdapterSignalVector^ Signals
        {
            BridgeRT::IAdapterSignalVector^ get() { return ref new BridgeRT::AdapterSignalVector(this->signals); }
        }

        //
        // IAdapter interface 
        //
        virtual uint32 SetConfiguration(_In_ const Platform::Array<byte>^ ConfigurationData);
        virtual uint32 GetConfiguration(_Out_ Platform::Array<byte>^* ConfigurationDataPtr);

        virtual uint32 Initialize();
        virtual uint32 Shutdown();

        virtual uint32 EnumDevices(
            _In_ BridgeRT::ENUM_DEVICES_OPTIONS Options,
            _Out_ BridgeRT::IAdapterDeviceVector^* DeviceListPtr,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );

        virtual uint32 GetProperty(
            _Inout_ BridgeRT::IAdapterProperty^ Property,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );
        virtual uint32 SetProperty(
            _In_ BridgeRT::IAdapterProperty^ Property,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );

        virtual uint32 GetPropertyValue(
            _In_ BridgeRT::IAdapterProperty^ Property,
            _In_ Platform::String^ AttributeName,
            _Out_ BridgeRT::IAdapterValue^* ValuePtr,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );
        virtual uint32 SetPropertyValue(
            _In_ BridgeRT::IAdapterProperty^ Property,
            _In_ BridgeRT::IAdapterValue^ Value,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );

        virtual uint32 CallMethod(
            _Inout_ BridgeRT::IAdapterMethod^ Method,
            _Out_opt_ BridgeRT::IAdapterIoRequest^* RequestPtr
            );

        virtual uint32 RegisterSignalListener(
            _In_ BridgeRT::IAdapterSignal^ Signal,
            _In_ BridgeRT::IAdapterSignalListener^ Listener,
            _In_opt_ Platform::Object^ ListenerContext
            );
        virtual uint32 UnregisterSignalListener(
            _In_ BridgeRT::IAdapterSignal^ Signal,
            _In_ BridgeRT::IAdapterSignalListener^ Listener
            );

		//
		// IBACnetNotificationListener implementation
		//
		virtual void OnEvent(_In_ BACNET_EVENT_PARAMETERS^ EventParameters);

        //
        //  Routine Description:
        //      NotifySignalListener is called by the Adapter to notify and registered
        //      signal listener of an intercepted signal.
        //
        //  Arguments:
        //
        //      Signal - The signal object to notify listeners.
        //
        //  Return Value:
        //      ERROR_SUCCESS,
        //      ERROR_INVALID_HANDLE: Invalid signal object.
        //
        uint32 NotifySignalListener(
            _In_ BridgeRT::IAdapterSignal^ Signal
            );

        //
        // Construction
        //
        BACnetAdapter();
        virtual ~BACnetAdapter();

    internal:
        bool IsAllowedDevice(Platform::String^ DeviceModelName);

    protected private:
        uint32 createSignals();

        BridgeRT::IAdapterSignal^ getSignalByName(Platform::String^ SignalName);

        DWORD deviceDiscoveryThreadEntry();

        //
        // IBACnetNotificationListener handlers
        //
        void onNewDeviceEvent(_In_ BACNET_EVENT_PARAMETERS^ EventParameters);
        void onChangeOfValue(_In_ BACNET_EVENT_PARAMETERS^ EventParameters);
        void onSetValueAck(_In_ BACNET_EVENT_PARAMETERS^ EventParameters);

    protected private:

        // Adapter information
        Platform::String^ vendor;
        Platform::String^ adapterName;
        Platform::String^ version;

        // Devices
        std::vector<BridgeRT::IAdapterDevice^> devices;

        // Signals 
        std::vector<BridgeRT::IAdapterSignal^> signals;

        // Sync object
        DsbCommon::CSLock lock;

        //
        // Signal listener entry
        //
        struct SIGNAL_LISTENER_ENTRY
        {
            SIGNAL_LISTENER_ENTRY(
                BridgeRT::IAdapterSignal^ SignalToRegisterTo,
                BridgeRT::IAdapterSignalListener^ ListenerObject,
                Platform::Object^ ListenerContext
                )
                : Signal(SignalToRegisterTo)
                , Listener(ListenerObject)
                , Context(ListenerContext)
            {
            }

            // The  signal object
            BridgeRT::IAdapterSignal^ Signal;

            // The listener object
            BridgeRT::IAdapterSignalListener^ Listener;

            //
            // The listener context that will be
            // passed to the signal handler
            //
            Platform::Object^ Context;
        };

        // A map of signal handle (object's hash code) and related listener entry
        std::multimap<int, SIGNAL_LISTENER_ENTRY> signalListeners;

        // Device object ID -> device lookup
        std::map<ULONG, BridgeRT::IAdapterDevice^> deviceLookup;

        // The device discovery thread
        DsbCommon::MemberThread<BACnetAdapter> deviceDiscoveryThread;

        // The BACnet stack interface
        BACnetInterface^ stackInterface;

        // Adapter configuration
        AdapterConfig adapterConfig;
    };

} // namespace AdapterLib