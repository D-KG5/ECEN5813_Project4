/*
 * Touch.c
 *
 *  Created on: Feb 9, 2020
 *      Author: Dhruva
 */
#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#include "slider.h"
#include "led_control.h"
#include "logger.h"
#include "timer.h"
#include "global_defines.h"

// global variable for Slider_poll function set to invalid state transition value
SliderState_t slider_transition = 3;

extern bool timeout;

/**
 * initialize slider
 * inspired by https://www.digikey.com/eewiki/display/microcontroller/Using+the+Capacitive+Touch+Sensor+on+the+FRDM-KL46Z
 */
void Slider_init(void){
	// Enable clock for TSI PortB 16 and 17
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;

	TSI0->GENCS = TSI_GENCS_OUTRGF_MASK |  // Out of range flag, set to 1 to clear
		TSI_GENCS_MODE(0U) |  // Set at 0 for capacitive sensing.  Other settings are 4 and 8 for threshold detection, and 12 for noise detection
		TSI_GENCS_REFCHRG(0U) | // 0-7 for Reference charge
		TSI_GENCS_DVOLT(0U) | // 0-3 sets the Voltage range
		TSI_GENCS_EXTCHRG(0U) | //0-7 for External charge
		TSI_GENCS_PS(0U) | // 0-7 for electrode prescaler
		TSI_GENCS_NSCN(31U) | // 0-31 + 1 for number of scans per electrode
		TSI_GENCS_TSIEN_MASK | // TSI enable bit
		TSI_GENCS_STPE_MASK | // Enables TSI in low power mode
		TSI_GENCS_EOSF_MASK ; // End of scan flag, set to 1 to clear
	Log_string("Slider Initialized\r\n", SLIDER_INIT, LOG_DEBUG);
}

/**
 * start slider scan
 * inspired by https://www.digikey.com/eewiki/display/microcontroller/Using+the+Capacitive+Touch+Sensor+on+the+FRDM-KL46Z
 */
uint16_t Slider_scan(void){
	int scan;
	TSI0->DATA = TSI_DATA_TSICH(9U); // Using channel 9 of the TSI
	TSI0->DATA |= TSI_DATA_SWTS_MASK; // Software trigger for scan
	scan = SCAN_DATA;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; // Reset end of scan flag
	return scan - SCAN_OFFSET;
}

/* set colour of LED based on value of slider left = red, right = blue
 * returns transition for each condition
 */
int32_t Slider_poll(void){
	static uint16_t val = 0;
	slider_transition = 3; // reset state transition value on each entry so that next state machine doesn't use stale state
	for(int i = 0; i < 10; i++){	// get multiple readings to smooth out anomalies
		val = Slider_scan();
		Delay(50000);
	}
//	Log_string("Slider value ", SLIDER_POLL, LOG_DEBUG);
//	PRINTF("%d\r\n", val);
	Delay(10000);	// wait a bit to allow for states to settle
	// slider transitions
	if(val < LEFT_LOWER){	// left slider touch transition
		LED_off(ALL);
	} else if((val > LEFT_LOWER) && (val <= LEFT_HIGHER)){
#ifdef TESTING_MODE
		UCUNIT_TestcaseBegin("Slider Scan Left Range Test\r\n");
		UCUNIT_CheckIsInRange(val, LEFT_LOWER, LEFT_HIGHER);
		UCUNIT_TestcaseEnd();
#endif
		LED_on(RED);
		Log_string("Slider valueL ", SLIDER_POLL, LOG_TEST);
		Log_integer(val, EMPTY_NAME, LOG_TEST);
		slider_transition = STATE_LEFT;
		return slider_transition;
	} else if((val >= RIGHT_LOWER) && (val <= RIGHT_HIGHER)){	// right slider touch transition
#ifdef TESTING_MODE
		UCUNIT_TestcaseBegin("Slider Scan Right Range Test\r\n");
		UCUNIT_CheckIsInRange(val, RIGHT_LOWER, RIGHT_HIGHER);
		UCUNIT_TestcaseEnd();
#endif
		LED_on(BLUE);
		Log_string("Slider valueR ", SLIDER_POLL, LOG_TEST);
		Log_integer(val, EMPTY_NAME, LOG_TEST);
		slider_transition = STATE_RIGHT;
		return slider_transition;
	} else{	// error
		LED_off(ALL);
		Log_string("ERROR: Slider value is out of bounds: ", SLIDER_POLL, LOG_DEBUG);
		Log_integer(val, EMPTY_NAME, LOG_TEST);
		slider_transition = 3;
	}

	return slider_transition;
}

