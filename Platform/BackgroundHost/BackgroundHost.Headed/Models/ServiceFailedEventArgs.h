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

namespace BackgroundHost { namespace Headed { namespace Models
{
    using namespace ::Platform;

    public ref class ServiceFailedEventArgs sealed
    {
    public:
        ServiceFailedEventArgs(Exception^ failureException)
            : _failureException(failureException)
        {
            if (!failureException)
                throw ref new InvalidArgumentException("failureException argument cannot be null");
        }

        [Windows::Foundation::Metadata::DefaultOverload]
        ServiceFailedEventArgs(String^ messageResourceKey)
            : _messageResourceKey(messageResourceKey)
        {
        }

        ServiceFailedEventArgs(String^ messageResourceKey, Exception^ failureException)
            : _messageResourceKey(messageResourceKey)
            , _failureException(failureException)
        {
            if (!failureException)
                throw ref new InvalidArgumentException("failureException argument cannot be null");
        }

        property String^ MessageResourceKey
        {
            String^ get() { return _messageResourceKey; }
        }

        property Exception^ FailureException
        {
            Exception^ get() { return _failureException; }
        }

    private:
        String^ _messageResourceKey;
        Exception^ _failureException;
    };
}}}