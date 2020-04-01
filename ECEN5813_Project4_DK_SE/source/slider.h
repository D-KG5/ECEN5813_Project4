/*
 * Touch.h
 *
 *  Created on: Feb 9, 2020
 *      Author: Dhruva
 */

#ifndef SLIDER_H_
#define SLIDER_H_
#include <stdint.h>

#define SCAN_OFFSET 544
#define SCAN_DATA TSI0->DATA & 0xFFFF

// limits for slider poll values
#define LEFT_LOWER 35
#define LEFT_HIGHER 680
#define RIGHT_LOWER 690
#define RIGHT_HIGHER 2700

// enum for wait/slider poll transitions
typedef enum{
	STATE_LEFT,
//	STATE_TIMEOUT_6,
//	STATE_TIMEOUT_15,
	STATE_RIGHT,
	SLIDER_TRANS_NUM_STATES
} SliderState_t;

void Slider_init(void);
uint16_t Slider_scan(void);
int32_t Slider_poll(void);

#endif /* SLIDER_H_ */
