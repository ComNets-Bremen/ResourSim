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

/*
 * This module represents the EventManager module. Currently, it mainly
 * forwards all messages received via the in ports to all out ports
 */

// TODO: Implement the following:
// Can be neglected for the first step: unknown, traffic
// More intelligent scheduling? Currently, everything is forwarded to all output ports
#include "EventManager.h"

namespace eventsimulator {

Define_Module(EventManager);

void EventManager::initialize() {
    EV_INFO << "Started" << endl;
    isDeviceCritical = false;
    isDeviceDead = false;
    getSimulation()->getSystemModule()->subscribe(BATTERY_PERCENTAGE_SIGNAL, this);
    getSimulation()->getSystemModule()->subscribe(BATTERY_INCONVENIENT_SIGNAL, this);
}

EventManager::~EventManager(){
    if(getSimulation()->getSystemModule()->isSubscribed(BATTERY_PERCENTAGE_SIGNAL, this)) getSimulation()->getSystemModule()->unsubscribe(BATTERY_PERCENTAGE_SIGNAL, this);
    if(getSimulation()->getSystemModule()->isSubscribed(BATTERY_INCONVENIENT_SIGNAL, this)) getSimulation()->getSystemModule()->unsubscribe(BATTERY_INCONVENIENT_SIGNAL, this);
}

void EventManager::handleMessage(cMessage *msg) {
    // Device dead: Only charging messages can pass
    if (par("stopForwardingDeviceDead").boolValue() && isDeviceDead && dynamic_cast<BatteryEventMessage *>(msg) == nullptr){
        delete msg;
        return;
    }

    if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        // Handle Background Events

        EV_INFO << "Background Message" << endl;
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<
                BackgroundEventMessage *>(msg);
        if (backgroundEventMessage == nullptr) {
            EV_ERROR << "Casting error" << std::endl;
            delete msg;
            return;
        }

        // Implement the background service scheduling here!


        // Forward to all nodes
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);

        delete msg;
        return;
    } else if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        // Phone events

        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message == nullptr) {
            EV_ERROR << "Casting error" << std::endl;
            delete msg;
            return;
        }
        EV_INFO << "Message type: " << message->getPayloadType() << endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;
    } else if (dynamic_cast<CapacityEvent *>(msg) != nullptr) {
        CapacityEvent *cEvent = check_and_cast<CapacityEvent *>(msg);
        if (cEvent == nullptr) {
            EV_ERROR << "Casting error" << std::endl;
            delete msg;
            return;
        }
        EV_INFO << "Capacy event" << std::endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;

    } else {
        EV_ERROR << "Undefined message Type" << endl;
        EV_INFO << "Forwarding to all connected entities" << std::endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;
    }
}

void EventManager::refreshDisplay() const {
    if (isDeviceDead)
        getDisplayString().setTagArg("i", 0, "status/status_dead");
    else if (isDeviceCritical)
        getDisplayString().setTagArg("i", 0, "status/status_critical");
    else
        getDisplayString().setTagArg("i", 0, "status/status_okay");
}

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, bool b, cObject *details){
    EV_INFO << "RECEIVED SIGNAL BOOL!!!" << signal << getSignalName(signal) <<  std::endl;
    if (signal == registerSignal(BATTERY_INCONVENIENT_SIGNAL)){
        EV_INFO << "DEVICE inconvenient" << std::endl;
        isDeviceCritical = b;
    }
}

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, double d, cObject *details){
    EV_INFO << "RECEIVED SIGNAL DOUBLE!!!" << signal << getSignalName(signal) <<  std::endl;
    if (signal == registerSignal(BATTERY_PERCENTAGE_SIGNAL)){
        if (d < 0.01){
            isDeviceDead = true;
            EV_INFO << "DEVICE IS DEAD" << std::endl;
        } else {
            isDeviceDead = false;
            EV_INFO << "DEVICE IS ALIVE" << std::endl;
        }
    }
}


};// namespace
