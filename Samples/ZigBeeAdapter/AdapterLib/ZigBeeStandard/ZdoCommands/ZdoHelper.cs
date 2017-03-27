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

namespace AdapterLib
{
    class ZdoHelper
    {
        // ZDO error codes
        public const byte ZDO_ERROR_SUCCESS = 0x00;
        public const byte ZDO_NOT_SUPPORTED = 0x84;
        public const byte ZDO_ERROR_TIMEOUT = 0x85;
        public const byte APS_ILLEGAL_REQUEST = 0xA3;
        public const byte APS_NOT_SUPPORTED = 0xAA;

        public static int ZigBeeStatusToHResult(byte zigBeeError)
        {
            int hresult = AdapterHelper.E_FAIL;

            switch (zigBeeError)
            {
                case ZDO_ERROR_SUCCESS:
                    hresult = AdapterHelper.S_OK;
                    break;

                case APS_NOT_SUPPORTED:     // fall through OK
                case ZDO_NOT_SUPPORTED:
                    {
                        System.NotSupportedException ex = new NotSupportedException();
                        hresult = ex.HResult;
                    }
                    break;

                case ZDO_ERROR_TIMEOUT:
                    {
                        System.TimeoutException ex = new TimeoutException();
                        hresult = ex.HResult;
                    }
                    break;
            }
            return hresult;
        }
    }
}