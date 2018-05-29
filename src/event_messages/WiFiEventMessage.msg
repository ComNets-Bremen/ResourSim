//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

cplusplus {{
#include "BaseEventMessage_m.h"

	// The WiFi Status codes from Android
    enum WiFiStatusCode {
           WIFI_AUTHENTICATING = 1,
           WIFI_BLOCKED = 2,
           WIFI_CAPTIVE_PORTAL_CHECK = 3,
           WIFI_CONNECTED = 4,
           WIFI_CONNECTING = 5,
           WIFI_DISCONNECTED = 6,
           WIFI_FAILED = 7,
           WIFI_IDLE = 8,
           WIFI_OBTAINING_IPADDR = 9,
           WIFI_SCANNING = 10,
           WIFI_SUSPENDED = 11,
           WIFI_VERIFYING_POOR_LINK = 12,
    };

	// Return the WiFi status as a string from the enum
    inline std::string getWiFiStatusString(WiFiStatusCode code){
        std::map<WiFiStatusCode, std::string> WiFiStatusCodeMap = {
                {WIFI_AUTHENTICATING, "WIFI_AUTHENTICATING"},
                {WIFI_BLOCKED, "WIFI_BLOCKED"},
                {WIFI_CAPTIVE_PORTAL_CHECK, "WIFI_CAPTIVE_PORTAL_CHECK"},
                {WIFI_CONNECTED, "WIFI_CONNECTED"},
                {WIFI_CONNECTING, "WIFI_CONNECTING"},
                {WIFI_DISCONNECTED, "WIFI_DISCONNECTED"},
                {WIFI_FAILED, "WIFI_FAILED"},
                {WIFI_IDLE, "WIFI_IDLE"},
                {WIFI_OBTAINING_IPADDR, "WIFI_OBTAINING_IPADDR"},
                {WIFI_SCANNING, "WIFI_SCANNING"},
                {WIFI_SUSPENDED, "WIFI_SUSPENDED"},
                {WIFI_VERIFYING_POOR_LINK, "WIFI_VERIFYING_POOR_LINK"},
            };
        return WiFiStatusCodeMap[code];
    }
    
    // Return the WiFi status as a string from an int
    inline std::string getWiFiStatusString(int code){
        return getWiFiStatusString(static_cast<WiFiStatusCode>(code));
    }
}}

message BaseEventMessage;

//
// TODO generated message class
//
message WiFiEventMessage extends BaseEventMessage {
    payloadType=EVENT_TYPE_WIFI;
    int wifi_status;
}
