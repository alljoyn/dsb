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

#pragma once

namespace AdapterLib
{
    //
    // AdapterConfig.
    // Description:
    //  The class that includes:
    //  - ZWave adapter configuration parameters.
    //  - OpenZWave (stack) parameters.
    //
    class AdapterConfig
    {
    public:
        // Constructor & Destructor
        AdapterConfig();
        virtual ~AdapterConfig();

        //*****************************************************************************************************
        //
        //	Reads the adapter configurations into an XML string.
        //
        //	XmlStringPtr: Returned adapter configuration in string format.
        //
        //	returns: S_OK on success, other HRESULT on fail
        //
        //*****************************************************************************************************
        HRESULT GetConfig(_Out_ Platform::String^* XmlStringPtr);

        //*****************************************************************************************************
        //
        //	Loads new configuration parameters from an XML string, and saves them
        //  to the adapter configuration file.
        //
        //	XmlString: A complete XML configuration
        //
        //	returns: S_OK on success, other HRESULT on fail
        //
        //*****************************************************************************************************
        HRESULT SetConfig(_In_ Platform::String^ XmlString);

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

    public:
        static std::wstring GetConfigPath();
        static std::wstring GetUserPath();
    };

}