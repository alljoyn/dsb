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

#pragma once

namespace DsbCommon
{
    // Default thread start time in mSec
    static DWORD DEF_THREAD_START_TIMEOUT_MSEC = DWORD(1000);

    //
    // Class Thread 
    // Description:
    //  Thread interface class.
    //  Thread is a pure virtual class that needs to be inherited
    //  for thread code implementation.
    //      
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();

        DWORD	Start(DWORD WaitStartedMsec);
        DWORD	Stop(void);
        bool    IsRunning(void);
        bool    IsMe(void);
        HANDLE  GetStopEvent(void);
        DWORD   GetStartStatus(void);
        void    SetStartStatus(DWORD StartStatus);

    protected:
        //
        // ThreadStart() is the thread entry point, which
        // should be implemented within the derived class.
        //
        virtual DWORD ThreadStart() = 0;

    private:
        static DWORD __stdcall ThreadLaunch(LPVOID ContextPtr);

    private:
        HANDLE          startedEvent;
        HANDLE          stopEvent;
        DWORD           startStatus;
        volatile LONG   isRunning;
        DWORD	        threadId;
        HANDLE          threadHandle;
    };

    //
    // Template MemberThread 
    // Description:
    //  A template that enables to have a class member running in a different thread
    //      
    template <class T>
    class MemberThread : public Thread
    {
    public:
        typedef DWORD (T::* PTHREAD_START)();

        MemberThread()
            : instance(nullptr)
            , threadStartPtr(nullptr)
        {
        }

        MemberThread(_In_ T^ Instance, _In_ PTHREAD_START ThreadStartPtr)
            : instance(Instance)
            , threadStartPtr(ThreadStartPtr)
        {
        }

        virtual ~MemberThread()
        {
        }

        void Init(_In_ T^ Instance, _In_ PTHREAD_START ThreadStartPtr)
        { 
            this->instance = Instance;
            this->threadStartPtr = ThreadStartPtr; 
        }

        virtual DWORD ThreadStart()
        {
            if (this->instance == nullptr ||
                this->threadStartPtr == nullptr)
            {
                return ERROR_NOT_READY;
            }

            return (this->instance->*threadStartPtr)();
        }

    private:
        T^ instance;
        PTHREAD_START threadStartPtr; 
    };

} // DsbCommon
