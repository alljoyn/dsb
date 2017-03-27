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

#pragma once

#include <alljoyn_c/BusAttachment.h>
#include <alljoyn_c/BusObject.h>
#include <alljoyn_c/AboutData.h>
#include <alljoyn_c/AboutObj.h>

#include "DsbServiceNames.h"

namespace BridgeRT
{
	class AllJoynAbout
	{
	public:
		AllJoynAbout();
		~AllJoynAbout();

		QStatus Initialize(_In_ alljoyn_busattachment bus);
		void ShutDown();

		QStatus Announce(_In_ alljoyn_sessionport sp=DSB_SERVICE_PORT);
        QStatus AddObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription);
        QStatus RemoveObject(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription interfaceDescription);

		QStatus SetManufacturer(_In_z_ const wchar_t *value);
		QStatus SetDeviceName(_In_z_ const wchar_t *value);
		QStatus SetSWVersion(_In_z_ const wchar_t *value);
        QStatus SetHWVersion(_In_z_ const wchar_t *value);
        QStatus SetApplicationName(_In_z_ const wchar_t *value);
        QStatus SetApplicationGuid(_In_ const GUID &value);
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