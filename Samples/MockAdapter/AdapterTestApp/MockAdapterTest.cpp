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
#include "AdapterDefinitions.h"
#include "MockAdapterTest.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace BridgeRT;
using namespace DsbCommon;


namespace MockAdapterTest
{
    MockAdapterTest::MockAdapterTest(IAdapter^ adapter)
        : m_adapter(adapter)
        , m_testThread(this, &MockAdapterTest::testThreadEntry)
    {
    }


    int32
    MockAdapterTest::Initialize()
    {
        int32 hr = S_OK;

        if (this->m_adapter == nullptr)
        {
            return E_NOT_VALID_STATE;
        }

        hr = this->m_adapter->Initialize();
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
    MockAdapterTest::Start()
    {
        HRESULT hr = S_OK;

        if (!this->m_testThread.IsRunning())
        {
            hr = HRESULT_FROM_WIN32(this->m_testThread.Start(1000));
            if (SUCCEEDED(hr))
            {
                this->m_deviceIndex = 0;
                this->m_propertyIndex = 0;
                this->m_methodIndex = 0;
                this->m_testId = TEST_ID::IDLE;
            }
        }

        return hr;
    }


    int32
    MockAdapterTest::Shutdown()
    {
        int32 hr = S_OK;

        this->m_testThread.Stop();

        if (this->m_adapter != nullptr)
        {
            (void)this->registerAdapterSignalHandlers(false);
            (void)this->registerDeviceSignalHandlers(false);

            hr = this->m_adapter->Shutdown();
        }

        return hr;
    }


    _Use_decl_annotations_
    void
    MockAdapterTest::AdapterSignalHandler(
        IAdapterSignal^ Signal,
        Platform::Object^ Context
        )
    {
        UNREFERENCED_PARAMETER(Signal);
        UNREFERENCED_PARAMETER(Context);
    }


    int32
    MockAdapterTest::registerAdapterSignalHandlers(bool IsRegister)
    {
        AutoLock sync(&this->m_lock, true);

        for (auto signal : this->m_adapter->Signals)
        {
            uint32 status;

            if (IsRegister)
            {
                status = this->m_adapter->RegisterSignalListener(signal, this, this->m_adapter);
            }
            else
            {
                status = this->m_adapter->UnregisterSignalListener(signal, this);
            }

            if (status != ERROR_SUCCESS)
            {
                return HRESULT_FROM_WIN32(status);
            }
        }

        return S_OK;
    }


    int32 
    MockAdapterTest::registerDeviceSignalHandlers(bool IsRegister)
    {
        if (this->m_devices == nullptr)
        {
            return S_OK;
        }

        for (auto device : this->m_devices)
        {
            for (auto signal : device->Signals)
            {
                uint32 status;

                if (IsRegister)
                {
                    status = this->m_adapter->RegisterSignalListener(signal, this, device);
                }
                else
                {
                    status = this->m_adapter->UnregisterSignalListener(signal, this);
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
    MockAdapterTest::testThreadEntry()
    {
        DWORD status = ERROR_SUCCESS;

        this->m_testThread.SetStartStatus(ERROR_SUCCESS);

        //
        // Wake up every TEST_THREAD_WAKEUP_MSEC and perform a test, until we shut down.
        //

        while (::WaitForSingleObjectEx(this->m_testThread.GetStopEvent(), TEST_THREAD_WAKEUP_MSEC, FALSE) == WAIT_TIMEOUT)
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
    MockAdapterTest::doTest()
    {
        uint32 status = ERROR_SUCCESS;
        bool isTestDone = false;

        switch (m_testId)
        {
        case TEST_ID::IDLE:
            break;

        case TEST_ID::ENUM_DEVICES:
            if (this->m_devices == nullptr)
            {
                status = this->m_adapter->EnumDevices(
                    ENUM_DEVICES_OPTIONS::CACHE_ONLY,
                    &this->m_devices,
                    nullptr
                    );
                if (status != ERROR_SUCCESS) break;

                status = this->registerDeviceSignalHandlers(true);
            }
            break;

        case TEST_ID::GET_PROPERTY:
            if (this->m_devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = m_devices->GetAt(this->m_deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->m_propertyIndex);

                status = this->m_adapter->GetProperty(adapterProperty, nullptr);
            }
            break;

        case TEST_ID::SET_PROPERTY:
            if (m_devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = m_devices->GetAt(this->m_deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->m_propertyIndex);

                status = this->m_adapter->SetProperty(adapterProperty, nullptr);
            }
            break;

        case TEST_ID::GET_PROPERTY_VALUE:
            if (m_devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = m_devices->GetAt(this->m_deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->m_propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->m_adapter->GetPropertyValue(
                    adapterProperty,
                    L"Present_Value",
                    &adapterValue,
                    nullptr
                    );
            }
            break;

        case TEST_ID::SET_PROPERTY_VALUE:
            if (m_devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = m_devices->GetAt(this->m_deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->m_propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->m_adapter->GetPropertyValue(
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
                    adapterValue->Data = PropertyValue::CreateBoolean(!ipv->GetBoolean());
                    break;
                case PropertyType::Double:
                    adapterValue->Data = PropertyValue::CreateDouble(ipv->GetDouble() + 0.5);
                    break;
                case PropertyType::UInt32:
                    adapterValue->Data = PropertyValue::CreateUInt32(ipv->GetUInt32() + 1);
                    break;
                default:
                    break;
                }

                status = this->m_adapter->SetPropertyValue(
                    adapterProperty,
                    adapterValue,
                    nullptr
                    );
                if (status != ERROR_SUCCESS) break;

                this->m_propertyIndex = (this->m_propertyIndex + 1) % adapterDevice->Properties->Size;
                if (this->m_propertyIndex != 0)
                {
                    // Test next property
                    this->m_testId = TEST_ID::GET_PROPERTY;
                }
            }
            break;

        case TEST_ID::CALL_METHOD:
            if (m_devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = m_devices->GetAt(this->m_deviceIndex);
                IAdapterMethodVector^ methodVector = adapterDevice->Methods;
                if (methodVector != nullptr && methodVector->Size != 0)
                {
                    IAdapterMethod^ adapterMethod = methodVector->GetAt(this->m_methodIndex);
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

                    status = this->m_adapter->CallMethod(adapterMethod, nullptr);
                    if (status != ERROR_SUCCESS) break;
                }

                this->m_methodIndex = (this->m_methodIndex + 1) % methodVector->Size;
                if (this->m_methodIndex == 0)
                {
                    this->m_deviceIndex = (this->m_deviceIndex + 1) % this->m_devices->Size;

                    isTestDone = this->m_deviceIndex == 0;
                }
                else
                {
                    // Keep testing methods
                    --this->m_testId;
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
        this->m_testId = (this->m_testId + 1) % TEST_ID::MAX;

        return isTestDone;
    }

} // namespace MockAdapterTest