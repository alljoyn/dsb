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
#include "LSFSignalHandler.h"

namespace BridgeRT
{
    class LSF;

    class LampState
    {
    public:
        LampState(LSF* pLightingService);
        virtual ~LampState();

        QStatus Initialize();
        
        QStatus Get(_In_z_ const char* propName, _Out_ alljoyn_msgarg val);
        QStatus Set(_In_z_ const char* propName, _In_ alljoyn_msgarg val);

        void RaiseStateChangedSignal();

    private:

        static void AJ_CALL AJMethod(_In_ alljoyn_busobject busObject, _In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message msg);
        QStatus invokeTransitionLampState(_In_ alljoyn_message msg, _Out_ alljoyn_msgarg *outArgs);
        QStatus invokeApplyPulseEffect(_In_ alljoyn_message msg, _Out_ alljoyn_msgarg *outArgs);
        static LampState* GetInstance(_In_ alljoyn_busobject busObject);

        QStatus setStateParameter(_In_ alljoyn_msgarg msgArg, _Out_ State^ newState);

        LSF* m_pLightingService;
        alljoyn_interfacedescription m_lampStateInterface;
        alljoyn_interfacedescription_member m_methodTransitionLampState_Member;
        alljoyn_interfacedescription_member m_methodApplyPulseEffect_Member;

        IAdapterSignal^ m_pLampState_LampStateChanged;
        LSFSignalHandler^ m_pLSFSignalHandler;
    };
}