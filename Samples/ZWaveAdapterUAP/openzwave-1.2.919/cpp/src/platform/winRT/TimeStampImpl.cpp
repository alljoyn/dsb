//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      TimeStampImpl.cpp
//
//-----------------------------------------------------------------------------
#include <string>
#include <Windows.h>
#include "Defs.h"
#include "TimeStampImpl.h"

using namespace OpenZWave;

//-----------------------------------------------------------------------------
//	<TimeStampImpl::TimeStampImpl>
//	Constructor
//-----------------------------------------------------------------------------
TimeStampImpl::TimeStampImpl
(
)
{
	SetTime(0);
}

//-----------------------------------------------------------------------------
//	<TimeStampImpl::~TimeStampImpl>
//	Destructor
//-----------------------------------------------------------------------------
TimeStampImpl::~TimeStampImpl
(
)
{
}

//-----------------------------------------------------------------------------
//	<TimeStampImpl::SetTime>
//	Sets the timestamp to now, plus an offset in milliseconds
//-----------------------------------------------------------------------------
void TimeStampImpl::SetTime
(
	int32 _milliseconds	// = 0
)
{
	int64 offset = ((int64)_milliseconds) * 10000LL;	// Timestamp is stored in 100ns steps.

	GetSystemTimeAsFileTime( (FILETIME*)&m_stamp );
	m_stamp += offset;
}

//-----------------------------------------------------------------------------
//	<TimeStampImpl::TimeRemaining>
//	Gets the difference between now and the timestamp time in milliseconds
//-----------------------------------------------------------------------------
int32 TimeStampImpl::TimeRemaining
(
)
{
	int64 now;
	GetSystemTimeAsFileTime( (FILETIME*)&now );

	return (int32)( ( m_stamp - now ) / 10000LL );
}

//-----------------------------------------------------------------------------
//	<TimeStampImpl::GetAsString>
//	Return a string representation
//-----------------------------------------------------------------------------
string TimeStampImpl::GetAsString
(
)
{
	// Convert m_stamp (FILETIME) to SYSTEMTIME for ease of use
	SYSTEMTIME time;
	::FileTimeToSystemTime( (FILETIME*)&m_stamp, &time );

	char buf[100];
	sprintf_s( buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d:%03d ", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );
	string str = buf;
	return str;
}

//-----------------------------------------------------------------------------
//	<TimeStampImpl::operator->
//	Overload the subtract operator to get the difference between two
//	timestamps in milliseconds
//-----------------------------------------------------------------------------
int32 TimeStampImpl::operator-
(
	TimeStampImpl const& _other
)
{
	return (int32)( ( m_stamp - _other.m_stamp ) / 10000LL );
}
