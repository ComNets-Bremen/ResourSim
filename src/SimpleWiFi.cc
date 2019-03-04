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

#include "SimpleWiFi.h"

namespace eventsimulator {

Define_Module(SimpleWiFi);

SimpleWiFi::SimpleWiFi() {
    wifiStatusValues.setName("WiFi Status");
    wifiStatusOn.setName("WiFi on (Window)");
    wifiStatusOff.setName("WiFi off (Window)");
    wifiDeviceState.setName("Simple WiFi device State");
    collectMeasurementsEvent = nullptr;
    backgroundServiceEndMessage = nullptr;
}

SimpleWiFi::~SimpleWiFi() {
    cancelAndDelete(collectMeasurementsEvent);
    if (getSimulation()->getSystemModule()->isSubscribed(
            CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);

}

void SimpleWiFi::initialize() {
    EV << "Init WiFi status" << endl;
    EV_INFO << "Window Size for statistics: "
                   << par("statsWindowSize").intValue() << "s" << std::endl;
    collectMeasurementsEvent = new cMessage("collectMeasurements");
    scheduleAt(0, collectMeasurementsEvent);
    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
            this);
    // TODO: Init something?
    initialized = true;
}

void SimpleWiFi::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        if (deviceState == DEVICE_STATE_OCCUPIED_USER || deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
            EV_INFO << "Recalc used energy for WiFi due to regular event." << std::endl;
            simtime_t duration = simTime() - startOccupiedTime;
            startOccupiedTime = simTime();
            sendBatteryConsumptionEvent(duration);
        }
    }

}


DeviceStates SimpleWiFi::getDeviceState() const{
    return deviceState;
}

void SimpleWiFi::handleMessage(cMessage *msg) {

    if (dynamic_cast<WiFiEventMessage *>(msg) != nullptr){
        if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
            EV_INFO << "Collision Background" << endl;
            collisionUser++;
            delete msg;
            return;
        }

        WiFiEventMessage *wifiMsg = check_and_cast<WiFiEventMessage *>(msg);

        wifiStatus = wifiMsg->getWifi_status();
        wifiStatusValues.record(wifiStatus);

        if (getWiFiIsOccupied(wifiStatus)) {
            deviceState = DEVICE_STATE_OCCUPIED_USER;
            startOccupiedTime = simTime();
        } else {
            deviceState = DEVICE_STATE_FREE;
            simtime_t duration = simTime() - startOccupiedTime;
            sendBatteryConsumptionEvent(duration);
        }

        addMessageForUserStats(new StatisticEntry(
                deviceState==DEVICE_STATE_OCCUPIED_USER,
                wifiMsg->getArrivalTime(),
                StatisticType::USAGE_USER
                ));
        cleanupMessagesForStats();

        delete wifiMsg;
    } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        //EV_INFO << "Background Message" << endl;
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<
                BackgroundEventMessage *>(msg);
        if (backgroundEventMessage->getBackgroundType()
                == BACKGROUND_EVENT_TYPE_WIFI) {
            if ((deviceState == DEVICE_STATE_FREE)
                    or (deviceState == DEVICE_STATE_UNKNOWN)) {
                deviceState = DEVICE_STATE_OCCUPIED_BACKGROUND;
                startOccupiedTime = simTime();

                backgroundServiceEndMessage = new cMessage(
                        "End Background Service");
                scheduleAt(simTime() + backgroundEventMessage->getDuration(),
                        backgroundServiceEndMessage);
                addMessageForUserStats(new StatisticEntry(
                        true,
                        msg->getArrivalTime(),
                        StatisticType::USAGE_BACKGROUND
                        ));
            } else if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
                EV_INFO << "self collision" << endl;
                collisionSelf++;
            } else {
                collisionBackground++;
            }
        }
        delete msg;
    } else if (msg == backgroundServiceEndMessage) {
        EV_INFO << "End background service" << endl;
        simtime_t duration = simTime() - startOccupiedTime;
        // Send message to battery
        sendBatteryConsumptionEvent(duration);
        deviceState = DEVICE_STATE_FREE;
        addMessageForUserStats(new StatisticEntry(
                                false,
                                msg->getArrivalTime(),
                                StatisticType::USAGE_BACKGROUND
                                ));
        delete msg;
    } else if (msg == collectMeasurementsEvent) {
        // Handle regular statistic events
        cleanupMessagesForStats();

        std::map<StatisticResult, double> result = calcUserStats(
                        par("statsWindowSize").intValue());

        double on = 0.0;
        double off = 0.0;
        for (auto s : result) {
            //EV_INFO << "CHECK ME: " << s.first << std::endl;
            if (s.first.isActive)
                on += s.second;
            else
                off += s.second;
        }

        EV_INFO << "Periodic WiFi status: on: " << on << ", off: " << off
                       << " in the last " << par("statsWindowSize").intValue()
                       << "s" << std::endl;
        //EV_INFO << "NUM OF PACKETS " << getNumOfMessagesForStats() << std::endl;
        // TODO Store for statistics!!!
        wifiStatusOn.record(on);
        wifiStatusOff.record(off);

        switch (deviceState) {
        case DEVICE_STATE_UNKNOWN:
            wifiDeviceState.record(-1);
            break;
        case DEVICE_STATE_OCCUPIED_USER:
            wifiDeviceState.record(2);
            break;
        case DEVICE_STATE_OCCUPIED_BACKGROUND:
            wifiDeviceState.record(1);
            break;
        case DEVICE_STATE_FREE:
            wifiDeviceState.record(0);
            break;
        default:
            wifiDeviceState.record(-2);
            EV_INFO << "Should not happen..." << std::endl;
        }

        scheduleAt(
                simTime() + par("periodicStatsCollectionInterval").intValue(),
                msg);
        return;
    } else {
        // Message not for this module
        delete msg;
    }
}

void SimpleWiFi::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "WiFi status: %s", getWiFiStatusString(wifiStatus).c_str());
    getDisplayString().setTagArg("t", 0, buf);
    if (deviceState == DEVICE_STATE_OCCUPIED_USER
            || deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND)
        getDisplayString().setTagArg("i", 0, "status/wifi_green");
    else
        getDisplayString().setTagArg("i", 0, "status/wifi_neutral");
}

void SimpleWiFi::finish() {
    recordScalar("#collisionUser", collisionUser);
    recordScalar("#collisionBackground", collisionBackground);
    recordScalar("#collisionSelf", collisionSelf);
}

void SimpleWiFi::sendBatteryConsumptionEvent(simtime_t duration) {
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_WIFI);
    double chargeChange = -1 * par("txCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "Used " << chargeChange << "C (As) (" << duration << "s)" << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb

    if (gateSize("out") < 1) throw cRuntimeError("Invalid number of output gates: %d; must be >=1", gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
