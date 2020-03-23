/*
 * statemachine_3.h
 *
 *  Created on: Mar 19, 2020
 *      Author: Dhruva
 */

#ifndef STATEMACHINE_3_H_
#define STATEMACHINE_3_H_
#include <stdint.h>
#include <stdbool.h>

// enum for state state machine states
typedef enum{
	STATE_READ_XYZ_SPI,
	STATE_DISPLAY_SPI,
	STATE_SLIDER_POLL_SPI,
	STATE_SENSOR_DISCONNECT_SPI,
	STATE_EXIT_SPI,
	STATE_NEXT_SM_SPI,
	EVENT_NUM_STATES_SPI
} SensorStateSPI_t;

// struct for state state machine transitions
typedef struct{
	SensorStateSPI_t current_state;
	SensorStateSPI_t next_state;
} StateMachine3_t;

int8_t spi_event_handler(void);
void spi_resetStateMachine(void);
void spi_StateMachine_init(void);
void spi_transitionState(void);

#endif /* STATEMACHINE_3_H_ */
