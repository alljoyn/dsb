//-----------------------------------------------------------------------------
//
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      EventImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _EventImpl_H
#define _EventImpl_H

#include <Windows.h>
#include "Defs.h"

namespace OpenZWave
{
	/** \brief Windows-specific implementation of the Event class.
	 */
	class EventImpl
	{
	private:
		friend class Event;
		friend class SocketImpl;
		friend class Wait;

		EventImpl();
		~EventImpl();

		void Set();
		void Reset();

		bool Wait( int32 _timeout );	// The wait method is to be used only by the Wait::Multiple method
		bool IsSignalled();

		HANDLE	m_hEvent;
	};

} // namespace OpenZWave

#endif //_EventImpl_H

