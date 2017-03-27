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

#include "Bridge.h"

namespace BridgeRT
{
    using namespace ::Platform;
    using namespace ::BackgroundHost;
    using namespace ::Windows::Foundation;

    public interface class IAdapterFactory
    {
        IAdapter^ CreateAdapter();
    };

    public ref class AdapterBridgeServiceImplementation sealed : public IServiceImplementation
    {
    public:
        AdapterBridgeServiceImplementation(IAdapterFactory^ adapterFactory)
            : _adapterFactory(adapterFactory)
        {
            if (!adapterFactory)
                throw ref new InvalidArgumentException(L"adapterFactory");
        }

        virtual void Start()
        {
            _adapter = _adapterFactory->CreateAdapter();
            _dsbBridge = ref new DsbBridge(_adapter);

            HRESULT hr = _dsbBridge->Initialize();
            if (FAILED(hr))
            {
                throw ref new Exception(hr, "Device System Bridge initialization failed!");
            }
        }

        virtual void Stop()
        {
            if (_dsbBridge != nullptr)
            {
                _dsbBridge->Shutdown();
                _dsbBridge = nullptr;
            }

            if (_adapter != nullptr)
            {
                _adapter->Shutdown();
                _adapter = nullptr;
            }
        }
        
        virtual event TypedEventHandler<IServiceImplementation ^, String ^> ^ Failed;

        virtual void RaiseFailedEvent(String ^desc) { this->Failed(this, desc); }
        
    private:
        IAdapterFactory^ _adapterFactory;
        IAdapter^ _adapter;
        DsbBridge^ _dsbBridge;
    };
}