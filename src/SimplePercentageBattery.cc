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

// TODO
// - Be more generic in charging / discharging. Currently, only a replay is done.
// - ...

#include "SimplePercentageBattery.h"

namespace eventsimulator {
Define_Module(SimplePercentageBattery);

void SimplePercentageBattery::handleMessage(cMessage *msg){
    BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
    if (message->getPayloadType() != EVENT_TYPE_BATTERY){
        // Not our message
        delete msg;
        return;
    }

    BatteryEventMessage *batMsg = check_and_cast<BatteryEventMessage *>(msg);

    EV_INFO << "Got message! " << batMsg->str() << endl;
    if (!initialized){
        setBatteryPercentage(batMsg->getTheoreticalAbsolutePercentage());
        initialized = true;
    } else {
        incrementalBatteryChange(batMsg->getPercentage());
    }

    // Valid?
    if (!checkBatteryPercentageValid()){
        EV_ERROR << "WRONG BATTERY VALUES: " << batteryPercentage << endl;
    }

    EV_INFO << "Delta: " << (batMsg->getTheoreticalAbsolutePercentage() - batteryPercentage) << endl;
    //delete batMsg;
    delete msg;

}

SimplePercentageBattery::SimplePercentageBattery() {
    batteryPercentage = 50.0f;
    initialized = false;
}

SimplePercentageBattery::~SimplePercentageBattery() {
    // TODO Auto-generated destructor stub
}

double SimplePercentageBattery::getBatteryPercentage(){
    return batteryPercentage;
}

void SimplePercentageBattery::setBatteryPercentage(double percentage){
    batteryPercentage = percentage;
}

bool SimplePercentageBattery::checkBatteryPercentageValid(){
    return (batteryPercentage < 1.0f && batteryPercentage > 0.0f);
}

void SimplePercentageBattery::checkBattery(){
    if (!checkBatteryPercentageValid()){
        BatteryCriticalWarningMessage *msg = new BatteryCriticalWarningMessage();
        msg->setCurrentBatteryLevel(batteryPercentage);
        for (int i = 0; i<gateSize("out"); i++)
            send(msg, "out", i);
    }
}

void SimplePercentageBattery::incrementalBatteryChange(double percentage){
    batteryPercentage += percentage;
    batteryPercentage = std::min(batteryPercentage, 1.0);
    checkBattery();
}

bool SimplePercentageBattery::isInitialized(){
    return initialized;
}

void SimplePercentageBattery::initialize(){
    EV_INFO << "Init battery" << endl;
    // TODO: Init?
    initialized = true;
}

} /* namespace eventsimulator */
