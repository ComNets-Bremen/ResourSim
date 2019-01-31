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
#include "DeviceStates.h"
#include "BaseResourceMode.h"

using namespace omnetpp;

namespace eventsimulator {

/**
 * TODO - Generated class
 */
class SimpleWiFi : public BaseResourceMode<WiFiEventMessage>
{
  public:
    SimpleWiFi();
    ~SimpleWiFi();

    void refreshDisplay() const;

    void finish();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  private:
    bool initialized = false;
    int wifiStatus;

    DeviceStates deviceState = DEVICE_STATE_UNKNOWN;

    // Statistics

    cOutVector wifiStatusValues;
    cOutVector wifiStatusOn;
    cOutVector wifiStatusOff;
    cOutVector wifiDeviceState;

    long collisionBackground = 0;
    long collisionUser = 0;
    long collisionSelf = 0;

    cMessage *collectMeasurementsEvent;
    cMessage *backgroundServiceEndMessage;
};

} //namespace

#endif
