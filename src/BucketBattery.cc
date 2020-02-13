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

#include "BucketBattery.h"

namespace eventsimulator {

Define_Module(BucketBattery);

BucketBattery::BucketBattery() {
    collectMeasurementsEvent = nullptr;
}

BucketBattery::~BucketBattery() {
    cancelAndDelete(collectMeasurementsEvent);
}

void BucketBattery::initialize() {

    currentBatteryCharge.setName("Current capacity in C");
    currentBatteryPercentage.setName("Current capacity in %");
    batteryCritical.setName("Battery critical");
    batteryIsCharging.setName("Is Charging");
    realBatteryLevel.setName("Battery level reported by real device");

    collectMeasurementsEvent = new cMessage("collectMeasurements");
    scheduleAt(0, collectMeasurementsEvent);

    batteryPercentageSignalId = registerSignal(BATTERY_PERCENTAGE_SIGNAL);
    batteryPercentageInconvinientSignalId = registerSignal(
    BATTERY_INCONVENIENT_SIGNAL);

    batteryCharge = 0.5 * par("batteryCapacityCoulomb").doubleValue(); // We start with a half filled battery
}

void BucketBattery::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {

        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message->getPayloadType() != EVENT_TYPE_BATTERY) {
            // Not our message
            delete msg;
            return;
        }

        BatteryEventMessage *batMsg = check_and_cast<BatteryEventMessage *>(
                msg);

        realBatteryLevel.record(batMsg->getTheoreticalAbsolutePercentage());

        bool currentBatteryState = batMsg->getChg_ac() || batMsg->getChg_usb()
                || batMsg->getChg_ac() || batMsg->getIs_charging();
        if (currentBatteryState != lastStateIsCharging) {
            // State changed
            simtime_t duration = simTime() - lastBatteryStateChange;
            double coulomb;
            if (lastStateIsCharging)
                coulomb = duration.dbl()
                        * par("batteryChargeCoulombPerHour").doubleValue()
                        / 60.0 / 60.0;
            else
                coulomb = -1 * duration.dbl()
                        * par("batteryDischargeCoulombPerHour").doubleValue()
                        / 60.0 / 60.0;
            EV_INFO << "Bucket change: " << coulomb << std::endl;

            batteryCharge += coulomb;
            batteryCharge = std::fmin(batteryCharge,
                    par("batteryCapacityCoulomb")); // Bucket model cannot be fuller than full
            batteryCharge = std::fmax(batteryCharge, 0); // Or less than null
        }

        recalculateBatteryCharge();

        // Tidy up
        lastStateIsCharging = currentBatteryState;
        lastBatteryStateChange = simTime();

        delete batMsg;

    } else if (msg == collectMeasurementsEvent) {
        EV_INFO << "Periodic data collection" << std::endl;
        currentBatteryCharge.record(getBatteryChargeCoulomb());
        currentBatteryPercentage.record(getBatteryChargePercent());

        scheduleAt(
                simTime() + par("periodicStatsCollectionInterval").intValue(),
                msg);
    } else if (dynamic_cast<CapacityEvent *>(msg) != nullptr) {
        EV_INFO << "Recalc battery stat start: " << batteryCharge << std::endl;
        CapacityEvent *cEvent = dynamic_cast<CapacityEvent *>(msg);
        EV_INFO << "Changing capacity by " << cEvent->getChargeChange() << "C in " << cEvent->getDischargeDuration() << "s " << cEvent->getSenderModule()
                       << std::endl;


        batteryCharge += cEvent->getChargeChange();
        recalculateBatteryCharge();
        delete cEvent;
        EV_INFO << "Recalc battery stat end: " << batteryCharge << std::endl;
    } else {
        // Not for us
        delete msg;
    }

    batteryIsCharging.record(lastStateIsCharging);

    if (mayHaveListeners(batteryPercentageSignalId)
            || mayHaveListeners(batteryPercentageInconvinientSignalId)) {
        double currentCharge = getBatteryChargePercent();
        emit(batteryPercentageSignalId, currentCharge);

        emit(batteryPercentageInconvinientSignalId,
                currentCharge
                        < par("inconvenientBatteryThreshold").doubleValue());

        EV_INFO << "EMITTED SIGNAL BATTERY" << std::endl;
    }
}

void BucketBattery::recalculateBatteryCharge() {
    simtime_t duration = simTime() - lastBatteryStateChange;
    double coulomb;
    EV_INFO << "Recalc battery stat start: " << batteryCharge
                   << " is charging: " << lastStateIsCharging << std::endl;
    if (lastStateIsCharging)
        coulomb = duration.dbl()
                * par("batteryChargeCoulombPerHour").doubleValue() / 60.0
                / 60.0;
    else
        coulomb = -1 * duration.dbl()
                * par("batteryDischargeCoulombPerHour").doubleValue() / 60.0
                / 60.0;

    EV_INFO << "Regular (dis)charge: " << coulomb << "C in " << duration << " seconds" << std::endl;

    batteryCharge += coulomb;
    batteryCharge = std::fmin(batteryCharge, par("batteryCapacityCoulomb")); // Bucket model cannot be fuller than full
    batteryCharge = std::fmax(batteryCharge, 0); // Or less than null
    lastBatteryStateChange = simTime();

    if (batteryCharge / par("batteryCapacityCoulomb").doubleValue()
            < par("inconvenientBatteryThreshold").doubleValue()) {
        // Critical battery value
        batteryCritical.record(1);
    } else {
        // battery level okay
        batteryCritical.record(0);
    }
    EV_INFO << "Recalc battery stat end: " << batteryCharge << std::endl;
}

double BucketBattery::getBatteryChargeCoulomb() {
    recalculateBatteryCharge();
    return batteryCharge;
}

double BucketBattery::getBatteryChargePercent() {
    recalculateBatteryCharge();
    return batteryCharge / par("batteryCapacityCoulomb").doubleValue();
}

void BucketBattery::refreshDisplay() const {
    char buf[40];
    if (lastStateIsCharging) {
        getDisplayString().setTagArg("i", 0, "status/battery_charging");
    } else {
        getDisplayString().setTagArg("i", 0, "status/battery_discharging");
    }
    sprintf(buf, "charging: %i, level: %.2f%%", lastStateIsCharging,
            batteryCharge / par("batteryCapacityCoulomb").doubleValue()
                    * 100.0);
    getDisplayString().setTagArg("t", 0, buf);
}

} //namespace
