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
SliderState_t slider_transition = 5;

static int8_t timeout_counter = 0;
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
 * check for number of entrances based on systick timer interval
 * returns transition for each condition
 */
int32_t Slider_poll(void){
	static uint16_t val = 0;
	timeout_counter++;
//	PRINTF("Counter: %d\r\n", timeout_counter);

	val = Slider_scan();
//	Log_string("Slider value ", SLIDER_POLL, LOG_DEBUG);
//	PRINTF("%d\r\n", val);
	// slider transitions
	if(val < LEFT_LOWER){	// left slider touch transition
		LED_off(ALL);
	} else if((val >= LEFT_LOWER) && (val <= LEFT_HIGHER)){
		LED_on(RED);
		slider_transition = STATE_LEFT;
		return slider_transition;
	} else if((val >= RIGHT_LOWER) && (val <= RIGHT_HIGHER)){	// right slider touch transition
		LED_on(BLUE);
		slider_transition = STATE_RIGHT;
		return slider_transition;
	} else{	// error
		LED_off(ALL);
		Log_string("ERROR: Slider value is out of bounds: ", SLIDER_POLL, LOG_DEBUG);
		PRINTF("%d\r\n", val);
		slider_transition = 5;
	}
	// timer transitions
	if(timeout_counter <= 5 || val == 64992){	// timeouts 1 to 5 transition (also include logic for initial slider poll value
		slider_transition = STATE_TIMEOUT_15;
	} else if(timeout_counter >= 6){	// timeout 6 transition and reset entrance counter
		timeout_counter = 0;
		slider_transition = STATE_TIMEOUT_6;
	} else{	// error
		LED_off(ALL);
		Log_string("ERROR: Timer value is out of bounds", SLIDER_POLL, LOG_DEBUG);
//		PRINTF("%d\r\n", val);
		slider_transition = 5;
		return slider_transition;
	}
	return slider_transition;
}

