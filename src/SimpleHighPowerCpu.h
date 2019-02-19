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

#ifndef __RESOURSIM_SIMPLEHIGHPOWERCPU_H_
#define __RESOURSIM_SIMPLEHIGHPOWERCPU_H_

#include <omnetpp.h>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "ResourceSignals.h"
#include "capacity_messages/CapacityMessages.h"

using namespace omnetpp;

namespace eventsimulator {

class SimpleHighPowerCpu: public cSimpleModule, public cListener {
public:
    virtual ~SimpleHighPowerCpu();
    virtual void receiveSignal(cComponent *component, simsignal_t signal,
            bool b, cObject *details);
    void refreshDisplay() const;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void sendBatteryConsumptionEvent(simtime_t duration);

private:
    bool isCpuUsed = false;
    simtime_t cpuUsageStarted = 0;
    cMessage *backgroundServiceEndMessage;
    bool initialized;
};

} //namespace

#endif
