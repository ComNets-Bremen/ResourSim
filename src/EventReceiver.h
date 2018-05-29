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

#ifndef __EVENTSIMULATOR_EVENTRECEIVER_H
#define __EVENTSIMULATOR_EVENTRECEIVER_H

#include <omnetpp.h>
#include "event_messages/EventMessages.h"
#include "SimplePercentageBattery.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 * Implements a basic event receiver
 */
class EventReceiver : public cSimpleModule
{
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


    SimplePercentageBattery battery;
};

}; // namespace

#endif
