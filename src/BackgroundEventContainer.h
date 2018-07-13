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

#ifndef BACKGROUNDEVENTCONTAINER_H_
#define BACKGROUNDEVENTCONTAINER_H_

#include <omnetpp.h>

#include "background_messages/BackgroundTypeDefinitions.h"

using namespace omnetpp;

namespace eventsimulator {

class BackgroundEventContainer {
public:
    BackgroundEventContainer();
    virtual ~BackgroundEventContainer();

    void set_ParameterType(const char *type);
    void set_ParameterType(std::string type);
    void set_Index(int index);

    const char *get_ParameterType(void);
    const int get_Index(void);

    simtime_t get_NextExecutionTime();
    void updateNextTime(cSimpleModule *m);

    cPar& get_NextStartTime(cSimpleModule *m);

    cPar& get_NextStopTime(cSimpleModule *m);

    cPar& get_NextRepetitionTime(cSimpleModule *m);

    cPar& get_NextDurationTime(cSimpleModule *m);

    std::string get_Type(cSimpleModule *m);
    BackgroundEventType get_EnumType(cSimpleModule *m);

    static std::string getArrayName(int index, const char *type, const char *name);

private:
    int index;
    std::string type;
    simtime_t nextExecutionTime = 0;
};

} /* namespace eventsimulator */

#endif /* BACKGROUNDEVENTCONTAINER_H_ */
