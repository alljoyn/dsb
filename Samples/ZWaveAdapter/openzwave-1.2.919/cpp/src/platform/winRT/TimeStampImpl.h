//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      TimeStampImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _TimeStampImpl_H
#define _TimeStampImpl_H

#include "Defs.h"

namespace OpenZWave
{
	/** \brief Windows implementation of a timestamp.
	 */
	class TimeStampImpl
	{
	public:
		/**
		 * Constructor.
		 * Creates a TimeStampImpl object.
		 */
		TimeStampImpl();

		/**
		 * Destructor.
		 * Destroys the TimeStampImpl object.
		 */
		~TimeStampImpl();

		/**
		 * SetTime.  Sets the timestamp to now, plus the offset in milliseconds.
		 * \param _milliseconds positive or negative offset from 
		 * now in milliseconds.
		 */
		void SetTime( int32 _milliseconds );

		/**
		 * TimeRemaining.  Gets the difference between now and the timestamp
		 * time in milliseconds.
		 * \return milliseconds remaining until we reach the timestamp.  The 
		 * return value is negative if the timestamp is in the past.
		 */
		int32 TimeRemaining();

		/**
		 * Return as as string
		 */
		string GetAsString();

		/**
		 * Overload the subtract operator to get the difference between
		 * two timestamps in milliseconds.
		 */
		int32 operator- ( TimeStampImpl const& _other );

	private:
		TimeStampImpl( TimeStampImpl const& );			// prevent copy
		TimeStampImpl& operator = ( TimeStampImpl const& );	// prevent assignment

		int64	m_stamp;
	};

} // namespace OpenZWave

#endif //_TimeStampImpl_H

