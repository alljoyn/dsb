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

#include "pch.h"
#include "AllJoynSession.h"
#include "AllJoynSessionImplementation.h"
#include "IBusObject.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;

namespace DeviceProviders
{
    AllJoynSession::AllJoynSession(AllJoynSessionImplementation^ sessionImpl)
        : m_sessionImpl(sessionImpl)
    {
        sessionImpl->IncrementUsage();
    }

    AllJoynSession::~AllJoynSession()
    {
        AutoLock lock(&m_lock, true);
        if (m_sessionImpl)
        {
            m_sessionImpl->DecrementUsage();
        }
    }

    alljoyn_sessionid AllJoynSession::SessionId::get()
    {
        AutoLock lock(&m_lock, true);
        return m_sessionImpl ? m_sessionImpl->GetSessionId() : 0;
    }

    int32 AllJoynSession::UsageCount::get()
    {
        AutoLock lock(&m_lock, true);
        return m_sessionImpl ? m_sessionImpl->GetUsageCount() : 0;
    }

    IVector<IBusObject^>^ AllJoynSession::Objects::get()
    {
        AutoLock lock(&m_lock, true);
        return m_sessionImpl ? m_sessionImpl->GetBusObjects() : ref new Vector<IBusObject^>();
    }

    IBusObject ^ AllJoynSession::GetBusObject(String ^ path)
    {
        AutoLock lock(&m_lock, true);
        return m_sessionImpl ? m_sessionImpl->GetBusObject(path) : nullptr;
    }

    IVector<IBusObject^>^ AllJoynSession::GetBusObjectsWhichImplementInterface(String ^ interfaceName)
    {
        AutoLock lock(&m_lock, true);
        return m_sessionImpl ? m_sessionImpl->GetBusObjectsWhichImplementInterface(interfaceName) : ref new Vector<IBusObject^>();
    }
}