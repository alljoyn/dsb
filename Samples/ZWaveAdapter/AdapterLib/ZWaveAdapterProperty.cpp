// Copyright (c) 2015, Microsoft Corporation
// 
// Permission to use, copy, modify, and/or distribute this software for any 
// purpose with or without fee is hereby granted, provided that the above 
// copyright notice and this permission notice appear in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
// IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

#include "pch.h"

#include "ZWaveAdapterProperty.h"
#include "ZWaveAdapterValue.h"
#include "Misc.h"

#include "Manager.h"
#include "value_classes\ValueID.h"
#include "value_classes\Value.h"
#include "command_classes\CommandClass.h"
#include "command_classes\CommandClasses.h"
#include "command_classes\Basic.h"

#include <sstream>
#include <string>

using namespace std;
using namespace OpenZWave;
using namespace Platform;
using namespace DsbCommon;
using namespace BridgeRT;
using namespace Windows::Foundation;

namespace AdapterLib
{
    ZWaveAdapterProperty::ZWaveAdapterProperty(const OpenZWave::ValueID & value)
        : m_valueId(value)
    {   
    }

    void ZWaveAdapterProperty::Initialize()
    {
        //name
        m_name = ref new String(ConvertTo<wstring>(Manager::Get()->GetValueLabel(m_valueId)).c_str());
        GetAttributes();
    }

    void ZWaveAdapterProperty::GetAttributes()
    {
        //Get all the attributes for the Value
        m_attributes.clear();

        //instance
        m_attributes.push_back(ref new ZWaveAdapterValue(L"instance", (int32)(m_valueId.GetInstance())));

        //index
        m_attributes.push_back(ref new ZWaveAdapterValue(L"index", (int32)(m_valueId.GetIndex())));

        //genre
        m_attributes.push_back(ref new ZWaveAdapterValue(L"genre", string(Value::GetGenreNameFromEnum(m_valueId.GetGenre()))));
        
        //label
        m_attributes.push_back(ref new ZWaveAdapterValue(L"label", Manager::Get()->GetValueLabel(m_valueId)));

        //units
        m_attributes.push_back(ref new ZWaveAdapterValue(L"units", Manager::Get()->GetValueUnits(m_valueId)));

        //help
        m_attributes.push_back(ref new ZWaveAdapterValue(L"help", Manager::Get()->GetValueHelp(m_valueId)));

        //min
        m_attributes.push_back(ref new ZWaveAdapterValue(L"min", Manager::Get()->GetValueMin(m_valueId)));

        //max
        m_attributes.push_back(ref new ZWaveAdapterValue(L"max", Manager::Get()->GetValueMax(m_valueId)));

        //readonly
        m_attributes.push_back(ref new ZWaveAdapterValue(L"read_only", Manager::Get()->IsValueReadOnly(m_valueId)));

        //writeonly
        m_attributes.push_back(ref new ZWaveAdapterValue(L"write_only", Manager::Get()->IsValueWriteOnly(m_valueId)));

        //values
        ValueID::ValueType type = m_valueId.GetType();
        if (type == ValueID::ValueType::ValueType_List)
        {
            //add list items
            vector<string> items;
            Manager::Get()->GetValueListItems(m_valueId, &items);

            m_attributes.push_back(ref new ZWaveAdapterValue(L"valid_values", items));
        }

        UpdateValue();
    }

    void ZWaveAdapterProperty::UpdateValue()
    {
        ValueID::ValueType type = m_valueId.GetType();

        //look for the value in AdapterValue
        auto iter = find_if(m_attributes.begin(), m_attributes.end(), [&](IAdapterValue^ adapterValue)
        {
            return dynamic_cast<ZWaveAdapterValue^>(adapterValue)->m_name->Data() == ValueName;
        });

        if (iter == m_attributes.end())
        {
            //add the AdapterValue
            m_attributes.push_back(ref new ZWaveAdapterValue(ref new String(ValueName.c_str()), nullptr));
            iter = prev(m_attributes.end());
        }

        switch (type)
        {
        case ValueID::ValueType_Bool:
        {
            bool bVal;
            Manager::Get()->GetValueAsBool(m_valueId, &bVal);
            (*iter)->Data = PropertyValue::CreateBoolean(bVal);
        }
        break;
        case ValueID::ValueType_Byte:
        {
            uint8 byteVal;
            Manager::Get()->GetValueAsByte(m_valueId, &byteVal);
            (*iter)->Data = PropertyValue::CreateUInt8(byteVal);
        }
        break;
        case ValueID::ValueType_Decimal:
        {
            stringstream ss;

            float val;
            uint8 precision = 0;
            Manager::Get()->GetValueAsFloat(m_valueId, &val);
            Manager::Get()->GetValueFloatPrecision(m_valueId, &precision);

            (*iter)->Data = PropertyValue::CreateDouble(val);
        }
        break;
        case ValueID::ValueType_Int:
        {
            int32 val;
            Manager::Get()->GetValueAsInt(m_valueId, &val);

            (*iter)->Data = PropertyValue::CreateInt32(val);
        }
        break;
        case ValueID::ValueType_Short:
        {
            int16 val;
            Manager::Get()->GetValueAsShort(m_valueId, &val);

            (*iter)->Data = PropertyValue::CreateInt16(val);
        }
        break;
        case ValueID::ValueType_String:
        {
            string strValue;
            Manager::Get()->GetValueAsString(m_valueId, &strValue);

            (*iter)->Data = PropertyValue::CreateString(ref new String(ConvertTo<wstring>(strValue).c_str()));
        }
        break;
        case ValueID::ValueType_Raw:
        {
            uint8 *pVal = nullptr;
            uint8 len = 0;
            Manager::Get()->GetValueAsRaw(m_valueId, &pVal, &len);

            Platform::Array<uint8_t>^ octetArray = ref new Platform::Array<uint8_t>(pVal, len);
            (*iter)->Data = PropertyValue::CreateUInt8Array(octetArray);

            delete[] pVal;
        }
        break;
        case ValueID::ValueType_List:
        {
            string strValue;
            Manager::Get()->GetValueListSelection(m_valueId, &strValue);

            (*iter)->Data = PropertyValue::CreateString(ref new String(ConvertTo<wstring>(strValue).c_str()));
        }
        break;
        default:
            break;
        }
    }

    uint32 ZWaveAdapterProperty::SetValue(Object^ data)
    {
        uint32 status = ERROR_SUCCESS;

        Value *pVal = nullptr;
        bool bRet = true;
        string strVal = ConvertTo<string>(wstring(data->ToString()->Data()));

        CommandClass *pCmd = CommandClasses::CreateCommandClass(m_valueId.GetCommandClassId(), m_valueId.GetHomeId(), m_valueId.GetNodeId());
        if (pCmd == nullptr)
        {
            status = WIN32_FROM_HRESULT(E_FAIL);
            goto done;
        }
        pVal = pCmd->GetValue(m_valueId.GetInstance(), m_valueId.GetIndex());
        if (pVal == nullptr)
        {
            status = WIN32_FROM_HRESULT(E_FAIL);
            goto done;
        }

        if (pVal->IsReadOnly())
        {
            status = ERROR_ACCESS_DENIED;
            goto done;
        }

        bRet = pVal->SetFromString(strVal);
        if (bRet == false)
        {
            status = WIN32_FROM_HRESULT(E_FAIL);
            goto done;
        }

    done:
        if (pVal)
        {
            pVal->Release();
        }
        return status;
    }

    ZWaveAdapterValue^ ZWaveAdapterProperty::GetAttributeByName(String^ name)
    {
        for (auto attr : this->m_attributes)
        {
            if (attr->Name == name)
            {
                return dynamic_cast<ZWaveAdapterValue^>(attr);
            }
        }

        return nullptr;
    }
}