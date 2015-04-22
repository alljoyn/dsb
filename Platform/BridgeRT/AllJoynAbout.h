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

#include <alljoyn_c/BusAttachment.h>
#include <alljoyn_c/BusObject.h>
#include <alljoyn_c/AboutData.h>
#include <alljoyn_c/AboutObj.h>

#include "Misc.h"

namespace BridgeRT
{
	class AllJoynAbout
	{
	public:
		AllJoynAbout();
		~AllJoynAbout();

		QStatus Initialize(_In_ alljoyn_busattachment bus);
		void ShutDown();

		QStatus Announce();
        QStatus AddObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription);
        QStatus RemoveObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription);

		QStatus SetManufacturer(_In_z_ const wchar_t *value);
		QStatus SetDeviceName(_In_z_ const wchar_t *value);
		QStatus SetVersion(_In_z_ const wchar_t *value);
		QStatus SetApplicationName(_In_z_ const char *value);
        QStatus SetDeviceId(_In_z_ const wchar_t *value);
        QStatus SetModel(_In_z_ const wchar_t *value);
        QStatus SetDescription(_In_z_ const wchar_t *value);

	private:
		alljoyn_aboutdata m_aboutData;
		alljoyn_aboutobj m_aboutObject;
		bool m_isAnnounced;

		QStatus SetDefaultAboutData();

		QStatus GetDeviceID(_Out_ std::string &deviceId);
		QStatus CreateAndSaveDeviceID(_Out_ std::wstring &deviceId);
		QStatus ReadDeviceID(_Out_ std::wstring &deviceId);
	};
}
