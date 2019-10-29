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

    sendSignal_calculateBatteryDiffsEvent = new cMessage(
    CALCULATE_BATTERY_DIFFS);
    scheduleAt(0, sendSignal_calculateBatteryDiffsEvent);

    collectDecisionDatasetsEvent = new cMessage("collectDecisionMeasurements");
    scheduleAt(0, collectDecisionDatasetsEvent);

    batteryRecalcId = registerSignal(CALCULATE_BATTERY_DIFFS);

    getSimulation()->getSystemModule()->subscribe(BATTERY_PERCENTAGE_SIGNAL,
            this);

    getSimulation()->getSystemModule()->subscribe(BATTERY_INCONVENIENT_SIGNAL,
            this);

    getSimulation()->getSystemModule()->subscribe(WIFI_STATUS_UPDATE_SIGNAL,
            this);

    getSimulation()->getSystemModule()->subscribe(SCREEN_STATUS_UPDATE_SIGNAL,
            this);

    screenDecision.setWindowSize(par("decisionWindow").doubleValueInUnit("s"));
    wifiDecision.setWindowSize(par("decisionWindow").doubleValueInUnit("s"));

    screenDecisionStats.setName("ScreenStats");
    screenDecisionStats24hrs.setName("ScreenStats24hrs");

    wifiDecisionStatsUser.setName("WifiStatsUser");
    wifiDecisionStatsUser24hrs.setName("WifiStatsUser24hrs");

    cancelEventTimes.setName("CancelEvent");

    numberBackgroundEvents = 0;
    numberCancelledBackgroundEvents = 0;
}

EventManager::~EventManager() {
    if (getSimulation()->getSystemModule()->isSubscribed(
    BATTERY_PERCENTAGE_SIGNAL, this))
        getSimulation()->getSystemModule()->unsubscribe(
        BATTERY_PERCENTAGE_SIGNAL, this);

    if (getSimulation()->getSystemModule()->isSubscribed(
    BATTERY_INCONVENIENT_SIGNAL, this))
        getSimulation()->getSystemModule()->unsubscribe(
        BATTERY_INCONVENIENT_SIGNAL, this);

    if (getSimulation()->getSystemModule()->isSubscribed(
    WIFI_STATUS_UPDATE_SIGNAL, this))
        getSimulation()->getSystemModule()->unsubscribe(
        WIFI_STATUS_UPDATE_SIGNAL, this);

    if (getSimulation()->getSystemModule()->isSubscribed(
    SCREEN_STATUS_UPDATE_SIGNAL, this))
        getSimulation()->getSystemModule()->unsubscribe(
        SCREEN_STATUS_UPDATE_SIGNAL, this);

    cancelAndDelete(sendSignal_calculateBatteryDiffsEvent);
    cancelAndDelete(collectDecisionDatasetsEvent);
}

void EventManager::handleMessage(cMessage *msg) {
    // Device dead: Only charging messages can pass
    if (par("stopForwardingDeviceDead").boolValue() && isDeviceDead
            && dynamic_cast<BatteryEventMessage *>(msg) == nullptr) {
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
        numberBackgroundEvents++;

        if (par("enableBackgroundOptimizations").boolValue()) {
            backgroundEventMessage->setBackgroundEventCancelled(false);

            // Do not send messages if battery is weak
            if (par("ignoreBackgroundEventsBatteryInconvenient").boolValue()
                    && isDeviceCritical) {
                EV_INFO
                               << "Ignoring background message: Device in inconvenient mode"
                               << std::endl;

                backgroundEventMessage->setBackgroundEventCancelled(true);

                numberCancelledBackgroundEvents++;
                cancelEventTimes.record(1);
            } /* Battery check */ else if (par("analyzeUserScreenActivity").boolValue()) {
                double correctionFactor = par("screenCorrectionFactor").doubleValue();
                if (screenDecision.getPercentageOfValue(true)
                        > (screenDecision.getPercentageOfValue24Hrs(true) * correctionFactor)) {
                    // User was using the device more often compared to the 24hrs average
                    EV_INFO << "Ignore background task: User is active!"
                                   << std::endl;

                    backgroundEventMessage->setBackgroundEventCancelled(true);
                    numberCancelledBackgroundEvents++;
                    cancelEventTimes.record(1);
                } // Drop message
            } // Analyze screen activity
            if (!backgroundEventMessage->getBackgroundEventCancelled())
                cancelEventTimes.record(0);
        } // Enable optimizations

        // Forward to all nodes
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);

        delete msg;
        return;
    } else if (msg == sendSignal_calculateBatteryDiffsEvent) {
        // Regular event, signal to recalc battery stats.
        if (mayHaveListeners(batteryRecalcId))
            emit(batteryRecalcId, true);
        scheduleAt(simTime() + par("sendBatteryCollectSignalEvent").intValue(),
                msg);
    } else if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        // Phone / user events

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
        EV_INFO << "Capacity event" << std::endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;

    } else if (msg == collectDecisionDatasetsEvent) {
        EV_INFO << "Plot statistics for decision making" << std::endl;

        screenDecisionStats.record(screenDecision.getPercentageOfValue(true));
        screenDecisionStats24hrs.record(
                screenDecision.getPercentageOfValue24Hrs(true));

        wifiDecisionStatsUser.record(
                wifiDecision.getPercentageOfValue(DEVICE_STATE_OCCUPIED_USER));
        wifiDecisionStatsUser24hrs.record(
                wifiDecision.getPercentageOfValue24Hrs(
                        DEVICE_STATE_OCCUPIED_USER));

        scheduleAt(simTime() + par("collectDecisionDatasets").intValue(), msg);

    } else {
        EV_ERROR << "Undefined message Type" << endl;
        EV_INFO << "Forwarding to all connected entities" << std::endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;

        // TODO: Throw exception?
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

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, bool b,
        cObject *details) {
    if (signal == registerSignal(BATTERY_INCONVENIENT_SIGNAL)) {
        EV_INFO << "DEVICE inconvenient" << std::endl;
        isDeviceCritical = b;
    } else if (signal == registerSignal(SCREEN_STATUS_UPDATE_SIGNAL)) {
        screenDecision.addDataset(b);
        EV_INFO << "RX SCREEN STATUS MESSAGE " << b
                       << " Screen was occupied by user: "
                       << screenDecision.getPercentageOfValue(true)
                       << std::endl;
    }
}

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, double d,
        cObject *details) {
    if (signal == registerSignal(BATTERY_PERCENTAGE_SIGNAL)) {
        EV_INFO << "Battery value: " << d << std::endl;
        if (d < 0.01) {
            isDeviceDead = true;
            EV_INFO << "DEVICE IS DEAD" << std::endl;
        } else {
            isDeviceDead = false;
            EV_INFO << "DEVICE IS ALIVE" << std::endl;
        }
    }
}

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, long l,
        cObject *details) {
    if (signal == registerSignal(WIFI_STATUS_UPDATE_SIGNAL)) {
        wifiDecision.addDataset(l);
        EV_INFO << "WIFI STATUS IS IN RECEIVER " << l << " WiFi was on: "
                       << wifiDecision.getPercentageOfValue(
                               DEVICE_STATE_OCCUPIED_USER) << std::endl;
    }
}

void EventManager::finish() {
    recordScalar("Number of Background Events", numberBackgroundEvents);
    recordScalar("Number of cancelled background Events",
            numberCancelledBackgroundEvents);
}

}
;
// namespace
