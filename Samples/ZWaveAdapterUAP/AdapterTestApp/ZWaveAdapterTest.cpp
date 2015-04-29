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
#include "ZWaveAdapterTest.h"

#include <string>
#include <fstream>

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace BridgeRT;
using namespace DsbCommon;
using namespace std;


namespace ZWaveAdapterTestUAP
{
    ZWaveAdapterTest::ZWaveAdapterTest(IAdapter^ adapter)
        : adapter(adapter)
        , testThread(this, &ZWaveAdapterTest::testThreadEntry)
    {
    }


    int32
    ZWaveAdapterTest::Initialize()
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
    ZWaveAdapterTest::Start()
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
                this->testId = TEST_ID::GET_CONFIG;
            }
        }

        return hr;
    }


    int32
    ZWaveAdapterTest::Shutdown()
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


    _Use_decl_annotations_
    void
    ZWaveAdapterTest::AdapterSignalHandler(
        IAdapterSignal^ Signal,
        Platform::Object^ Context
        )
    {
        UNREFERENCED_PARAMETER(Signal);
        UNREFERENCED_PARAMETER(Context);
    }


    int32
    ZWaveAdapterTest::registerAdapterSignalHandlers(bool IsRegister)
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
    ZWaveAdapterTest::registerDeviceSignalHandlers(bool IsRegister)
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
    ZWaveAdapterTest::testThreadEntry()
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
    ZWaveAdapterTest::doTest()
    {
        uint32 status = ERROR_SUCCESS;
        bool isTestDone = false;

        switch (testId)
        {
        case TEST_ID::IDLE:
            this->testId = TEST_ID::ENUM_DEVICES;
            break;

        case TEST_ID::ENUM_DEVICES:
            if (this->devices == nullptr)
            {
                status = this->adapter->EnumDevices(
                    ENUM_DEVICES_OPTIONS::CACHE_ONLY,
                    &this->devices,
                    nullptr
                    );
                if (status != ERROR_SUCCESS) break;

                status = this->registerDeviceSignalHandlers(true);
            }
            this->testId = TEST_ID::GET_PROPERTY;
            break;

        case TEST_ID::GET_PROPERTY:
            if (this->devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);

                status = this->adapter->GetProperty(adapterProperty, nullptr);
            }
            this->testId = TEST_ID::SET_PROPERTY;
            break;

        case TEST_ID::SET_PROPERTY:
            if (devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);

                status = this->adapter->SetProperty(adapterProperty, nullptr);
            }
            this->testId = TEST_ID::GET_PROPERTY_VALUE;
            break;

        case TEST_ID::GET_PROPERTY_VALUE:
            if (devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->adapter->GetPropertyValue(
                    adapterProperty,
                    L"value",
                    &adapterValue,
                    nullptr
                    );
            }
            this->testId = TEST_ID::SET_PROPERTY_VALUE;
            break;

        case TEST_ID::SET_PROPERTY_VALUE:
            if (devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterProperty^ adapterProperty = adapterDevice->Properties->GetAt(this->propertyIndex);
                IAdapterValue^ adapterValue;

                status = this->adapter->GetPropertyValue(
                    adapterProperty,
                    L"value",
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

                status = this->adapter->SetPropertyValue(
                    adapterProperty,
                    adapterValue,
                    nullptr
                    );
                if (status != ERROR_SUCCESS) break;

                this->propertyIndex = (this->propertyIndex + 1) % adapterDevice->Properties->Size;
                if (this->propertyIndex != 0)
                {
                    // Test next property
                    this->testId = TEST_ID::GET_PROPERTY;
                }
                else
                {
                    this->testId = TEST_ID::CALL_METHOD;
                }
            }
            break;

        case TEST_ID::CALL_METHOD:
            if (devices != nullptr)
            {
                IAdapterDevice^ adapterDevice = devices->GetAt(this->deviceIndex);
                IAdapterMethodVector^ methodVector = adapterDevice->Methods;
                if (methodVector != nullptr && methodVector->Size != 0)
                {
                    IAdapterMethod^ adapterMethod = methodVector->GetAt(this->methodIndex);
                    
                    status = this->adapter->CallMethod(adapterMethod, nullptr);
                    if (status != ERROR_SUCCESS) break;
                }

                this->methodIndex = (this->methodIndex + 1) % methodVector->Size;
                if (this->methodIndex == 0)
                {
                    this->deviceIndex = (this->deviceIndex + 1) % this->devices->Size;

                    isTestDone = this->deviceIndex == 0;

                    this->testId = TEST_ID::GET_PROPERTY;
                }
                else
                {
                    // Keep testing methods
                    this->testId = TEST_ID::CALL_METHOD;
                }
            }
            break;

        case TEST_ID::GET_CONFIG:
        {
            Array<byte>^ configData = nullptr;
            status = this->adapter->GetConfiguration(&configData);
            if (status == ERROR_SUCCESS)
            {
                String^ configXml = ref new String((wchar_t*)configData->Data);
                OutputDebugString(configXml->Data());

                this->testId = TEST_ID::SET_CONFIG;
            }
            break;
        }
        case TEST_ID::SET_CONFIG:
        {
            wstring buffer;
            wchar_t c;
            wifstream iFile{ L"options.xml" };

            while (iFile.get(c))          // loop getting single characters
                buffer += c;

            iFile.close();

            auto nLen = (buffer.size() + 1) * sizeof(wchar_t);
            Platform::Array<BYTE>^ data = ref new Platform::Array<BYTE>((BYTE *)buffer.data(), static_cast<unsigned int>(nLen));

            status = adapter->SetConfiguration(data);

            this->testId = TEST_ID::IDLE;
            break;
        }

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
        return isTestDone;
    }

} // namespace ZWaveAdapterTestUAP