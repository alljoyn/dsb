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

namespace BridgeRT
{
    class LampService;
    class LampParameters;
    class LampDetails;
    class LampState;
    
    class LSF
    {
    public:
        LSF(_In_ IAdapterDevice^ pLightingDevice);
        virtual ~LSF();

        QStatus Initialize(_In_ alljoyn_busattachment bus);

        alljoyn_busattachment GetBus()
        {
            return m_bus;
        }

        alljoyn_busobject GetBusObject()
        {
            return m_busObject;
        }

        ILSFHandler^ GetLSFHandler()
        {
            return m_pLightingServiceHandler;
        }

        LampService* GetLampServiceInterfacePtr()
        {
            return m_pLampServiceInterface;
        }

        LampParameters* GetLampParametersInterfacePtr()
        {
            return m_pLampParametersInterface;
        }

        LampDetails* GetLampDetailsInterfacePtr()
        {
            return m_pLampDetailsInterface;
        }

        LampState* GetLampStateInterfacePtr()
        {
            return m_pLampStateInterface;
        }

    private:
        static QStatus AJ_CALL getProperty(_In_ const void* context, _In_z_ const char* ifcName, _In_z_ const char* propName, _Out_ alljoyn_msgarg val);
        static QStatus AJ_CALL setProperty(_In_ const void* context, _In_z_ const char* ifcName, _In_z_ const char* propName, _In_ alljoyn_msgarg val);

        bool m_bRegistered;
        alljoyn_busobject m_busObject;
        alljoyn_busattachment m_bus;

        ILSFHandler^ m_pLightingServiceHandler;

        LampService* m_pLampServiceInterface;
        LampParameters* m_pLampParametersInterface;
        LampDetails* m_pLampDetailsInterface;
        LampState* m_pLampStateInterface;
        
        IAdapterDevice^ m_pLightingDevice;
    };
}
