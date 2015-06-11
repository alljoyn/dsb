//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      FileOpsImpl.h
//
//-----------------------------------------------------------------------------
#ifndef _FileOpsImpl_H
#define _FileOpsImpl_H

#include <stdarg.h>
#include <string>
#include "Defs.h"
#include "platform/FileOps.h"

namespace OpenZWave
{
	class FileOpsImpl
	{
		friend class FileOps;

	private:
		FileOpsImpl();
		~FileOpsImpl();

		bool FolderExists( const string &_filename );
	};

} // namespace OpenZWave

#endif //_FileOpsImpl_H

