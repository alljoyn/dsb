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
    class TemperatureCluster : ZclCluster
    {
        internal const UInt16 CLUSTER_ID = 0x0402;
        internal const string CLUSTER_NAME = "Temperature";
        internal const UInt16 ATTRIBUTE_TEMPERATURE = 0x0000;
        internal const UInt16 ATTRIBUTE_MINMEASURED = 0x0001;
        internal const UInt16 ATTRIBUTE_MAXMEASURED = 0x0002;
        internal const UInt16 ATTRIBUTE_TOLERANCE = 0x0003;

        public TemperatureCluster(ZigBeeEndPoint endPoint, List<UInt16> supportedAttributes) 
            : base(endPoint)
        {
            // set cluster name and cluster Id
            Name = CLUSTER_NAME;
            m_Id = CLUSTER_ID;

            // if supportedAttributes is null assume all attributes supported
            bool supportAllAttributesBydefault = false;
            if (supportedAttributes == null)
            {
                supportAllAttributesBydefault = true;
            }

            // Temperature attribute (read only)
            if (supportAllAttributesBydefault ||
               supportedAttributes.Contains(ATTRIBUTE_TEMPERATURE))
            {
                var attrib = new ZclAttribute(this, ATTRIBUTE_TEMPERATURE, "MeasuredValue",
                    ZclHelper.INT16_TYPE, true, false, true);
                m_attributeList.Add(attrib.Id, attrib);
            }
            if (supportAllAttributesBydefault ||
               supportedAttributes.Contains(ATTRIBUTE_MINMEASURED))
            {
                var attrib = new ZclAttribute(this, ATTRIBUTE_MINMEASURED, "MinMeasuredValue ",
                    ZclHelper.INT16_TYPE, true, false);
                m_attributeList.Add(attrib.Id, attrib);
            }
            if (supportAllAttributesBydefault ||
               supportedAttributes.Contains(ATTRIBUTE_MAXMEASURED))
            {
                var attrib = new ZclAttribute(this, ATTRIBUTE_MAXMEASURED, "MaxMeasuredValue ",
                    ZclHelper.INT16_TYPE, true, false);
                m_attributeList.Add(attrib.Id, attrib);
            }
            if (supportAllAttributesBydefault ||
               supportedAttributes.Contains(ATTRIBUTE_TOLERANCE))
            {
                var attrib = new ZclAttribute(this, ATTRIBUTE_TOLERANCE, "Tolerance",
                    ZclHelper.UINT16_TYPE, true, true);
                m_attributeList.Add(attrib.Id, attrib);
            }

            // call parent class "post constructor"
            PostChildClassConstructor();
        }
    }
}