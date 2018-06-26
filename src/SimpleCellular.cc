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

SimpleCellular::SimpleCellular(){
    cellularStatusValues.setName("Cellular Status");
}

SimpleCellular::~SimpleCellular(){

}

void SimpleCellular::initialize()
{
    EV << "Init cellular status" << endl;
    // TODO: Init something?
    initialized = true;
}

void SimpleCellular::handleMessage(cMessage *msg)
{
    if (check_and_cast<BaseEventMessage *>(msg)->getPayloadType() != EVENT_TYPE_CELLULAR){
        // Not our message
        delete msg;
        return;
    }
    CellularEventMessage *cellularMsg = check_and_cast<CellularEventMessage *>(msg);
    cellularStatus = cellularMsg->getCellular_state();
    networkTypeName = cellularMsg->getCellular_type();
    cellularStatusValues.record(cellularStatus);

    switch(cellularStatus){
        // TODO: Check if all offers internet connectivity
        case CELLULAR_CONNECTED:
        case CELLULAR_CONNECTING:
            cellularIsUsed = true;
            break;
        default:
            cellularIsUsed = false;
    }
    // TODO: Check if transition is valid?

    delete cellularMsg;
}

void SimpleCellular::refreshDisplay() const {
    std::string msgString;
    msgString = "Cellular status: " + getCellularStateString(cellularStatus) + "\n";
    msgString += "Type: " + networkTypeName;

    getDisplayString().setTagArg("t", 0, msgString.c_str());
    if (cellularIsUsed)
        getDisplayString().setTagArg("i", 0, "status/cell_active");
    else
        getDisplayString().setTagArg("i", 0, "status/cell_idle");
}

} //namespace
