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

#ifndef __RESOURSIM_SIMPLEAIRPLANEMODE_H_
#define __RESOURSIM_SIMPLEAIRPLANEMODE_H_

#include <omnetpp.h>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"

using namespace omnetpp;

namespace eventsimulator{

class SimpleAirplaneMode : public cSimpleModule
{

public:
    SimpleAirplaneMode();
    virtual ~SimpleAirplaneMode();

    void refreshDisplay() const;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  private:
    bool initialized = false;
    bool airplaneModeOn = false;

    cOutVector airplaneModeValues;
};

} // namespace

#endif
