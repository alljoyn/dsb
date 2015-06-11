//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      WaitImpl.cpp
//
//-----------------------------------------------------------------------------
#include "Defs.h"
#include "platform/Wait.h"
#include "WaitImpl.h"
#include "platform/Log.h"

using namespace OpenZWave;

//-----------------------------------------------------------------------------
//	<WaitImpl::WaitImpl>
//	Constructor
//-----------------------------------------------------------------------------
WaitImpl::WaitImpl
(	
	Wait* _owner
):
	m_owner( _owner )
{
	InitializeCriticalSectionEx( &m_criticalSection, 0, 0 );
}

//-----------------------------------------------------------------------------
//	<WaitImpl::~WaitImpl>
//	Destructor
//-----------------------------------------------------------------------------
WaitImpl::~WaitImpl
(
)
{
	DeleteCriticalSection( &m_criticalSection );
}

//-----------------------------------------------------------------------------
//	<WaitImpl::AddWatcher>
//	Add a watcher to our object.
//-----------------------------------------------------------------------------
void WaitImpl::AddWatcher
( 
	Wait::pfnWaitNotification_t _callback, 
	void* _context
)
{
	// Add the watcher to our list
	Watcher watcher;
	watcher.m_callback = _callback;
	watcher.m_context = _context;
	
	EnterCriticalSection( &m_criticalSection );

	m_watchers.push_back( watcher );

	LeaveCriticalSection( &m_criticalSection );

	// If the object is already in a signalled state, notify the watcher immediately
	if( m_owner->IsSignalled() )
	{
		_callback( _context );
	}

}

//-----------------------------------------------------------------------------
//	<WaitImpl::RemoveWatcher>
//	Remove a watcher from our object.
//-----------------------------------------------------------------------------
bool WaitImpl::RemoveWatcher
( 
	Wait::pfnWaitNotification_t _callback, 
	void* _context
)
{
	bool res = false;
	EnterCriticalSection( &m_criticalSection );

	for( list<Watcher>::iterator it=m_watchers.begin(); it!=m_watchers.end(); ++it )
	{
		Watcher const& watcher = *it;
		if( ( watcher.m_callback == _callback ) && ( watcher.m_context == _context ) )
		{
			m_watchers.erase( it );
			res = true;
			break;
		}
	}

	LeaveCriticalSection( &m_criticalSection );
	return res;
}

//-----------------------------------------------------------------------------
//	<WaitImpl::Notify>
//	Notify all the watchers that the object has become signalled
//-----------------------------------------------------------------------------
void WaitImpl::Notify
(
)
{
	EnterCriticalSection( &m_criticalSection );

	for( list<Watcher>::iterator it=m_watchers.begin(); it!=m_watchers.end(); ++it )
	{
		Watcher const& watcher = *it;
		watcher.m_callback( watcher.m_context );
	}

	LeaveCriticalSection( &m_criticalSection );
}
