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

#include "pch.h"
#include "StartupTask.h"

using namespace MockAdapterBackgroundService;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Background; 
using namespace Windows::System::Threading;

void StartupTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    deferral = taskInstance->GetDeferral();

    try
    {
        ThreadPool::RunAsync(ref new WorkItemHandler([&](IAsyncAction^ operation)
        {
            AdapterLib::MockAdapter^ adapter = ref new AdapterLib::MockAdapter();

            dsbBridge = ref new BridgeRT::DsbBridge(adapter);
            if (this->dsbBridge == nullptr)
            {
                throw ref new Exception(
                    HRESULT_FROM_WIN32(ERROR_NOT_READY),
                    "DSB Bridge not instantiated!");
            }

            HRESULT hr = this->dsbBridge->Initialize();
            if (FAILED(hr))
            {
                throw  ref new Exception(hr, "DSB Bridge initialization failed!");
            }
        }));
    }
    catch (Exception^ ex)
    {

    }

    //deferral->Complete();
   
}
