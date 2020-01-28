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

#ifndef __EVENTSIMULATOR_EVENTMANAGER_H
#define __EVENTSIMULATOR_EVENTMANAGER_H

#include <omnetpp.h>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "capacity_messages/CapacityMessages.h"
#include "ResourceSignals.h"

#include "DeviceStates.h"

#include "SlidingDataset.h"
#include "OnOffDataset.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 * Implements a basic event manager
 */
class EventManager: public cSimpleModule, public cListener {

public:
    //EventManager(){};
    //EventManager(cComponent *component, simsignal_t signal): component(component), signal(signal) {}
    virtual ~EventManager();
    virtual void receiveSignal(cComponent *, simsignal_t, bool b, cObject *details);
    virtual void receiveSignal(cComponent *, simsignal_t, double d, cObject *details);
    virtual void receiveSignal(cComponent *, simsignal_t, long l, cObject *details);

    void refreshDisplay() const;
    void finish();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

private:
    void handleScreenEvent(ScreenEventMessage *msg);
    void handleBatteryEvent(BatteryEventMessage *msg);
    void handleWiFiEvent(WiFiEventMessage *msg);
    void handleTrafficEvent(TrafficEventMessage *msg);
    void handleCellularEvent(CellularEventMessage *msg);
    void handleAirplaneModeEvent(AirplaneModeEventMessage *msg);
    void handleBluetoothEvent(BluetoothEventMessage *msg);
    void handleUnknownEvent(BaseEventMessage *msg);

    bool isDeviceCritical;
    bool isDeviceDead;
    bool isDeviceCharging;

    simtime_t lastChargingStopped = 0.0;
    double lastChargingStoppedBatteryLevel = 0.0;
    double batteryLevel = 0.0;

    simsignal_t batteryRecalcId;

    cMessage *sendSignal_calculateBatteryDiffsEvent;

    SlidingDataset<bool> screenDecision;
    SlidingDataset<long> wifiDecision;

    OnOffDataset screenOffTimes;
    OnOffDataset timeBetweenChg;

    cMessage *collectDecisionDatasetsEvent;

    cOutVector screenDecisionStats;
    cOutVector screenDecisionStats24hrs;

    cOutVector wifiDecisionStatsUser;
    cOutVector wifiDecisionStatsUser24hrs;

    long numberBackgroundEvents;
    long numberCancelledBackgroundEvents;
};

}; // namespace

#endif // __EVENTSIMULATOR_EVENTMANAGER_H
