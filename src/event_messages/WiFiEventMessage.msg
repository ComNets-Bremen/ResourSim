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

// A message type representing the status of the WiFi connection
// This file also implements some helper functions

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
           WIFI_UNKNOWN = -1,
    };

	// Return the WiFi status as a string from the enum
    static inline std::string getWiFiStatusString(WiFiStatusCode code){
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
                {WIFI_UNKNOWN, "WIFI_UNKNOWN"},
            };
        return WiFiStatusCodeMap[code];
    }
    
    // Return the WiFi status as a string from an int
    static inline std::string getWiFiStatusString(int code){
        return getWiFiStatusString(static_cast<WiFiStatusCode>(code));
    }
    
    // Convert a string code back to the enum
    static inline WiFiStatusCode getWiFiStatusCode(std::string code){
		if (code == "WIFI_AUTHENTICATING")
			return WIFI_AUTHENTICATING;
        if (code == "WIFI_BLOCKED")
        	return WIFI_BLOCKED;
       	if (code == "WIFI_CAPTIVE_PORTAL_CHECK")
       		return WIFI_CAPTIVE_PORTAL_CHECK;
       	if (code == "WIFI_CONNECTED")
       		return WIFI_CONNECTED;
       	if (code == "WIFI_CONNECTING")
       		return WIFI_CONNECTING;
       	if (code == "WIFI_DISCONNECTED")
       		return WIFI_DISCONNECTED;
       	if (code == "WIFI_FAILED")
       		return WIFI_FAILED;
       	if (code == "WIFI_IDLE")
       		return WIFI_IDLE;
       	if (code == "WIFI_OBTAINING_IPADDR")
       		return WIFI_OBTAINING_IPADDR;
       	if (code == "WIFI_SCANNING")
       		return WIFI_SCANNING;
       	if (code == "WIFI_SUSPENDED")
       		return WIFI_SUSPENDED;
       	if (code == "WIFI_VERIFYING_POOR_LINK")
       		return WIFI_VERIFYING_POOR_LINK;
       	return WIFI_UNKNOWN;
    }
    
    // Check if WiFi is occupied by the user
    static bool getWiFiIsOccupied(int code){
        switch(static_cast<WiFiStatusCode>(code)){
            case WIFI_CONNECTING:
            case WIFI_CONNECTED:
            case WIFI_AUTHENTICATING:
            case WIFI_OBTAINING_IPADDR:
            case WIFI_VERIFYING_POOR_LINK:
            	return true;
            	break;
            default:
            	return false;
        }
        return false;        
    }
}}

message BaseEventMessage;

message WiFiEventMessage extends BaseEventMessage {
    payloadType=EVENT_TYPE_WIFI;
    int wifi_status;
}
