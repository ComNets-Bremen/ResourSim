/*
 * EventTypeDefinitions.h
 *
 *  Created on: Mar 20, 2018
 *      Author: jd
 *
 * Type definitions and functions for the user events
 */

#ifndef EVENTTYPEDEFINITIONS_H_
#define EVENTTYPEDEFINITIONS_H_

enum EventType
{
    EVENT_TYPE_SCREEN,
    EVENT_TYPE_BATTERY,
    EVENT_TYPE_WIFI,
    EVENT_TYPE_TRAFFIC,
    EVENT_TYPE_CELLULAR,
    EVENT_TYPE_AIRPLANE_MODE,
    EVENT_TYPE_BLUETOOTH,
    EVENT_TYPE_UNKNOWN = -1,
};

/**
 * Static function to create a string out of the enum #EventType
 *
 * @param   #EventType    The type of the event
 * @return  The string representation of the #EventType
 */
static inline std::map<std::string, EventType> getEventTypeMap(){
    std::map<std::string, EventType> EventTypeMap = {
            {"EVENT_TYPE_SCREEN", EVENT_TYPE_SCREEN},
            {"EVENT_TYPE_BATTERY", EVENT_TYPE_BATTERY},
            {"EVENT_TYPE_WIFI", EVENT_TYPE_WIFI},
            {"EVENT_TYPE_TRAFFIC", EVENT_TYPE_TRAFFIC},
            {"EVENT_TYPE_CELLULAR", EVENT_TYPE_CELLULAR},
            {"EVENT_TYPE_AIRPLANE_MODE", EVENT_TYPE_AIRPLANE_MODE},
            {"EVENT_TYPE_BLUETOOTH", EVENT_TYPE_BLUETOOTH},
            {"EVENT_TYPE_UNKNOWN", EVENT_TYPE_UNKNOWN}
    };

    return EventTypeMap;
}


#endif /* EVENTTYPEDEFINITIONS_H_ */
