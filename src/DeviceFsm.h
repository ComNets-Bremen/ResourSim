/*
 * DeviceFsm.h
 *
 *  Created on: Feb 3, 2020
 *      Author: jd
 */

#ifndef DEVICEFSM_H_
#define DEVICEFSM_H_

#include <omnetpp.h>
#include <queue>
#import "DeviceStates.h"

using namespace omnetpp;

namespace eventsimulator {

class DeviceFsm {
public:
    /**
     * Default constructor
     */
    DeviceFsm();

    /**
     * Default destructor
     */
    virtual ~DeviceFsm();

    /**
     * Return current device state
     *
     * @return DeviceStates
     */
    DeviceStates getDeviceState(void) const;

    /**
     * Returns the time the node is in the current state
     */
    simtime_t getInCurrentStateSince() const;

    /**
     * Initialize the FSM
     *
     * @par initState The init state
     * @par param   Pointer to params
     */
    void initState(DeviceStates initState, void *param);

    /**
     * Initialize the FSM with a given state
     */
    void initState(DeviceStates initState);

    /**
     * Initialize the FSM with the default state "FREE"
     */
    void initState();

    /**
     * Trigger a device transition
     */
    bool setNewState(DeviceStates newState, void *param);

    /**
     * Called if the Background active period is entered.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     */
    virtual void onEnterBackgroundActive(DeviceStates oldState,
            DeviceStates newState, void *param);

    /**
     * Called if the Background active period is exited.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     *
     * @return true if transition was successful, false otherwise
     */
    virtual bool onExitBackgroundActive(DeviceStates oldState,
            DeviceStates newState, void *param);

    /**
     * Called if the User active period is exited.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     *
     * @return true if transition was successful, false otherwise
     */
    virtual void onEnterUserActive(DeviceStates oldState, DeviceStates newState,
            void *param);

    /**
     * Called if the User active period is exited.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     *
     * @return true if transition was successful, false otherwise
     */
    virtual bool onExitUserActive(DeviceStates oldState, DeviceStates newState,
            void *param);

    /**
     * Called if the Idle period is exited.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     *
     * @return true if transition was successful, false otherwise
     */
    virtual void onEnterIdle(DeviceStates oldState, DeviceStates newState,
            void *param);

    /**
     * Called if the idle period is exited.
     * Should be overwritten
     *
     * @par oldState    the old state
     * @par newState    the new state
     *
     * @return true if transition was successful, false otherwise
     */
    virtual bool onExitIdle(DeviceStates oldState, DeviceStates newState,
            void *param);

    /**
     * Called after the state transition
     *
     * @par deviceState The new device state
     */
    virtual void onTransitionDone(DeviceStates deviceState);


    /**
     * Is the FSM in transition?
     *
     * @return true, if in transition, otherwise false
     */
    bool getIsInTransition();

private:
    DeviceStates deviceState = DEVICE_STATE_UNKNOWN;
    simtime_t inCurrentStateSince = 0;
    bool isInTransition = false;
};

}

#endif /* DEVICEFSM_H_ */
