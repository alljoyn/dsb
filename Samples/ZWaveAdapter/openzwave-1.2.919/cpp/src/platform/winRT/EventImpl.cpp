//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//	    EventImpl.cpp
//
//-----------------------------------------------------------------------------
#include <windows.h>

#include "Defs.h"
#include "EventImpl.h"

using namespace OpenZWave;


//-----------------------------------------------------------------------------
//	<EventImpl::EventImpl>
//	Constructor
//-----------------------------------------------------------------------------
EventImpl::EventImpl
(
)
{
	// Create a manual reset event
	m_hEvent = ::CreateEventEx( NULL, NULL, CREATE_EVENT_MANUAL_RESET, SYNCHRONIZE | EVENT_MODIFY_STATE );
}

//-----------------------------------------------------------------------------
//	<EventImpl::~EventImpl>
//	Destructor
//-----------------------------------------------------------------------------
EventImpl::~EventImpl
(
)
{
	::CloseHandle( m_hEvent );
}

//-----------------------------------------------------------------------------
//	<EventImpl::Set>
//	Set the event to signalled
//-----------------------------------------------------------------------------
void EventImpl::Set
(
)
{
	::SetEvent( m_hEvent );
}

//-----------------------------------------------------------------------------
//	<EventImpl::Reset>
//	Set the event to not signalled
//-----------------------------------------------------------------------------
void EventImpl::Reset
(
)
{
	::ResetEvent( m_hEvent );
}

//-----------------------------------------------------------------------------
//	<EventImpl::IsSignalled>
//	Test whether the event is set
//-----------------------------------------------------------------------------
bool EventImpl::IsSignalled
(
)
{
	return( WAIT_OBJECT_0 == WaitForSingleObjectEx( m_hEvent, 0, FALSE) );
}

//-----------------------------------------------------------------------------
//	<EventImpl::Wait>
//	Wait for the event to become signalled
//-----------------------------------------------------------------------------
bool EventImpl::Wait
(
	int32 const _timeout
)
{
	return( WAIT_TIMEOUT != ::WaitForSingleObjectEx( m_hEvent, (DWORD)_timeout, FALSE ) );
}
