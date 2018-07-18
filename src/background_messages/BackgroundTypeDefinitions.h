/*
 * BackgroundTypeDefinitions.h
 *
 *  Created on: Jul 12, 2018
 *      Author: jd
 *
 *  Type definitions for the background services / events
 */

#ifndef BACKGROUND_MESSAGES_BACKGROUNDTYPEDEFINITIONS_H_
#define BACKGROUND_MESSAGES_BACKGROUNDTYPEDEFINITIONS_H_

enum BackgroundEventType
{
    BACKGROUND_EVENT_TYPE_CPU,
    BACKGROUND_EVENT_TYPE_WIFI,
    BACKGROUND_EVENT_TYPE_CELLULAR,
    BACKGROUND_EVENT_TYPE_BLUETOOTH,
    BACKGROUND_EVENT_TYPE_UNKNOWN = -1,
};


/**
 * Static function to create a string out of the enum #BackgroundEventType
 *
 * @param   #BackgroundEventType    The type of the background event
 * @return  The string representation of the #BackgroundEventType
 */
static inline std::map<std::string, BackgroundEventType> getBackgroundEventTypeMap(){
    std::map<std::string, BackgroundEventType> BackgroundEventTypeMap = {
        {"BACKGROUND_EVENT_TYPE_CPU", BACKGROUND_EVENT_TYPE_CPU},
        {"BACKGROUND_EVENT_TYPE_WIFI", BACKGROUND_EVENT_TYPE_WIFI},
        {"BACKGROUND_EVENT_TYPE_CELLULAR",BACKGROUND_EVENT_TYPE_CELLULAR},
        {"BACKGROUND_EVENT_TYPE_BLUETOOTH",BACKGROUND_EVENT_TYPE_BLUETOOTH},
        {"BACKGROUND_EVENT_TYPE_UNKNOWN", BACKGROUND_EVENT_TYPE_UNKNOWN}
    };

    return BackgroundEventTypeMap;
}


#endif /* BACKGROUND_MESSAGES_BACKGROUNDTYPEDEFINITIONS_H_ */
