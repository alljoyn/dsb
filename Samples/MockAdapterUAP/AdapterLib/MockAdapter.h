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
#include "Misc.h"
#include "AdapterConfig.h"

namespace AdapterLib
{
    //
    // MockAdapter class.
    // Description:
    //  The class that implements the Mock Adapter as IAdapter.
    //
    public ref class MockAdapter sealed : public BridgeRT::IAdapter
    {
    public:
        MockAdapter();
        virtual ~MockAdapter();

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

        // Adapter signals
        virtual property BridgeRT::IAdapterSignalVector^ Signals
        {
            BridgeRT::IAdapterSignalVector^ get() { return ref new BridgeRT::AdapterSignalVector(this->signals); }
        }

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

    private:

        uint32 createMockDevices();
        uint32 createSignals();

    private:
        Platform::String^ vendor;
        Platform::String^ adapterName;
        Platform::String^ version;

        //Adapter Configuration
        AdapterConfig adapterConfig;

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
    };

} // namespace AdapterLib