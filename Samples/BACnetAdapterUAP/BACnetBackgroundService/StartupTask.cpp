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
#include "StartupTask.h"

using namespace BACnetBackgroundService;
using namespace Platform;
using namespace Windows::ApplicationModel::Background; 
using namespace Windows::Foundation;
using namespace Windows::System::Threading;

void StartupTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    deferral = taskInstance->GetDeferral();

    try
    {
        AdapterLib::BACnetAdapter^ adapter = ref new AdapterLib::BACnetAdapter();

        dsbBridge = ref new BridgeRT::DsbBridge(adapter);
        if (this->dsbBridge == nullptr)
        {
            throw ref new Exception(HRESULT_FROM_WIN32(ERROR_NOT_READY), "DSB Bridge not instantiated!");
        }

        HRESULT hr = this->dsbBridge->Initialize();
        if (FAILED(hr))
        {
            throw  ref new Exception(hr, "DSB Bridge initialization failed!");
        }
    }
    catch (Exception^ ex)
    {
        deferral->Complete();
        throw ref new Exception(E_FAIL, "Exception occurred during the execution of the DSB Bridge!");
    }

}
