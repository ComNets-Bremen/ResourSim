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

#include "SimpleAirplaneMode.h"

namespace eventsimulator {

Define_Module(SimpleAirplaneMode);

SimpleAirplaneMode::SimpleAirplaneMode() {
    airplaneModeValues.setName("Airplane Mode on");
}

SimpleAirplaneMode::~SimpleAirplaneMode() {

}

void SimpleAirplaneMode::initialize() {
    EV_INFO << "Init airplane mode" << endl;
    // TODO: Init??
    initialized = true;
}

void SimpleAirplaneMode::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message->getPayloadType() != EVENT_TYPE_AIRPLANE_MODE) {
            // Not our message
            delete msg;
            return;
        }
        AirplaneModeEventMessage * airplaneMsg = check_and_cast<
                AirplaneModeEventMessage *>(msg);
        airplaneModeValues.record(airplaneMsg->getAirplaneModeOn());
        airplaneModeOn = airplaneMsg->getAirplaneModeOn();

        delete airplaneMsg;
    } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        EV_INFO << "Background Event Message" << endl;
        // TODO: Handle?
        delete msg;
    } else {
        EV_ERROR << "Unhandled message type" << endl;
        delete msg;
    }
}

void SimpleAirplaneMode::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "Airplane mode active: %i", airplaneModeOn);
    getDisplayString().setTagArg("t", 0, buf);

    if (airplaneModeOn)
        getDisplayString().setTagArg("i", 0, "status/airplane_mode_red");
    else
        getDisplayString().setTagArg("i", 0, "status/airplane_mode_neutral");
}

} // namespace
