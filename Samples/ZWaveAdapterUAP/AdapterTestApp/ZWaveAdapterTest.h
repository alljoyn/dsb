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

#include "Misc.h"
#include "Thread.h"

namespace ZWaveAdapterTestUAP
{
    public ref class ZWaveAdapterTest sealed : BridgeRT::IAdapterSignalListener
    {
    public:
        ZWaveAdapterTest(BridgeRT::IAdapter^ adapter);

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
            GET_CONFIG,
            SET_CONFIG,

            MAX
        };

        int32 registerAdapterSignalHandlers(bool IsRegister);
        int32 registerDeviceSignalHandlers(bool IsRegister);

        DWORD testThreadEntry();

        bool doTest();

    private:

        // Sync object
        DsbCommon::CSLock lock;

        // Test runtime vars
        BridgeRT::IAdapterDeviceVector^ devices;
        uint32 deviceIndex;
        uint32 propertyIndex;
        uint32 methodIndex;
        uint32 testId;

        // Thread object
        DsbCommon::MemberThread<ZWaveAdapterTest> testThread;

        // The adapter interface
        BridgeRT::IAdapter^ adapter;
    };

}  // namespace ZWaveAdapterTestUAP
