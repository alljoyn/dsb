//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      WaitImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _WaitImpl_H
#define _WaitImpl_H

#include <windows.h>
#include <list>
#include "Defs.h"
#include "platform/Ref.h"
#include "platform/Wait.h"

namespace OpenZWave
{
	/** \brief Windows specific implementation of Wait objects.
	 */
	class WaitImpl
	{
	private:
		friend class Wait;

		WaitImpl( Wait* _owner );
		virtual ~WaitImpl();

		void AddWatcher( Wait::pfnWaitNotification_t _callback, void* _context );
		bool RemoveWatcher( Wait::pfnWaitNotification_t _callback, void* _context );
		void Notify();

		static int32 Multiple( Wait** _objects, uint32 _numObjects, int32 _timeout = -1 );

		WaitImpl( Wait const&	);					// prevent copy
		WaitImpl& operator = ( WaitImpl const& );	// prevent assignment

		struct Watcher
		{
			Wait::pfnWaitNotification_t		m_callback;
			void*							m_context;
		};

		list<Watcher>		m_watchers;
		Wait*				m_owner;
		CRITICAL_SECTION	m_criticalSection;
	};

} // namespace OpenZWave

#endif //_WaitImpl_H

