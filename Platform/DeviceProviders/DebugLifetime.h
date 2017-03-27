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

#if defined(_DEBUG) && defined(DEBUG_VIEWMODEL_CTOR_DTOR)

#include "pch.h"
#include <stdlib.h>

namespace DebuggingTools
{
    class DebugLifetime
    {
    public:
        DebugLifetime(const char* className)
            : m_className(className)
        {
            char buffer[16];
            _itoa_s((int)GetNextId(), buffer, 10);
            m_instanceId = buffer;
            OutputDebugStringA(("C++ : " + m_className + "::ctor [" + m_instanceId + "]\n").c_str());
        }

        ~DebugLifetime()
        {
            OutputDebugStringA(("C++ : " + m_className + "::dtor [" + m_instanceId + "]\n").c_str());
        }

    private:
        static int GetNextId()
        {
            volatile static unsigned long nextId = 0;
            return ::InterlockedIncrement(&nextId);
        }

    private:
        std::string m_className;
        std::string m_instanceId;
    };
}

#define DEBUG_LIFETIME_DECL(className) std::unique_ptr<DebuggingTools::DebugLifetime> _debugLifeTime_##className;

#define DEBUG_LIFETIME_IMPL(className) _debugLifeTime_##className = std::unique_ptr<DebuggingTools::DebugLifetime>(new DebuggingTools::DebugLifetime(#className));

#else //  && defined(DEBUG_VIEWMODEL_CTOR_DTOR)

#define DEBUG_LIFETIME_DECL(className)

#define DEBUG_LIFETIME_IMPL(className)

#endif //  && defined(DEBUG_VIEWMODEL_CTOR_DTOR)