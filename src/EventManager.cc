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
    isDeviceCritical = false;   // Battery level is under a certain threshold
    isDeviceDead = false;       // Battery is dead

    // Recalculate battery level regularly
    sendSignal_calculateBatteryDiffsEvent = new cMessage(
    CALCULATE_BATTERY_DIFFS);
    scheduleAt(0, sendSignal_calculateBatteryDiffsEvent);

    // Collect measurements for the decisions
    collectDecisionDatasetsEvent = new cMessage("collectDecisionMeasurements");
    scheduleAt(0, collectDecisionDatasetsEvent);

    // Signal to initiate battery recalculations
    batteryRecalcId = registerSignal(CALCULATE_BATTERY_DIFFS);

    // Receive Battery percentage changed signals
    getSimulation()->getSystemModule()->subscribe(BATTERY_PERCENTAGE_SIGNAL,
            this);

    // Receive Battery inconvinient signals
    getSimulation()->getSystemModule()->subscribe(BATTERY_INCONVENIENT_SIGNAL,
            this);

    // Receive WiFi status changed signals
    getSimulation()->getSystemModule()->subscribe(WIFI_STATUS_UPDATE_SIGNAL,
            this);

    // Receive screen status changed signals
    getSimulation()->getSystemModule()->subscribe(SCREEN_STATUS_UPDATE_SIGNAL,
            this);

    // receive battery state changes
    getSimulation()->getSystemModule()->subscribe(BATTERY_STATE_CHANGED_SIGNAL,
            this);

    // Set decision window for screen status
    screenDecision.setWindowSize(par("decisionWindow").doubleValueInUnit("s"));

    // Set decision window for wifi status
    wifiDecision.setWindowSize(par("decisionWindow").doubleValueInUnit("s"));

    // Set stats names
    screenDecisionStats.setName("ScreenStats");
    screenDecisionStats24hrs.setName("ScreenStats24hrs");

    wifiDecisionStatsUser.setName("WifiStatsUser");
    wifiDecisionStatsUser24hrs.setName("WifiStatsUser24hrs");

    batteryLifetimeDecision.setName("Decision Battery usage");
    percentageCancelledEvents.setName("Percentage cancelled Events");

    numberBackgroundEvents = 0;
    numberCancelledBackgroundEvents = 0;

    //cAutoRangeHistogramStrategy *strategy = new cAutoRangeHistogramStrategy();
    //cDefaultHistogramStrategy *strategy = new cDefaultHistogramStrategy();

    cFixedRangeHistogramStrategy *strategy = new cFixedRangeHistogramStrategy(0,
            24 * 60 * 60, 24 * 60 * 60 / 120);

    userEventHistogram.setName("Time between user events");
    userEventHistogram.setMode(cHistogram::MODE_DOUBLES);
    userEventHistogram.setStrategy(strategy);

    cAutoRangeHistogramStrategy *batteryStrategy =
            new cAutoRangeHistogramStrategy();
    dischargingHistogram.setName("Time between charging events");
    dischargingHistogram.setMode(cHistogram::MODE_DOUBLES);
    dischargingHistogram.setStrategy(batteryStrategy);


    cAutoRangeHistogramStrategy *batteryPercentageLeftStrategy =
                new cAutoRangeHistogramStrategy();
    startChargingBatteryPercentage.setName("Start charging percentage");
    startChargingBatteryPercentage.setMode(cHistogram::MODE_DOUBLES);
    startChargingBatteryPercentage.setStrategy(batteryPercentageLeftStrategy);
}

EventManager::~EventManager() {
    // Unregister signals
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

    if (getSimulation()->getSystemModule()->isSubscribed(
            BATTERY_STATE_CHANGED_SIGNAL, this))
        getSimulation()->getSystemModule()->unsubscribe(
                BATTERY_STATE_CHANGED_SIGNAL, this);

    // Cancel already scheduled events
    cancelAndDelete(sendSignal_calculateBatteryDiffsEvent);
    cancelAndDelete(collectDecisionDatasetsEvent);
}

void EventManager::handleMessage(cMessage *msg) {
    // Device dead: Only charging messages can pass
    if (par("stopForwardingDeviceDead").boolValue() && isDeviceDead
            && dynamic_cast<BatteryEventMessage*>(msg) == nullptr) {
        // Do not forward message if
        // - Setup via config
        // - device is in dead status (battery empty)
        // - This is not a battery message (charging?)
        delete msg;
        return;
    }

    if (dynamic_cast<BatteryEventMessage*>(msg) != nullptr) {
        BatteryEventMessage *batteryEventMessage = check_and_cast<
                BatteryEventMessage*>(msg);
        bool newChargingState = batteryEventMessage->getIs_charging();

        // Unplugged device: Was charging and now is not charging
        if (isDeviceCharging && !newChargingState) {
            lastChargingStopped = simTime();
            lastChargingStoppedBatteryLevel = -1;
        }

        isDeviceCharging = newChargingState;

        // add dataset will ignore same datasets, i.e. was charging / is still charging
        timeBetweenChg.addDataset(isDeviceCharging);
    }

    if (dynamic_cast<BackgroundEventMessage*>(msg) != nullptr) {
        // Handle Background Events

        EV_INFO << "Background Message" << endl;
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<
                BackgroundEventMessage*>(msg);
        if (backgroundEventMessage == nullptr) {
            EV_ERROR << "Casting error" << std::endl;
            delete msg;
            return;
        }

        // Implement the background service scheduling here!
        numberBackgroundEvents++;

        if (par("enableBackgroundOptimizations").boolValue()) { // General switch regardin bg optimization
            backgroundEventMessage->setBackgroundEventCancelled(false);
            if (!isDeviceCharging ||        // Device is not charging
                    (par("limitEventsWhileCharging").boolValue()
                            && isDeviceCharging) // perform limitation if charging and device is charging
                    ) { // Cancel events

                // Do not send messages if battery is weak and it is not getting better
                if (par("ignoreBackgroundEventsBatteryInconvenient").boolValue()
                        && isDeviceCritical && !isDeviceCharging) {
                    EV_INFO
                                   << "Ignoring background message: Device in inconvenient mode"
                                   << std::endl;

                    backgroundEventMessage->setBackgroundEventCancelled(true);
                }


                if (par("cancelIfScreenIsOn").boolValue() && screenIsOn){
                    EV_INFO << "Cancel event: screen is on" << std::endl;
                    backgroundEventMessage->setBackgroundEventCancelled(true);
                }


                // Do not send messages if user might be active
                if (par("analyzeUserScreenActivity").boolValue()) {
                    double correctionFactor = par("screenCorrectionFactor").doubleValue();
                    if (screenDecision.getPercentageOfValue(true) > (screenDecision.getPercentageOfValue24Hrs(true) * correctionFactor)) {
                        // User was using the device more often compared to the 24hrs average
                        EV_INFO << "Ignore background task: User is active!" << std::endl;

                        backgroundEventMessage->setBackgroundEventCancelled(true);
                    } // Drop message
                }

                // Do not send messages if battery might be drained and it is not getting better
                if (par("analyzeTimeToCharge").boolValue()) {
                    if ((timeBetweenChg.getLen() > par("analyzeTimeToChargeMinDataLen").intValue()) && !isDeviceCharging) {
                        simtime_t quantilTtc = timeBetweenChg.getQuantil(par("chargeDecisionQuantil").doubleValue());
                        simtime_t runningOnBat = simTime() - lastChargingStopped;

                        double chgTimeWindowPercentage = runningOnBat / quantilTtc;

                        double calculatedDischargeTime = runningOnBat.dbl() + (lastChargingStoppedBatteryLevel/ ((lastChargingStoppedBatteryLevel - batteryLevel) / runningOnBat));

                        EV_INFO << "Last charging was " << runningOnBat << " seconds ago and we are at level " << batteryLevel << " and in average, we start charging with the " << par("chargeDecisionQuantil").doubleValue() * 100.0 << "% quantil after " << quantilTtc << "seconds. percentage of quantil: " << chgTimeWindowPercentage << " Battery level when we started discharging: " << lastChargingStoppedBatteryLevel << std::endl;

                        EV_INFO << "######" << std::endl;
                        EV_INFO << "lastChargingStoppedBatteryLevel=" << lastChargingStoppedBatteryLevel  << std::endl; // battery level when last charging stopped
                        EV_INFO << "runningOnBat=" << runningOnBat << std::endl; // seconds since last charging stopped
                        EV_INFO << "isDeviceCharging=" << isDeviceCharging << std::endl; // is device charging?
                        EV_INFO << "batteryLevel=" << batteryLevel << std::endl; // the current battery level
                        EV_INFO << "chargeDecisionQuantil=" << par("chargeDecisionQuantil").doubleValue() * 100.0 << std::endl; // Quantil we take for the decision
                        EV_INFO << "quantilTtc=" << quantilTtc << std::endl; // The time value of the given quantil
                        EV_INFO << "chgTimeWindowPercentage=" << chgTimeWindowPercentage << std::endl; // The percentage  we are in the current quantil
                        EV_INFO << "Calculated discharging per hour:" << (lastChargingStoppedBatteryLevel - batteryLevel) / runningOnBat/60.0/60.0 << std::endl;
                        EV_INFO << "calculatedDischargeTime=" << calculatedDischargeTime << std::endl;
                        EV_INFO << "Avg battery level left=" << startChargingBatteryPercentage.getMean() << std::endl;
                        EV_INFO << "*****" << std::endl;

                        // TODO: Handle according to battery level and time to charge
                        if ((lastChargingStoppedBatteryLevel - batteryLevel) > 0.05 || runningOnBat > 240.0) { // Only perform optimization if we are discharging for a certain percentage or time
                            // We need some percent of the battery drained to perform useful calculations. TODO: Param?
                            EV_INFO << "decision: <1 okay, >1 bad:" << calculatedDischargeTime / quantilTtc << std::endl;
                            batteryLifetimeDecision.record(calculatedDischargeTime / quantilTtc);

                            if ((calculatedDischargeTime / quantilTtc) > 1) {
                                // Cancel if forecast says: critical
                                backgroundEventMessage->setBackgroundEventCancelled(true);
                            }
                        }


                    } else {
                        EV_INFO
                                       << "Not enough data to perform optimization according to charging behavior."
                                       << std::endl;
                    }
                }

                // Count cancelled messages for stats
                if (backgroundEventMessage->getBackgroundEventCancelled()) {
                    numberCancelledBackgroundEvents++;
                }
            } // charging / not charging etc.
        } // cancelled events by optimization

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
    } else if (dynamic_cast<BaseEventMessage*>(msg) != nullptr) {
        // Phone / user events

        BaseEventMessage *message = check_and_cast<BaseEventMessage*>(msg);
        if (message == nullptr) {
            EV_ERROR << "Casting error" << std::endl;
            delete msg;
            return;
        }
        EV_INFO << "Message type: " << message->getPayloadType() << endl;
        for (int i = 0; i < gateSize("out"); i++)
            send(msg->dup(), "out", i);
        delete msg;
    } else if (dynamic_cast<CapacityEvent*>(msg) != nullptr) {
        CapacityEvent *cEvent = check_and_cast<CapacityEvent*>(msg);
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

        percentageCancelledEvents.record(double(numberCancelledBackgroundEvents)
                / double(numberBackgroundEvents));

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

/**
 * Signal receiver: Type bool
 */
void EventManager::receiveSignal(cComponent *src, simsignal_t signal, bool b,
        cObject *details) {
    if (signal == registerSignal(BATTERY_INCONVENIENT_SIGNAL)) {
        if (b)
            EV_INFO << "DEVICE inconvenient" << std::endl;
        isDeviceCritical = b;
    } else if (signal == registerSignal(SCREEN_STATUS_UPDATE_SIGNAL)) {
        screenOffTimes.addDataset(b);
        screenDecision.addDataset(b);
        screenIsOn = b;
        EV_INFO << "RX SCREEN STATUS MESSAGE " << b
                       << " Screen was occupied by user: "
                       << screenDecision.getPercentageOfValue(true)
                       << " Number of stored values: "
                       << screenOffTimes.getLen() << " Median: "
                       << screenOffTimes.getMedian() << std::endl;
    } else if (signal == registerSignal(BATTERY_STATE_CHANGED_SIGNAL)) {
        if (b) {
            EV_INFO << "was discharging for "
                           << (simTime() - lastChargingStateChange).dbl()
                           << std::endl;
            dischargingHistogram.collect(
                    (simTime() - lastChargingStateChange).dbl());

            BucketBattery *bucketBattery = check_and_cast<BucketBattery*>(src);

            startChargingBatteryPercentage.collect(bucketBattery->getBatteryChargePercent());
        } else {
            EV_INFO << "was charging for "
                           << (simTime() - lastChargingStateChange).dbl()
                           << std::endl;
        }
        lastChargingStateChange = simTime();
    } else {
        EV_ERROR << "Received unhandled signal!" << std::endl;
    }
}

void EventManager::receiveSignal(cComponent *src, simsignal_t signal, double d,
        cObject *details) {
    if (signal == registerSignal(BATTERY_PERCENTAGE_SIGNAL)) {
        batteryLevel = d;

        EV_INFO << "Battery value: " << d << std::endl;

        // Charging was stopped just before. Store last Battery Level.
        if (lastChargingStoppedBatteryLevel < 0)
            lastChargingStoppedBatteryLevel = batteryLevel;

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
        DeviceStates deviceState = static_cast<DeviceStates>(l);
        wifiDecision.addDataset(l);
        EV_INFO << "WIFI STATUS IS IN RECEIVER "
                       << getDeviceStateName(deviceState) << " WiFi was on: "
                       << wifiDecision.getPercentageOfValue(
                               DEVICE_STATE_OCCUPIED_USER) << std::endl;

        // Calc avg time between user activity

        SimpleWiFi *simpleWiFi = check_and_cast<SimpleWiFi*>(src);
        switch (deviceState) {
        case DEVICE_STATE_OCCUPIED_USER:
            if (startWiFiFreePeriod != 0) {
                simtime_t lastFreePeriod = simTime() - startWiFiFreePeriod;
                EV_INFO << "WiFi was not used by user for " << lastFreePeriod
                               << "s" << std::endl;
                userEventHistogram.collect(lastFreePeriod.dbl());

                // TODO add for histogram, ensure free was after user state
            }

            break;
        case DEVICE_STATE_FREE:
            // transition user -> Free
            if (simpleWiFi->getPreviousDeviceState()
                    == DEVICE_STATE_OCCUPIED_USER) {
                EV_INFO << "Transition from user -> free" << std::endl;
                startWiFiFreePeriod = simTime();
            }
            break;

        default:
            EV_INFO << "Ignoring bg services" << std::endl;
            // only interested in user wifi activity
        }
    }

}

void EventManager::finish() {
    recordScalar("Number of Background Events", numberBackgroundEvents);
    recordScalar("Number of cancelled background Events",
            numberCancelledBackgroundEvents);

    recordScalar("#percentageCancelledEvents",
            double(numberCancelledBackgroundEvents)
                    / double(numberBackgroundEvents));

    if (par("writeScreenOffTimes").stdstringValue().length() != 0) {
        std::string path = par("writeScreenOffTimes").stdstringValue();
        EV_INFO << "Storing data to " << path << std::endl;
        if (screenOffTimes.storeVectorToFile(path)) {
            EV_INFO << "Data successfully stored. Median: "
                           << screenOffTimes.getMedian() << std::endl;
        } else {
            EV_INFO << "Error storing data. Dataset empty?" << std::endl;
        }
    }

    if (par("writeTimesBetweenChg").stdstringValue().length() != 0) {
        std::string path = par("writeTimesBetweenChg").stdstringValue();
        EV_INFO << "Storing data to " << path << std::endl;
        if (timeBetweenChg.storeVectorToFile(path)) {
            EV_INFO << "Data successfully stored. Median: "
                           << timeBetweenChg.getMedian() << std::endl;
        } else {
            EV_INFO << "Error storing data. Dataset empty?" << std::endl;
        }
    }
    userEventHistogram.recordAs("Time between user events");
    dischargingHistogram.recordAs("Time between charging");
    startChargingBatteryPercentage.recordAs("percentage left in battery");
}

}
;
// namespace
