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


// A message type representing a cellular event message and the corresponding
// handling classes

cplusplus {{
#include "BaseEventMessage_m.h"


	// The Cellular State codes from Android
    enum CellularStateCode {
        CELLULAR_UNKNOWN = -1,
        CELLULAR_CONNECTED = 4,
        CELLULAR_CONNECTING = 5,
        CELLULAR_DISCONNECTED = 6,
        CELLULAR_SUSPENDED = 11,
    };

	// Return the Cellular state as a string from the enum
    static inline std::string getCellularStatusString(CellularStateCode code){
        std::map<CellularStateCode, std::string> CellularStateCodeMap = {
            {CELLULAR_UNKNOWN, "CELLULAR_UNKNOWN"},
            {CELLULAR_CONNECTED, "CELLULAR_CONNECTED"},
            {CELLULAR_CONNECTING, "CELLULAR_CONNECTING"},
            {CELLULAR_DISCONNECTED, "CELLULAR_DISCONNECTED"},
            {CELLULAR_SUSPENDED, "CELLULAR_SUSPENDED"},
            };
        return CellularStateCodeMap[code];
    }
    
    // Return the Cellular state as a string from an int
    static inline std::string getCellularStatusString(int code){
        return getCellularStatusString(static_cast<CellularStateCode>(code));
    }
}}
message BaseEventMessage;

message CellularEventMessage extends BaseEventMessage {
    payloadType=EVENT_TYPE_CELLULAR;
    int cellular_state;
    string cellular_type;
}
