//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      FileOpsImpl.cpp
//
//-----------------------------------------------------------------------------

#include <windows.h>
#include "FileOpsImpl.h"

using namespace OpenZWave;

//-----------------------------------------------------------------------------
//	<FileOpsImpl::FileOpsImpl>
//	Constructor
//-----------------------------------------------------------------------------
FileOpsImpl::FileOpsImpl
(
)
{
}

//-----------------------------------------------------------------------------
//	<FileOpsImpl::~FileOpsImpl>
//	Destructor
//-----------------------------------------------------------------------------
FileOpsImpl::~FileOpsImpl
(
)
{
}

//-----------------------------------------------------------------------------
//	<FileOpsImpl::FolderExists>
//	Determine if a folder exists and is accessible by the calling App
//-----------------------------------------------------------------------------
bool FileOpsImpl::FolderExists( 
	const string &_folderName
)
{
	WIN32_FILE_ATTRIBUTE_DATA fad = { 0 };
	wstring wFolderName(_folderName.begin(), _folderName.end());

	if (0 == GetFileAttributesEx(wFolderName.c_str(), GetFileExInfoStandard, &fad))
		return false;

	return (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)? true: false;
}
