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
