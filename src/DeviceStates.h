/*
 * DeviceStates.h
 *
 *  Created on: Jul 13, 2018
 *      Author: jd
 */

#ifndef DEVICESTATES_H_
#define DEVICESTATES_H_


enum DeviceStates {
    DEVICE_STATE_UNKNOWN = -1,
    DEVICE_STATE_OCCUPIED_USER,
    DEVICE_STATE_OCCUPIED_BACKGROUND,
    DEVICE_STATE_FREE,
};

static inline std::map<std::string, DeviceStates> getDeviceStatesTypeMap(){
    std::map<std::string, DeviceStates> BackgroundEventTypeMap = {
            {"DEVICE_STATE_UNKNOWN", DEVICE_STATE_UNKNOWN},
            {"DEVICE_STATE_OCCUPIED_USER", DEVICE_STATE_OCCUPIED_USER},
            {"DEVICE_STATE_OCCUPIED_BACKGROUND", DEVICE_STATE_OCCUPIED_BACKGROUND},
            {"DEVICE_STATE_FREE", DEVICE_STATE_FREE},
    };

    return BackgroundEventTypeMap;
}

static inline std::string getDeviceStateName(DeviceStates state){
    for (auto &s : getDeviceStatesTypeMap()){
        if(s.second == state){
            return s.first;
        }
    }
    return "DEVICE_STATE_UNKNOWN";
}


#endif /* DEVICESTATES_H_ */
