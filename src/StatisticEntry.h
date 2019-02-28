/*
 * StatisticEntry.h
 *
 *  Created on: Feb 28, 2019
 *      Author: jd
 */

#ifndef STATISTICENTRY_H_
#define STATISTICENTRY_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace eventsimulator {

class StatisticEntry {
public:
    enum UsageType {USAGE_USER, USAGE_BACKGROUND};

    StatisticEntry();

    StatisticEntry(bool isActive, simtime_t startTime, enum UsageType type) : m_isActive(isActive), m_startTime(startTime), m_usageType(type){};
    virtual ~StatisticEntry();

    void setActive(bool isActive);
    bool getActive() const;

    void setStartTime(simtime_t starttime);
    simtime_t getStartTime() const;

    UsageType getUsageType() const;
    void setUsageType(UsageType type);
    std::string getUsageTypeString() const;

    friend std::ostream& operator<<(std::ostream& os, const StatisticEntry& obj);

private:
    bool m_isActive = false;
    simtime_t m_startTime = 0;
    enum UsageType m_usageType;
};

} /* namespace eventsimulator */

#endif /* STATISTICENTRY_H_ */
