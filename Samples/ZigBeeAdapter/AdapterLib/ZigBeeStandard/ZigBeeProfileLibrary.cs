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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdapterLib
{
    class ZigBeeProfileLibrary
    {
        // Singleton class 
        private static readonly ZigBeeProfileLibrary instance = new ZigBeeProfileLibrary();

        private const string UNKNOWN_PROFILE_NAME = "Unknown profile id";
        private const string UNKNOWN_DEVICE_NAME = "Unknown device type";

        // profile IDs
        private const UInt16 ZIGBEE_HOME_AUTOMATION_ID = 0x0104;
        private const UInt16 ZIGBEE_LIGHT_LINK_ID = 0xC05E;

        private Dictionary<UInt16, string> m_profiles = new Dictionary<UInt16, string>();
        private Dictionary<UInt16, Dictionary<UInt16, string>> m_devicesPerProfile = new Dictionary<UInt16, Dictionary<UInt16, string>>();

        private Dictionary<UInt16, string> m_deviceTypesZigBeeLightLink = new Dictionary<UInt16, string>();
        private Dictionary<UInt16, string> m_deviceTypesZigBeeHomeAutomation = new Dictionary<UInt16, string>();

        private ZigBeeProfileLibrary()
        {
            m_profiles.Add(ZIGBEE_LIGHT_LINK_ID, "ZigBee Light Link");
            m_profiles.Add(ZIGBEE_HOME_AUTOMATION_ID, "ZigBee Home Automation");
            m_devicesPerProfile.Add(ZIGBEE_LIGHT_LINK_ID, m_deviceTypesZigBeeLightLink);
            m_devicesPerProfile.Add(ZIGBEE_HOME_AUTOMATION_ID, m_deviceTypesZigBeeHomeAutomation);

            // supported ZigBee Light Link devices
            m_deviceTypesZigBeeLightLink.Add(0x0000, "On-off light");
            m_deviceTypesZigBeeLightLink.Add(0x0100, "Dimmable light");
            m_deviceTypesZigBeeLightLink.Add(0x0200, "Color light");
            m_deviceTypesZigBeeLightLink.Add(0x0210, "Extended color light");
            m_deviceTypesZigBeeLightLink.Add(0x0220, "Color temperature light");

            // supported ZigBee Home Automation devices
            m_deviceTypesZigBeeHomeAutomation.Add(0x000A, "Door lock");
            m_deviceTypesZigBeeHomeAutomation.Add(0x0100, "On-Off light");
            m_deviceTypesZigBeeHomeAutomation.Add(0x0101, "Dimmable light");
            m_deviceTypesZigBeeHomeAutomation.Add(0x0102, "Color dimmable light");
            m_deviceTypesZigBeeHomeAutomation.Add(0x0402, "Ias Zone");
        }

        public bool GetProfileAndDeviceNames(UInt16 profileId, UInt16 deviceId, out string profileName, out string deviceName)
        {
            profileName = UNKNOWN_PROFILE_NAME;
            deviceName = UNKNOWN_DEVICE_NAME;

            if(m_profiles.TryGetValue(profileId, out profileName))
            {
                Dictionary<UInt16, string> devicesDictionary = null;
                if (m_devicesPerProfile.TryGetValue(profileId, out devicesDictionary))
                {
                    if(devicesDictionary.TryGetValue(deviceId, out deviceName))
                    {
                        return true;
                    }
                    else
                    {
                        // set to unknown (TryGetValue "nullify" its out param)
                        deviceName = UNKNOWN_DEVICE_NAME;
                    }
                }
            }
            else
            {
                // set to unknown (TryGetValue "nullify" its out param)
                profileName = UNKNOWN_PROFILE_NAME;
            }

            return false;
        } 
        public static ZigBeeProfileLibrary Instance
        {
            get { return instance; }
        }
    }
}
