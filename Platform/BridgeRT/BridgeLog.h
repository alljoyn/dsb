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
#include <Windows.h>


//************************************************************************************************
//
//  The BridgeLog Singleton provides assistance for logging Bridge Information and errors to
//  a local log etl log file.  Logging is always in memory and is only flushed to disk on ERROR.
//
//  Use "LogInfo" for non-fatal errors, that would be helpful context for error investigations.
//
//************************************************************************************************
class BridgeLog sealed
{
public:
    static BridgeLog* Instance(void);

    void LogEnter(_In_z_ wchar_t* funcName);
    void LogLeave(_In_z_ wchar_t* funcName, int hr=0);
    void LogInfo(_In_ Platform::String^ LogMsg);
    void LogError(_In_ Platform::String^ LogMsg, _In_ Platform::Exception^ ex);
    void LogError(_In_ Platform::String^ LogMsg, int hr);

private:
    BridgeLog(void);
    Windows::Foundation::Diagnostics::LoggingSession^ m_session;
    Windows::Foundation::Diagnostics::LoggingChannel^ m_channel;

    void Shutdown(void);
    volatile long m_shutdown;
};
