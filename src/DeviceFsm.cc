/*
 * DeviceFsm.cpp
 *
 *  Created on: Feb 3, 2020
 *      Author: jd
 */

#include "DeviceFsm.h"

namespace eventsimulator {

DeviceFsm::DeviceFsm() {
    // TODO Auto-generated constructor stub

}

DeviceFsm::~DeviceFsm() {
    // TODO Auto-generated destructor stub
}

DeviceStates DeviceFsm::getDeviceState(void) const {
    return deviceState;
}

DeviceStates DeviceFsm::getPreviousDeviceState(void) const {
    return previousState;
}


void DeviceFsm::initState() {
    initState(DEVICE_STATE_FREE, nullptr);
}

void DeviceFsm::initState(DeviceStates startState){
    initState(startState, nullptr);
}

void DeviceFsm::initState(DeviceStates initState, void *param) {
    EV_INFO << "Init FSM to state " << getDeviceStateName(initState) << std::endl;
    switch (initState) {
    case DEVICE_STATE_OCCUPIED_USER:
        onEnterUserActive(deviceState, initState, param);
        break;
    case DEVICE_STATE_OCCUPIED_BACKGROUND:
        onEnterBackgroundActive(deviceState, initState, param);
        break;
    case DEVICE_STATE_FREE:
        onEnterIdle(deviceState, initState, param);
        break;
    default:
        throw cRuntimeError("Undefined state!");

    }
    deviceState = initState;
}

bool DeviceFsm::getIsInTransition(){
    return isInTransition;
}

bool DeviceFsm::setNewState(DeviceStates newState, void *param) {
    isInTransition = true;

    EV_INFO << "State transition from " << getDeviceStateName(deviceState)
                   << " to " << getDeviceStateName(newState) << std::endl;

    bool valid = true;
    // Inform leaving states
    switch (deviceState) {
    case DEVICE_STATE_OCCUPIED_USER:
        valid = onExitUserActive(deviceState, newState, param);
        break;
    case DEVICE_STATE_OCCUPIED_BACKGROUND:
        valid = onExitBackgroundActive(deviceState, newState, param);
        break;
    case DEVICE_STATE_FREE:
        valid = onExitIdle(deviceState, newState, param);
        break;
    default:
        throw cRuntimeError(
                "Undefined state! Maybe FSM not properly initialized?");
    }

    if (valid) {
        // Inform new state
        switch (newState) {
        case DEVICE_STATE_OCCUPIED_USER:
            onEnterUserActive(deviceState, newState, param);
            break;
        case DEVICE_STATE_OCCUPIED_BACKGROUND:
            onEnterBackgroundActive(deviceState, newState, param);
            break;
        case DEVICE_STATE_FREE:
            onEnterIdle(deviceState, newState, param);
            break;
        default:
            throw cRuntimeError("Undefined state!");

        }
        inCurrentStateSince = simTime();
        previousState = deviceState;
        deviceState = newState;
        isInTransition = false;

        onTransitionDone(deviceState);
        EV_INFO << "NEW STATE IN FSM: " << getDeviceStateName(getDeviceState()) << std::endl;
        return true; // valid transition
    } else {
        EV_INFO << "Invalid transition" << std::endl;
        isInTransition = false;
        EV_INFO << "NEW STATE IN FSM: " << getDeviceStateName(getDeviceState()) << std::endl;
        return false;
    }
}

simtime_t DeviceFsm::getInCurrentStateSince() const{
    return inCurrentStateSince;
}

void DeviceFsm::onEnterBackgroundActive(DeviceStates oldState,
        DeviceStates newState, void *param) {
    EV_WARN << "onEnterBackgroundActive was not overwritten!" << std::endl;
}
bool DeviceFsm::onExitBackgroundActive(DeviceStates oldState,
        DeviceStates newState, void *param) {
    EV_WARN << "onExitBackgroundActive was not overwritten!" << std::endl;
    return false;
}
void DeviceFsm::onEnterUserActive(DeviceStates oldState,
        DeviceStates newState, void *param) {
    EV_WARN << "onEnterUserActive was not overwritten!" << std::endl;
}
bool DeviceFsm::onExitUserActive(DeviceStates oldState, DeviceStates newState, void *param) {
    EV_WARN << "onExitUserActive was not overwritten!" << std::endl;
    return false;
}
void DeviceFsm::onEnterIdle(DeviceStates oldState, DeviceStates newState, void *param) {
    EV_WARN << "onEnterIdle was not overwritten!" << std::endl;
}
bool DeviceFsm::onExitIdle(DeviceStates oldState, DeviceStates newState, void *param) {
    EV_WARN << "onExitIdle was not overwritten!" << std::endl;
    return false;
}

void DeviceFsm::onTransitionDone(DeviceStates deviceState) {
    EV_WARN << "onTransitionDone was not overwritten!" << std::endl;
}

} // Namespace
