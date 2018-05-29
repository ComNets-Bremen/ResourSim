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

	// The Bluetooth Status codes from Android
    enum BluetoothStatusCode {
        BLUETOOTH_STATE_ON = 11,
        BLUETOOTH_STATE_TURNING_ON = 12,
        BLUETOOTH_STATE_OFF = 10,
        BLUETOOTH_STATE_TURNING_OFF = 13,
    };

	// Return the Bluetooth status as a string from the enum
    inline std::string getBluetoothStatusString(BluetoothStatusCode code){
        std::map<BluetoothStatusCode, std::string> BluetoothStatusCodeMap = {
            {BLUETOOTH_STATE_ON, "BLUETOOTH_STATE_ON"},
            {BLUETOOTH_STATE_OFF, "BLUETOOTH_STATE_OFF"},
            {BLUETOOTH_STATE_TURNING_ON, "BLUETOOTH_STATE_TURNING_ON"},
            {BLUETOOTH_STATE_TURNING_OFF, "BLUETOOTH_STATE_TURNING_OFF"},
            };
        return BluetoothStatusCodeMap[code];
    }
    
    // Return the Bluetooth status as a string from an int
    inline std::string getBluetoothStatusString(int code){
        return getBluetoothStatusString(static_cast<BluetoothStatusCode>(code));
    }
}}
message BaseEventMessage;

//
// TODO generated message class
//
message BluetoothEventMessage extends BaseEventMessage {
    payloadType=EVENT_TYPE_BLUETOOTH;
    int bluetooth_status;
}
