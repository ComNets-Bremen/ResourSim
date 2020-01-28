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
    DeviceStates asyncDeviceState = DEVICE_STATE_UNKNOWN;

    long trafficTxNeglectable = 0;
    long trafficRxNeglectable = 0;
    long trafficTxTotal = 0;
    long trafficRxTotal = 0;

    // Statistics

    cOutVector wifiStatusValues;
    cOutVector wifiStatusOn;
    cOutVector wifiStatusOff;
    cOutVector wifiDeviceState;
    cOutVector wifiAsyncDeviceState;

    cOutVector txBitPerSecond;
    cOutVector RxBitPerSecond;

    cOutVector TotalRxBitinInterval;
    cOutVector TotalTxBitinInterval;

//    cHistogram totalRxKbHist;
//    cHistogram totalTxKbHist;
    /*
     * --------------------------------------------------
     * running / start  |     user      |  Background   |
     *------------------|---------------|---------------|
     *                  |               |unwanted should|
     * user             | won't happen  |not happen,    |
     *                  |               |prevented      |
     * -----------------|---------------|---------------|
     *                  |               |               |
     * background       | prediction    | self collision|
     *                  | prevent       | acceptable    |
     *---------------------------------------------------
     *                  |               |               |
     * free             |   okay        |   okay        |
     * (also bg cancel) |               |               |
     *---------------------------------------------------
     *
     * In case of free for new tasks: check if free or cancelled bg:
     * 
     * Ideally #cancelled bg events == #user events in case of cancelled events
     * 
     * 
     * Optimize:
     * - # of cancelled bg events low
     * - # of user collisions (bg running, user requesting) low
     * - # of user events in case of cancelled bg event high
     * 
     */

    long occupiedUserStartBackground = 0;       // User is using the device and a background should be started
    long occupiedBackgroundStartUser = 0;       // A background task is running and the user accesses the device
    long occupiedBackgroundStartBackground = 0; // A background task is running and a second background task is started (self collision)
    long cancelledBackgroundOccupiedUser = 0;   // Background service was cancelled and user is now using the device.   
    long bgCancelledUserActivePeriods = 0;      // Background service was cancelled and the user was using the device in this period
    
    long bgCancelledAndUserActiveInPeriod = 0;  // Number of collisions with cancelled bg events by user in this period

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
