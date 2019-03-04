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

class StatisticType {
public:
    enum UsageType {
        USAGE_USER, USAGE_BACKGROUND
    };

};

class StatisticEntry {
public:

    StatisticEntry();

    StatisticEntry(bool isActive, simtime_t startTime,
            enum StatisticType::UsageType type) :
            m_isActive(isActive), m_startTime(startTime), m_usageType(type) {
    }
    ;
    virtual ~StatisticEntry();

    void setActive(bool isActive);
    bool getActive() const;

    void setStartTime(simtime_t starttime);
    simtime_t getStartTime() const;

    StatisticType::UsageType getUsageType() const;
    void setUsageType(StatisticType::UsageType type);
    std::string getUsageTypeString() const;

    friend std::ostream& operator<<(std::ostream& os,
            const StatisticEntry& obj) {
        return os << "Start: " << obj.getStartTime() << " Type: "
                << obj.getUsageTypeString() << " isActive: " << obj.getActive();
    }

private:
    bool m_isActive = false;
    simtime_t m_startTime = 0;
    enum StatisticType::UsageType m_usageType;
};

class StatisticResult {
public:

    enum StatisticType::UsageType usageType;
    bool isActive;

    StatisticResult() {
    }
    ;

    StatisticResult(bool isActive, enum StatisticType::UsageType type) :
            isActive(isActive), usageType(type) {
    }
    ;

    friend bool operator ==(const StatisticResult& lhs,
            const StatisticResult& rhs) {
        return lhs.usageType == rhs.usageType && lhs.isActive == rhs.isActive;
    }

    static inline std::string getTypeName(const StatisticResult& obj) {
        switch (obj.usageType) {
        case StatisticType::USAGE_USER:
            return "USER";
        case StatisticType::USAGE_BACKGROUND:
            return "BACKGROUND";
        default:
            throw cRuntimeError("Unhandled statistic type");
        }
    }

    friend std::ostream& operator<<(std::ostream& os,
            const StatisticResult& obj) {
        return os << "Type: " << StatisticResult::getTypeName(obj)
                << " Active: " << obj.isActive << std::endl;

    }

    friend bool operator<(const StatisticResult& lhs,
            const StatisticResult& rhs) {
        if (lhs.isActive != rhs.isActive) {
            return !lhs.isActive;
        } else if (lhs.usageType != rhs.usageType) {
            return lhs.usageType == StatisticType::USAGE_BACKGROUND;
        } else {
            return false;
        }
    }

};

} /* namespace eventsimulator */

#endif /* STATISTICENTRY_H_ */
