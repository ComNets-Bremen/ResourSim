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

/**
 * Default constructor
 */
SimpleWiFi::SimpleWiFi() {
    collectMeasurementsEvent = nullptr;
}

/**
 * Default destructor
 */
SimpleWiFi::~SimpleWiFi() {
    cancelAndDelete(collectMeasurementsEvent);
    if (getSimulation()->getSystemModule()->isSubscribed(
    CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);
}

/**
 * Init simulation
 */
void SimpleWiFi::initialize() {
    EV << "Init WiFi status" << endl;

    // Set names for stats
    wifiStatusValues.setName("WiFi Status");
    wifiStatusOn.setName("WiFi on (Window)");
    wifiStatusOff.setName("WiFi off (Window)");
    regularWifiDeviceState.setName("Simple WiFi device State");
    wifiAsyncDeviceState.setName("Async WiFi device State");

    txBitPerSecond.setName("WiFi bps TX");
    RxBitPerSecond.setName("WiFi bps RX");

    TotalRxBitinInterval.setName("WiFi Bit TX in Interval");
    TotalTxBitinInterval.setName("WiFi Bit RX in Interval");

    percentageInterrupedWiFiEvents.setName("Percentage cancelled WiFi events");

    // Print config pars
    EV_INFO << "Window Size for statistics: "
                   << par("statsWindowSize").intValue() << "s" << std::endl;

    // Create and schedule regular events
    collectMeasurementsEvent = new cMessage("collectMeasurements");

    initState(DEVICE_STATE_FREE); // Init the FSM to state "IDLE"

    scheduleAt(0, collectMeasurementsEvent);

    // Callbacks for broadcatsts
    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
            this);
    // TODO: Init something?
    initialized = true;
}

/**
 * Update visual simulation changes
 */
void SimpleWiFi::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "WiFi status: %s",
            getWiFiStatusString(lastReceivedWifiStatus).c_str());
    getDisplayString().setTagArg("t", 0, buf);
    if (getDeviceState() == DEVICE_STATE_OCCUPIED_USER
            || getDeviceState() == DEVICE_STATE_OCCUPIED_BACKGROUND)
        getDisplayString().setTagArg("i", 0, "status/wifi_green");
    else
        getDisplayString().setTagArg("i", 0, "status/wifi_neutral");
}

/**
 * Finish the simulation, record the stats
 */
void SimpleWiFi::finish() {
    recordScalar("#txNeglectable", trafficTxNeglectable);
    recordScalar("#rxNeglectable", trafficRxNeglectable);
    recordScalar("#txTotal", trafficTxTotal);
    recordScalar("#rxTotal", trafficRxTotal);

    // collision scalars
    recordScalar("#userActiveBgRequested", userActiveBgRequested);
    recordScalar("#bgTotalStarted", bgTotalStarted);
    recordScalar("#bgJobsInterruptedByUser", bgJobsInterruptedByUser);
    if (bgTotalStarted > 0)
        recordScalar("#percentage of not successfully run bg jobs",
                bgJobsInterruptedByUser / bgTotalStarted);
    else
        recordScalar("#percentage of not successfully run bg jobs", 0);
}

/**
 * Callback for message broadcasts
 */
void SimpleWiFi::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    // Callable by other modules:
    //Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    Enter_Method
    ("receiveSignal(signal=%s, value=%b", getSignalName(signal), b);

    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        // inCurrentStateSince()
        if (getDeviceState() == DEVICE_STATE_OCCUPIED_USER
                || getDeviceState() == DEVICE_STATE_OCCUPIED_BACKGROUND) {
            EV_INFO << "Recalc used energy for WiFi due to regular event."
                           << std::endl;
            sendBatteryConsumptionEvent(
                    simMax(getInCurrentStateSince(),
                            lastCalculateBatteryDiffs));
        }
    }
}

/**
 * General message handler
 */
void SimpleWiFi::handleMessage(cMessage *msg) {
    if (msg == collectMeasurementsEvent
            && collectMeasurementsEvent != nullptr) {
        /*
         * Collect measurements / calculate diffs
         */

        if (bgTotalStarted > 0){
            percentageInterrupedWiFiEvents.record(double(bgJobsInterruptedByUser) / double(bgTotalStarted));
        }

        // rm old messages from vector
        cleanupMessagesForStats();

        std::map<StatisticResult, double> result = calcUserStats(
                par("statsWindowSize").intValue());

        double on = 0.0;
        double off = 0.0;

        // Calc on off time of both: user and bg service
        for (auto s : result) {
            //EV_INFO << "CHECK ME: " << s.first << std::endl;
            if (s.first.isActive)
                on += s.second;
            else
                off += s.second;
        }

        EV_INFO << "Periodic WiFi status: on: " << on << ", off: " << off
                       << " in the last " << par("statsWindowSize").intValue()
                       << "s calculated using "
                       << getNumOfMessagesForUserStats() << std::endl;

        wifiStatusOn.record(on);
        wifiStatusOff.record(off);
        regularWifiDeviceState.record(getDeviceState());

        scheduleAt(
                simTime() + par("periodicStatsCollectionInterval").intValue(),
                msg);
        return;
    } else if (dynamic_cast<TrafficEventMessage*>(msg) != nullptr) {
        /*
         * Handle traffic messages
         */
        TrafficEventMessage *trafficEventMessage = check_and_cast<
                TrafficEventMessage*>(msg);
        EV_INFO << "TRAFFIC EVENT!!" << trafficEventMessage->str() << std::endl;

        lastTrafficValues.received = trafficEventMessage->getArrivalTime();
        lastTrafficValues.mobile_rx = trafficEventMessage->getMobile_rx();
        lastTrafficValues.mobile_tx = trafficEventMessage->getMobile_tx();
        lastTrafficValues.total_rx = trafficEventMessage->getTotal_rx();
        lastTrafficValues.total_tx = trafficEventMessage->getTotal_tx();
        delete trafficEventMessage;
    }
    /*
     *  Start with state transitions
     */

    else if (dynamic_cast<WiFiEventMessage*>(msg) != nullptr) {
        WiFiEventMessage *wifiMsg = check_and_cast<WiFiEventMessage*>(msg);
        /*
         * TODO: Check if transition was successful
         */
        if (getWiFiIsOccupied(wifiMsg->getWifi_status())) {
            if (!setNewState(DEVICE_STATE_OCCUPIED_USER, nullptr)) {
                EV_ERROR << "Can not change to state OCCUPIED_USER"
                                << std::endl;
            }
        } else {
            if (getDeviceState() == DEVICE_STATE_OCCUPIED_USER) {
                // Only cancel event if user is active. Do not cancel bg events
                setNewState(DEVICE_STATE_FREE, nullptr);
            }
        }

        delete wifiMsg;
    } else if (dynamic_cast<BackgroundEventMessage*>(msg) != nullptr) {
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<
                BackgroundEventMessage*>(msg);
        if ((backgroundEventMessage->getBackgroundType()
                == BACKGROUND_EVENT_TYPE_WIFI)
                && (!backgroundEventMessage->getBackgroundEventCancelled())) {
            /*
             * Not cancelled and WiFi event -> try to change the state
             */
            // TODO: Handle return value
            simtime_t duration = backgroundEventMessage->getDuration();
            setNewState(DEVICE_STATE_OCCUPIED_BACKGROUND, &duration);
        }
        delete backgroundEventMessage;
    } else if (dynamic_cast<BackgroundEventEndMessage*>(msg) != nullptr) {
        EV_INFO << "End background service" << endl;
        BackgroundEventEndMessage *endMessage =
                dynamic_cast<BackgroundEventEndMessage*>(msg);
        simtime_t sendingTime = endMessage->getSendingTime();
        EV_INFO << "SENDING TIME " << sendingTime << std::endl;
        delete endMessage;
        backgroundEventEndMessage = nullptr;
        setNewState(DEVICE_STATE_FREE, &sendingTime);
    } else {
        // Message not for this module
        delete msg;
    }

    /*

     if (dynamic_cast<WiFiEventMessage *>(msg) != nullptr) {
     // check_and_cast throws an exception if it fails.
     WiFiEventMessage *wifiMsg = check_and_cast<WiFiEventMessage *>(msg);

     // User event

     if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
     if (lastSentBackgroundEventMessage == nullptr)
     throw cRuntimeError(
     "Device is occupied in the bg but no bg message set. Should never happen!");

     lastSentBackgroundEventMessage->setBackgroundEventCancelled(true);
     deviceState = DEVICE_STATE_FREE;

     //sendBatteryConsumptionEvent(simTime() lastSentBackgroundEventMessage startOccupiedTime

     // Equal:
     //EV_INFO << "MY TESTDIFF: " << lastSentBackgroundEventMessage->getCreationTime() << " " << startOccupiedTime << std::endl;

     sendBatteryConsumptionEvent(simTime() - startOccupiedTime);

     EV_INFO << "Collision Background. Canceled bg job for user" << std::endl;
     occupiedBackgroundStartUser++;
     inCancelledBgState = true;
     }

     if (inCancelledBgState) {
     cancelledBackgroundOccupiedUser++;
     }
     lastUserWifiEvent = wifiMsg->getArrivalTime();
     EV_INFO << "WiFi: " << lastTrafficEvent - lastUserWifiEvent
     << " Delta; Status: "
     << getWiFiStatusString(wifiMsg->getWifi_status())
     << std::endl;

     wifiStatusValues.record(wifiMsg->getWifi_status());

     if (getWiFiIsOccupied(wifiMsg->getWifi_status())
     != getWiFiIsOccupied(lastReceivedWifiStatus)) {
     EV_INFO << "Status change: current: " << wifiMsg->getWifi_status()
     << " "
     << getWiFiIsOccupied(wifiMsg->getWifi_status())
     << " Last: " << lastReceivedWifiStatus << " "
     << getWiFiIsOccupied(lastReceivedWifiStatus) << " |"
     << (getWiFiIsOccupied(
     wifiMsg->getWifi_status()
     != getWiFiIsOccupied(
     lastReceivedWifiStatus)))
     << std::endl;
     // state change
     if (getWiFiIsOccupied(wifiMsg->getWifi_status())) {
     deviceState = DEVICE_STATE_OCCUPIED_USER;
     wifiAsyncDeviceState.record(2);
     startOccupiedTime = simTime();
     trafficWifiStartValues = lastTrafficValues;

     EV_INFO << "Traffic should be 1st" << std::endl;
     } else {
     EV_INFO << "State: Free" << std::endl;
     deviceState = DEVICE_STATE_FREE;
     wifiAsyncDeviceState.record(0);
     simtime_t duration = simTime() - startOccupiedTime;
     sendBatteryConsumptionEvent(duration);

     if ((simTime() - lastTrafficEvent) < 2) {
     calcTrafficDelta(trafficWifiStartValues, lastTrafficValues,
     duration);
     }
     }

     } else {
     // ignore, no state transition
     }

     addMessageForUserStats(
     new StatisticEntry(deviceState == DEVICE_STATE_OCCUPIED_USER,
     wifiMsg->getArrivalTime(), StatisticType::USAGE_USER));
     cleanupMessagesForStats();

     lastReceivedWifiStatus = wifiMsg->getWifi_status();

     delete wifiMsg;
     } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
     //EV_INFO << "Background Message" << endl;
     BackgroundEventMessage *backgroundEventMessage = check_and_cast<
     BackgroundEventMessage *>(msg);
     if (backgroundEventMessage->getBackgroundType()
     == BACKGROUND_EVENT_TYPE_WIFI) {

     EV_INFO << "WIFI BG MSG" << std::endl;

     BackgroundEventEndMessage *endMessage = new BackgroundEventEndMessage();
     endMessage->setBackgroundEventCancelled(
     backgroundEventMessage->getBackgroundEventCancelled());

     if (backgroundEventMessage->getBackgroundEventCancelled()) {
     inCancelledBgState = true;
     } else {
     // Not cancelled job
     if ((deviceState == DEVICE_STATE_FREE)
     or (deviceState == DEVICE_STATE_UNKNOWN)) {
     deviceState = DEVICE_STATE_OCCUPIED_BACKGROUND;
     lastSentBackgroundEventMessage = endMessage;
     wifiAsyncDeviceState.record(1);
     startOccupiedTime = simTime();

     addMessageForUserStats(
     new StatisticEntry(true, msg->getArrivalTime(),
     StatisticType::USAGE_BACKGROUND));
     if (inCancelledBgState) {
     bgCancelledAndUserActiveInPeriod++;
     }
     } else if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
     EV_INFO << "self collision" << endl;
     occupiedBackgroundStartBackground++;
     } else {
     // Occupied user
     occupiedUserStartBackground++;
     }

     } // cancelled / not cancelled event
     if (endMessage != nullptr) {
     scheduleAt(simTime() + backgroundEventMessage->getDuration(),
     endMessage);
     } // tx bg event end msg

     } // BG Event type WiFi

     delete msg;
     } else if (dynamic_cast<BackgroundEventEndMessage *>(msg) != nullptr) {
     EV_INFO << "End background service" << endl;
     BackgroundEventEndMessage *endMessage =
     dynamic_cast<BackgroundEventEndMessage *>(msg);
     if (endMessage->getBackgroundEventCancelled()) {
     //cancelled
     inCancelledBgState = false;
     if (bgCancelledAndUserActiveInPeriod > 0) {
     bgCancelledUserActivePeriods++;
     }
     bgCancelledAndUserActiveInPeriod = 0;
     } else {
     simtime_t duration = simTime() - startOccupiedTime;
     // Send message to battery
     sendBatteryConsumptionEvent(duration);
     deviceState = DEVICE_STATE_FREE;
     lastSentBackgroundEventMessage = nullptr;
     wifiAsyncDeviceState.record(0);
     addMessageForUserStats(
     new StatisticEntry(false, msg->getArrivalTime(),
     StatisticType::USAGE_BACKGROUND));
     }

     EV_INFO << "TESTOUTPUT OF STATE: " << deviceState << " " << DEVICE_STATE_OCCUPIED_BACKGROUND << std::endl;
     delete msg;

     } else if
     */
}

/**
 * Tell the battery that it got drained
 */
void SimpleWiFi::sendBatteryConsumptionEvent(simtime_t startTime) {
    simtime_t duration = simTime() - startTime;
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_WIFI);
    double chargeChange = -1 * par("txCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "WiFi used " << chargeChange << "C (As) (" << duration << "s or "
                   << duration / 60.0 << "mins)" << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb
    cEvent->setDischargeDuration(duration); // in seconds

    if (gateSize("out") < 1)
        throw cRuntimeError("Invalid number of output gates: %d; must be >=1",
                gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
    lastCalculateBatteryDiffs = simTime();
}

void SimpleWiFi::calcTrafficDelta(TrafficEventValues start,
        TrafficEventValues stop) {
    simtime_t duration = stop.received - start.received;

    if (duration < 0) {
        throw cRuntimeError("duration cannot be negative!: %d", duration.dbl());
    }

    EV_INFO << "Mobile rx: " << stop.mobile_rx << " - " << start.mobile_rx
                   << " = " << (stop.mobile_rx - start.mobile_rx) << std::endl;
    EV_INFO << "Mobile tx: " << stop.mobile_tx << " - " << start.mobile_tx
                   << " = " << (stop.mobile_tx - start.mobile_tx) << std::endl;
    EV_INFO << "Total  rx: " << stop.total_rx << " - " << start.total_rx
                   << " = " << (stop.total_rx - start.total_rx) << std::endl;
    EV_INFO << "Total  tx: " << stop.total_tx << " - " << start.total_tx
                   << " = " << (stop.total_tx - start.total_tx) << std::endl;
    EV_INFO << "Duration: " << duration << std::endl;

    double kbyteRx = (stop.total_rx - start.total_rx) / 1024;
    double kbyteTx = (stop.total_tx - start.total_tx) / 1024;

    EV_INFO << "Total kb rx: " << kbyteRx << std::endl;
    EV_INFO << "Total kb tx: " << kbyteTx << std::endl;

//        totalRxKbHist.collect((stop.total_rx - start.total_rx));
//        totalTxKbHist.collect((stop.total_tx - start.total_tx));
    TotalRxBitinInterval.record((stop.total_rx - start.total_rx));
    TotalTxBitinInterval.record((stop.total_tx - start.total_tx));

    txBitPerSecond.record((double) (stop.total_tx - start.total_tx) / duration);
    RxBitPerSecond.record((double) (stop.total_rx - start.total_rx) / duration);

    if ((stop.total_rx - start.total_rx)
            < par("trafficNeglectable").doubleValue())
        trafficRxNeglectable++;
    if ((stop.total_tx - start.total_tx)
            < par("trafficNeglectable").doubleValue())
        trafficTxNeglectable++;

    trafficTxTotal++;
    trafficRxTotal++;
}

void SimpleWiFi::onEnterBackgroundActive(DeviceStates oldState,
        DeviceStates newState, void *param) {
    EV_INFO << "In onEnterBackgroundActive" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
        return;
    }
    if (param != nullptr) {
        simtime_t *duration = static_cast<simtime_t*>(param);
        if (backgroundEventEndMessage != nullptr)
            throw cRuntimeError(
                    "There seems to be a scheduled BackgroundEventEnd Message!");
        backgroundEventEndMessage = new BackgroundEventEndMessage();
        backgroundEventEndMessage->setBackgroundEventCancelled(false);
        scheduleAt(simTime() + *duration, backgroundEventEndMessage);
        addMessageForUserStats(
                new StatisticEntry(true, simTime(),
                        StatisticType::USAGE_BACKGROUND));
        cleanupMessagesForStats();
        wifiAsyncDeviceState.record(DEVICE_STATE_OCCUPIED_BACKGROUND);
        bgTotalStarted++; // Count number of started bg jobs
    } else {
        throw cRuntimeError(
                "A maximum duration has to be entered when entering the bg active state");
    }
}

bool SimpleWiFi::onExitBackgroundActive(DeviceStates oldState,
        DeviceStates newState, void *param) {
    EV_INFO << "In onExitBackgroundActive" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
        EV_INFO << "Self collision: bg bg" << std::endl;
        return false; // ignore self loops for bg active
    } else if (newState == DEVICE_STATE_OCCUPIED_USER) {
        // we have to cancel the bg event: User needs resources!
        if (backgroundEventEndMessage == nullptr)
            throw cRuntimeError(
                    "This should not happen: in bg job state but no end message scheduled?");
        simtime_t startTime = backgroundEventEndMessage->getSendingTime();
        EV_INFO << "Message was due in "
                       << backgroundEventEndMessage->getArrivalTime()
                               - simTime() << "seconds" << std::endl;
        if (par("rescheduleInteruptedBackgroundTasks").boolValue()) {
            EV_INFO << "Keeping remaining time for job rescheduling"
                           << std::endl;
            bgJobsToContinue.push(
                    backgroundEventEndMessage->getArrivalTime() - simTime());
        }
        cancelAndDelete(backgroundEventEndMessage);
        backgroundEventEndMessage = nullptr;
        sendBatteryConsumptionEvent(
                simMax(startTime, lastCalculateBatteryDiffs));
        addMessageForUserStats(
                new StatisticEntry(false, simTime(),
                        StatisticType::USAGE_BACKGROUND));
        cleanupMessagesForStats();

        // Change to state free. There, we will handle the remaining bg jobs
        wifiAsyncDeviceState.record(DEVICE_STATE_FREE);

        bgJobsInterruptedByUser++;
        return true; // Can change to new state
    }

    if (param != nullptr) {
        // Regular cancel: bg event timed out
        simtime_t *startTime = static_cast<simtime_t*>(param);
        sendBatteryConsumptionEvent(
                simMax(*startTime, lastCalculateBatteryDiffs));
        addMessageForUserStats(
                new StatisticEntry(false, simTime(),
                        StatisticType::USAGE_BACKGROUND));
        cleanupMessagesForStats();
        wifiAsyncDeviceState.record(DEVICE_STATE_FREE);
    } else {
        throw cRuntimeError(
                "The start time should have been set on state change.");
    }

    return true; // Transition successful
}

void SimpleWiFi::onEnterUserActive(DeviceStates oldState, DeviceStates newState,
        void *param) {
    EV_INFO << "In onEnterUserActive" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
        return;
    }
    addMessageForUserStats(
            new StatisticEntry(true, simTime(), StatisticType::USAGE_USER));
    cleanupMessagesForStats();
    wifiAsyncDeviceState.record(DEVICE_STATE_OCCUPIED_USER);
    trafficWifiStartValues = lastTrafficValues; // for traffic calculation
}

bool SimpleWiFi::onExitUserActive(DeviceStates oldState, DeviceStates newState,
        void *param) {
    EV_INFO << "In onExitUserActive" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
        return false;
    } else if (newState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
        userActiveBgRequested++; // count for stats
        if (par("enqueueBackgroundTasks").boolValue()) {
            // run the job when in idle stat
            simtime_t *duration = static_cast<simtime_t*>(param);
            bgJobsToContinue.push(*duration);
        }
        EV_INFO << "Do not perform transition. User is preferred!" << std::endl;
        return false; // Do not perform state transition
    }

    EV_INFO << "Battery timings: " << std::endl;
    EV_INFO << "  simTime=" << simTime() << std::endl;
    EV_INFO << "  lastCalcBatteryDiffs=" << lastCalculateBatteryDiffs
                   << std::endl;
    EV_INFO << "  inCurrentStateSince=" << getInCurrentStateSince()
                   << std::endl;
    EV_INFO << std::endl;

    sendBatteryConsumptionEvent(
            simMax(getInCurrentStateSince(), lastCalculateBatteryDiffs));
    addMessageForUserStats(
            new StatisticEntry(false, simTime(), StatisticType::USAGE_USER));
    cleanupMessagesForStats();
    wifiAsyncDeviceState.record(DEVICE_STATE_FREE);

    calcTrafficDelta(trafficWifiStartValues, lastTrafficValues);

    return true; // Transition successful
}

bool SimpleWiFi::onExitIdle(DeviceStates oldState, DeviceStates newState,
        void *param) {
    EV_INFO << "In onExitIdle" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
    }
    return true; // We can always leave the idle state
}

/*
 * Enter Methods
 */

void SimpleWiFi::onEnterIdle(DeviceStates oldState, DeviceStates newState,
        void *param) {
    EV_INFO << "In onEnterIdle" << std::endl;
    if (oldState == newState) {
        EV_INFO << "SELF LOOP" << std::endl;
    }
// TODO Implement
}

void SimpleWiFi::onTransitionDone(DeviceStates deviceState) {
    if (deviceState == DEVICE_STATE_FREE) {
        // Schedule next state?
        if (par("rescheduleInteruptedBackgroundTasks").boolValue()) {
            if (bgJobsToContinue.size() > 0) {
                simtime_t remainingDuration = bgJobsToContinue.front();
                bgJobsToContinue.pop();
                setNewState(DEVICE_STATE_OCCUPIED_BACKGROUND,
                        &remainingDuration);
            }
        }

    }
    // State change done -> notify listeners
    simsignal_t signal = registerSignal(WIFI_STATUS_UPDATE_SIGNAL);
    if (mayHaveListeners(signal))
        emit(signal, getDeviceState());
}

simtime_t SimpleWiFi::simMax(simtime_t a, simtime_t b) {
    if (a > b)
        return a;
    return b;
}

} //namespace
