﻿// 
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
    class BasicCluster : ZclCluster
    {
        internal const UInt16 CLUSTER_ID = 0x0000;
        internal const string CLUSTER_NAME = "Basic";

        internal const UInt16 ATTRIBUTE_ZCL_VERSION           = 0x0000;
        internal const UInt16 ATTRIBUTE_APPLICATION_VERSION   = 0x0001;
        internal const UInt16 ATTRIBUTE_STACK_VERSION         = 0x0002;
        internal const UInt16 ATTRIBUTE_HW_VERSION            = 0x0003;
        internal const UInt16 ATTRIBUTE_MANUFACTURER_NAME     = 0x0004;
        internal const UInt16 ATTRIBUTE_MODEL_IDENTIFIER      = 0x0005;
        internal const UInt16 ATTRIBUTE_DATE_CODE             = 0x0006;
        internal const UInt16 ATTRIBUTE_POWER_SOURCE          = 0x0007;

        internal const UInt16 ATTRIBUTE_LOCATION_DESCRIPTION  = 0x0010;
        internal const UInt16 ATTRIBUTE_PHYSICAL_ENVIRONMENT  = 0x0011;
        internal const UInt16 ATTRIBUTE_DEVICE_ENABLE         = 0x0012;
        internal const UInt16 ATTRIBUTE_ALARM_MASK            = 0x0013;
        internal const UInt16 ATTRIBUTE_DISABLE_LOCAL_CONFIG  = 0x0014;

        internal BasicCluster(ZigBeeEndPoint endPoint)
            : base(endPoint)
        {
            // set cluster name and cluster Id
            Name = CLUSTER_NAME;
            m_Id = CLUSTER_ID;

            // Basic cluster attributes
            var attrib = new ZclAttribute(this, ATTRIBUTE_ZCL_VERSION, "ZclVersion", ZclHelper.UINT8_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_APPLICATION_VERSION, "ApplicationVersion", ZclHelper.UINT8_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_STACK_VERSION, "StackVersion", ZclHelper.UINT8_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_HW_VERSION, "HwVersion", ZclHelper.UINT8_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_MANUFACTURER_NAME, "ManufacturerName", ZclHelper.CHAR_STRING_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_MODEL_IDENTIFIER, "ModelIdentifier", ZclHelper.CHAR_STRING_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_DATE_CODE, "DateCode", ZclHelper.CHAR_STRING_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_POWER_SOURCE, "PowerSource", ZclHelper.ENUMERATION_8_BIT_TYPE, true);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_LOCATION_DESCRIPTION, "LocationDescription", ZclHelper.CHAR_STRING_TYPE);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_PHYSICAL_ENVIRONMENT, "PhysicalEnvironment", ZclHelper.ENUMERATION_8_BIT_TYPE);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_DEVICE_ENABLE, "DeviceEnable", ZclHelper.BOOLEAN_TYPE);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_ALARM_MASK, "AlarmMask", ZclHelper.BITMAP_8_BIT_TYPE);
            m_attributeList.Add(attrib.Id, attrib);

            attrib = new ZclAttribute(this, ATTRIBUTE_DISABLE_LOCAL_CONFIG, "DisableLocalConfig", ZclHelper.BITMAP_8_BIT_TYPE);
            m_attributeList.Add(attrib.Id, attrib);
            
            // call parent class "post constructor"
            PostChildClassConstructor();
        }
    }
}