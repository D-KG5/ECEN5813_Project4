/*
 * statemachine_1.h
 *
 *  Created on: Mar 19, 2020
 *      Author: Dhruva
 */

#ifndef STATEMACHINE_STATE_H_
#define STATEMACHINE_STATE_H_
#include <stdint.h>
#include <stdbool.h>

// enum for state state machine states
typedef enum{
	STATE_READ_XYZ_STATE,
	STATE_DISPLAY_STATE,
	STATE_SLIDER_POLL_STATE,
	STATE_SENSOR_DISCONNECT_STATE,
	STATE_EXIT_STATE,
	STATE_NEXT_SM_STATE,
	EVENT_NUM_STATES_STATE
} SensorState_t;

// struct for state state machine transitions
typedef struct{
	SensorState_t current_state;
	SensorState_t next_state;
} StateMachine1_t;

int8_t state_event_handler(void);
void state_resetStateMachine(void);
void state_StateMachine_init(void);
void state_transitionState(void);

#endif /* STATEMACHINE_STATE_H_ */
