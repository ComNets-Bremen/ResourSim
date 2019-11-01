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

#include "SimpleHighPowerCpu.h"

namespace eventsimulator {

Define_Module(SimpleHighPowerCpu);

SimpleHighPowerCpu::~SimpleHighPowerCpu() {
    if (getSimulation()->getSystemModule()->isSubscribed(
    CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);
}



void SimpleHighPowerCpu::initialize() {
    EV_INFO << "init CPU" << std::endl;

    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
            this);
    initialized = true;
}

void SimpleHighPowerCpu::handleMessage(cMessage *msg) {
    if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr){
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<BackgroundEventMessage *>(msg);

        if (backgroundEventMessage->getBackgroundType() == BACKGROUND_EVENT_TYPE_CPU && backgroundServiceEndMessage != nullptr){
            EV_INFO << "High power CPU usage started" << std::endl;
            simtime_t duration = backgroundEventMessage->getDuration();
            cpuUsageStarted = simTime();
            backgroundServiceEndMessage = new cMessage("End Background Service");
            scheduleAt(simTime() + duration, backgroundServiceEndMessage);
        }
    } else if (backgroundServiceEndMessage == msg){
        EV_INFO << "High power CPU usage finished" << std::endl;
        simtime_t duration = simTime() - cpuUsageStarted;
        sendBatteryConsumptionEvent(duration);
        backgroundServiceEndMessage = nullptr;
    }
    delete msg;
}

void SimpleHighPowerCpu::refreshDisplay() const {
    std::string msgString;
        msgString = "High Power CPU: ";


    if (backgroundServiceEndMessage != nullptr){
        getDisplayString().setTagArg("i", 0, "status/cpu_red");
        msgString += "in use";
    } else {
        getDisplayString().setTagArg("i", 0, "status/cpu_grey");
        msgString += "not in use";
    }
    getDisplayString().setTagArg("t", 0, msgString.c_str());
}

void SimpleHighPowerCpu::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        if (backgroundServiceEndMessage != nullptr) {
            EV_INFO << "Recalc used energy for CPU usage due to regular event." << std::endl;
            simtime_t duration = simTime() - cpuUsageStarted;
            cpuUsageStarted = simTime();
            sendBatteryConsumptionEvent(duration);
        }
    }

}

void SimpleHighPowerCpu::sendBatteryConsumptionEvent(simtime_t duration) {
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_CPU);
    double chargeChange = -1 * par("cpuCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "Used " << chargeChange << "C (As) (" << duration << "s)" << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb

    if (gateSize("out") < 1) throw cRuntimeError("Invalid number of output gates: %d; must be >=1", gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
