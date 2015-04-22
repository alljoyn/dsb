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

#pragma once
#include <string>

#include <alljoyn_c/status.h>

#include "AllJoynFileTransfer.h"
#include "BridgeConfig.h"

namespace BridgeRT
{
    class CspBridge : public AllJoynFileTransfer
	{
	public:

		CspBridge();
		virtual ~CspBridge();

		QStatus Initialize(_In_ alljoyn_busattachment* bus, _In_ ConfigManager *configManager, _In_ BridgeConfig& configFileName);
		inline const char *GetBusObjectPath()
		{
			return m_busObjectPath.c_str();
		}

        HRESULT PreFileReadAction(_Out_ std::wstring &appRelativeFileName);
		HRESULT PostFileWriteAction(_In_ std::wstring &appRelativeFileName, _Out_ HANDLE *finalEvent);
        HRESULT PostFileReadAction(void);

	private:
		std::string m_busObjectPath;
		std::wstring m_srcConfigFilePath;
        std::wstring m_tempConfigFilePath;

        ConfigManager *m_configManager;
	};
}

