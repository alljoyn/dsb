//
// Copyright (c) 2015, Microsoft Corporation
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
// IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

#pragma once

namespace DeviceProviders
{
    interface class IAboutData;
    interface class IBusObject;
    interface class IProvider;
    ref class AllJoynStatus;

    public interface class IService
    {
        property Windows::Foundation::Collections::IVector<IBusObject ^>^ Objects
        {
            Windows::Foundation::Collections::IVector<IBusObject ^>^ get();
        }
        property Platform::String ^ Name
        {
            Platform::String ^ get();
        }
        property uint16 SessionPort
        {
            uint16 get();
        }
        property uint32 SessionId
        {
            uint32 get();
        }
        property IAboutData^ AboutData
        {
            IAboutData^ get();
        }
        property IProvider^ Provider
        {
            IProvider^ get();
        }

        AllJoynStatus^ Ping();
        bool ImplementsInterface(Platform::String^ interfaceName);
        IBusObject^ GetBusObject(Platform::String^ path);
        Windows::Foundation::Collections::IVector<IBusObject^>^ GetBusObjectsWhichImplementInterface(Platform::String^ interfaceName);
    };
}