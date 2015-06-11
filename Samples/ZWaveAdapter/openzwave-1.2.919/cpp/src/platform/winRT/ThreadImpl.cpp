//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      ThreadImpl.cpp
//
//-----------------------------------------------------------------------------
#include "Defs.h"
#include "platform/Event.h"
#include "platform/Thread.h"
#include "ThreadImpl.h"
#include "Options.h"

using namespace OpenZWave;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;

int32 ThreadImpl::s_threadTerminateTimeout = -1;

//-----------------------------------------------------------------------------
//	<ThreadImpl::ThreadImpl>
//	Constructor
//-----------------------------------------------------------------------------
ThreadImpl::ThreadImpl
(
	Thread* _owner,
	string const& _name
):
	m_owner( _owner ),
	m_bIsRunning( false ),
	m_name( _name )
{
	static bool staticsInitialized = false;
	if (!staticsInitialized)
	{
		if (Options::Get() != nullptr)
		{
			Options::Get()->GetOptionAsInt("ThreadTerminateTimeout", &s_threadTerminateTimeout);
		}
		staticsInitialized = true;
	}	
}

//-----------------------------------------------------------------------------
//	<ThreadImpl::~ThreadImpl>
//	Destructor
//-----------------------------------------------------------------------------
ThreadImpl::~ThreadImpl ()
{
}

//-----------------------------------------------------------------------------
//	<ThreadImpl::Start>
//	Start a function running on this thread
//-----------------------------------------------------------------------------
bool ThreadImpl::Start
(
	Thread::pfnThreadProc_t _pfnThreadProc,
	Event* _exitEvent,
	void* _context 
)
{
	// Create a thread to run the specified function
	m_pfnThreadProc = _pfnThreadProc;
	m_context = _context;
	m_exitEvent = _exitEvent;
	m_exitEvent->Reset();
    
    create_task([this]()
    {
        m_bIsRunning = true;
        try
        {
            m_pfnThreadProc(m_exitEvent, m_context);
        }
        catch (Platform::Exception^)
        {
        }

        m_bIsRunning = false;
        // Let any watchers know that the thread has finished running.
        m_owner->Notify();
    });
	return true;
}

//-----------------------------------------------------------------------------
//	<ThreadImpl::Sleep>
//	Cause thread to sleep for the specified number of milliseconds
//-----------------------------------------------------------------------------
void ThreadImpl::Sleep
(
	uint32 _millisecs
)
{
	::Sleep(_millisecs);
}

//-----------------------------------------------------------------------------
//	<ThreadImpl::Terminate>
//	Force the thread to stop
//-----------------------------------------------------------------------------
bool ThreadImpl::Terminate
(
)
{
	// No way to do this on WinRT, so give the thread a bit of extra time to exit on its own
	if( !m_bIsRunning )
	{
		return false;
	}

	if (Wait::Single(m_owner, s_threadTerminateTimeout) < 0)
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//	<ThreadImpl::IsSignalled>
//	Test whether the thread has completed
//-----------------------------------------------------------------------------
bool ThreadImpl::IsSignalled()
{
	return !m_bIsRunning;
}