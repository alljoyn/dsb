
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
#include "WidgetConsts.h"
#include "WidgetPropertyLabel.h"
#include "BridgeUtils.h"

using namespace BridgeRT;

const uint16_t WIDGET_LABEL_TYPE = 11;

//**************************************************************************************************************************************
//
//  Constructor
//
//  pControlPanel    The control panel hosting this widget
//  srcValue         The source content of this widget
//
//**************************************************************************************************************************************
WidgetPropertyLabel::WidgetPropertyLabel(_In_ ControlPanel* pControlPanel, _In_ IAdapterValue^ srcValue)
    : WidgetProperty(pControlPanel, WIDGET_LABEL_TYPE, true)
    , m_srcValue(srcValue)
{
}

//**************************************************************************************************************************************
//
//  Destructor
//
//**************************************************************************************************************************************
WidgetPropertyLabel::~WidgetPropertyLabel()
{
}

//**************************************************************************************************************************************
//
//  Gets the current value of this label's content
//
//  val     Variant message arg value to return to the caller.
//
//**************************************************************************************************************************************
QStatus WidgetPropertyLabel::GetValue(_Out_ alljoyn_msgarg val) const
{
    QStatus status = ER_OK;

    std::string srcContent = ConvertTo<std::string>(m_srcValue->Data->ToString());
    alljoyn_msgarg variantarg = alljoyn_msgarg_create();
    CHK_POINTER(variantarg);
    CHK_AJSTATUS(alljoyn_msgarg_set(variantarg, ARG_STRING_STR, srcContent.c_str()));
    CHK_AJSTATUS(alljoyn_msgarg_set(val, ARG_VARIANT_STR, variantarg));
    alljoyn_msgarg_stabilize(val);

leave:
    if (variantarg != nullptr)
    {
        alljoyn_msgarg_destroy(variantarg);
        variantarg = nullptr;
    }
    return status;
}