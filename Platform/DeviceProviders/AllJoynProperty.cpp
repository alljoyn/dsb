//
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
#include "AllJoynProperty.h"
#include "AllJoynInterface.h"
#include "AllJoynBusObject.h"
#include "AllJoynStatus.h"
#include "AllJoynTypeDefinition.h"
#include "TypeConversionHelpers.h"

using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Platform::Collections;
using namespace Platform;
using namespace std;
using namespace concurrency;

namespace DeviceProviders
{
    AllJoynProperty::AllJoynProperty(AllJoynInterface ^ parent, const alljoyn_interfacedescription_property& propertyDescription)
        : m_interface(parent)
        , m_name(propertyDescription.name)
        , m_signature(propertyDescription.signature)
        , m_weakThis(this)
    {
        DEBUG_LIFETIME_IMPL(AllJoynProperty);

        m_canRead  = propertyDescription.access == ALLJOYN_PROP_ACCESS_READ  || propertyDescription.access == ALLJOYN_PROP_ACCESS_RW;
        m_canWrite = propertyDescription.access == ALLJOYN_PROP_ACCESS_WRITE || propertyDescription.access == ALLJOYN_PROP_ACCESS_RW;

        const char * namePtr = m_name.c_str();
        auto status = alljoyn_proxybusobject_registerpropertieschangedlistener(
            parent->GetBusObject()->GetProxyBusObject(),
            parent->GetName().c_str(),
            &namePtr,
            1,
            AllJoynProperty::OnPropertyChanged,
            &m_weakThis);

        auto typeDefinitons = AllJoynTypeDefinition::CreateTypeDefintions(m_signature);
        if (typeDefinitons->Size == 1)
        {
            m_typeInfo = typeDefinitons->GetAt(0);
        }

        //annotations
        auto annotations = ref new Map<String^, String^>();
        auto nCount = alljoyn_interfacedescription_property_getannotationscount(propertyDescription);
        for (size_t i = 0; i < nCount; ++i)
        {
            size_t nSizeName = 0;
            size_t nSizeValue = 0;

            //get the buffer size for name and value pair
            alljoyn_interfacedescription_property_getannotationatindex(propertyDescription, i, nullptr, &nSizeName, nullptr, &nSizeValue);
            auto pName = vector<char>(nSizeName);
            auto pValue = vector<char>(nSizeValue);

            //get the annotation at index i
            alljoyn_interfacedescription_property_getannotationatindex(propertyDescription, i, pName.data(), &nSizeName, pValue.data(), &nSizeValue);
            annotations->Insert(AllJoynHelpers::MultibyteToPlatformString(pName.data()), AllJoynHelpers::MultibyteToPlatformString(pValue.data()));
        }
        m_annotations = annotations->GetView();
    }

    AllJoynProperty::~AllJoynProperty()
    {
        alljoyn_proxybusobject_unregisterpropertieschangedlistener(
            m_interface->GetBusObject()->GetProxyBusObject(),
            m_interface->GetName().c_str(),
            AllJoynProperty::OnPropertyChanged);
    }

    void AllJoynProperty::OnPropertyChanged(_In_ alljoyn_proxybusobject busObject,
        _In_ const char* interfaceName,
        _In_ alljoyn_msgarg changed,
        _In_ alljoyn_msgarg invalidated,
        _In_ void *context)
    {
        UNREFERENCED_PARAMETER(invalidated);
        UNREFERENCED_PARAMETER(busObject);

        AllJoynProperty ^allJoynProperty = static_cast<const WeakReference *>(context)->Resolve<AllJoynProperty>();

        if (allJoynProperty)
        {
            IVector<IKeyValuePair<Object ^, Object ^>^>^ changedProperties;
            int32 status = TypeConversionHelpers::GetAllJoynMessageArg(changed, "a{sv}", &changedProperties);

            if (ER_OK == status && changedProperties->Size > 0)
            {
                auto key = changedProperties->GetAt(0)->Key->ToString();
                auto value = dynamic_cast<AllJoynMessageArgVariant^>(changedProperties->GetAt(0)->Value);

                if (key == allJoynProperty->Name &&
                    value != nullptr &&
                    value->TypeDefinition->Type == allJoynProperty->TypeInfo->Type)
                {
                    try
                    {
                        allJoynProperty->ValueChanged(allJoynProperty, value->Value);
                    }
                    catch (Exception^ ex)
                    {
                        OutputDebugString(ex->Message->Data());
                    }
                }
            }
        }
        return;
    }

    IAsyncOperation<ReadValueResult^>^ AllJoynProperty::ReadValueAsync()
    {
        return create_async([this]()-> ReadValueResult ^
        {
            auto result = ref new ReadValueResult();

            auto msgarg = alljoyn_msgarg_create();
            auto status = alljoyn_proxybusobject_getproperty(
                m_interface->GetBusObject()->GetProxyBusObject(),
                m_interface->GetName().c_str(),
                m_name.c_str(),
                msgarg);

            if (ER_OK == status)
            {
                // Property queries wrap the result in a variant, so first unpack it here
                alljoyn_msgarg_get(msgarg, "v", &msgarg);
                Object ^ value;
                status = (QStatus)TypeConversionHelpers::GetAllJoynMessageArg(msgarg, m_signature.c_str(), &value);
                result->Value = value;
            }

            result->Status = ref new AllJoynStatus(status);
            return result;
        });
    }

    IAsyncOperation<AllJoynStatus^> ^ AllJoynProperty::SetValueAsync(Object ^ newValue)
    {
        return create_async([this, newValue]() -> AllJoynStatus^
        {
            auto msgarg = alljoyn_msgarg_create();
            auto status = (QStatus)TypeConversionHelpers::SetAllJoynMessageArg(msgarg, m_signature.c_str(), newValue);
            if (status == ER_OK)
            {
                status = alljoyn_proxybusobject_setproperty(
                    m_interface->GetBusObject()->GetProxyBusObject(),
                    m_interface->GetName().c_str(),
                    m_name.c_str(),
                    msgarg);

                alljoyn_msgarg_destroy(msgarg);
            }
            return ref new AllJoynStatus(status);
        });
    }

    String ^ AllJoynProperty::Name::get()
    {
        return AllJoynHelpers::MultibyteToPlatformString(m_name.c_str());
    }
}