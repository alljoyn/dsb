//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      ThreadImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _ThreadImpl_H
#define _ThreadImpl_H

#include <string>
#include <windows.h>
#include "platform/Thread.h"
#include <ppltasks.h>

namespace OpenZWave
{
	class Thread;
	class Event;

	/** \brief Windows-specific implementation of the Thread class.
	 */
	class ThreadImpl
	{
	private:
		friend class Thread;

		ThreadImpl( Thread* _owner, string const& _name );
		~ThreadImpl();

		bool Start( Thread::pfnThreadProc_t _pfnThreadProc, Event* _exitEvent, void* _context );
		void Sleep( uint32 _milliseconds );
		bool Terminate();

		bool IsSignalled();

		void Run();
		static DWORD WINAPI ThreadProc( void* _pArg );

		Thread*					m_owner;
		Event*					m_exitEvent;
		Thread::pfnThreadProc_t	m_pfnThreadProc;
		void*					m_context;
		bool					m_bIsRunning;
		string					m_name;
		
		static int32			s_threadTerminateTimeout;
	};

} // namespace OpenZWave

#endif //_ThreadImpl_H

