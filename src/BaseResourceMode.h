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
 * Layer to calculate some packet statistics
 *
 * Jens Dede, <jd@comnets.uni-bremen.de>
 */

#ifndef BASERESOURCEMODE_H_
#define BASERESOURCEMODE_H_

#include <omnetpp.h>
#include <deque>
#include <algorithm>
#include <map>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "DeviceStates.h"
#include "BaseResourceMode.h"

using namespace omnetpp;

namespace eventsimulator {

template<class T>
class BaseResourceMode : public cSimpleModule {
public:
    void addMessageForStats(T *msg);
    ~BaseResourceMode();

    void cleanupMessagesForStats();
    void printEventsForStats();

    std::map<std::string, double> calcStats(int windowSize, std::map<std::string, double> (*f)(std::deque<T *> packets, int windowSize));

private:
    std::deque<T *> myPackets;
};

/**
 * Destructor, tidy up everything
 */
template<class T>
inline BaseResourceMode<T>::~BaseResourceMode(){
    while(!myPackets.empty()){
        delete(myPackets.front());
        myPackets.pop_front();
    }
}

/**
 * Calculate the statistics
 */
template<class T>
inline std::map<std::string, double> BaseResourceMode<T>::calcStats(int windowSize, std::map<std::string, double> (*f)(std::deque<T *> packets, int windowSize)){
    return (*f)(myPackets, windowSize);
}

/**
 * Add a message to the stats. All messages from a certain module should be added
 */
template<class T>
inline void BaseResourceMode<T>::addMessageForStats(T *msg){
    //myPackets.push_back(msg->dup());
    myPackets.push_back(msg->dup());
}

/**
 * Print some statistics to EV_INFO
 */
template<class T>
inline void BaseResourceMode<T>::printEventsForStats(){
    EV_INFO << "Event statistics: " << std::endl;
    EV_INFO << "Current time: " << simTime() << std::endl;
    for (T *e : myPackets){
        EV_INFO << "Packet time: " << e->getArrivalTime() << std::endl;
    }
}


/**
 * Tidy up the list of messages
 */
template<class T>
inline void BaseResourceMode<T>::cleanupMessagesForStats(){

    int statsWindowSize = par("statsWindowSize").intValue();

    while (myPackets.size() > 1 && (simTime() - myPackets[1]->getArrivalTime() > statsWindowSize)){
        EV_INFO << "Drop message: " << myPackets.front()->getArrivalTime() << std::endl;
        delete(myPackets.front());
        myPackets.pop_front();
    }
}

} /* namespace eventsimulator */

#endif /* BASERESOURCEMODE_H_ */
