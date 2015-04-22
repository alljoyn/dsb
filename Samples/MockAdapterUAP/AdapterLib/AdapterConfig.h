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

#pragma once
#include <ppltasks.h>
#include <string>

using namespace concurrency;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel;
using namespace Platform;

class AdapterConfig
{
public:
    AdapterConfig();
    virtual ~AdapterConfig();

    //******************************************************************************************************
    //
    //  If the configuration file exists, then load it to the memory. Otherwise, create a configuration file
    //	with the default settings.
    //  
    //  returns: S_OK on success, 
    //           HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) if the file is missing.
    //           Other HRESULT on fail
    //      
    //******************************************************************************************************
    HRESULT Init();

    //*****************************************************************************************************
    //
    //	Writes the given XML configuration into the adapter configuration file.
    //
    //	xmlString: A complete XML configuration
    //
    //	returns: S_OK on success, other HRESULT on fail
    //
    //*****************************************************************************************************
    HRESULT SetConfig(_In_ String^ xmlString);

    //*****************************************************************************************************
    //
    //	Reads the adapter configurations into an XML string. 
    //
    //	xmlString:	Returned adapter configuration in string format.
    //
    //	returns: S_OK on success, other HRESULT on fail
    //
    //*****************************************************************************************************
    HRESULT GetConfig(_Out_ String^* xmlString);

private:
    //******************************************************************************************************
    //
    //  Loads adapter configuration from the xml file.
    //
    //  returns: S_OK on success, other HRESULT on fail
    //      
    //******************************************************************************************************
    HRESULT fromFile();

    //******************************************************************************************************
    //
    //  Loads this configuration object from the specified String.
    //  
    //  xmlString:  A complete XML configuration.
    //
    //  returns: S_OK on success, other HRESULT on fail
    //      
    //******************************************************************************************************
    HRESULT fromString(_In_ String^ xmlString);

    //******************************************************************************************************
    //
    //  Saves the current in memory XML file to disk
    // 
    //  returns: S_OK on success, other HRESULT on fail
    //      
    //******************************************************************************************************
    HRESULT toFile();

    //******************************************************************************************************
    //
    //  Converts the in memory XML configuration to a string
    //  
    //  xmlString:  The returned string-ized version of the current XML document
    //
    //  returns: S_OK on success, other HRESULT on fail
    //      
    //******************************************************************************************************
    HRESULT toString(_Out_ String^* xmlString);

    //******************************************************************************************************
    //	
    //	Loads default adapter configuration xml.
    //
    //	returns: S_OK on success, other HRESULT on fail
    //
    //******************************************************************************************************
    HRESULT initXml();

    //******************************************************************************************************
    //
    //	Checks if the configuration file exists.
    //
    //******************************************************************************************************
    bool isConfigFilePresent();

    //
    //  The current XML Document
    //
    XmlDocument^ m_pXmlDocument;

};