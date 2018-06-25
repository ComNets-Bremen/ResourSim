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
// - Be more generic in charging / discharging. Currently, only the mean values are used
// - Mean values do not fit to the real characteristics. Be more generic

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

    simtime_t curTime = simTime().dbl();

    expectedBatteryPercentage = batMsg->getTheoreticalAbsolutePercentage();

    EV_INFO << "Got message! " << batMsg->str() << endl;
    if (!initialized){
        setBatteryPercentage(batMsg->getTheoreticalAbsolutePercentage());
        lastBatteryEventTime = curTime;
        initialized = true;
    }

    if (lastBatteryEventTime != curTime){
        double timedelta = (curTime - lastBatteryEventTime).dbl(); // in seconds
        if (batMsg->getIs_charging()){
            double deltaPercent = par("chargePerHour").doubleValue() * timedelta / 3600.0;
            EV_INFO << "Charge: " << deltaPercent << " in " << timedelta << "seconds" << endl;
            incrementalBatteryChange(deltaPercent);
        } else {
            double deltaPercent = par("dischargePerHour").doubleValue() * timedelta / 3600.0;
            EV_INFO << "Discharge: " << deltaPercent << " in " << timedelta << "seconds" << endl;
            incrementalBatteryChange(deltaPercent);
        }
    }

    // Valid?
    if (!checkBatteryPercentageValid()){
        EV_ERROR << "WRONG BATTERY VALUES: " << batteryPercentage << endl;
    }

    EV_INFO << "Delta: " << (batMsg->getTheoreticalAbsolutePercentage() - batteryPercentage) << endl;
    //delete batMsg;
    delete msg;

    // Collect data for statistical analysis
    batteryPercentageValues.record(batteryPercentage);
    expectedBatteryPercentageValues.record(expectedBatteryPercentage);
    batteryPercentageDelta.record(expectedBatteryPercentage - batteryPercentage);


    lastBatteryEventTime = curTime;

}

SimplePercentageBattery::SimplePercentageBattery() {
    batteryPercentage = 0.5;
    expectedBatteryPercentage = 0.5;
    lastBatteryEventTime = 0.0;
    initialized = false;

    batteryPercentageValues.setName("Calculated battery percentage");
    expectedBatteryPercentageValues.setName("Expected battery percentage");
    batteryPercentageDelta.setName("Delta between expected and calculated battery percentage");
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
        // TODO: more checks?
        for (int i = 0; i<gateSize("out"); i++){
            BatteryCriticalWarningMessage *msg = new BatteryCriticalWarningMessage();
            msg->setCurrentBatteryLevel(batteryPercentage);
            send(msg, "out", i);
        }

    }
}

void SimplePercentageBattery::incrementalBatteryChange(double percentage){
    batteryPercentage += percentage;
    batteryPercentage = std::min(batteryPercentage, 1.0);
    batteryPercentage = std::max(batteryPercentage, 0.0);
    // TODO: Handle zero, inform that the simulation should end / stop
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

void SimplePercentageBattery::refreshDisplay() const{
    char buf [40];
    if (par("detailedStatus"))
            sprintf(buf, "is: %.2f%%, should: %.2f%%", batteryPercentage*100.0, expectedBatteryPercentage*100.0);
    else
        sprintf(buf, "Value: %.2f%%", batteryPercentage*100.0);
    getDisplayString().setTagArg("t", 0, buf);
}

void SimplePercentageBattery::finish(){

}

} /* namespace eventsimulator */
