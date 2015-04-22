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

#include "Misc.h"
#include "Thread.h"
#include "AdapterDefinitions.h"

namespace MockAdapterTestUAP
{
    public ref class MockAdapterTest sealed : BridgeRT::IAdapterSignalListener
    {
    public:
        MockAdapterTest(BridgeRT::IAdapter^ adapter);

        int32 Initialize();

        int32 Start();

        int32 Shutdown();

        //
        // IAdapterSignalListener implementation
        //
        virtual void AdapterSignalHandler(
            _In_ BridgeRT::IAdapterSignal^ Signal,
            _In_opt_ Platform::Object^ Context
            );

    private:

        // Test ID
        enum TEST_ID : uint32
        {
            IDLE,
            ENUM_DEVICES,
            GET_PROPERTY,
            SET_PROPERTY,
            GET_PROPERTY_VALUE,
            SET_PROPERTY_VALUE,
            CALL_METHOD,

            MAX
        };

        int32 registerAdapterSignalHandlers(bool IsRegister);
        int32 registerDeviceSignalHandlers(bool IsRegister);

        DWORD testThreadEntry();

        bool doTest();

    private:

        // Sync object
        DsbCommon::CSLock m_lock;

        // Test runtime vars
        BridgeRT::IAdapterDeviceVector^ m_devices;
        uint32 m_deviceIndex;
        uint32 m_propertyIndex;
        uint32 m_methodIndex;
        uint32 m_testId;

        // Thread object
        DsbCommon::MemberThread<MockAdapterTest> m_testThread;

        // The adapter interface
        BridgeRT::IAdapter^ m_adapter;
    };

}  // namespace MockAdapterTestUAP
