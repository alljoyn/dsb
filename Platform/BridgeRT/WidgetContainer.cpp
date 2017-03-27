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
#include "WidgetContainer.h"


using namespace BridgeRT;

static const char CP_CONTAINER_INTERFACE_NAME[] = "org.alljoyn.ControlPanel.Container";
const uint16_t VERTICAL_ALIGNMENT = 1;

WidgetContainer::WidgetContainer(ControlPanel* pControlPanel) : Widget(pControlPanel)
{
}

//**************************************************************************************************************************************
//
//  Destructor
//
//**************************************************************************************************************************************
WidgetContainer::~WidgetContainer()
{
}

//**************************************************************************************************************************************
//
//  AllJoyn Interface Name for this Widget Type
//
//**************************************************************************************************************************************
const char* WidgetContainer::GetInterfaceName()
{
    return CP_CONTAINER_INTERFACE_NAME;
}

QStatus WidgetContainer::Get(_In_z_ const char* interfaceName, _In_z_ const char* propName, _Out_ alljoyn_msgarg val) const
{
    QStatus status = ER_OK;
    alljoyn_msgarg values = nullptr;
    alljoyn_msgarg dictEntries = nullptr;

    // Handle the Optional Parameters if requested
    if (strcmp(propName, PROPERTY_OPTPARAMS_STR) == 0)
    {
        int keys[] = { LABEL_KEY, BGCOLOR_KEY, HINT_KEY };
        uint16_t layoutHints[] = { VERTICAL_ALIGNMENT };

        // Create an array of Variant Type-Data value pairs
        values = alljoyn_msgarg_array_create(_countof(keys));
        CHK_POINTER(values);

        // Set each Variant "Type-Data Pair"
        CHK_AJSTATUS(alljoyn_msgarg_set(alljoyn_msgarg_array_element(values, LABEL_KEY), ARG_STRING_STR, GetLabel()));
        CHK_AJSTATUS(alljoyn_msgarg_set(alljoyn_msgarg_array_element(values, BGCOLOR_KEY), ARG_UINT32_STR, GetBgColor()));
        CHK_AJSTATUS(alljoyn_msgarg_set(alljoyn_msgarg_array_element(values, HINT_KEY), ARG_UINT16_ARRY_STR, _countof(layoutHints), layoutHints));

        // Create an array of Dictionary Entries
        dictEntries = alljoyn_msgarg_array_create(_countof(keys));
        CHK_POINTER(dictEntries);
        // Load the Dictionary Entries into the array of dictionary entries where the Keys are Number values that map to Variant Type-Data pairs
        for (int idx = 0; idx < _countof(keys); idx++)
        {
            CHK_AJSTATUS(alljoyn_msgarg_set(
                alljoyn_msgarg_array_element(dictEntries, idx),
                ARG_DICT_ITEM_STR,
                keys[idx],
                alljoyn_msgarg_array_element(values, keys[idx])));
        }

        // Return the array of dictionary entries for the Optional Parameters Output Property
        CHK_AJSTATUS(alljoyn_msgarg_set(val, ARG_DICT_STR, _countof(keys), dictEntries));
        alljoyn_msgarg_stabilize(val);

    }
    // The Properties are unknown, hand them up to the base Widget to process.
    else
    {
        status = Widget::Get(interfaceName, propName, val);
    }

leave:
    if (values != nullptr)
    {
        alljoyn_msgarg_destroy(values);
        values = nullptr;
    }
    if (dictEntries != nullptr)
    {
        alljoyn_msgarg_destroy(dictEntries);
        dictEntries = nullptr;
    }
    return status;
}