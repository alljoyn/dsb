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
    /// <summary>
    /// Configure options for API. Current options select the type of receive 
    /// API frame to send out the Uart for received RF data packets. 
    /// </summary>
    class AO_Command : XBeeATCommand
    {
        private static readonly byte[] DEFAULT_API_MODE = { 0 };
        private static readonly byte[] EXPLICIT_RX_INDICATOR = { 1 };

        // API Option
        public AO_Command()
        {
            m_atCommmand = new byte[] { (byte)'A', (byte)'O' };
            m_responseSize = 3;  // 'AO' 0x00
            m_payload = DEFAULT_API_MODE;
        }
        public bool SetDefaultApiMode(XBeeModule xbeeCommand)
        {
            m_payload = DEFAULT_API_MODE;
            return SendAndWaitResponse(xbeeCommand);
        }
        public bool SetRxIndicatorMode(XBeeModule xbeeCommand)
        {
            m_payload = EXPLICIT_RX_INDICATOR;
            return SendAndWaitResponse(xbeeCommand);
        }
        public override bool ParseResponse(byte[] buffer)
        {
            if (!IsResponseOK(buffer))
            {
                // response not OK => do nothing
                return false;
            }

            // just need to response to be OK to get confirmation command
            // has been successful
            return true;
        }
    }
}