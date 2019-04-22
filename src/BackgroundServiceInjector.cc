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

/**
 * Inject background messages according to the pattern defined in the
 * configuration, i.e., in the omnetpp.ini or the corresponding .ned files
 */

#include "BackgroundServiceInjector.h"
#include "background_messages/BackgroundMessages.h"

namespace eventsimulator {

Define_Module(BackgroundServiceInjector);

void BackgroundServiceInjector::initialize() {
    int numEvents = par("numBackgroundEvents");
    EV_INFO << "Num params: " << numEvents << endl;

    simtime_t nextEvent = 0;
    for (int i = 0; i < numEvents; i++) {
        BackgroundEventContainer eventContainer;
        eventContainer.set_ParameterType("backgroundEvent");
        eventContainer.set_Index(i);

        eventContainer.updateNextTime(this);

        backgroundEvents.push_back(eventContainer);

        simtime_t t = SimTime(eventContainer.get_NextExecutionTime());
        if ((t < nextEvent) or (nextEvent == 0)) {
            nextEvent = t;
        }
    }

    if (par("numBackgroundEvents").intValue() > 0) {
        nextEventMessage = new cMessage("NextEventMessage");
        scheduleAt(nextEvent, nextEventMessage);
    }
}

void BackgroundServiceInjector::handleMessage(cMessage *msg) {
    if (msg == nextEventMessage) {
        EV_INFO << "@" << simTime() << " Next Event" << endl;

        // Handle all events
        for (int eventNumber = 0; eventNumber < backgroundEvents.size();
                eventNumber++) {
            if (backgroundEvents[eventNumber].get_NextExecutionTime()
                    <= simTime()) {
                // Schedule event

                BackgroundEventMessage *backgroundMsg =
                        new BackgroundEventMessage("BackgroundEventMessage");
                backgroundMsg->setStartTime(
                        backgroundEvents[eventNumber].get_NextExecutionTime());
                backgroundMsg->setDuration(
                        backgroundEvents[eventNumber].get_NextDurationTime(
                                this));
                backgroundMsg->setBackgroundType(
                        backgroundEvents[eventNumber].get_EnumType(this));

                for (int i = 0; i < gateSize("out"); i++)
                    send(backgroundMsg->dup(), "out", i);

                delete backgroundMsg;

                backgroundEvents[eventNumber].updateNextTime(this);
            }
        }

        simtime_t nextEvent = 0;
        for (int eventNumber = 0; eventNumber < backgroundEvents.size();
                eventNumber++) {
            if ((backgroundEvents[eventNumber].get_NextExecutionTime()
                    < nextEvent) or (nextEvent == 0)) {
                nextEvent =
                        backgroundEvents[eventNumber].get_NextExecutionTime();
            }
        }
        if (nextEvent > 0)
            scheduleAt(nextEvent, msg);
    }
}

void BackgroundServiceInjector::finish() {
    if (backgroundEvents.size() > 0){
        backgroundEvents.clear();
    }
}

} //namespace
