//
// Device System Bridge BridgeRT
//
// Copyright (c) Microsoft Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "pch.h"

#include "Bridge.h"
#include "DeviceMain.h"
#include "DeviceMethod.h"
#include "AllJoynHelper.h"

#include <sstream>

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

using namespace BridgeRT;
using namespace std;
using namespace DsbCommon;

DeviceMethod::DeviceMethod()
    : m_adapterMethod(nullptr),
    m_parent(nullptr)
{
}

DeviceMethod::~DeviceMethod()
{
}

uint32 DeviceMethod::InvokeMethod(_In_ alljoyn_message msg, _Out_ alljoyn_msgarg *outArgs, _Out_ size_t *nbOfArgs)
{
    QStatus status = ER_OK;
    uint32 adapterStatus = ERROR_SUCCESS;
    size_t inArgsIndex = 0;

    IAdapterIoRequest^ request = nullptr;

    if (nullptr != outArgs)
    {
        *outArgs = NULL;
    }
    if (nullptr != nbOfArgs)
    {
        *nbOfArgs = 0;
    }

    // create out arguments if necessary
    if (m_adapterMethod->OutputParams->Size != 0)
    {
        // sanity check
        if (nullptr == outArgs ||
            nullptr == nbOfArgs)
        {
            adapterStatus = ERROR_BAD_ARGUMENTS;
            goto leave;
        }

        *outArgs = alljoyn_msgarg_array_create(m_adapterMethod->OutputParams->Size);
        if (NULL == *outArgs)
        {
            adapterStatus = ERROR_NOT_ENOUGH_MEMORY;
            goto leave;
        }
    }

    // get in parameters from message
    for (auto methodInParam : m_adapterMethod->InputParams)
    {
        alljoyn_msgarg inArg = alljoyn_message_getarg(msg, inArgsIndex);
        if (NULL == inArg)
        {
            adapterStatus = ERROR_BAD_ARGUMENTS;
            goto leave;
        }

        //check if the param is of type IPropertyValue^
        auto propertyValue = dynamic_cast<IPropertyValue^>(methodInParam->Data);
        if (nullptr == propertyValue)
        {
            if (TypeCode::Object == Type::GetTypeCode(methodInParam->Data->GetType()))
            {
                status = AllJoynHelper::GetAdapterObject(methodInParam, inArg, m_parent);
            }
            else
            {
                adapterStatus = ERROR_BAD_ARGUMENTS;
                goto leave;
            }
        }
        else
        {
            status = AllJoynHelper::GetAdapterValue(methodInParam, inArg);
        }
        if (ER_OK != status)
        {
            adapterStatus = ERROR_BAD_FORMAT;
            goto leave;
        }
        inArgsIndex++;
    }

    // invoke the adapter method
    adapterStatus = DsbBridge::SingleInstance()->GetAdapter()->CallMethod(m_adapterMethod, &request);
    if (ERROR_IO_PENDING == adapterStatus &&
        nullptr != request)
    {
        // wait for completion
        adapterStatus = request->Wait(WAIT_TIMEOUT_FOR_ADAPTER_OPERATION);
    }
    if (ERROR_SUCCESS != adapterStatus)
    {
        goto leave;
    }

    if (FAILED(m_adapterMethod->HResult))
    {
        // method failed on device
        status = ER_OS_ERROR;
        goto leave;
    }

    // set out arguments
    for (auto methodOutParam : m_adapterMethod->OutputParams)
    {
        //check if the param is of type IPropertyValue
        auto propertyValue = dynamic_cast<IPropertyValue^>(methodOutParam->Data);
        if (nullptr == propertyValue)
        {
            //Not a property value, see if its an Object
            if (TypeCode::Object == Type::GetTypeCode(methodOutParam->Data->GetType()))
            {
                status = AllJoynHelper::SetMsgArgFromAdapterObject(methodOutParam, alljoyn_msgarg_array_element(*outArgs, *nbOfArgs), m_parent);
            }
            else
            {
                adapterStatus = ERROR_BAD_ARGUMENTS;
            }
        }
        else
        {
            status = AllJoynHelper::SetMsgArg(methodOutParam, alljoyn_msgarg_array_element(*outArgs, *nbOfArgs));
        }

        if (ER_OK != status)
        {
            adapterStatus = ERROR_BAD_FORMAT;
            break;
        }
        *nbOfArgs++;
    }

leave:
    if (ERROR_SUCCESS != adapterStatus &&
        nullptr != outArgs &&
        NULL != *outArgs)
    {
        alljoyn_msgarg_destroy(*outArgs);
        *outArgs = NULL;
        *nbOfArgs = 0;
    }
    return adapterStatus;
}

QStatus DeviceMethod::Initialize(DeviceMain *parent, IAdapterMethod ^adapterMethod)
{
    QStatus status = ER_OK;

    // sanity check
    if (nullptr == parent)
    {
        status = ER_BAD_ARG_1;
        goto leave;
    }
    if (nullptr == adapterMethod)
    {
        status = ER_BAD_ARG_2;
        goto leave;
    }

    m_parent = parent;
    m_adapterMethod = adapterMethod;

    // build method name
    status = SetName(m_adapterMethod->Name);
    if (ER_OK != status)
    {
        goto leave;
    }

    // build in/out signatures and parameter list
    m_inSignature.clear();
    m_outSignature.clear();
    m_parameterNames.clear();

    status = BuildSignature(m_adapterMethod->InputParams, m_inSignature, m_parameterNames);
    if (ER_OK != status)
    {
        goto leave;
    }
    status = BuildSignature(m_adapterMethod->OutputParams, m_outSignature, m_parameterNames);
    if (ER_OK != status)
    {
        goto leave;
    }

    // add method to interface
    status = alljoyn_interfacedescription_addmethod(m_parent->GetInterfaceDescription(),
        m_exposedName.c_str(),
        m_inSignature.c_str(),
        m_outSignature.c_str(),
        m_parameterNames.c_str(),
        0,
        nullptr);
    if (ER_OK != status)
    {
        goto leave;
    }

leave:
    return status;
}

QStatus DeviceMethod::SetName(Platform::String ^name)
{
    QStatus status = ER_OK;

    m_exposedName.clear();

    if (name->IsEmpty())
    {
        status = ER_INVALID_DATA;
        goto leave;
    }

    AllJoynHelper::BuildPropertyOrMethodOrSignalName(name, m_exposedName);
    if (!m_parent->IsMethodNameUnique(m_exposedName))
    {
        // append unique id
        std::ostringstream tempString;
        m_exposedName += '_';
        tempString << m_parent->GetIndexForMethod();
        m_exposedName += tempString.str();
    }

leave:
    return status;
}

QStatus DeviceMethod::BuildSignature(_In_ IAdapterValueVector ^valueList, _Inout_ std::string &signature, _Inout_ std::string &parameterNames)
{
    QStatus status = ER_OK;

    if (nullptr == valueList)
    {
        // no value in list (this is OK, adapter can set list to nullptr)
        goto leave;
    }

    for (auto adapterValue : valueList)
    {
        std::string tempSignature;
        std::string hint;
        if (nullptr == adapterValue->Data)
        {
            // can't do anything with this param
            status = ER_BUS_BAD_SIGNATURE;
            goto leave;
        }

        //check if its of IPropertyValue^ type
        auto propertyValue = dynamic_cast<IPropertyValue ^>(adapterValue->Data);
        if (nullptr == propertyValue)
        {
            if (TypeCode::Object == Type::GetTypeCode(adapterValue->Data->GetType()))
            {
                IAdapterProperty ^tempProperty = dynamic_cast<IAdapterProperty ^> (adapterValue->Data);
                if (nullptr == tempProperty)
                {
                    // wrong object type
                    status = ER_BUS_BAD_SIGNATURE;
                    goto leave;
                }

                // adapter object are exposed as string on AllJoyn
                tempSignature += "s";
                hint = " (bus object path)";
            }
            else
            {
                // wrong object type
                status = ER_BUS_BAD_SIGNATURE;
                goto leave;
            }
        }
        else
        {
            status = AllJoynHelper::GetSignature(propertyValue->Type, tempSignature);
            if (ER_OK != status)
            {
                goto leave;
            }
        }
        signature += tempSignature;

        // add parameter name to parameter list
        if (0 != parameterNames.length())
        {
            parameterNames == ",";
        }
        parameterNames += To_Ascii_String(adapterValue->Name->Data());
        parameterNames += hint;
    }

leave:
    return status;
}

