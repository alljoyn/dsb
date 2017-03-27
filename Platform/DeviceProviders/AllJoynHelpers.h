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

#include "pch.h"

// The amount of time to wait (ms) for a response after sending a message before timing out.
const int c_MessageTimeoutInMilliseconds = 10000;

// The maximum length of an AllJoyn type signature allowed by the AllJoyn Core library.
const int c_MaximumSignatureLength = 255;

#define RETURN_IF_QSTATUS_ERROR(status) \
{   \
int32 alljoynStatus = static_cast<int32>(status);    \
if (Windows::Devices::AllJoyn::AllJoynStatus::Ok != alljoynStatus)  \
{   \
    return status;   \
}   \
}

#define LEAVE_IF_QSTATUS_ERROR(status) \
{   \
int32 alljoynStatus = static_cast<int32>(status);    \
if (Windows::Devices::AllJoyn::AllJoynStatus::Ok != alljoynStatus)  \
{   \
    goto leave;   \
}   \
}

class AllJoynHelpers
{
public:
    static Platform::String^ MultibyteToPlatformString(_In_ PCSTR str)
    {
        if (!str)
        {
            return ref new Platform::String();
        }

        UINT32 requiredCharacters = MultiByteToWideChar(
            CP_UTF8,
            0,
            str,
            -1,
            nullptr,
            0);

        if (requiredCharacters == 0)
        {
            return ref new Platform::String();
        }

        std::vector<wchar_t> buffer(requiredCharacters);

        UINT32 convertedBytes = MultiByteToWideChar(
            CP_UTF8,
            0,
            str,
            -1,
            buffer.data(),
            requiredCharacters);

        return ref new Platform::String(buffer.data());
    }

    static std::vector<char> PlatformToMultibyteString(_In_ Platform::String^ str)
    {
        UINT32 requiredBytes = WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            str->Data(),
            str->Length(),
            nullptr,
            0,
            nullptr,
            nullptr);

        if (requiredBytes == 0)
        {
            return std::vector<char>();
        }

        std::vector<char> buffer(requiredBytes + 1);

        UINT32 convertedBytes = WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            str->Data(),
            str->Length(),
            buffer.data(),
            requiredBytes,
            nullptr,
            nullptr);

        return buffer;
    }

    static std::string PlatformToMultibyteStandardString(_In_ Platform::String^ str)
    {
        const auto& stringVector = PlatformToMultibyteString(str);
        return std::string(stringVector.data());
    }

    static std::vector<std::string> TokenizeArgNamesString(_In_ std::string argNames)
    {
        std::vector<std::string> argNamesVector;

        for (size_t index = 0; index < argNames.size();)
        {
            size_t commaIndex = argNames.find(',', index);
            size_t len = commaIndex == std::string::npos ? commaIndex : commaIndex - index;
            argNamesVector.push_back(argNames.substr(index, len));
            index = commaIndex == std::string::npos ? commaIndex : commaIndex + 1;
        }
        return argNamesVector;
    }
};