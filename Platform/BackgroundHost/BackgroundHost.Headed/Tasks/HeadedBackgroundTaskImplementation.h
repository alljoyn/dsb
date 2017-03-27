//
// 
// IN Copyright (c) %s, Microsoft Corporation
// IN 
// IN SPDX-License-Identifier: Apache-2.0 *
// IN 
// IN All rights reserved. This program and the accompanying materials are
// IN made available under the terms of the Apache License, Version 2.0
// IN which accompanies this distribution, and is available at
// IN http://www.apache.org/licenses/LICENSE-2.0
// IN 
// IN Permission to use, copy, modify, and/or distribute this software for
// IN any purpose with or without fee is hereby granted, provided that the
// IN above copyright notice and this permission notice appear in all
// IN copies.
// IN 
// IN THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// IN WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// IN WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// IN AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// IN DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// IN PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// IN TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// IN PERFORMANCE OF THIS SOFTWARE.
//

#pragma once

#include "pch.h"
#include "HeartbeatDuration.h"
#include "TaskProgress.h"

namespace BackgroundHost { namespace Headed { namespace Tasks
{
    using namespace ::Concurrency;
    using namespace ::Platform;
    using namespace ::BackgroundHost;
    using namespace ::Windows::ApplicationModel;
    using namespace ::Windows::ApplicationModel::Background;
    using namespace ::Windows::System::Threading;
    using namespace ::Windows::UI::Xaml;

    public ref class HeadedBackgroundTaskImplementation sealed
    {
    public:
        void Run(IBackgroundTaskInstance^ taskInstance, IServiceImplementation^ serviceImplementation)
        {
            if (!taskInstance)
                throw ref new InvalidArgumentException(L"taskInstance");
            if (!serviceImplementation)
                throw ref new InvalidArgumentException(L"serviceImplementation");

            _taskInstance = taskInstance;
            _serviceImplementation = serviceImplementation;
            _deferral = taskInstance->GetDeferral();

            _taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &HeadedBackgroundTaskImplementation::OnCanceled);

            try
            {
                create_task([=] 
                { 
                    try
                    {
                        _serviceImplementation->Start();
                        _heartbeatTimer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(this, &HeadedBackgroundTaskImplementation::TimerElapsed), HeartbeatDuration::Heartbeat);
                        _taskInstance->Progress = TaskProgress::Started;
                    }
                    catch (Exception^ ex)
                    {
                        // Shutdown and raise the Failed event
                        this->Shutdown(_taskInstance);
                        _serviceImplementation->RaiseFailedEvent(ex->Message);
                    }
                    catch (...)
                    {
                        // Shutdown and raise the Failed event
                        this->Shutdown(_taskInstance);
                        _serviceImplementation->RaiseFailedEvent("Unknown Exception");
                    }
                });
            }
            catch (...)
            {
                // Shutdown
                this->Shutdown(_taskInstance);
            }

            _onShutdown.wait(std::unique_lock<std::mutex>(_onShutdownMutex));
        }

    private:
        void Shutdown(IBackgroundTaskInstance^ taskInstance)
        {
            _serviceImplementation->Stop();
            _heartbeatTimer->Cancel();
            _taskInstance = nullptr;
            _onShutdown.notify_all();
            _deferral->Complete();
        }

        void OnCanceled(IBackgroundTaskInstance^ sender, BackgroundTaskCancellationReason reason)
        {
            this->Shutdown(sender);
        }

        void TimerElapsed(ThreadPoolTimer^ timer)
        {
            IBackgroundTaskInstance^ taskInstance = _taskInstance;

            if (taskInstance)
            {
                taskInstance->Progress = TaskProgress::Heartbeat;
            }
        }

    private:
        IServiceImplementation^ _serviceImplementation;
        ThreadPoolTimer^ _heartbeatTimer;
        IBackgroundTaskInstance^ _taskInstance;
        Agile<BackgroundTaskDeferral> _deferral;
        std::condition_variable _onShutdown;
        std::mutex _onShutdownMutex;
    };
}}}
