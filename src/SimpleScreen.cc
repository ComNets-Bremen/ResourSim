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

#include "SimpleScreen.h"

namespace eventsimulator {

Define_Module(SimpleScreen);

SimpleScreen::SimpleScreen() {
    screenStatusValues.setName("Screen Status");
}

SimpleScreen::~SimpleScreen() {

}

void SimpleScreen::initialize() {
    EV_INFO << "Init screen" << endl;
    // TODO: Init?
    initialized = true;
}

void SimpleScreen::handleMessage(cMessage *msg) {
    if (dynamic_cast<BaseEventMessage *>(msg) != nullptr) {
        BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
        if (message->getPayloadType() != EVENT_TYPE_SCREEN) {
            // Not our message
            delete msg;
            return;
        }

        if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
            EV_INFO << "Collision Background" << endl;
            collisionUser++;
            delete msg;
            return;
        }

        ScreenEventMessage *screenMsg = check_and_cast<ScreenEventMessage *>(
                msg);
        screenStatusValues.record(screenMsg->getScreenOn());
        screenOn = screenMsg->getScreenOn();

        if (screenOn)
            deviceState = DEVICE_STATE_OCCUPIED_USER;
        else
            deviceState = DEVICE_STATE_FREE;

        delete screenMsg;
    } else if (dynamic_cast<BackgroundEventMessage *>(msg) != nullptr) {
        EV_INFO << "Background Message" << endl;
        BackgroundEventMessage *backgroundEventMessage = check_and_cast<
                BackgroundEventMessage *>(msg);
        if (backgroundEventMessage->getBackgroundType()
                == BACKGROUND_EVENT_TYPE_CPU) {

            if ((deviceState == DEVICE_STATE_FREE)
                    or (deviceState == DEVICE_STATE_UNKNOWN)) {
                deviceState = DEVICE_STATE_OCCUPIED_BACKGROUND;
                backgroundServiceEndMessage = new cMessage(
                        "End Background Service");
                scheduleAt(simTime() + backgroundEventMessage->getDuration(),
                        backgroundServiceEndMessage);
            } else if (deviceState == DEVICE_STATE_OCCUPIED_BACKGROUND) {
                EV_INFO << "self collision" << endl;
                collisionSelf++;
            } else {
                collisionBackground++;
            }

        }
        delete msg;
    } else if (msg == backgroundServiceEndMessage) {
        EV_INFO << "End background service" << endl;
        deviceState = DEVICE_STATE_FREE;
        delete msg;
    } else {
        EV_ERROR << "Unknown Message" << endl;
        delete msg;
    }
}

void SimpleScreen::finish() {
    recordScalar("#collisionUser", collisionUser);
    recordScalar("#collisionBackground", collisionBackground);
    recordScalar("#collisionSelf", collisionSelf);
}

void SimpleScreen::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "Screen on: %i", screenOn);
    getDisplayString().setTagArg("t", 0, buf);

    if (screenOn)
        getDisplayString().setTagArg("i", 0, "status/screen_on");
    else
        getDisplayString().setTagArg("i", 0, "status/screen_off");
}

} //namespace
