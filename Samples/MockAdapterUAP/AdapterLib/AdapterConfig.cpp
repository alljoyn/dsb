//
// Device System Bridge MockAdapter
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
#include "AdapterConfig.h"

#define ADAPTER_CONFIG_XML_FILE L"AdapterConfig.xml"

const WCHAR DEFAULT_ADAPTER_CFG_XML[] =
L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
L"<AdapterCfg></AdapterCfg>\n";

AdapterConfig::AdapterConfig()
{
    m_pXmlDocument = ref new XmlDocument();
}

AdapterConfig::~AdapterConfig()
{
}

_Use_decl_annotations_
HRESULT
AdapterConfig::SetConfig(String^ xmlString)
{
    HRESULT hr = S_OK;

    //If the xml string is empty, then set default configurations
    if (xmlString == nullptr)
    {
        hr = E_INVALIDARG;
        goto CleanUp;
    }
    else
    {
        CHK_HR(fromString(xmlString));
        CHK_HR(toFile());
    }

CleanUp:
    return hr;
}

_Use_decl_annotations_
HRESULT
AdapterConfig::GetConfig(String^* xmlString)
{
    HRESULT hr = S_OK;
    CHK_HR(fromFile());
    CHK_HR(toString(xmlString));

CleanUp:
    return hr;
}

HRESULT
AdapterConfig::Init()
{
    HRESULT hr = S_OK;
    if (!isConfigFilePresent())
    {
        CHK_HR(initXml());
        CHK_HR(toFile());
    }

CleanUp:
    return hr;
}

HRESULT
AdapterConfig::fromFile()
{
    HRESULT hr = S_OK;
    task<StorageFile^> getConfigFileTask;
    StorageFolder^ appFolder = ApplicationData::Current->LocalFolder;

    if (m_pXmlDocument == nullptr)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
        goto CleanUp;
    }

    // Load the XML file 
    try
    {
        getConfigFileTask = create_task(appFolder->GetFileAsync(ADAPTER_CONFIG_XML_FILE));
    }
    catch (Platform::Exception^ ex)
    {
        hr = ex->HResult;
        goto CleanUp;
    }

    getConfigFileTask
    .then([this](StorageFile^ sFile)
    {
        return  XmlDocument::LoadFromFileAsync(sFile);
    })
    .then([this](XmlDocument^ doc)
    {
        m_pXmlDocument = doc;
    })
    .then([&](task<void> checkExceptionTask)
    {
        try
        {
            checkExceptionTask.get();
        }
        catch (Platform::Exception^ ex)
        {
            hr = ex->HResult;
        }
    })
    .wait();

CleanUp:
    return hr;
}

_Use_decl_annotations_
HRESULT
AdapterConfig::fromString(String^ xmlString)
{
    HRESULT hr = S_OK;

    if (m_pXmlDocument == nullptr)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
        goto CleanUp;
    }

    m_pXmlDocument->LoadXml(xmlString);

CleanUp:
    return hr;
}

HRESULT
AdapterConfig::toFile()
{
    HRESULT hr = S_OK;
    task<StorageFile^> createConfigFileTask;
    StorageFolder^ appFolder = ApplicationData::Current->LocalFolder;

    // Save/Update the configuration file
    try
    {
        createConfigFileTask = create_task(appFolder->CreateFileAsync(
            ADAPTER_CONFIG_XML_FILE,
            CreationCollisionOption::ReplaceExisting));
    }
    catch (Platform::Exception^ ex)
    {
        hr = ex->HResult;
        goto CleanUp;
    }

    createConfigFileTask
    .then([this](StorageFile^ sFile)
    {
        return m_pXmlDocument->SaveToFileAsync(sFile);
    })
    .then([&](task<void> checkExceptionTask)
    {
        try
        {
            checkExceptionTask.get();
        }
        catch (Platform::Exception^ ex)
        {
            hr = ex->HResult;
        }
    })
    .wait();

CleanUp:
    return hr;
}

_Use_decl_annotations_
HRESULT
AdapterConfig::toString(String^* xmlString)
{
    HRESULT hr = S_OK;

    if (m_pXmlDocument == nullptr)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
        goto CleanUp;
    }

    *(xmlString) = m_pXmlDocument->GetXml();

CleanUp:
    return hr;
}

HRESULT
AdapterConfig::initXml()
{
    HRESULT hr = S_OK;
    StringReference target(DEFAULT_ADAPTER_CFG_XML);
    m_pXmlDocument->LoadXml(target.GetString());

    return hr;
}

bool
AdapterConfig::isConfigFilePresent()
{
    StorageFolder^ appFolder = ApplicationData::Current->LocalFolder;
    IStorageItem^ sItem;

    task<IStorageItem^> tryGetFileTask = create_task(appFolder->TryGetItemAsync(ADAPTER_CONFIG_XML_FILE));
    tryGetFileTask.then([&](IStorageItem^ item)
    {
        sItem = item;
    }).wait();

    return sItem != nullptr;
}