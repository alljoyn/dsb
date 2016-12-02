/**
* @file
*/
/******************************************************************************
* * 
*    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
*    Source Project Contributors and others.
*    
*    All rights reserved. This program and the accompanying materials are
*    made available under the terms of the Apache License, Version 2.0
*    which accompanies this distribution, and is available at
*    http://www.apache.org/licenses/LICENSE-2.0

******************************************************************************/

#include <windows.h>
#include <stdio.h>

extern "C" {

BOOL CryptAcquireContext(
    _Out_ HCRYPTPROV *phProv,
    _In_  LPCTSTR    pszContainer,
    _In_  LPCTSTR    pszProvider,
    _In_  DWORD      dwProvType,
    _In_  DWORD      dwFlags
    )
{
    *phProv = NULL;
    return TRUE;
}

BOOL CryptGenRandom(
    _In_    HCRYPTPROV hProv,
    _In_    DWORD      dwLen,
    _Inout_ BYTE       *pbBuffer
    )
{
    if (pbBuffer)
    {
        Windows::Storage::Streams::IBuffer^ buffer = Windows::Security::Cryptography::CryptographicBuffer::GenerateRandom(dwLen);
        Platform::Array<unsigned char, 1U>^ charArray = ref new Platform::Array<unsigned char, 1U>(dwLen);
        Windows::Security::Cryptography::CryptographicBuffer::CopyToByteArray(buffer, &charArray);
        memcpy((void*)pbBuffer, (const void*)charArray->Data, dwLen);
        return TRUE;
    }
    return FALSE;
}

}