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
    /// Read firmware version of the module. The firmware version returns 4 hexadecimal values (2 bytes)
    /// "ABCD".  Digits ABC are the main release number and D is the revision number from the main release.
    /// "B" is a variant designator.
    /// </summary>
    class VR_Command : XBeeATCommand
    {
        private UInt16 m_SWVersion = 0;
        public UInt16 SWVersion
        {
            get { return m_SWVersion; }
        }
        // hardware version 
        public VR_Command()
        {
            m_atCommmand = new byte[] { (byte)'V', (byte)'R' };
            m_responseSize = 5;  // 'VR' 0x00 0x1X 0xXX (version)
        }
        public override bool ParseResponse(byte[] buffer)
        {
            if (!IsResponseOK(buffer))
            {
                // response not OK => do nothing
                return false;
            }

            m_SWVersion = AdapterHelper.UInt16FromXbeeFrame(buffer, DATA_OFFSET_IN_RESPONSE_BUFFER);

            return true;
        }
    }
}