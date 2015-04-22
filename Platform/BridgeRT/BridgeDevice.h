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

#include <vector>
#include "BridgeAuthHandler.h"
#include "AllJoynAbout.h"

namespace BridgeRT
{
    class DeviceMain;
    class DeviceProperty;
    class PropertyInterface;

    ref class BridgeDevice sealed : IAdapterSignalListener
    {
    public:
        BridgeDevice();
        virtual ~BridgeDevice();

        // IAdapterSignalListener implementation
        virtual void AdapterSignalHandler(
            _In_ IAdapterSignal^ Signal,
            _In_opt_ Platform::Object^ Context
            );

    internal:
        QStatus Initialize(_In_ IAdapterDevice ^device);
        void Shutdown();

        IAdapterProperty ^GetAdapterProperty(_In_ std::string busObjectPath);
        std::string GetBusObjectPath(_In_ IAdapterProperty ^adapterProperty);
        bool IsEqual(_In_ IAdapterDevice ^device);
        bool IsBusObjectPathUnique(std::string &path);
        inline DWORD GetUniqueIdForProperty()
        {
            return m_uniqueIdForProperties++;
        }
        inline DWORD GetUniqueIdForInterface()
        {
            return m_uniqueIdForInterfaces++;
        }
        inline alljoyn_busattachment GetBusAttachment()
        {
            return m_AJBusAttachment;
        }
        inline IAdapterDevice ^GetAdapterDevice()
        {
            return m_device;
        }
        inline std::string &GetRootNameForInterface()
        {
            return m_RootStringForAllJoynNames;
        }
        inline DeviceMain *GetDeviceMainObject()
        {
            return m_deviceMain;
        }
        inline bool IsCOVSupported()
        {
            return m_supportCOVSignal;
        }

    private:
        void VerifyCOVSupport();
        HRESULT registerSignalHandlers(_In_ bool IsRegister);
        void HandleCOVSignal(_In_ IAdapterSignal ^signal);
        QStatus InitializeAllJoyn();
        QStatus ConnectToAllJoyn();
        void ShutdownAllJoyn();

        QStatus CreateDeviceProperties();
        PropertyInterface *FindMatchingInterfaceProperty(_In_ IAdapterProperty ^adapterProperty);
        QStatus CreateInterfaceProperty(_In_ IAdapterProperty ^adapterProperty, _Out_ PropertyInterface **propertyInterface);

        QStatus BuildServiceName();

        // callback for session listener
        static QCC_BOOL AJ_CALL AcceptSessionJoinerCallback(_In_ const void* context, _In_ alljoyn_sessionport sessionPort, _In_z_ const char* joiner, _In_ const alljoyn_sessionopts opts);
        static void AJ_CALL SessionJoined(_In_ void *context, _In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_z_ const char *joiner);
        static void AJ_CALL MemberRemoved(_In_ void* context, _In_ alljoyn_sessionid sessionid, _In_z_ const char* uniqueName);

        // list of active sessions
		std::vector<alljoyn_sessionid> m_activeSessions;
		
		// list of device properties
        std::map<std::string, DeviceProperty *>	m_deviceProperties;
        // list of AllJoyn interfaces that a device property can expose
        std::vector<PropertyInterface *>	m_propertyInterfaces;
        DWORD m_uniqueIdForProperties;
        DWORD m_uniqueIdForInterfaces;

        // main interface
        // that handles methods and signals defined at IAdapterDevice level
        DeviceMain *m_deviceMain;

        // authentication management
        BridgeAuthHandler m_authHandler;

        // corresponding adapter device
        IAdapterDevice ^m_device;
        bool m_supportCOVSignal;

        // AllJoyn related data
        alljoyn_busattachment m_AJBusAttachment;
        alljoyn_buslistener	m_AJBusListener;
        alljoyn_sessionportlistener	m_AJSessionPortListener;
        alljoyn_sessionlistener m_AJsessionListener;

        std::string m_RootStringForAllJoynNames;
        std::string m_ServiceName;

        // about service
        AllJoynAbout m_about;
    };
}
