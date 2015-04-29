//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      LogImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _LogImpl_H
#define _LogImpl_H

#include "Defs.h"
#include <string>
#include "platform/Log.h"
#include "Windows.h"

namespace OpenZWave
{
	/** \brief Windows-specific implementation of the Log class.
	 */
	class LogImpl : public i_LogImpl
	{
	private:
		friend class Log;

		LogImpl( string const& _filename, bool const _bAppendLog, bool const _bConsoleOutput, LogLevel const _saveLevel, LogLevel const _queueLevel, LogLevel const _dumpTrigger );
		~LogImpl();

		void Write( LogLevel _level, uint8 const _nodeId, char const* _format, va_list _args );
		void Queue( char const* _buffer );
		void QueueDump();
		void QueueClear();
		void SetLoggingState( LogLevel _saveLevel, LogLevel _queueLevel, LogLevel _dumpTrigger );
		void SetLogFileName( const string &_filename );

		string GetTimeStampString();
		string GetNodeString( uint8 const _nodeId );
		string GetThreadId();
		string GetLogLevelString(LogLevel _level);

		string m_filename;						/**< filename specified by user (default is ozw_log.txt) */
		bool m_bConsoleOutput;					/**< if true, send log output to console as well as to the file */
		bool m_bAppendLog;						/**< if true, the log file should be appended to any with the same name */
		list<string> m_logQueue;				/**< list of queued log messages */
		LogLevel m_saveLevel;
		LogLevel m_queueLevel;
		LogLevel m_dumpTrigger;
	};

} // namespace OpenZWave

#endif //_LogImpl_H

