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

#ifndef __RESOURSIM_BUCKETBATTERY_H_
#define __RESOURSIM_BUCKETBATTERY_H_

#include <omnetpp.h>
#include <algorithm>
#include "capacity_messages/CapacityMessages.h"
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "warning_messages/BatteryCriticalWarningMessage_m.h"
#include "ResourceSignals.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 */
class BucketBattery: public cSimpleModule {
public:
    BucketBattery();
    virtual ~BucketBattery();
    void refreshDisplay() const;
    void recalculateBatteryCharge();
    double getBatteryChargeCoulomb();
    double getBatteryChargePercent();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

private:
    double batteryCharge;

    cMessage *collectMeasurementsEvent;

    bool lastStateIsCharging = false;
    simtime_t lastBatteryStateChange;

    cOutVector currentBatteryCharge;   // C
    cOutVector currentBatteryPercentage; // %
    cOutVector batteryCritical; // 1/0
    cOutVector batteryIsCharging; // 1/0
    cOutVector realBatteryLevel; // %

    simsignal_t batteryPercentageSignalId;
    simsignal_t batteryPercentageInconvinientSignalId;
    simsignal_t batteryStateChangedSignalId;
};

} //namespace

#endif
