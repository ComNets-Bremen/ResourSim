/*
 * DeviceStates.h
 *
 *  Created on: Jul 13, 2018
 *      Author: jd
 *
 * The device states and some handling functions.
 * These states represent whether the module is currently occupied by
 *
 * - the user
 * - the background task
 */

#ifndef DEVICESTATES_H_
#define DEVICESTATES_H_


enum DeviceStates {
    DEVICE_STATE_UNKNOWN = -1,
    DEVICE_STATE_OCCUPIED_USER ,
    DEVICE_STATE_OCCUPIED_BACKGROUND ,
    DEVICE_STATE_CANCELLED_BACKGROUND,
    DEVICE_STATE_FREE,
};

/**
 * Return a map of device states
 *
 * @return map of #DeviceStates
 */
static inline std::map<std::string, DeviceStates> getDeviceStatesTypeMap(){
    std::map<std::string, DeviceStates> BackgroundEventTypeMap = {
            {"DEVICE_STATE_UNKNOWN", DEVICE_STATE_UNKNOWN},
            {"DEVICE_STATE_OCCUPIED_USER", DEVICE_STATE_OCCUPIED_USER},
            {"DEVICE_STATE_OCCUPIED_BACKGROUND", DEVICE_STATE_OCCUPIED_BACKGROUND},
            {"DEVICE_STATE_CANCELLED_BACKGROUND", DEVICE_STATE_CANCELLED_BACKGROUND},
            {"DEVICE_STATE_FREE", DEVICE_STATE_FREE},
    };

    return BackgroundEventTypeMap;
}

/**
 * Return a string representing the device state name
 *
 * @param   #DeviceStates the device state
 * @return  string the device state as as string
 */
static inline std::string getDeviceStateName(DeviceStates state){
    for (auto &s : getDeviceStatesTypeMap()){
        if(s.second == state){
            return s.first;
        }
    }
    return "DEVICE_STATE_UNKNOWN";
}


#endif /* DEVICESTATES_H_ */
