//
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
#pragma once

namespace AdapterLib
{

    //
    //  Routine Description:
    //      Conversion from Platform::String to Platform::Array<byte>
    //
    //  Arguments:
    //
    //      SourceString - The source string to be converted
    //
    //      TargetDataArrayPtr - Address of a caller platform::Array<BYTE>^ variable
    //          to receive the array version of the string.
    //
    //  Return Value:
    //
    //      - ERROR_SUCCESS.
    //      - ERROR_INVALID_PARAMETER: SourceString or TargetDataArrayPtr is null.
    //      - ERROR_NOT_ENOUGH_MEMORY: Failed to allocate destination array.
    //
    uint32 StringToArray(
        _In_ Platform::String^ SourceString,
        _Out_ Platform::Array<BYTE>^* TargetDataArrayPtr);


    bool FileExist(_In_ Platform::String^ filePath);
    bool FolderExist(_In_ Platform::String^ folderName);
    HRESULT CreateFolder(_In_ Platform::String^ folderName);
    HRESULT CopyFolder(_In_ Platform::String^ source, _In_ Platform::String^ destination);
    HRESULT DeleteFolder(_In_ Platform::String^ dir);
}