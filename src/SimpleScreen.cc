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

#include "SimpleScreen.h"

namespace eventsimulator {

Define_Module(SimpleScreen);

SimpleScreen::SimpleScreen() {
    collectMeasurementsEvent = nullptr;
}

SimpleScreen::~SimpleScreen() {
    if (getSimulation()->getSystemModule()->isSubscribed(
    CALCULATE_BATTERY_DIFFS, this))
        getSimulation()->getSystemModule()->unsubscribe(CALCULATE_BATTERY_DIFFS,
                this);

    cancelAndDelete(collectMeasurementsEvent);
}

void SimpleScreen::initialize() {
    EV_INFO << "Init screen" << endl;
    screenStatusValues.setName("Screen Status");
    screenStatusPropability.setName("Screen Status in Window");

    EV_INFO << "Window Size for statistics: "
                   << par("statsWindowSize").intValue() << "s" << std::endl;
    collectMeasurementsEvent = new cMessage("collectMeasurements");

    getSimulation()->getSystemModule()->subscribe(CALCULATE_BATTERY_DIFFS,
            this);

    initialized = true;
}

void SimpleScreen::receiveSignal(cComponent *component, simsignal_t signal,
        bool b, cObject *details) {
    Enter_Method("receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details)");
    if (signal == registerSignal(CALCULATE_BATTERY_DIFFS)) {
        if (screenOn) {
            EV_INFO << "Recalc used energy for screen due to regular event." << std::endl;
            simtime_t duration = simTime() - screenSwitchedOn;
            screenSwitchedOn = simTime();
            sendBatteryConsumptionEvent(duration);
        }
    }

}

void SimpleScreen::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message->getPayloadType() != EVENT_TYPE_SCREEN) {
            // Not our message
            delete msg;
            return;
        }

        ScreenEventMessage *screenMsg = check_and_cast<ScreenEventMessage *>(
                msg);

        addMessageForUserStats(
                new StatisticEntry(screenMsg->getScreenOn(),
                        screenMsg->getArrivalTime(),
                        StatisticType::USAGE_USER));

        bool previousState = screenOn;

        screenStatusValues.record(screenMsg->getScreenOn());
        screenOn = screenMsg->getScreenOn();

        if (screenOn && !previousState) {
            // off -> on
            screenSwitchedOn = simTime();
        } else if (!screenOn && previousState) {
            // on -> off
            simtime_t duration = simTime() - screenSwitchedOn;
            EV_INFO << "Screen duration " << duration << std::endl;
            sendBatteryConsumptionEvent(duration);
        } else {
            // Only count device changes
        }

        delete screenMsg;
    } else if (msg == collectMeasurementsEvent) {
        cleanupMessagesForStats();
        // printEventsForStats();
        std::map<StatisticResult, double> result = calcUserStats(
                par("statsWindowSize").intValue());

        double on = 0.0;
        double off = 0.0;

        for (auto v : result) {
            if (v.first.isActive) {
                on += v.second;
            } else if (!v.first.isActive) {
                off += v.second;
            } else {
                EV_ERROR << "Unknown type: " << v.first << " " << std::endl;
            }
        }

        EV_INFO << "SCREEN PERIODIC: " << on << " " << off << std::endl;

        screenStatusPropability.record(on);

        scheduleAt(
                simTime() + par("periodicStatsCollectionInterval").intValue(),
                msg);
    } else {
        // Message not for this module
        delete msg;
    }

    simsignal_t signal = registerSignal(SCREEN_STATUS_UPDATE_SIGNAL);
    if (mayHaveListeners(signal))
        emit(signal, screenOn);
}

void SimpleScreen::refreshDisplay() const {
    std::string msgString;

    if (screenOn)
        msgString = "Screen on";
    else
        msgString = "Screen off";
    getDisplayString().setTagArg("t", 0, msgString.c_str());

    if (screenOn)
        getDisplayString().setTagArg("i", 0, "status/screen_on");
    else
        getDisplayString().setTagArg("i", 0, "status/screen_off");
}

void SimpleScreen::sendBatteryConsumptionEvent(simtime_t duration) {
    CapacityEvent *cEvent = new CapacityEvent();
    cEvent->setSenderType(CAPACITY_EVENT_TYPE_SCREEN);
    double chargeChange = -1 * par("screenCurrentDrawn").doubleValue()
            * duration.dbl();
    EV_INFO << "Screen used " << chargeChange << "C (As) (" << duration << "s)"
                   << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb
    cEvent->setDischargeDuration(duration);

    if (gateSize("out") < 1)
        throw cRuntimeError("Invalid number of output gates: %d; must be >=1",
                gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
