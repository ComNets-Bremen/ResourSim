/*
 * StatisticEntry.cc
 *
 * Represents the statistic value of an entry.
 *
 *  Created on: Feb 28, 2019
 *      Author: jd
 */

#include "StatisticEntry.h"

namespace eventsimulator {

StatisticEntry::StatisticEntry() {
    // TODO Auto-generated constructor stub
}


StatisticEntry::~StatisticEntry() {
    // TODO Auto-generated destructor stub
}

void StatisticEntry::setActive(bool isActive) {
    m_isActive = isActive;
}
bool StatisticEntry::getActive() const{
    return m_isActive;
}

void StatisticEntry::setStartTime(simtime_t starttime) {
    m_startTime = starttime;
}
simtime_t StatisticEntry::getStartTime() const{
    return m_startTime;
}

StatisticType::UsageType StatisticEntry::getUsageType() const {
    return m_usageType;
}

void StatisticEntry::setUsageType(StatisticType::UsageType type){
    m_usageType = type;
}

std::string StatisticEntry::getUsageTypeString() const{
    switch (m_usageType){
    case StatisticType::USAGE_USER:
        return "User";
    case StatisticType::USAGE_BACKGROUND:
        return "Background";
    default:
        throw cRuntimeError("Unhandled statistic type");
    }
}


} /* namespace eventsimulator */
