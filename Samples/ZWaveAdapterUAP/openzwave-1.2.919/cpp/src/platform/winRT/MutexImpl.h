//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      MutexImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _MutexImpl_H
#define _MutexImpl_H

#include <windows.h>


namespace OpenZWave
{
	/** \brief Windows-specific implementation of the Mutex class.
	 */
	class MutexImpl
	{
	private:
		friend class Mutex;

		MutexImpl();
		~MutexImpl();

		bool Lock( bool const _bWait = true );
		void Unlock();

		bool IsSignalled();

		CRITICAL_SECTION	m_criticalSection;
		uint32				m_lockCount;				// Keep track of the locks (there can be more than one if they occur on the same thread.
	};

} // namespace OpenZWave

#endif //_MutexIF_H

