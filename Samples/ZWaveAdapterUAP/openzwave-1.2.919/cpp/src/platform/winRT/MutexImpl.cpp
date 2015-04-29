//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      MutexImpl.cpp
//
//-----------------------------------------------------------------------------
#include "Defs.h"
#include "MutexImpl.h"


using namespace OpenZWave;

//-----------------------------------------------------------------------------
//	<MutexImpl::MutexImpl>
//	Constructor
//-----------------------------------------------------------------------------
MutexImpl::MutexImpl
(
):
	m_lockCount( 0 )
{
	InitializeCriticalSectionEx( &m_criticalSection, 0, 0 );
}

//-----------------------------------------------------------------------------
//	<MutexImpl::~MutexImpl>
//	Destructor
//-----------------------------------------------------------------------------
MutexImpl::~MutexImpl
(
)
{
	DeleteCriticalSection( &m_criticalSection );
}

//-----------------------------------------------------------------------------
//	<MutexImpl::Lock>
//	Lock the mutex
//-----------------------------------------------------------------------------
bool MutexImpl::Lock
(
	bool const _bWait // = true;
)
{
	if( _bWait )
	{
		// We will wait for the lock
		EnterCriticalSection( &m_criticalSection );
		++m_lockCount;
		return true;
	}

	// Returns immediately, even if the lock was not available.
	if( TryEnterCriticalSection( &m_criticalSection ) )
	{
		++m_lockCount;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//	<MutexImpl::Unlock>
//	Release our lock on the mutex
//-----------------------------------------------------------------------------
void MutexImpl::Unlock
(
)
{
	if( !m_lockCount )
	{
		// No locks - we have a mismatched lock/release pair
		assert(0);
	}
	else
	{
		--m_lockCount;
		LeaveCriticalSection( &m_criticalSection );
	}
}

//-----------------------------------------------------------------------------
//	<MutexImpl::IsSignalled>
//	Test whether the mutex is free
//-----------------------------------------------------------------------------
bool MutexImpl::IsSignalled
(
)
{
	return( 0 == m_lockCount );
}

