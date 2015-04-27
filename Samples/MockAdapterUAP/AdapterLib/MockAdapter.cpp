//
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

#include "pch.h"
#include "MockAdapter.h"
#include "MockAdapterDevice.h"
#include "MockDevices.h"
#include "BfiDefinitions.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;

using namespace BridgeRT;
using namespace DsbCommon;

namespace AdapterLib
{

    MockAdapter::MockAdapter()
    {
        this->vendor = L"Microsoft";
        this->adapterName = L"DSB Mock Adapter";
        this->version = L"1.0.0.1";
    }


    MockAdapter::~MockAdapter()
    {
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::SetConfiguration(const Platform::Array<byte>^ ConfigurationData)
    {
        uint32 status = ERROR_SUCCESS;

        try
        {
            String^ configurationXml = ref new String((wchar_t*)ConfigurationData->Data);
            status = WIN32_FROM_HRESULT(adapterConfig.SetConfig(configurationXml));
        }
        catch (Platform::OutOfMemoryException^ ex)
        {
            status = WIN32_FROM_HRESULT(ex->HResult);
        }

        return status;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::GetConfiguration(Platform::Array<byte>^* ConfigurationDataPtr)
    {
        uint32 status = ERROR_SUCCESS;
        String^ configurationXml = nullptr;

        status = WIN32_FROM_HRESULT(adapterConfig.GetConfig(&configurationXml));
        if (status != ERROR_SUCCESS)
        {
            goto done;
        }

        status = StringToArray(configurationXml, ConfigurationDataPtr);

    done:
        return status;
    }


    uint32
    MockAdapter::Initialize()
    {
        uint32 status = WIN32_FROM_HRESULT(adapterConfig.Init());
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        status = this->createMockDevices();
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        return this->createSignals();
    }


    uint32
    MockAdapter::Shutdown()
    {
        return ERROR_SUCCESS;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::EnumDevices(
        ENUM_DEVICES_OPTIONS Options,
        IAdapterDeviceVector^* DeviceListPtr,
        IAdapterIoRequest^* RequestPtr
        )
    {
        UNREFERENCED_PARAMETER(Options);

        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        if (DeviceListPtr == nullptr)
        {
            return ERROR_INVALID_PARAMETER;
        }

        *DeviceListPtr = ref new BridgeRT::AdapterDeviceVector(this->devices);

        return ERROR_SUCCESS;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::GetProperty(
        IAdapterProperty^ Property,
        IAdapterIoRequest^* RequestPtr
        )
    {
        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        //
        // Mock adapter GetProperty() does not do much, it only 
        // validates the given property.
        // In a real adapter, all property's attributes are re-read
        // from the device.
        //

        MockAdapterProperty^ mockAdapterProperty = dynamic_cast<MockAdapterProperty^>(Property);
        if (mockAdapterProperty == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        MockAdapterDevice^ mockDevice = dynamic_cast<MockAdapterDevice^>(mockAdapterProperty->Parent);
        if (mockDevice == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        MockAdapter^ mockAdapter = dynamic_cast<MockAdapter^>(mockDevice->Parent);
        if (mockDevice == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        if (mockAdapter != this)
        {
            return ERROR_INVALID_HANDLE;
        }

        //
        // At this point a real adapter will re-read all property's attributes 
        // from the device.
        //

        return ERROR_SUCCESS;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::SetProperty(
        IAdapterProperty^ Property,
        IAdapterIoRequest^* RequestPtr
        )
    {
        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        //
        // Mock adapter SetProperty() does not do much, it only 
        // validates the given property.
        // In a real adapter, all property's attributes are written
        // to the device.
        //

        MockAdapterProperty^ mockAdapterProperty = dynamic_cast<MockAdapterProperty^>(Property);
        if (mockAdapterProperty == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        uint32 status = mockAdapterProperty->CheckAccess(AdapterPropertyAccessRW);
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        MockAdapterDevice^ mockDevice = dynamic_cast<MockAdapterDevice^>(mockAdapterProperty->Parent);
        if (mockDevice == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        MockAdapter^ mockAdapter = dynamic_cast<MockAdapter^>(mockDevice->Parent);
        if (mockDevice == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        if (mockAdapter != this)
        {
            return ERROR_INVALID_HANDLE;
        }

        //
        // At this point a real adapter will write all property's attributes 
        // to the device.
        //

        return ERROR_SUCCESS;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::GetPropertyValue(
        IAdapterProperty^ Property,
        String^ AttributeName,
        IAdapterValue^* ValuePtr,
        IAdapterIoRequest^* RequestPtr
        )
    {
        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        MockAdapterProperty^ mockAdapterProperty = dynamic_cast<MockAdapterProperty^>(Property);
        if (mockAdapterProperty == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        MockAdapterValue^ attribute = mockAdapterProperty->GetAttributeByName(AttributeName);
        if (attribute == nullptr)
        {
            return ERROR_NOT_FOUND;
        }

        try
        {
            *ValuePtr = ref new MockAdapterValue(attribute);
        }
        catch (OutOfMemoryException^)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        return ERROR_SUCCESS;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::SetPropertyValue(
        IAdapterProperty^ Property,
        IAdapterValue^ Value,
        IAdapterIoRequest^* RequestPtr
        )
    {
        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        MockAdapterProperty^ mockAdapterProperty = dynamic_cast<MockAdapterProperty^>(Property);
        if (mockAdapterProperty == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        uint32 status = mockAdapterProperty->CheckAccess(AdapterPropertyAccessRW);
        if (status != ERROR_SUCCESS)
        {
            return status;
        }

        MockAdapterValue^ attribute = mockAdapterProperty->GetAttributeByName(Value->Name);
        if (attribute == nullptr)
        {
            return ERROR_NOT_FOUND;
        }

        return attribute->Set(Value);
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::CallMethod(
        IAdapterMethod^ Method,
        IAdapterIoRequest^* RequestPtr
        )
    {
        if (RequestPtr != nullptr)
        {
            *RequestPtr = nullptr;
        }

        MockAdapterMethod^ mockMethod = dynamic_cast<MockAdapterMethod^>(Method);
        if (mockMethod == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        MockAdapterDevice^ mockDevice = dynamic_cast<MockAdapterDevice^>(mockMethod->Parent);
        if (mockDevice == nullptr)
        {
            return ERROR_INVALID_HANDLE;
        }

        return mockDevice->MethodDispatch(Method, RequestPtr);
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::RegisterSignalListener(
        IAdapterSignal^ Signal,
        IAdapterSignalListener^ Listener,
        Object^ ListenerContext
        )
    {
        uint32 status = ERROR_SUCCESS;

        try
        {
            // We use the hash code as the signal key
            int mmapkey = Signal->GetHashCode();

            // Sync access to listeners list
            AutoLock sync(&this->lock, true);

            // check if the listener is already registered for the signal
            auto handlers = this->signalListeners.equal_range(mmapkey);

            for (auto iter = handlers.first; iter != handlers.second; ++iter)
            {
                if (iter->second.Listener == Listener)
                {
                    status = ERROR_ALREADY_EXISTS;
                    goto done;
                }
            }

            // add it to the map. 
            this->signalListeners.insert(
            { mmapkey, SIGNAL_LISTENER_ENTRY(Signal, Listener, ListenerContext) }
            );
        }
        catch (OutOfMemoryException^)
        {
            status = ERROR_NOT_ENOUGH_MEMORY;
        }

    done:

        return status;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::UnregisterSignalListener(
        IAdapterSignal^ Signal,
        IAdapterSignalListener^ Listener
        )
    {
        uint32 status = ERROR_NOT_FOUND;

        // We use the hash code as the signal key
        int mmapkey = Signal->GetHashCode();

        // Sync access to listeners list
        AutoLock sync(&this->lock, true);

        // get all the listeners for the SignalHandle
        auto handlers = this->signalListeners.equal_range(mmapkey);

        for (auto iter = handlers.first; iter != handlers.second; ++iter)
        {
            if (iter->second.Listener == Listener)
            {
                // found it remove it
                this->signalListeners.erase(iter);

                status = ERROR_SUCCESS;
                break;
            }
        }

        return status;
    }


    _Use_decl_annotations_
    uint32
    MockAdapter::NotifySignalListener(
        IAdapterSignal^ Signal
        )
    {
        uint32 status = ERROR_INVALID_HANDLE;

        // We use the hash code as the signal key
        int mmapkey = Signal->GetHashCode();

        // Sync access to listeners list
        AutoLock sync(&this->lock, true);

        //search for the listeners
        auto handlers = signalListeners.equal_range(mmapkey);

        for (auto iter = handlers.first; iter != handlers.second; ++iter)
        {
            //notify the listener
            IAdapterSignalListener^ listener = iter->second.Listener;
            Object^ listenerContex = iter->second.Context;

            listener->AdapterSignalHandler(Signal, listenerContex);

            status = ERROR_SUCCESS;
        }

        return status;
    }


    uint32
    MockAdapter::createMockDevices()
    {
        uint32  status = ERROR_SUCCESS;

        for (ULONG devDescInx = 0; devDescInx < MockDevicesCount; ++devDescInx)
        {
            const MOCK_DEVICE_DESCRIPTOR* mockDeviceDescPtr = &MockDevices[devDescInx];

            try
            {
                MockAdapterDevice^ newDevice = ref new MockAdapterDevice(mockDeviceDescPtr, this);

                this->devices.push_back(std::move(newDevice));
            }
            catch (OutOfMemoryException^)
            {
                status = ERROR_NOT_ENOUGH_MEMORY;
            }
        }

        return status;
    }


    uint32
    MockAdapter::createSignals()
    {
        uint32  status = ERROR_SUCCESS;

        try
        {
            // Device arrival signal
            {
                MockAdapterSignal^ signal = ref new MockAdapterSignal(DEVICE_ARRIVAL_SIGNAL, this);

                //
                // Signal parameters
                // 
                signal += ref new MockAdapterValue(
                                    DEVICE_ARRIVAL__DEVICE_HANDLE, 
                                    signal, 
                                    ref new MockAdapterDevice(L"DsbDevice", this) // For signature spec
                                    );

                this->signals.push_back(std::move(signal));
            }
        }
        catch (OutOfMemoryException^)
        {
            status = ERROR_NOT_ENOUGH_MEMORY;
        }

        return status;
    }
} // namespace AdapterLib