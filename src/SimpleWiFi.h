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
#include <queue>
#include <algorithm>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "capacity_messages/CapacityMessages.h"
#include "DeviceStates.h"
#include "BaseResourceMode.h"
#include "ResourceSignals.h"
#include "DeviceFsm.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 * TODO - Generated class
 */
class SimpleWiFi: public BaseResourceMode, public cListener , public DeviceFsm{
public:
    SimpleWiFi();
    ~SimpleWiFi();

    void refreshDisplay() const;

    virtual void receiveSignal(cComponent *component, simsignal_t signal,
            bool b, cObject *details);

    void finish();

    virtual void onEnterIdle(DeviceStates oldState, DeviceStates newState, void *param);
    virtual void onEnterBackgroundActive(DeviceStates oldState, DeviceStates newState, void *param);
    virtual void onEnterUserActive(DeviceStates oldState, DeviceStates newState, void *param);

    virtual bool onExitBackgroundActive(DeviceStates oldState, DeviceStates newState, void *param);
    virtual bool onExitUserActive(DeviceStates oldState, DeviceStates newState, void *param);
    virtual bool onExitIdle(DeviceStates oldState, DeviceStates newState, void *param);

    virtual void onTransitionDone(DeviceStates deviceState);

    static simtime_t simMax(simtime_t a, simtime_t b);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void sendBatteryConsumptionEvent(simtime_t duration);

private:
    bool initialized = false;
    int lastReceivedWifiStatus = WIFI_UNKNOWN;

    void calcTrafficDelta(TrafficEventValues start, TrafficEventValues stop,
            simtime_t duration);

    long trafficTxNeglectable = 0;
    long trafficRxNeglectable = 0;
    long trafficTxTotal = 0;
    long trafficRxTotal = 0;

    // Statistics

    cOutVector wifiStatusValues;
    cOutVector wifiStatusOn;
    cOutVector wifiStatusOff;
    cOutVector regularWifiDeviceState;
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

    
    simtime_t lastTrafficEvent = 0;
    simtime_t lastUserWifiEvent = 0;
    simtime_t lastTrafficCalculation = 0;

    simtime_t lastCalculateBatteryDiffs = 0;

    long mobile_rx = 0;
    long mobile_tx = 0;
    long total_rx = 0;
    long total_tx = 0;

    TrafficEventValues lastTrafficValues = {.mobile_rx = 0, .mobile_tx=0, .total_rx=0, .total_tx=0};
    TrafficEventValues trafficWifiStartValues = {.mobile_rx = 0, .mobile_tx=0, .total_rx=0, .total_tx=0};

    DeviceStates lastTrafficDeviceState = DEVICE_STATE_UNKNOWN;

    cMessage *collectMeasurementsEvent = nullptr;

    BackgroundEventEndMessage *backgroundEventEndMessage = nullptr;

    std::queue<simtime_t> bgJobsToContinue;

    long userActiveBgRequested = 0;
    long bgTotalStarted = 0;
    long bgJobsInterruptedByUser = 0;

};

} //namespace

#endif
