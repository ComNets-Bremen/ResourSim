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
    if (getSimulation()->getSystemModule()->isSubscribed(
    CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);

}

void SimpleBluetooth::initialize() {
    EV << "Init Bluetooth status" << endl;

    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
                this);
    // TODO: Init something?
    initialized = true;
}

void SimpleBluetooth::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        if (bluetoothIsUsed) {
            EV_INFO << "Recalc used energy for Bluetooth network due to regular event." << std::endl;
            simtime_t duration = simTime() - startOccupiedTime;
            startOccupiedTime = simTime();
            sendBatteryConsumptionEvent(duration);
        }
    }

}

void SimpleBluetooth::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        if (check_and_cast<BaseEventMessage *>(msg)->getPayloadType()
                != EVENT_TYPE_BLUETOOTH) {
            // Not our message
            delete msg;
            return;
        }

        bool previousBluetoothIsUsed = bluetoothIsUsed;
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

        // Handle energy consumption
        if (previousBluetoothIsUsed != bluetoothIsUsed) {
            // State changed
            if (bluetoothIsUsed) {
                startOccupiedTime = simTime();
            } else {
                simtime_t duration = simTime() - startOccupiedTime;
                sendBatteryConsumptionEvent(duration);
            }
        }
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

void SimpleBluetooth::sendBatteryConsumptionEvent(simtime_t duration) {
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_BLUETOOTH);
    double chargeChange = -1 * par("bluetoothCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "Used " << chargeChange << "C (As) (" << duration << "s)"
                   << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb

    if (gateSize("out") < 1)
        throw cRuntimeError("Invalid number of output gates: %d; must be >=1",
                gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
