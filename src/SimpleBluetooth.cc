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

#include "SimpleBluetooth.h"

namespace eventsimulator {

Define_Module(SimpleBluetooth);

SimpleBluetooth::SimpleBluetooth() {
    bluetoothStatusValues.setName("Bluetooth Status");
}

SimpleBluetooth::~SimpleBluetooth() {

}

void SimpleBluetooth::initialize() {
    EV << "Init Bluetooth status" << endl;
    // TODO: Init something?
    initialized = true;
}

void SimpleBluetooth::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        if (check_and_cast<BaseEventMessage *>(msg)->getPayloadType()
                != EVENT_TYPE_BLUETOOTH) {
            // Not our message
            delete msg;
            return;
        }
        BluetoothEventMessage *bluetoothMsg = check_and_cast<
                BluetoothEventMessage *>(msg);
        bluetoothStatus = bluetoothMsg->getBluetooth_status();
        bluetoothStatusValues.record(bluetoothStatus);

        switch (bluetoothStatus) {
        case BLUETOOTH_STATE_ON:
        case BLUETOOTH_STATE_TURNING_ON:
        case BLUETOOTH_STATE_TURNING_OFF:
            bluetoothIsUsed = true;
            break;
        default:
            bluetoothIsUsed = false;
        }
        // TODO: Check if transition is valid?

        delete bluetoothMsg;
    } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        EV_INFO << "Background Event Message" << endl;
        // TODO implement
        delete msg;
    } else {
        EV_ERROR << "Unhandled message type" << endl;
        delete msg;
    }
}

void SimpleBluetooth::refreshDisplay() const {
    std::string statusString;
    statusString = "Bluetooth status: "
            + getBluetoothStatusString(bluetoothStatus);

    getDisplayString().setTagArg("t", 0, statusString.c_str());
    if (bluetoothIsUsed)
        getDisplayString().setTagArg("i", 0, "status/bluetooth_green");
    else
        getDisplayString().setTagArg("i", 0, "status/bluetooth_neutral");
}

} //namespace
