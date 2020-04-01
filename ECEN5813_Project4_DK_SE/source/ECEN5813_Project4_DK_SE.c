/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    ECEN5813_Project4_DK_SE.c
 * @brief   Application entry point.
 */
#include <statemachine_state.h>
#include <statemachine_state_spi.h>
#include <statemachine_table.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "global_defines.h"
#include "i2c_poll.h"
#include "i2c_interrupt.h"
#include "spi.h"
#include "sensor.h"
#include "timer.h"
#include "led_control.h"
#include "slider.h"
#include "logger.h"
/* TODO: insert other definitions and declarations here. */
// enum for state machine state achine
sm_num_t state_machine;
// end status for table-driven state machine
int end;

// i2c and sensor status for POST
bool i2c_status = false;
bool spi_poll_status = false;
bool sensor_poll_status = false;

// POST function to initialize and check I2C, SP, and sensor modules were initialized correctly
bool POST(void){
	LED_on(BLUE);// set blue LED
    i2c_status = I2C_init();	// init and check I2C state
#ifdef TESTING_MODE
	UCUNIT_TestcaseBegin("I2C Poll Initialization Test\r\n");
	UCUNIT_CheckIsEqual(1, i2c_status);
	UCUNIT_TestcaseEnd();
#endif
    if(!i2c_status){
    	// failed to init i2c
    	Log_string("POST FAILED: I2C\r\n", POST_FUNC, LOG_STATUS);
    	LED_flash(RED, 1); // blink red led once
    	return false;
    }
    sensor_poll_status = Sensor_poll_enable();	// init and check sensor state
#ifdef TESTING_MODE
	UCUNIT_TestcaseBegin("Sensor Poll Initialization Test\r\n");
	UCUNIT_CheckIsEqual(1, sensor_poll_status);
	UCUNIT_TestcaseEnd();
#endif
    if(!sensor_poll_status){
    	// failed to init sensor
    	Log_string("POST FAILED: SENSOR\r\n", POST_FUNC, LOG_STATUS);
    	LED_flash(RED, 2); // blink red led twice
    	return false;
    }
    spi_poll_status = SPI_init();	// init and check SPI state
#ifdef TESTING_MODE
	UCUNIT_TestcaseBegin("SPI Poll Initialization Test\r\n");
	UCUNIT_CheckIsEqual(1, spi_poll_status);
	UCUNIT_TestcaseEnd();
#endif
    if(!spi_poll_status){
    	// failed to init spi
    	Log_string("POST FAILED: SPI\r\n", POST_FUNC, LOG_STATUS);
    	LED_flash(RED, 3); // blink red led thrice
    	return false;
    }
    Log_string("POST SUCCESS\r\n\n", POST_FUNC, LOG_STATUS);
    LED_flash(GREEN, 1);	// blink green led once
    return true;
}

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    // enable logging
    Log_enable();
    // if TESTING_MODE toggled in global_defines.h then set log level to TEST and start unit tests
#ifdef TESTING_MODE
    Log_level(LOG_TEST);
    UCUNIT_Init();
#else
    Log_level(LOG_DEBUG);
#endif

    // enable peripherals
    LED_init();
    Slider_init();
    SysTick_init();
    SysTick_disable();

    // enable state machine initial states
    state_StateMachine_init();
    spi_StateMachine_init();

    // if POST fails then end program
    if(!POST()){
    	// exit
    	Log_string("POST FAILED\r\n", MAIN, LOG_STATUS);
    	return 1;
    }

#ifdef TESTING_MODE
    UCUNIT_TestcaseBegin("LED Flashing Tests\r\n");
    LED_off(ALL);
    UCUNIT_CheckIsEqual(1, LED_flash(GREEN, 1));
    UCUNIT_CheckIsEqual(2, LED_flash(BLUE, 2));
    UCUNIT_CheckIsEqual(3, LED_flash(RED, 3));
    UCUNIT_TestcaseEnd();
#endif
    static int8_t ret = 0;
    state_machine = STATE_MACHINE_STATE;	// init state machine state machine
    /* Enter an infinite loop, containing a state machine switching state machine */
    while(1) {
    	__asm volatile ("nop");
    	switch(state_machine){
    	// use state driven state machine
    	case STATE_MACHINE_STATE:
        	ret = state_event_handler();
        	// if ret = 1 then exit current state machine and end program
    		if(ret == 1){
    			Log_string("Exiting state machine state\r\n", MAIN, LOG_STATUS);
    			LED_off(ALL);
    			SysTick_disable();
            	return 0;
        	}
    		// if ret = 2 then exit current state machine and end program
    		if(ret == 2){
            	//start next state machine
    			Log_string("Start next state machine Table\r\n\n", MAIN, LOG_STATUS);
    			ret = 0;
    			end = 0;
    			LED_off(ALL);
    			state_machine = STATE_MACHINE_TABLE;
        	}
    		break;
		// use table state machine
    	case STATE_MACHINE_TABLE:
    		Log_string("Starting Table\r\n", MAIN, LOG_STATUS);

    		Tabledriven_StateMachine_init();//initialize the table driven state machine
    		if(end==1)//for the right slider touch
    		{
    			return 0;
    		}

    		if(end==2)//either left slider touch or timeout 6 times
    		{
    			Log_string("Start next state machine SPI\r\n\n", MAIN, LOG_STATUS);
    			state_machine = STATE_MACHINE_SPI;
    			spi_StateMachine_init();
    		}
    		break;
		// use spi state machine
    	case STATE_MACHINE_SPI:
    		ret = spi_event_handler();
        	// if ret = 1 then exit current state machine and end program
    		if(ret == 1){
    			Log_string("Exiting state machine SPI\r\n", MAIN, LOG_STATUS);
    			LED_off(ALL);
    			SysTick_disable();
            	return 0;
        	}
    		// if ret = 2 then exit current state machine and end program
    		if(ret == 2){
            	// start next state machine
#ifdef TESTING_MODE
    			// goes to STATE_MACHINE_NONE: end of program in TEST mode
    			Log_string("Start next state machine NONE\r\n", MAIN, LOG_STATUS);
    			ret = 0;
    			LED_off(ALL);
    			state_machine = STATE_MACHINE_NONE;
#endif
#ifndef TESTING_MODE
    			// goes to STATE_MACHINE_STATE: end of program in TEST mode
    			Log_string("Start next state machine STATE\r\n", MAIN, LOG_STATUS);
    			ret = 0;
    			state_StateMachine_init();
    			LED_off(ALL);
    			state_machine = STATE_MACHINE_STATE;
#endif
        	}
    		break;
    	case STATE_MACHINE_NONE:	// print summary and end program after testing
    		Log_string("End of the line\r\n", MAIN, LOG_STATUS);
#ifdef TESTING_MODE
    	    UCUNIT_WriteSummary();
    	    UCUNIT_Shutdown();
#endif
    		return 0; // end program
    	default:
    		break;
    	}
    }
    return 0;
}
