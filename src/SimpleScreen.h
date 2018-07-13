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
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "DeviceStates.h"

using namespace omnetpp;

namespace eventsimulator {

class SimpleScreen : public cSimpleModule
{
  public:
    SimpleScreen();
    virtual ~SimpleScreen();

    void refreshDisplay() const;
    void finish();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);


  private:
    bool initialized = false;
    bool screenOn = false;

    // We assume the screen as an indicator if the device is actively being used.
    // The user should not face problems due to an (uninterruptible) task running in the background
    // So the screen status corresponds more to the CPU usage than the actual screen usage.
    DeviceStates deviceState = DEVICE_STATE_UNKNOWN;
    cMessage *backgroundServiceEndMessage;

    // statistics
    long collisionBackground = 0;
    long collisionUser = 0;
    long collisionSelf = 0;

    cOutVector screenStatusValues;
};

} //namespace

#endif
