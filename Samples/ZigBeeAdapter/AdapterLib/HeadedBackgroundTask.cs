﻿//
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BackgroundHost.Headed.Tasks;
using BridgeRT;
using Windows.ApplicationModel.Background;
using Windows.Foundation.Metadata;

namespace AdapterLib
{

    public sealed class HeadedBackgroundTask : IBackgroundTask, IAdapterFactory
    {      
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            var serviceImplementation = new AdapterBridgeServiceImplementation(this);

            var task = new HeadedBackgroundTaskImplementation();
            task.Run(taskInstance, serviceImplementation);
        }

        public IAdapter CreateAdapter()
        {
            return new Adapter();
        }
    }
}
