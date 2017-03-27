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
#include "ZWaveAdapterMethod.h"
#include "AdapterUtils.h"

#include "Manager.h"
#include "value_classes\ValueID.h"
#include "command_classes\CommandClass.h"

#include <sstream>
#include <string>


using namespace std;
using namespace OpenZWave;
using namespace Platform;
using namespace BridgeRT;

namespace AdapterLib
{
    ZWaveAdapterMethod::ZWaveAdapterMethod(const ValueID & value)
        : m_valueId(value)
        , m_result(E_FAIL)
    {
    }

    void ZWaveAdapterMethod::Initialize()
    {
        m_name = ref new String(ConvertTo<wstring>(Manager::Get()->GetValueLabel(m_valueId)).c_str());
        m_description = ref new String(ConvertTo<wstring>(Manager::Get()->GetValueHelp(m_valueId)).c_str());
    }

    void ZWaveAdapterMethod::Execute()
    {
        bool bRet = Manager::Get()->PressButton(m_valueId);
        m_result = bRet ? S_OK : E_FAIL;
    }
}