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
class BaseResourceMode: public cSimpleModule {
public:
    void addMessageForUserStats(T *msg);
    void addMessageForBackgroundStats(cMessage *msg);
    ~BaseResourceMode();
    BaseResourceMode();

    void cleanupMessagesForStats();
    void printEventsForStats();
    int getNumOfMessagesForUserStats();
    int getNumOfMessagesBackgroundStats();
    int getNumOfMessagesForBackgroundStats();

    std::map<std::string, double> calcUserStats(int windowSize,
            std::map<std::string, double> (*f)(std::deque<T *> packets,
                    int windowSize));
    std::map<std::string, double> calcBackgroundStats(int windowSize,
            std::map<std::string, double> (*f)(std::deque<cMessage *> packets,
                    int windowSize));

private:
    std::deque<T *> myUserPackets;
    std::deque<cMessage *> myBackgroundPackets;
};

/**
 * Constructor
 */
template<class T>
inline BaseResourceMode<T>::BaseResourceMode() {
    // nop
}

/**
 * Destructor, tidy up everything
 */
template<class T>
inline BaseResourceMode<T>::~BaseResourceMode() {
    while (!myUserPackets.empty()) {
        delete (myUserPackets.front());
        myUserPackets.pop_front();
    }

    while (!myBackgroundPackets.empty()) {
        delete (myBackgroundPackets.front());
        myBackgroundPackets.pop_front();
    }
}

/**
 * Calculate the statistics (User)
 */
template<class T>
inline std::map<std::string, double> BaseResourceMode<T>::calcUserStats(
        int windowSize,
        std::map<std::string, double> (*f)(std::deque<T *> packets,
                int windowSize)) {
    return (*f)(myUserPackets, windowSize);
}

/**
 * Calculate the statistics (Background)
 */
template<class T>
inline std::map<std::string, double> BaseResourceMode<T>::calcBackgroundStats(
        int windowSize,
        std::map<std::string, double> (*f)(std::deque<cMessage *> packets,
                int windowSize)) {
    return (*f)(myUserPackets, windowSize);
}

/**
 * Add a message to the stats. All messages from a certain module should be added (User)
 */
template<class T>
inline void BaseResourceMode<T>::addMessageForUserStats(T *msg) {
    //myPackets.push_back(msg->dup());
    myUserPackets.push_back(msg->dup());
}

/**
 * Add a message to the stats. All messages from a certain module should be added (Background)
 */
template<class T>
inline void BaseResourceMode<T>::addMessageForBackgroundStats(cMessage *msg) {
    //myPackets.push_back(msg->dup());
    myUserPackets.push_back(msg->dup());
}

/**
 * Print some statistics to EV_INFO
 */
template<class T>
inline void BaseResourceMode<T>::printEventsForStats() {
    EV_INFO << "Statistics: " << std::endl;
    EV_INFO << "Current time: " << simTime() << std::endl;
    EV_INFO << "User statistics: " << simTime() << std::endl;
    for (T *e : myUserPackets) {
        EV_INFO << "Packet time: " << e->getArrivalTime() << std::endl;
    }

    EV_INFO << "Background statistics: " << simTime() << std::endl;
    for (cMessage *e : myUserPackets) {
        EV_INFO << "Packet time: " << e->getArrivalTime() << std::endl;
    }
}

/**
 * Tidy up the list of messages
 */
template<class T>
inline void BaseResourceMode<T>::cleanupMessagesForStats() {

    int statsWindowSize = par("statsWindowSize").intValue();

    while (myUserPackets.size() > 1
            && (simTime() - myUserPackets[1]->getArrivalTime() > statsWindowSize)) {
        EV_INFO << "Drop user message: "
                       << myUserPackets.front()->getArrivalTime() << std::endl;
        delete (myUserPackets.front());
        myUserPackets.pop_front();
    }

    while (myBackgroundPackets.size() > 1
            && (simTime() - myBackgroundPackets[1]->getArrivalTime()
                    > statsWindowSize)) {
        EV_INFO << "Drop background message: "
                       << myBackgroundPackets.front()->getArrivalTime()
                       << std::endl;
        delete (myBackgroundPackets.front());
        myBackgroundPackets.pop_front();
    }

}

/**
 * Get number of messages used for the current stats
 */
template<class T>
inline int BaseResourceMode<T>::getNumOfMessagesForUserStats() {
    return myUserPackets.size();
}

/**
 * Get number of messages used for the current stats
 */
template<class T>
inline int BaseResourceMode<T>::getNumOfMessagesForBackgroundStats() {
    return myBackgroundPackets.size();
}

} /* namespace eventsimulator */

#endif /* BASERESOURCEMODE_H_ */
