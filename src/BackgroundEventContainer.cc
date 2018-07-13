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

#include "BackgroundEventContainer.h"

namespace eventsimulator {

BackgroundEventContainer::BackgroundEventContainer(){
}


BackgroundEventContainer::~BackgroundEventContainer() {
    // TODO Auto-generated destructor stub
}

void BackgroundEventContainer::set_ParameterType(const char *type){
    this->type = std::string(type);
}

void BackgroundEventContainer::set_ParameterType(std::string type){
    this->type = type;
}

void BackgroundEventContainer::set_Index(int index){
    this->index = index;
}

const char *BackgroundEventContainer::get_ParameterType(void){
    return type.c_str();
}

const int BackgroundEventContainer::get_Index(void){
    return index;
}

simtime_t BackgroundEventContainer::get_NextExecutionTime(){
    return nextExecutionTime;
}

void BackgroundEventContainer::updateNextTime(cSimpleModule *m){
    if (nextExecutionTime == 0){
        nextExecutionTime = SimTime(get_NextStartTime(m).doubleValue(), SimTimeUnit::SIMTIME_S);
    } else {
        nextExecutionTime = simTime() + SimTime(get_NextRepetitionTime(m).doubleValue(), SimTimeUnit::SIMTIME_S);
    }
    // TODO Handle stop time?
}


cPar& BackgroundEventContainer::get_NextStartTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "start").c_str());
}

cPar& BackgroundEventContainer::get_NextStopTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "stop").c_str());
}


cPar& BackgroundEventContainer::get_NextRepetitionTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "repetition").c_str());
}

cPar& BackgroundEventContainer::get_NextDurationTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "duration").c_str());
}

std::string BackgroundEventContainer::get_Type(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "type").c_str()).stdstringValue();
}


BackgroundEventType BackgroundEventContainer::get_EnumType(cSimpleModule *m){
    std::string s = get_Type(m);

    for (auto const& v : getBackgroundEventTypeMap()){
        if (v.first.compare(s) == 0)
            return v.second;
    }

    return BACKGROUND_EVENT_TYPE_UNKNOWN;
}
/**
     * Return a string describing a parameter from a parameter array in the type
     * <type><index>_<name>
     *
     * @param   index   The index
     * @param   type    The base name of the parameter
     * @param   name    The name of the parameter
*/
std::string BackgroundEventContainer::getArrayName(int index, const char *type, const char *name){
    std::string baseString(type);
    baseString += std::to_string(index) + "_" + std::string(name);
    return baseString;
}

} /* namespace eventsimulator */
