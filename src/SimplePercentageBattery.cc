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

void SimplePercentageBattery::handleMessage(cMessage *msg) {

    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {

        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message->getPayloadType() != EVENT_TYPE_BATTERY) {
            // Not our message
            delete msg;
            return;
        }

        BatteryEventMessage *batMsg = check_and_cast<BatteryEventMessage *>(
                msg);

        simtime_t curTime = simTime().dbl();

        realBatteryPercentage = batMsg->getTheoreticalAbsolutePercentage();

        EV_INFO << "Got message! " << batMsg->str() << endl;
        EV_ERROR << "AM I INITIALIZED? " << initialized << endl;
        if (!initialized) {
            setBatteryPercentage(batMsg->getTheoreticalAbsolutePercentage());
            EV_ERROR << "Set Battery Level to "
                            << batMsg->getTheoreticalAbsolutePercentage()
                            << endl;
            lastBatteryEventTime = curTime;
            initialized = true;
        }

        if (lastBatteryEventTime != curTime) {
            double timedelta = (curTime - lastBatteryEventTime).dbl(); // in seconds
            if (batMsg->getIs_charging()) {
                double deltaPercent = getClosestValue(batteryPercentage,
                        chargePerHourArray) * timedelta / 3600.0;
                EV_INFO << "Charge: " << deltaPercent << " in " << timedelta
                               << "seconds" << endl;
                incrementalBatteryChange(deltaPercent);
            } else {
                //double deltaPercent = par("dischargePerHour").doubleValue() * timedelta / 3600.0;
                double deltaPercent = getClosestValue(batteryPercentage,
                        dischargePerHourArray) * timedelta / 3600.0;
                EV_INFO << "Discharge: " << deltaPercent << " in " << timedelta
                               << "seconds" << endl;
                incrementalBatteryChange(deltaPercent);
            }
        }

        // Valid?
        if (!checkBatteryPercentageValid()) {
            EV_ERROR << "WRONG BATTERY VALUES: " << batteryPercentage << endl;
        }

        EV_INFO << "Delta: "
                       << (batMsg->getTheoreticalAbsolutePercentage()
                               - batteryPercentage) << endl;
        //delete batMsg;
        delete msg;

        // Collect data for statistical analysis
        theoreticalBatteryPercentageValues.record(batteryPercentage);
        realBatteryPercentageValues.record(realBatteryPercentage);
        //batteryPercentageDelta.record(std::sqrt(std::abs(std::pow(expectedBatteryPercentage, 2) - std::pow(batteryPercentage, 2))));
        batteryPercentageDelta.record(
                std::abs(realBatteryPercentage - batteryPercentage));

        lastBatteryEventTime = curTime;
    } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        EV_INFO << "Background Message" << endl;
        // TODO: Handle
        delete msg;
    } else if (msg == collectMeasurementsEvent) {
        EV_INFO << "Periodic data collection" << std::endl;

        if (realBatteryPercentage < par("inconvenientBatteryThreshold").doubleValue()) {
            realBatteryCritical.record(1);
        } else {
            realBatteryCritical.record(0);
        }

        if (batteryPercentage < par("inconvenientBatteryThreshold").doubleValue()) {
            theoreticalBatteryCritical.record(1);
        } else {
            theoreticalBatteryCritical.record(0);
        }

        scheduleAt(
                simTime() + par("periodicStatsCollectionInterval").intValue(),
                msg);
    } else {
        EV_ERROR << "Unknown message type" << endl;
        delete msg;
    }
}

SimplePercentageBattery::SimplePercentageBattery() {
    batteryPercentage = 0.5;
    realBatteryPercentage = 0.5;
    lastBatteryEventTime = 0.0;
    initialized = false;

    collectMeasurementsEvent = nullptr;

    theoreticalBatteryPercentageValues.setName("Calculated battery percentage");
    realBatteryPercentageValues.setName("Real battery percentage");
    batteryPercentageDelta.setName(
            "Delta between expected and calculated battery percentage");

    theoreticalBatteryCritical.setName("Theoretical Battery: critical");
    realBatteryCritical.setName("Real Battery: critical");

}

SimplePercentageBattery::~SimplePercentageBattery() {
    cancelAndDelete(collectMeasurementsEvent);
}

double SimplePercentageBattery::getBatteryPercentage() {
    return batteryPercentage;
}

void SimplePercentageBattery::setBatteryPercentage(double percentage) {
    batteryPercentage = percentage;
}

bool SimplePercentageBattery::checkBatteryPercentageValid() {
    return (batteryPercentage < 1.0f && batteryPercentage > 0.0f);
}

void SimplePercentageBattery::checkBattery() {
    if (!checkBatteryPercentageValid()) {
        // TODO: more checks?
        for (int i = 0; i < gateSize("out"); i++) {
            BatteryCriticalWarningMessage *msg =
                    new BatteryCriticalWarningMessage();
            msg->setCurrentBatteryLevel(batteryPercentage);
            send(msg, "out", i);
        }

    }
}

void SimplePercentageBattery::incrementalBatteryChange(double percentage) {
    batteryPercentage += percentage;
    batteryPercentage = std::min(batteryPercentage, 1.0);
    batteryPercentage = std::max(batteryPercentage, 0.0);
    // TODO: Handle zero, inform that the simulation should end / stop
    checkBattery();
}

bool SimplePercentageBattery::isInitialized() {
    return initialized;
}

void SimplePercentageBattery::initialize() {
    EV_INFO << "Init battery" << endl;
    // TODO: Init?

    // Add charging values
    const char *chargePerHourChar = par("chargePerHourArray").stringValue();
    //EV_INFO << "STRING FROM CONFIG: " << chargePerHourChar << endl;
    addToMap(chargePerHourChar, chargePerHourArray);
    EV_INFO << "Charging Values: " << chargePerHourArray.size() << endl;

    // Add discharging values
    const char *dischargePerHourChar =
            par("dischargePerHourArray").stringValue();
    //EV_INFO << "STRING FROM CONFIG: " << dischargePerHourChar << endl;
    addToMap(dischargePerHourChar, dischargePerHourArray);
    EV_INFO << "Discharging Values: " << dischargePerHourArray.size() << endl;

    //EV_INFO << "#### " << getClosestValue(33, dischargePerHourArray) << endl;
    //EV_INFO << "#### " << getClosestValue(33, chargePerHourArray) << endl;

    collectMeasurementsEvent = new cMessage("collectMeasurements");
    scheduleAt(0, collectMeasurementsEvent);
}

void SimplePercentageBattery::addToMap(const char *str,
        std::map<int, float> &m) {
    cStringTokenizer tokenizer(str, ",");
    while (tokenizer.hasMoreTokens()) {
        std::string t = tokenizer.nextToken();
        int pos = t.find(":");
        if (pos < 1) {
            EV_ERROR << "Invalid array!" << endl;
        } else {
            int key = std::stoi(t.substr(0, pos));
            float value = std::stof(t.substr(pos + 1, t.size() - 1));

            //EV_INFO << "Adding value to array: " << key << ": " << value <<  endl;
            m[key] = value;
        }
    }
}

float SimplePercentageBattery::getClosestValue(float value,
        std::map<int, float> &m) {

    int roundedValue = (int) std::round(value * 100.0);

    int closestKey = m.begin()->first;
    float closestValue = m.begin()->second;

    for (auto const& v : m) {
        if (abs(roundedValue - closestKey) > abs(roundedValue - v.first)) {
            closestKey = v.first;
            closestValue = v.second;
        }
    }
    return closestValue;

}

void SimplePercentageBattery::refreshDisplay() const {
    char buf[40];
    if (par("detailedStatus"))
        sprintf(buf, "is: %.2f%%, should: %.2f%%", batteryPercentage * 100.0,
                realBatteryPercentage * 100.0);
    else
        sprintf(buf, "Value: %.2f%%", batteryPercentage * 100.0);
    getDisplayString().setTagArg("t", 0, buf);
}

void SimplePercentageBattery::finish() {

}

} /* namespace eventsimulator */
