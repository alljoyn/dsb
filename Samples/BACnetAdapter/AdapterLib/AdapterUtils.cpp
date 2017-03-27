// 
// IN Copyright (c) %s, Microsoft Corporation
// IN 
// IN SPDX-License-Identifier: Apache-2.0 *
// IN 
// IN All rights reserved. This program and the accompanying materials are
// IN made available under the terms of the Apache License, Version 2.0
// IN which accompanies this distribution, and is available at
// IN http://www.apache.org/licenses/LICENSE-2.0
// IN 
// IN Permission to use, copy, modify, and/or distribute this software for
// IN any purpose with or without fee is hereby granted, provided that the
// IN above copyright notice and this permission notice appear in all
// IN copies.
// IN 
// IN THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// IN WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// IN WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// IN AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// IN DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// IN PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// IN TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// IN PERFORMANCE OF THIS SOFTWARE.
//
#include "pch.h"
#include "BridgeUtils.h"
#include "AdapterUtils.h"
#include <stdio.h>
#include <exception>

using namespace BridgeRT;
using namespace Platform;

int
AdapterLib::FormatString(String^& OutputString, const wchar_t* FormatWsz, ...)
{
    // Default string buffer
    wchar_t stringBufferWsz[1024] = { 0 };
    //
    // Alternate buffer in case the default buffer is not
    // big enough to hold the output string.
    //
    wchar_t* altStringBufferPtr = nullptr;
    // Current buffer parameters
    size_t stringMaxCharCount = ARRAYSIZE(stringBufferWsz);
    wchar_t* stringBufferToUsePtr = &stringBufferWsz[0];
    int charCount = 0;

    va_list argList;
    va_start(argList, FormatWsz);

    try
    {
        do
        {
            charCount = _vsnwprintf_s(
                stringBufferToUsePtr,
                stringMaxCharCount - 1,
                _TRUNCATE,
                FormatWsz,
                argList
                );
            if (charCount == -1)
            {
                //
                // Default buffer is not big enough,
                // reallocate, and use alternate buffer...
                //

                if (stringMaxCharCount >= MAX_STRING_SIZE)
                {
                    goto done;
                }
                stringMaxCharCount *= 2;

                delete[] altStringBufferPtr;
                altStringBufferPtr = new wchar_t[stringMaxCharCount];

                RtlZeroMemory(altStringBufferPtr, stringMaxCharCount*sizeof(wchar_t));

                stringBufferToUsePtr = altStringBufferPtr;

            } // Buffer is too small...

        } while (charCount == -1);

        OutputString = ref new String(stringBufferToUsePtr);
    }
    catch (std::bad_alloc)
    {
        charCount = -1;
    }
    catch (OutOfMemoryException^)
    {
        charCount = -1;
    }

done:
    va_end(argList);

    delete[] altStringBufferPtr;

    return charCount;
}

uint32
AdapterLib::StringToArray(String^ SourceString, Array<BYTE>^* TargetDataArrayPtr)
{
    uint32 status = ERROR_SUCCESS;
    int stringByteLen = 0;

    if ((SourceString == nullptr) || (TargetDataArrayPtr == nullptr))
    {
        status = ERROR_INVALID_PARAMETER;
        goto done;
    }

    stringByteLen = (SourceString->Length() + 1) * sizeof(wchar_t);

    try
    {
        (*TargetDataArrayPtr) = ref new Array<BYTE>(stringByteLen);

        CopyMemory((*TargetDataArrayPtr)->Data, SourceString->Data(), stringByteLen);
    }
    catch (OutOfMemoryException^ ex)
    {
        status = WIN32_FROM_HRESULT(ex->HResult);
        goto done;
    }

done:
    return status;
}