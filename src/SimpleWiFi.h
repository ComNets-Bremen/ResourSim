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

#ifndef __RESOURSIM_SIMPLEWIFI_H_
#define __RESOURSIM_SIMPLEWIFI_H_

#include <omnetpp.h>
#include <map>
#include <algorithm>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "capacity_messages/CapacityMessages.h"
#include "DeviceStates.h"
#include "BaseResourceMode.h"
#include "ResourceSignals.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 * TODO - Generated class
 */
class SimpleWiFi: public BaseResourceMode, public cListener {
public:
    SimpleWiFi();
    ~SimpleWiFi();

    void refreshDisplay() const;

    DeviceStates getDeviceState() const;
    virtual void receiveSignal(cComponent *component, simsignal_t signal,
            bool b, cObject *details);

    void finish();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void sendBatteryConsumptionEvent(simtime_t duration);

private:
    bool initialized = false;
    int lastReceivedWifiStatus = WIFI_UNKNOWN;

    void calcTrafficDelta(TrafficEventValues start, TrafficEventValues stop,
            simtime_t duration);

    DeviceStates deviceState = DEVICE_STATE_UNKNOWN;

    long trafficTxNeglectable = 0;
    long trafficRxNeglectable = 0;
    long trafficTxTotal = 0;
    long trafficRxTotal = 0;

    // Statistics

    cOutVector wifiStatusValues;
    cOutVector wifiStatusOn;
    cOutVector wifiStatusOff;
    cOutVector wifiDeviceState;

    cOutVector txBitPerSecond;
    cOutVector RxBitPerSecond;

    cOutVector TotalRxBitinInterval;
    cOutVector TotalTxBitinInterval;

//    cHistogram totalRxKbHist;
//    cHistogram totalTxKbHist;

    long occupiedUserCollisionBackground = 0;
    long occupiedBackgroundCollisionUser = 0;
    long occupiedBackgroundCollisionBackground = 0;
    long cancelledBackgroundOccupiedUser = 0;

    simtime_t lastTrafficEvent = 0;
    simtime_t lastUserWifiEvent = 0;
    simtime_t lastTrafficCalculation = 0;

    long mobile_rx = 0;
    long mobile_tx = 0;
    long total_rx = 0;
    long total_tx = 0;

    TrafficEventValues lastTrafficValues = {.mobile_rx = 0, .mobile_tx=0, .total_rx=0, .total_tx=0};
    TrafficEventValues trafficWifiStartValues = {.mobile_rx = 0, .mobile_tx=0, .total_rx=0, .total_tx=0};

    DeviceStates lastTrafficDeviceState = DEVICE_STATE_UNKNOWN;

    bool inCancelledBgState = false;

    simtime_t startOccupiedTime = 0;

    cMessage *collectMeasurementsEvent = nullptr;
};

} //namespace

#endif
