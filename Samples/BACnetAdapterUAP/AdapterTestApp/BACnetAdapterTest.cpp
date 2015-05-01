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
#include "BACnetAdapterTest.h"


using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace BridgeRT;
using namespace DsbCommon;


namespace AdapterTestApp
{
    BACnetAdapterTest::BACnetAdapterTest(IAdapter^ adapter)
        : adapter(adapter)
        , testThread(this, &BACnetAdapterTest::testThreadEntry)
    {
    }


    int32
    BACnetAdapterTest::Initialize()
    {
        int32 hr = S_OK;

        if (this->adapter == nullptr)
        {
            return E_NOT_VALID_STATE;
        }

        hr = this->adapter->Initialize();
        if (FAILED(hr))
        {
            return hr;
        }

        hr = this->registerAdapterSignalHandlers(true);
        if (FAILED(hr))
        {
            return hr;
        }

        //
        // Other objects initialization...
        //

        return hr;
    }

    int32 
    BACnetAdapterTest::Start()
    {
        HRESULT hr = S_OK;

        if (!this->testThread.IsRunning())
        {
            hr = HRESULT_FROM_WIN32(this->testThread.Start(1000));
            if (SUCCEEDED(hr))
            {
                this->deviceIndex = 0;
                this->propertyIndex = 0;
                this->methodIndex = 0;
                this->testId = TEST_ID::IDLE;
            }
        }

        return hr;
    }


    int32
    BACnetAdapterTest::Shutdown()
    {
        int32 hr = S_OK;

        this->testThread.Stop();

        if (this->adapter != nullptr)
        {
            (void)this->registerAdapterSignalHandlers(false);
            (void)this->registerDeviceSignalHandlers(false);

            hr = this->adapter->Shutdown();
        }

        return hr;
    }

    size_t
    BACnetAdapterTest::DeviceCount()
    {
        AutoLock sync(&this->lock, true);

        if (this->devices == nullptr)
        {
            return 0;
        }

        return this->devices->Size;
    }



    _Use_decl_annotations_
    void
    BACnetAdapterTest::AdapterSignalHandler(
        IAdapterSignal^ Signal,
        Platform::Object^ Context
        )
    {
        if (Signal->Name == DEVICE_ARRIVAL_SIGNAL)
        {
            size_t deviceCount = this->devices == nullptr ? 0 : this->devices->Size;

            // Get the updated device list from the adapter cache
            uint32 status = this->adapter->EnumDevices(
                                                ENUM_DEVICES_OPTIONS::CACHE_ONLY,
                                                &this->devices,
                                                nullptr
                                                );
            if ((status == ERROR_SUCCESS) && (deviceCount != this->devices->Size))
            {
                this->registerDeviceSignalHandlers(true);

                this->Start();
            }
        }
        else
        {
            //
            // Device level signals....
            //
            IAdapterDevice^ device = dynamic_cast<IAdapterDevice^>(Context);
            if (device == nullptr)
            {
                DSB_ASSERT(FALSE);
            }
        }
    }


    int32
    BACnetAdapterTest::registerAdapterSignalHandlers(bool IsRegister)
    {
        AutoLock sync(&this->lock, true);

        for (auto signal : this->adapter->Signals)
        {
            uint32 status;

            if (IsRegister)
            {
                status = this->adapter->RegisterSignalListener(signal, this, this->adapter);
            }
            else
            {
                status = this->adapter->UnregisterSignalListener(signal, this);
            }

            if (status != ERROR_SUCCESS)
            {
                return HRESULT_FROM_WIN32(status);
            }
        }

        return S_OK;
    }


    int32 
    BACnetAdapterTest::registerDeviceSignalHandlers(bool IsRegister)
    {
        if (this->devices == nullptr)
        {
            return S_OK;
        }

        for (auto device : this->devices)
        {
            for (auto signal : device->Signals)
            {
                uint32 status;

                if (IsRegister)
                {
                    status = this->adapter->RegisterSignalListener(signal, this, device);
                }
                else
                {
                    status = this->adapter->UnregisterSignalListener(signal, this);
                }

                if (status == ERROR_ALREADY_EXISTS)
                {
                    status = ERROR_SUCCESS;
                }

                if (status != ERROR_SUCCESS)
                {
                    return HRESULT_FROM_WIN32(status);
                }

            } // More signals

        } // More devices

        return S_OK;
    }


    #define TEST_THREAD_WAKEUP_MSEC DWORD(100)

    DWORD
    BACnetAdapterTest::testThreadEntry()
    {
        DWORD status = ERROR_SUCCESS;

        this->testThread.SetStartStatus(ERROR_SUCCESS);

        //
        // Wake up every TEST_THREAD_WAKEUP_MSEC and perform a test, until we shut down.
        //
        while (::WaitForSingleObjectEx(this->testThread.GetStopEvent(), TEST_THREAD_WAKEUP_MSEC, FALSE) == WAIT_TIMEOUT)
        {
            try
            {
                if (this->doTest())
                {
                    // Testing is done!
                    break;
                }
            }
            catch (Exception^ e)
            {
                status = WIN32_FROM_HRESULT(e->HResult);
                break;
            }
        }

        return status;
    }


    bool
    BACnetAdapterTest::doTest()
    {
        uint32 status = ERROR_SUCCESS;
        bool isTestDone = false;

        switch (testId)
        {
        case TEST_ID::IDLE:
            break;

        case TEST_ID::ENUM_DEVICES:
            if (this->devices == nullptr)
            {
                status = this->adapter->EnumDevices(
                                            ENUM_DEVICES_OPTIONS::FORCE_REFRESH,
                                            &this->devices,
                                            nullptr
                                            );
            }
            break;

        case TEST_ID::GET_PROPERTY:
            if (this->DeviceCount() != 0)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                if (adapterDevice->Properties->Size == 0)
                {
                    // Device has no properties, skip property testing
                    this->testId = TEST_ID::CALL_METHOD - 1;
                    break;
                }

                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);

                status = this->adapter->GetProperty(adapterProperty, nullptr);
            }
            break;

        case TEST_ID::SET_PROPERTY:
            if (this->DeviceCount() != 0)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                if (adapterDevice->Properties->Size == 0)
                {
                    // Device has no properties, skip property testing
                    this->testId = TEST_ID::CALL_METHOD - 1;
                    break;
                }

                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);

                status = this->adapter->SetProperty(adapterProperty, nullptr);
                if (status != ERROR_SUCCESS)
                {
                    //
                    // ERROR_ACCESS_DENIED is a benign,
                    // means the property is 'read only'
                    //
                    if (status == ERROR_ACCESS_DENIED)
                    {
                        status = ERROR_SUCCESS;
                    }
                }
            }
            break;

        case TEST_ID::GET_PROPERTY_VALUE:
            if (this->DeviceCount() != 0)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                if (adapterDevice->Properties->Size == 0)
                {
                    // Device has no properties, skip property testing
                    this->testId = TEST_ID::CALL_METHOD - 1;
                    break;
                }

                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->adapter->GetPropertyValue(
                    adapterProperty,
                    L"Present_Value",
                    &adapterValue,
                    nullptr
                    );
            }
            break;

        case TEST_ID::SET_PROPERTY_VALUE:
            if (this->DeviceCount() != 0)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                if (adapterDevice->Properties->Size == 0)
                {
                    // Device has no properties, skip property testing
                    this->testId = TEST_ID::CALL_METHOD - 1;
                    break;
                }

                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->adapter->GetPropertyValue(
                    adapterProperty,
                    L"Present_Value",
                    &adapterValue,
                    nullptr
                    );
                if (status != ERROR_SUCCESS) break;

                IPropertyValue^ ipv = dynamic_cast<IPropertyValue^>(adapterValue->Data);
                if (ipv == nullptr) break;

                //
                // Change something...
                //
                switch (ipv->Type)
                {
                case PropertyType::Boolean:
                {
                    bool curValue = ipv->GetBoolean();
                    adapterValue->Data = PropertyValue::CreateBoolean(!curValue);
                    break;
                }
                case PropertyType::Double:
                {
                    double curValue = ipv->GetDouble();
                    adapterValue->Data = PropertyValue::CreateDouble(curValue + 0.5);
                    break;
                }
                case PropertyType::UInt32:
                {
                    // For testing handle as a boolean value
                    uint32 curValue = ipv->GetUInt32();
                    adapterValue->Data = PropertyValue::CreateUInt32((curValue + 1)%2);
                    break;
                }
                default:
                    break;
                }

                status = this->adapter->SetPropertyValue(
                    adapterProperty,
                    adapterValue,
                    nullptr
                    );
                if (status != ERROR_SUCCESS)
                {
                    //
                    // ERROR_ACCESS_DENIED is a benign,
                    // means the property is 'read only'
                    //
                    if (status == ERROR_ACCESS_DENIED)
                    {
                        status = ERROR_SUCCESS;
                    }
                }
                else
                {
                    // Just for setting a breakpoint
                    DSB_ASSERT(this->DeviceCount() != 0);
                }
                if (status != ERROR_SUCCESS) break;

                this->propertyIndex = (this->propertyIndex + 1) % adapterDevice->Properties->Size;
                if (this->propertyIndex != 0)
                {
                    // Test next property
                    this->testId = TEST_ID::GET_PROPERTY;
                }
            }
            break;

        case TEST_ID::CALL_METHOD:
            if (this->DeviceCount() != 0)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterMethodVector^ methodVector = adapterDevice->Methods;
                if ((methodVector != nullptr) && (methodVector->Size != 0))
                {
                    IAdapterMethod^ adapterMethod = methodVector->GetAt(this->methodIndex);
                    if (adapterMethod->Name == L"Device_Reset")
                    {
                        DSB_ASSERT(adapterMethod->InputParams->Size == 1);

                        //
                        // Set the input parameter, the target property.
                        // For doing a device level reset do:
                        // adapterMethod->InputParams->GetAt(0)->Data = nullptr;
                        //
                        adapterMethod->InputParams->GetAt(0)->Data = adapterDevice->Properties->GetAt(0);
                    }

                    status = this->adapter->CallMethod(adapterMethod, nullptr);
                    if (status != ERROR_SUCCESS) break;

                    this->methodIndex = (this->methodIndex + 1) % methodVector->Size;
                }

                if (this->methodIndex == 0)
                {
                    this->deviceIndex = (this->deviceIndex + 1) % this->devices->Size;

                    isTestDone = this->deviceIndex == 0;
                }
                else
                {
                    // Keep testing methods
                    --this->testId;
                }
            }
            break;
        }

        if (status != ERROR_SUCCESS)
        {
            //
            // Log the failure, and leave...
            //
            // Log(....)

            throw ref new Exception(HRESULT_FROM_WIN32(status));
        }

        // Do next test...
        this->testId = (this->testId + 1) % TEST_ID::MAX;

        return isTestDone;
    }

} // namespace App1