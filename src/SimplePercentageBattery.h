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

#ifndef SIMPLEPERCENTAGEBATTERY_H_
#define SIMPLEPERCENTAGEBATTERY_H_

#include <omnetpp.h>
#include <algorithm>
#include "event_messages/EventMessages.h"
#include "background_messages/BackgroundMessages.h"
#include "warning_messages/BatteryCriticalWarningMessage_m.h"

using namespace omnetpp;

namespace eventsimulator {

class SimplePercentageBattery : public cSimpleModule {
public:
    SimplePercentageBattery();
    virtual ~SimplePercentageBattery();

    double getBatteryPercentage();
    void setBatteryPercentage(double percentage);

    bool checkBatteryPercentageValid();
    void checkBattery();

    void incrementalBatteryChange(double percentage);

    bool isInitialized();
    void initialize();

    void refreshDisplay() const;
    void finish();

protected:
    virtual void handleMessage(cMessage *msg);

private:
    /**
     * Splits a *char in the format "<int>key:<float>value, ..." and adds the values to a map<int, float>
     *
     * The int value represents the percentage as an integer (i.e. 50 for 50%), the float
     * represents the average change at this level as a float (i.e. 0.4 for 40% per hour)
     *
     * @param *str   : The *char
     * @param &m     : The map
     */
    void addToMap(const char *str, std::map<int, float> &m);

    /**
     * Get the closest float value for a certain int from a map<int, float>.
     *
     * The int value represents the percentage as an integer (i.e. 50 for 50%), the float
     * represents the average change at this level as a float (i.e. 0.4 for 40% per hour)
     *
     * @param value     : The value in percent, i.e. 0.5 for 50%
     * @param &m        : The map
     */
    float getClosestValue(float value, std::map<int, float> &m);

    std::map<int,float> chargePerHourArray;
    std::map<int,float> dischargePerHourArray;

    double batteryPercentage;
    double expectedBatteryPercentage;

    cOutVector batteryPercentageValues;
    cOutVector expectedBatteryPercentageValues;
    cOutVector batteryPercentageDelta;

    bool initialized;
    simtime_t lastBatteryEventTime;
};

} /* namespace eventsimulator */

#endif /* SIMPLEPERCENTAGEBATTERY_H_ */
