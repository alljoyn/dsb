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

#include <collection.h>
#include <algorithm>

namespace BridgeRT
{
    //
    // Adapter objects vector/vector interfaces
    //

    interface class IAdapterValue;
    typedef Platform::Collections::Vector<IAdapterValue^> AdapterValueVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterValue^> IAdapterValueVector;
    typedef Windows::Foundation::PropertyType AdapterValueType;

    interface class IAdapterAttribute;
    typedef Platform::Collections::Vector<IAdapterAttribute^> AdapterAttributeVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterAttribute^> IAdapterAttributeVector;

    interface class IAdapterProperty;
    typedef Platform::Collections::Vector<IAdapterProperty^> AdapterPropertyVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterProperty^> IAdapterPropertyVector;

    interface class IAdapterMethod;
    typedef Platform::Collections::Vector<IAdapterMethod^> AdapterMethodVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterMethod^> IAdapterMethodVector;

    interface class IAdapterSignal;
    typedef Platform::Collections::Vector<IAdapterSignal^> AdapterSignalVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterSignal^> IAdapterSignalVector;

    interface class IAdapterDevice;
    typedef Platform::Collections::Vector<IAdapterDevice^> AdapterDeviceVector;
    typedef Windows::Foundation::Collections::IVector<IAdapterDevice^> IAdapterDeviceVector;

    typedef Platform::Collections::Map<Platform::String^, Platform::String^> AnnotationMap;
    typedef Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^> IAnnotationMap;
}