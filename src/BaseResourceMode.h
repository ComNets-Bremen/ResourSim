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
#include "StatisticEntry.h"

using namespace omnetpp;

namespace eventsimulator {

class BaseResourceMode: public cSimpleModule {
public:
    void addMessageForUserStats(StatisticEntry *msg);
    ~BaseResourceMode();
    BaseResourceMode();

    void cleanupMessagesForStats();
    void printEventsForStats();
    int getNumOfMessagesForUserStats();
    int getNumOfMessagesBackgroundStats();
    int getNumOfMessagesForBackgroundStats();

    std::map<std::string, double> calcUserStats(int windowSize,
            std::map<std::string, double> (*f)(
                    std::deque<StatisticEntry *> packets, int windowSize));

    std::map<std::string, double> calcUserStats(int windowSize);

private:
    std::deque<StatisticEntry *> myUserPackets;

    static std::map<std::string, double> defaultStatisticFunction(
            std::deque<StatisticEntry *> msg, int windowSize);
};

/**
 * Constructor
 */
inline BaseResourceMode::BaseResourceMode() {
    // nop
}

/**
 * Destructor, tidy up everything
 */
inline BaseResourceMode::~BaseResourceMode() {
    while (!myUserPackets.empty()) {
        delete (myUserPackets.front());
        myUserPackets.pop_front();
    }
}

/**
 * Default statistic function
 */
inline std::map<std::string, double> BaseResourceMode::defaultStatisticFunction(
        std::deque<StatisticEntry *> msg, int windowSize) {
    std::map<std::string, double> resultMap;

    std::string lastStatus;
    simtime_t lastTimestamp;
    bool lastSet = false;

    double period = std::min((double) windowSize, simTime().dbl()); // Should be 120 or the simulation time if less than 120

    for (auto *e : msg) {
        std::string status;

        if (e->getActive()) {
            if (e->getUsageType() == StatisticEntry::USAGE_BACKGROUND) {
                status = "USED: BACKGROUND";
            } else if (e->getUsageType() == StatisticEntry::USAGE_USER) {
                status = "USED: USER";
            } else {
                throw cRuntimeError("Invalid type: %s",
                        e->getUsageTypeString().c_str());
            }

        } else {
            status = "FREE";
        }

        if (!lastSet) {
            // first run: Store start values
            lastStatus = status;
            if (simTime() < windowSize)
                lastTimestamp = e->getStartTime();
            else
                lastTimestamp = simTime() - period;
            lastSet = true;
        } else if (msg.size() > 1) {
            // Add values only if we have more than one

            if (resultMap.count(status) == 0) {
                // Add time for this status
                resultMap[status] = 0.0;
            }

            simtime_t difference = e->getStartTime() - lastTimestamp;
            resultMap[lastStatus] += difference.dbl() / period;
            lastTimestamp = e->getStartTime();
            lastStatus = status;
        } else {
            // This is the first run and we do not have sufficient data
        }
    } // for

    // Add remaining values if the current time and the last timestamp are different
    if (lastSet && lastTimestamp != simTime() && msg.size() > 0) {
        resultMap[lastStatus] += (simTime() - lastTimestamp).dbl() / period;
    }

    return resultMap;
}

/**
 * Calculate the statistics using a callback function
 */
inline std::map<std::string, double> BaseResourceMode::calcUserStats(
        int windowSize,
        std::map<std::string, double> (*f)(std::deque<StatisticEntry *> packets,
                int windowSize)) {
    return (*f)(myUserPackets, windowSize);
}

/**
 * Calculates the statistics using the default function
 */
inline std::map<std::string, double> BaseResourceMode::calcUserStats(int windowSize) {
    return calcUserStats(windowSize, defaultStatisticFunction);
}


/**
 * Add a message to the stats. All messages from a certain module should be added (User)
 */
inline void BaseResourceMode::addMessageForUserStats(StatisticEntry *msg) {
    //myPackets.push_back(msg->dup());
    myUserPackets.push_back(msg);
}

/**
 * Print some statistics to EV_INFO
 */
inline void BaseResourceMode::printEventsForStats() {
    EV_INFO << "Statistics: " << std::endl;
    EV_INFO << "Current time: " << simTime() << std::endl;
    EV_INFO << "User statistics: " << simTime() << std::endl;
    for (auto *e : myUserPackets) {
        EV_INFO << "Packet time: " << e->getStartTime() << std::endl;
    }
}

/**
 * Tidy up the list of messages
 */
inline void BaseResourceMode::cleanupMessagesForStats() {

    int statsWindowSize = par("statsWindowSize").intValue();

    while (myUserPackets.size() > 1
            && (simTime() - myUserPackets[1]->getStartTime() > statsWindowSize)) {
        EV_INFO << "Drop user message: "
                       << myUserPackets.front()->getStartTime() << std::endl;
        delete (myUserPackets.front());
        myUserPackets.pop_front();
    }
}

/**
 * Get number of messages used for the current stats
 */
inline int BaseResourceMode::getNumOfMessagesForUserStats() {
    return myUserPackets.size();
}

} /* namespace eventsimulator */

#endif /* BASERESOURCEMODE_H_ */
