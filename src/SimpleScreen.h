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

#ifndef __RESOURSIM_SIMPLESCREEN_H_
#define __RESOURSIM_SIMPLESCREEN_H_

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

class SimpleScreen : public BaseResourceMode, public cListener
{
  public:
    SimpleScreen();
    virtual ~SimpleScreen();

    virtual void receiveSignal(cComponent *component, simsignal_t signal, bool b, cObject *details);

    void refreshDisplay() const;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void sendBatteryConsumptionEvent(simtime_t duration);


  private:
    bool initialized = false;
    bool screenOn = false;

    simtime_t screenSwitchedOn = 0;

    cMessage *collectMeasurementsEvent;

    cOutVector screenStatusValues;
    cOutVector screenStatusPropability;
};

} //namespace

#endif
