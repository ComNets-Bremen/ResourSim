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
 * Container class for handling background events
 *
 * It helps handling the parameters from the .ned and omnetpp.ini files for
 * setting up the different parameters.
 *
 * The assumed format in the configuration files is
 *
 * **.<type><index>_<name> = ...
 *
 * whereas <type> and <name> are string values and <index> is an integer
 * starting from 0
 */

#include "BackgroundEventContainer.h"

namespace eventsimulator {

/**
 * Constructor
 */
BackgroundEventContainer::BackgroundEventContainer(){
}

/**
 * Destructor
 */
BackgroundEventContainer::~BackgroundEventContainer() {
}

/**
 * Set the configuration parameter type
 *
 * @param *type The type of the parameter as a const char*
 */
void BackgroundEventContainer::set_ParameterType(const char *type){
    this->type = std::string(type);
}

/**
 * Set the configuration parameter type
 *
 * @param *type The type of the parameter as a string
 */
void BackgroundEventContainer::set_ParameterType(std::string type){
    this->type = type;
}

/**
 * Returns the configuration parameter type
 *
 * @return  The parameter type
 */
const char *BackgroundEventContainer::get_ParameterType(void){
    return type.c_str();
}

/**
 * Set the index of the configuration parameter
 *
 * @param   index   The index, starting from 0
 */
void BackgroundEventContainer::set_Index(int index){
    this->index = index;
}

/**
 * Get the index of the configuration parameter
 *
 * @return  int     the index
 */
const int BackgroundEventContainer::get_Index(void){
    return index;
}

/**
 * Get the next execution time. The value is updated by calling @updateNextTime
 *
 * @return  simtime_t the next timestamp this modules changes
 */
simtime_t BackgroundEventContainer::get_NextExecutionTime(){
    return nextExecutionTime;
}


/**
 * Update the next execution time. The timestamp can be accessed using the
 * method @get_NextExecutionTime()
 *
 * @param   *m  Pointer to a cSimpleModule instance (for accessing par())
 */
void BackgroundEventContainer::updateNextTime(cSimpleModule *m){
    if (nextExecutionTime == 0){
        nextExecutionTime = SimTime(get_NextStartTime(m).doubleValue(), SimTimeUnit::SIMTIME_S);
    } else {
        nextExecutionTime = simTime() + SimTime(get_NextRepetitionTime(m).doubleValue(), SimTimeUnit::SIMTIME_S);
    }
    // TODO Handle stop time?
}


/**
 * Get the start time from the configuration
 *
 * @param   *m      Pointer to a cSimpleModule instance
 * @return  cPar&   The parameter
 */
cPar& BackgroundEventContainer::get_NextStartTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "start").c_str());
}

/**
 * Get the stop time from the configuration
 *
 * @param   *m      Pointer to a cSimpleModule instance
 * @return  cPar&   The parameter
 */
cPar& BackgroundEventContainer::get_NextStopTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "stop").c_str());
}

/**
 * Get the repetition (inter-process time) time from the configuration
 *
 * @param   *m      Pointer to a cSimpleModule instance
 * @return  cPar&   The parameter
 */
cPar& BackgroundEventContainer::get_NextRepetitionTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "repetition").c_str());
}

/**
 * Get the duration of a task from the configuration
 *
 * @param   *m      Pointer to a cSimpleModule instance
 * @return  cPar&   The parameter
 */
cPar& BackgroundEventContainer::get_NextDurationTime(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "duration").c_str());
}


/**
 * Get the type of a service from the configuration
 *
 * @param   *m      Pointer to a cSimpleModule instance
 * @return  string  The name of the service
 */
std::string BackgroundEventContainer::get_Type(cSimpleModule *m){
    return m->par(getArrayName(index, type.c_str(), "type").c_str()).stdstringValue();
}



/**
 * Returns the type of the event as an enum of type #BackgroundEventType
 * from the configuration
 *
 * @param   *m  Pointer to a cSimpleModule instance
 * @return  #BackgroundEventType representing the service type
 */
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
