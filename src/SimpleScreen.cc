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
    screenStatusValues.setName("Screen Status");
    screenStatusPropability.setName("Screen Status in Window");
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
    EV_INFO << "Window Size for statistics: " << par("statsWindowSize").intValue() << "s" << std::endl;
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

/**
 * This function is used to create the required statistics out of the given dataset.
 *
 * It is used as a parameter for the calcStats function
 */
static std::map<std::string, double> statisticFunction(
        std::deque<ScreenEventMessage *> msg, int windowSize) {
    std::map<std::string, double> resultMap;

    std::string lastStatus;
    simtime_t lastTimestamp;
    bool lastSet = false;

    double period = std::min((double) windowSize, simTime().dbl()); // Should be 120 or the simulation time if less than 120

    for (ScreenEventMessage *e : msg) {
        std::string status;
        if (e->getScreenOn())
            status = "on";
        else
            status = "off";

        if (!lastSet) {
            // first run: Store start values
            lastStatus = status;
            if (simTime() < windowSize)
                lastTimestamp = simTime();
            else
                lastTimestamp = simTime() - period;
            lastSet = true;
        } else if (msg.size() > 1) {
            // Add values only if we have more than one

            if (resultMap.count(status) == 0) {
                // Add time for this status
                resultMap[status] = 0.0;
            }

            simtime_t difference = e->getArrivalTime() - lastTimestamp;

            //EV_INFO << lastStatus << " " << difference << " " << lastTimestamp << " " << simTime() << std::endl;

            resultMap[lastStatus] += difference.dbl() / period;
            lastTimestamp = e->getArrivalTime();
            lastStatus = status;
        } else {
            // This is the first run and we do not have sufficient data
        }
    }

    // Add remaining values if the current time and the last timestamp are different
    if (lastSet && lastTimestamp != simTime() && msg.size() > 0) {
        resultMap[lastStatus] = (simTime() - lastTimestamp).dbl() / period;
    }

    double checkNumber = 0; // Check if the sum of all values is 1 and print the results
    EV_INFO << "RESULTS: " << std::endl;
    for (auto r : resultMap) {
        EV_INFO << r.first << ": " << r.second << std::endl;
        checkNumber += r.second;
    }
    EV_INFO << "END RESULTS" << std::endl;

    if ((checkNumber < 0.99 || checkNumber > 1.01) && msg.size() > 0) {
        EV_ERROR << "DOUBLE NOT EQUAL 1.0. Value: " << checkNumber << std::endl;
    }

    return resultMap;
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

        addMessageForUserStats(screenMsg);

        bool previousState = screenOn;

        screenStatusValues.record(screenMsg->getScreenOn());
        screenOn = screenMsg->getScreenOn();

        if (screenOn && !previousState){
            // off -> on
            screenSwitchedOn = simTime();
        } else if (!screenOn && previousState){
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
        std::map<std::string, double> result = calcUserStats(
                par("statsWindowSize").intValue(), statisticFunction);

        double on = 0.0;
        double off = 0.0;

        for (auto v : result) {
            if (v.first == "on") {
                on += v.second;
            } else if (v.first == "off") {
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
        EV_ERROR << "Unknown Message" << endl;
        delete msg;
    }
}


void SimpleScreen::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "Screen on: %i", screenOn);
    getDisplayString().setTagArg("t", 0, buf);

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
    EV_INFO << "Used " << chargeChange << "C (As) (" << duration << "s)" << std::endl;
    cEvent->setChargeChange(chargeChange); // difference in Coulomb

    if (gateSize("out") < 1) throw cRuntimeError("Invalid number of output gates: %d; must be >=1", gateSize("out"));

    for (int i = 0; i < gateSize("out"); i++)
        send(cEvent->dup(), "out", i);
    delete cEvent;
}

} //namespace
