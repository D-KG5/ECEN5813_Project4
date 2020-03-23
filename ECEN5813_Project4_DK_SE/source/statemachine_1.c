/*
 * statemachine_1.c
 *
 *  Created on: Mar 19, 2020
 *      Author: Dhruva
 */

#include <i2c_poll.h>
#include "statemachine_1.h"
#include "logger.h"
#include "sensor.h"
#include "led_control.h"
#include "slider.h"
#include "global_defines.h"
#include "timer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// state machine state enum and accompanying strings
StateMachine1_t sensor_sm1;
//static const char * sensor_states[] = {"READ_XYZ", "DISPLAY", "SLIDER_POLL", "SENSOR_DISCONNECT", "STATE_EXIT", "STATE_NEXT_SM", "EVENT_NUM_STATES"};

static bool read_state = false;

static int32_t slider_poll_ret = 5;
extern bool timeout;
static bool timer_enabled = false;

// event handler for state driven state machine
int8_t state_event_handler(void){
	switch(sensor_sm1.current_state){
	case STATE_READ_XYZ_STATE:	// in Read XYZ_Acc
		Log_string("STATE_READ_XYZ_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		read_state = read_xyz();
		// decide next state if able to read xyz values state
		if(read_state){
			sensor_sm1.next_state = STATE_DISPLAY_STATE;
		} else{
			sensor_sm1.next_state = STATE_SENSOR_DISCONNECT_STATE;
		}
		break;
	case STATE_DISPLAY_STATE:	// in Process/Display state
		Log_string("STATE_DISPLAY_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		display_state();
		sensor_sm1.next_state = STATE_SLIDER_POLL_STATE;
		break;
	case STATE_SLIDER_POLL_STATE:	// in Wait/Poll Slider state
		Log_string("STATE_SLIDER_POLL_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		// start timer only once in slider poll state
		if(!timer_enabled){
			SysTick_enable();
			timer_enabled = true;
		}
		// only enter slider_poll function on systick timer timeout (every 3 seconds)
		if(timeout){
			slider_poll_ret = Slider_poll();
			timeout = false;
			// decide next state on slider poll transition event
        	switch(slider_poll_ret){
        	case STATE_LEFT:
        		Log_string("Left transition. Go to other state machine\r\n", STATE_EVENT_HANDLER, LOG_STATUS);
        		sensor_sm1.next_state = STATE_NEXT_SM_STATE;
        		break;
        	case STATE_TIMEOUT_6:
        		Log_string("Timeout 6. Go to other state machine\r\n", STATE_EVENT_HANDLER, LOG_STATUS);
        		sensor_sm1.next_state = STATE_NEXT_SM_STATE;
        		break;
        	case STATE_TIMEOUT_15:
        		Log_string("Timeout 1 to 5. Reading xyz\r\n", STATE_EVENT_HANDLER, LOG_STATUS);
        		sensor_sm1.next_state = STATE_READ_XYZ_STATE;
        		break;
        	case STATE_RIGHT:
        		Log_string("Right transition. End program\r\n", STATE_EVENT_HANDLER, LOG_STATUS);
        		sensor_sm1.next_state = STATE_EXIT_STATE;
        		break;
        	default:
        		Log_string("ERROR: Invalid state\r\n", STATE_EVENT_HANDLER, LOG_STATUS);
        		break;
        	}
		}
		break;
	case STATE_SENSOR_DISCONNECT_STATE:
		Log_string("STATE_SENSOR_DISCONNECT_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		// go to exit
		sensor_sm1.next_state = STATE_EXIT_STATE;
		break;
	case STATE_EXIT_STATE:
		Log_string("STATE_EXIT_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		// exit program
		return 1;
	case STATE_NEXT_SM_STATE:
		Log_string("STATE_NEXT_SM_STATE\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		// go to next state machine and reset timer enable status
		timer_enabled = false;
		return 2;
	case EVENT_NUM_STATES_STATE:
	default:
		// error
		Log_string("ERROR: Invalid state in state state machine\r\n", STATE_EVENT_HANDLER, LOG_TEST);
		break;
	}
	state_transitionState(); //transition state machine
	return 0;
}

// sets and resets state driven state machine
void state_resetStateMachine(void){
	/* initialize state machine states */
	sensor_sm1.current_state = STATE_READ_XYZ_STATE;
	sensor_sm1.next_state = STATE_DISPLAY_STATE;
}

// initializes state driven state machine
void state_StateMachine_init(void){
	state_resetStateMachine();
}

// transitions state driven state machine states
void state_transitionState(void){
	if(sensor_sm1.current_state != sensor_sm1.next_state){
		sensor_sm1.current_state = sensor_sm1.next_state;
	}
//	PRINTF("Current state is %s\r\n", sensor_states[sensor_sm1.current_state]);
//	PRINTF("Next state is %s\r\n", sensor_states[sensor_sm1.next_state]);
}

