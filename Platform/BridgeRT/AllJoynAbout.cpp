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
#include <vector>
#include <strsafe.h>

#include "DsbServiceNames.h" 
#include "AllJoynAbout.h"

#include "Shlwapi.h"
#include "alljoyn_c/AjAPI.h"

using namespace BridgeRT;
using namespace DsbCommon;

using namespace Windows::Storage;
using namespace Platform;

#define APPDATA_CONTAINER_DSB_SETTINGS  L"DSBSettings"
#define DSB_SETTING_DEVICE_ID           L"ID"

static const size_t DEVICE_GUID_STRING_LEN = 39;   // 38 chars + terminator null

static const char DEFAULT_LANGUAGE_FOR_ABOUT[] = "en";
static const WCHAR UNKNOWN_ADAPTER[] = L"Unknown device";
static const WCHAR DSB_MANUFACTURER[] = L"Microsoft";
static const WCHAR DSB_VERSION[] = L"0.0.0.0";
static const WCHAR DSB_MODEL[] = L"DSB";
static const WCHAR DSB_DESCRIPTION[] = L"Device System Bridge";

AllJoynAbout::AllJoynAbout()
    : m_aboutData(NULL),
    m_aboutObject(NULL),
    m_isAnnounced(false)
{
}

AllJoynAbout::~AllJoynAbout()
{
    ShutDown();
}

QStatus AllJoynAbout::Initialize(_In_ alljoyn_busattachment bus)
{
    QStatus status = ER_OK;

    // sanity check
    if (NULL == bus)
    {
        status = ER_BAD_ARG_1;
        goto leave;
    }

    // create the about object that is used to communicate about data
    // note that the about interface won't be part of the announce
    m_aboutObject = alljoyn_aboutobj_create(bus, UNANNOUNCED);
    if (NULL == m_aboutObject)
    {
        status = ER_OUT_OF_MEMORY;
        goto leave;
    }

    // create about data with default language
    m_aboutData = alljoyn_aboutdata_create(DEFAULT_LANGUAGE_FOR_ABOUT);
    if (NULL == m_aboutData)
    {
        status = ER_OUT_OF_MEMORY;
        goto leave;
    }

    // fill about data with default value
    status = SetDefaultAboutData();

leave:
    if (ER_OK != status)
    {
        ShutDown();
    }

    return status;
}

void AllJoynAbout::ShutDown()
{
    if (NULL != m_aboutObject)
    {
        if (m_isAnnounced)
        {
            alljoyn_aboutobj_unannounce(m_aboutObject);
        }
        alljoyn_aboutobj_destroy(m_aboutObject);
        m_aboutObject = NULL;
        m_isAnnounced = false;
    }
    if (NULL != m_aboutData)
    {
        alljoyn_aboutdata_destroy(m_aboutData);
        m_aboutData = NULL;
    }
}

QStatus AllJoynAbout::Announce()
{
    QStatus status = ER_OK;

    // sanity check
    if (NULL == m_aboutObject)
    {
        status = ER_INIT_FAILED;
        goto leave;
    }

    status = alljoyn_aboutobj_announce(m_aboutObject, DSB_SERVICE_PORT, m_aboutData);
    if (ER_OK != status)
    {
        goto leave;
    }

    m_isAnnounced = true;

leave:
    return status;
}

QStatus AllJoynAbout::AddObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription)
{
    return alljoyn_busobject_setannounceflag(busObject, interfaceDescription, ANNOUNCED);
}

QStatus AllJoynAbout::RemoveObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription)
{
    return alljoyn_busobject_setannounceflag(busObject, interfaceDescription, UNANNOUNCED);
}

QStatus AllJoynAbout::SetManufacturer(_In_z_ const wchar_t* value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setmanufacturer(m_aboutData, stringValue.c_str(), DEFAULT_LANGUAGE_FOR_ABOUT);
}

QStatus AllJoynAbout::SetDeviceName(_In_z_ const wchar_t *value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setdevicename(m_aboutData, stringValue.c_str(), DEFAULT_LANGUAGE_FOR_ABOUT);
}

QStatus AllJoynAbout::SetVersion(_In_z_ const wchar_t *value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setsoftwareversion(m_aboutData, stringValue.c_str());
}

QStatus AllJoynAbout::SetDeviceId(_In_z_ const wchar_t * value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setdeviceid(m_aboutData, stringValue.c_str());
}

QStatus AllJoynAbout::SetModel(_In_z_ const wchar_t * value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setmodelnumber(m_aboutData, stringValue.c_str());
}

QStatus AllJoynAbout::SetDescription(_In_z_ const wchar_t * value)
{
    std::string stringValue = To_Ascii_String(value);
    return alljoyn_aboutdata_setdescription(m_aboutData, stringValue.c_str(), DEFAULT_LANGUAGE_FOR_ABOUT);
}

QStatus AllJoynAbout::SetApplicationName(_In_z_ const char *value)
{
    return alljoyn_aboutdata_setappname(m_aboutData, value, DEFAULT_LANGUAGE_FOR_ABOUT);
}

QStatus AllJoynAbout::SetDefaultAboutData()
{
    QStatus status = ER_OK;
    std::string deviceId;
    std::string deviceName;

    // only set the required fields:
    // - DefaultLanguage (already set upon m_aboutData creation)
    // - DeviceId
    // - DeviceName
    // - AppId
    // - AppName
    // - Manufacturer
    // - ModelNumber
    // - Description
    // - SoftwareVersion

    // app Id is always the bridge app Id
    alljoyn_aboutdata_setappid(m_aboutData, DSB_APP_GUID, sizeof(DSB_APP_GUID));

    // default device ID to bridge device Id
    status = GetDeviceID(deviceId);
    if (ER_OK == status)
    {
        alljoyn_aboutdata_setdeviceid(m_aboutData, deviceId.c_str());
    }

    // default about data to bridge about data
    SetDeviceName(UNKNOWN_ADAPTER);
    SetApplicationName(DSB_APP_NAME);
    SetManufacturer(DSB_MANUFACTURER);
    SetModel(DSB_MODEL);
    SetVersion(DSB_VERSION);
    SetDescription(DSB_DESCRIPTION);

    if (!alljoyn_aboutdata_isvalid(m_aboutData, DEFAULT_LANGUAGE_FOR_ABOUT))
    {
        status = ER_ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD;
    }

    return status;
}

QStatus AllJoynAbout::ReadDeviceID(_Out_ std::wstring &deviceId)
{
    QStatus status = ER_OK;

    auto localSettings = ApplicationData::Current->LocalSettings;
    auto container = localSettings->CreateContainer(APPDATA_CONTAINER_DSB_SETTINGS, ApplicationDataCreateDisposition::Always);

    // empty output string
    deviceId.clear();

    if (localSettings->Containers->HasKey(APPDATA_CONTAINER_DSB_SETTINGS))
    {
        auto values = localSettings->Containers->Lookup(APPDATA_CONTAINER_DSB_SETTINGS)->Values;

        //check if the id exists
        bool idPresent = values->HasKey(DSB_SETTING_DEVICE_ID);
        if (idPresent)
        {
            //get the value
            String^ value = dynamic_cast<String^>(values->Lookup(DSB_SETTING_DEVICE_ID));
            if (!value || value->IsEmpty())
            {
                status = ER_FAIL;
            }
            else
            {
                deviceId = value->Data();
            }
        }
        else
        {
            status = ER_FAIL;
        }
    }
    else
    {
        status = ER_FAIL;
    }

    return status;
}

QStatus AllJoynAbout::CreateAndSaveDeviceID(_Out_ std::wstring &deviceId)
{
    QStatus status = ER_OK;
    HRESULT hr = S_OK;

    GUID guid = { 0 };
    int length = 0;
    WCHAR *tempGuid = nullptr;
    WCHAR tempString[DEVICE_GUID_STRING_LEN];

    // empty output string
    deviceId.clear();

    //Create GUID
    hr = ::CoCreateGuid(&guid);
    if (FAILED(hr))
    {
        status = ER_FAIL;
        goto leave;
    }

    //Convert GUID into String
    length = ::StringFromGUID2(guid, tempString, DEVICE_GUID_STRING_LEN);
    if (0 == length)
    {
        status = ER_FAIL;
        goto leave;
    }

    // remove '{' and '}' from GUID string
    tempGuid = tempString;
    if (tempString[length - 2] == L'}')
    {
        tempString[length - 2] = L'\0';
    }
    if (tempString[0] == L'{')
    {
        tempGuid = &tempString[1];
    }

    deviceId = tempGuid;

    {
        //create the setting
        auto localSettings = ApplicationData::Current->LocalSettings;
        auto container = localSettings->CreateContainer(APPDATA_CONTAINER_DSB_SETTINGS, ApplicationDataCreateDisposition::Always);

        if (localSettings->Containers->HasKey(APPDATA_CONTAINER_DSB_SETTINGS))
        {
            auto values = localSettings->Containers->Lookup(APPDATA_CONTAINER_DSB_SETTINGS)->Values;
            values->Insert(DSB_SETTING_DEVICE_ID, ref new String(deviceId.c_str()));
        }
        else
        {
            status = ER_FAIL;
            goto leave;
        }
    }

leave:
    if (ER_OK != status)
    {
        // reset device Id in case of error
        deviceId.clear();
    }
    return status;
}

QStatus AllJoynAbout::GetDeviceID(_Out_ std::string &deviceId)
{
    QStatus status = ER_OK;
    std::wstring tempId;

    // reset out param
    deviceId.clear();

    // read device Id (create it if necessary)
    status = ReadDeviceID(tempId);
    if (status != ER_OK)
    {
        status = CreateAndSaveDeviceID(tempId);
        if (status != ER_OK)
        {
            goto leave;
        }
    }

    //convert types
    deviceId = To_Ascii_String(tempId);

leave:
    return status;
}

