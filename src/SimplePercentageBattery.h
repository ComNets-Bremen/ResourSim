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

namespace eventsimulator {

class SimplePercentageBattery {
public:
    SimplePercentageBattery();
    virtual ~SimplePercentageBattery();

    double getBatteryPercentage();
    void setBatteryPercentage(double percentage);

    bool checkBatteryPercentageValid();

    void incrementalBatteryChange(double percentage);

    bool isInitialized();
    void initialize();

private:
    double batteryPercentage;
    bool initialized;
};

} /* namespace eventsimulator */

#endif /* SIMPLEPERCENTAGEBATTERY_H_ */
