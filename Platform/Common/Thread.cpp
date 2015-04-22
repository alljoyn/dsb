//
// Device System Bridge Common
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
#include "Thread.h"

namespace DsbCommon
{
    //
    // Class Thread 
    // Description:
    //  Thread interface class.
    //  Thread is a pure virtual class that needs to be inherited
    //  for thread code implementation.
    //      
    Thread::Thread()
        : isRunning(FALSE)
        , threadId(0)
        , threadHandle(NULL)
        , startStatus(ERROR_NOT_READY)
        , startedEvent(NULL)
        , stopEvent(NULL)

    {
    }

    Thread::~Thread()
    {
        (void)Thread::Stop();

        if (this->stopEvent != NULL)
        {
            ::CloseHandle(this->startedEvent);
        }

        if (this->stopEvent != NULL)
        {
            ::CloseHandle(this->stopEvent);
        }
    }

    DWORD
    __stdcall
    Thread::ThreadLaunch(LPVOID ContextPtr)
    {
        Thread* threadPtr = static_cast<Thread*>(ContextPtr);

        // Call thread object entry
        DWORD extitConde = threadPtr->ThreadStart();

        ::InterlockedExchange(&threadPtr->isRunning, FALSE);
    
        threadPtr->threadHandle = NULL;
        threadPtr->threadId = 0;

        return extitConde;
    }

    DWORD
    Thread::Start(DWORD WaitStartedMsec)
    {
        if (this->startedEvent == NULL)
        {
            this->startedEvent = ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);

            if (this->startedEvent == NULL)
            {
                return::GetLastError();
            }
        }

        if (this->stopEvent == NULL)
        {
            this->stopEvent = ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);

            if (this->stopEvent == NULL)
            {
                return::GetLastError();
            }
        }

        if (::InterlockedExchange(&this->isRunning, TRUE))
        {
            return ERROR_ALREADY_INITIALIZED;
        }

        this->threadHandle = ::CreateThread(
                                nullptr,
                                0,
                                &ThreadLaunch,
                                this,
                                0, 
                                &this->threadId
                                );

        if (this->threadHandle == NULL) 
        {
            ::InterlockedExchange(&this->isRunning, FALSE);

            return ::GetLastError();
        }

        if (WaitStartedMsec == 0)
        {
            return startStatus;
        }

        // Wait for thread to start, or for a stop request...
        HANDLE events[2] = {this->stopEvent, this->startedEvent};

        DWORD waitRes = ::WaitForMultipleObjectsEx(ARRAYSIZE(events), events, FALSE, WaitStartedMsec, FALSE);

        if (waitRes == (WAIT_OBJECT_0+1))
        {
            return this->startStatus;
        }

        return ::GetLastError();
    }

    DWORD
    Thread::Stop(void)
    {
        if (!::InterlockedExchange(&this->isRunning, FALSE))
        {
            if (this->threadHandle == NULL)
            {
                return ERROR_SUCCESS;
            }

            return ERROR_IO_PENDING;
        }

        ::SetEvent(this->stopEvent);

        //
        // If we are stopping the thread from within, 
        // we cannot wait here...
        //
        if (this->IsMe())
        {
            return ERROR_IO_PENDING;
        }

        (void)::WaitForSingleObjectEx(this->threadHandle, INFINITE, FALSE);

        this->threadHandle = NULL;
        this->threadId = 0;

        return ERROR_SUCCESS;
    }

    void
    Thread::SetStartStatus(DWORD StartStatus)
    {
        this->startStatus = StartStatus;

        SetEvent(this->startedEvent);
    }

    DWORD   
    Thread::GetStartStatus(void)
    {
        return this->startStatus;
    }

    bool    
    Thread::IsRunning(void)
    {
        return ::InterlockedExchangeAdd(&this->isRunning, 0) != FALSE;
    }

    bool    
    Thread::IsMe(void)
    {
        return ::GetCurrentThreadId() == this->threadId;
    }

    HANDLE  
    Thread::GetStopEvent(void)
    {
        return this->stopEvent;
    }

} // DsbCommon
