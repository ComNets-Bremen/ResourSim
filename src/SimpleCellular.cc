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

#include "SimpleCellular.h"

namespace eventsimulator {

Define_Module(SimpleCellular);

SimpleCellular::SimpleCellular() {}

SimpleCellular::~SimpleCellular() {
    if (getSimulation()->getSystemModule()->isSubscribed(
            CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);

}

void SimpleCellular::initialize() {
    EV << "Init cellular status" << endl;

    cellularStatusValues.setName("Cellular Status");

    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
            this);

    // TODO: Init something?
    initialized = true;
}

void SimpleCellular::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        if (cellularIsUsed) {
            EV_INFO << "Recalc used energy for cellular network due to regular event." << std::endl;
            simtime_t duration = simTime() - startOccupiedTime;
            startOccupiedTime = simTime();
            sendBatteryConsumptionEvent(duration);
        }
    }

}

void SimpleCellular::handleMessage(cMessage *msg) {
    if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        EV << "Background Message" << endl;
        // TODO: implement?
        delete msg;
    } else if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        if (check_and_cast<BaseEventMessage *>(msg)->getPayloadType()
                != EVENT_TYPE_CELLULAR) {
            // Not our message
            delete msg;
            return;
        }
        bool previousCellularIsUsed = cellularIsUsed;

        CellularEventMessage *cellularMsg = check_and_cast<
                CellularEventMessage *>(msg);
        cellularStatus = cellularMsg->getCellular_state();
        networkTypeName = cellularMsg->getCellular_type();
        cellularStatusValues.record(cellularStatus);

        switch (cellularStatus) {
        // TODO: Check if all offer Internet connectivity
        case CELLULAR_CONNECTED:
        case CELLULAR_CONNECTING:
            cellularIsUsed = true;
            break;
        default:
            cellularIsUsed = false;
        }
        // TODO: Check if transition is valid?

        // Energy consumption checks
        if (cellularIsUsed != previousCellularIsUsed) {
            // State change
            if (cellularIsUsed) {
                // Switched on
                startOccupiedTime = simTime();
            } else {
                simtime_t duration = simTime() - startOccupiedTime;
                sendBatteryConsumptionEvent(duration);
            }
        }

        delete cellularMsg;
    } else {
        EV_ERROR << "Unknown message type" << endl;
        delete msg;
    }
}

void SimpleCellular::refreshDisplay() const {
    std::string msgString;
    msgString = "Cellular status: " + getCellularStatusString(cellularStatus)
            + "\n";
    msgString += "Type: " + networkTypeName;

    getDisplayString().setTagArg("t", 0, msgString.c_str());
    if (cellularIsUsed)
        getDisplayString().setTagArg("i", 0, "status/cell_active");
    else
        getDisplayString().setTagArg("i", 0, "status/cell_idle");
}

void SimpleCellular::sendBatteryConsumptionEvent(simtime_t duration) {
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_CELLULAR);
    double chargeChange = -1 * par("cellularCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "Cell used " << chargeChange << "C (As) (" << duration << "s)"
                   << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb
    cEvent->setDischargeDuration(duration);


    if (gateSize("out") < 1)
        throw cRuntimeError("Invalid number of output gates: %d; must be >=1",
                gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
