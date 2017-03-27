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

namespace BridgeRT
{
    class BridgeAuthHandler
    {
    public:
        BridgeAuthHandler();
        ~BridgeAuthHandler();

		inline QStatus InitializeWithKeyXAuthentication(_In_ alljoyn_busattachment bus, _In_ Platform::String^ keyX)
		{
			return InitializeWithAllAuthenticationMethods(bus, keyX, nullptr, nullptr, nullptr, nullptr);
		}
        inline QStatus InitializeWithCredentialAuthentication(_In_ alljoyn_busattachment bus, _In_ Platform::String^ username, _In_ Platform::String^ password)
        {
            return InitializeWithAllAuthenticationMethods(bus, nullptr, username, password, nullptr, nullptr);
        }
        inline QStatus InitializeWithEcdheEcdsaAuthentication(_In_ alljoyn_busattachment bus, _In_ Platform::String^ ecdheEcdsaPrivateKey, _In_ Platform::String^ ecdheEcdsaCertChain)
        {
            return InitializeWithAllAuthenticationMethods(bus, nullptr, nullptr, nullptr, ecdheEcdsaPrivateKey, ecdheEcdsaCertChain);
        }
        void ResetAccess(_In_ std::string remoteName);

        QStatus InitializeWithAllAuthenticationMethods(_In_ alljoyn_busattachment bus,
            _In_ Platform::String^ keyx,
            _In_ Platform::String^ username, _In_ Platform::String^ password,
            _In_ Platform::String^ ecdheEcdsaPrivateKey, _In_ Platform::String^ ecdheEcdsaCertChain);

        void ShutDown();

    private:
        alljoyn_busattachment m_busAttachment;
        alljoyn_authlistener m_authListener;
        std::string m_keyx;
		std::string m_username;
		std::string m_password;
        std::string m_ecdheEcdsaPrivateKey;
        std::string m_ecdheEcdsaCertChain;

		static QStatus AJ_CALL ReqCredentialsAsync(
            void* context,
            alljoyn_authlistener listener,
            const char* authMechanism,
            const char* authPeer,
            uint16_t authCount,
            const char* userId,
            uint16_t credMask,
            void* authContext);

        static void AJ_CALL AuthComplete(
            const void* context,
            const char* authMechanism,
            const char* authPeer,
            QCC_BOOL success);

        static QStatus AJ_CALL VerifyCredentialsAsync(
            const void*context,
            alljoyn_authlistener listener,
            const char* authMechanism,
            const char* authPeer,
            const alljoyn_credentials creds,
            void*authContext);
    };
}