/*
 * global_defines.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Dhruva
 */

#include "global_defines.h"
#include <stdio.h>
#include <stdint.h>

// function to delay for X ticks
void Delay(volatile uint32_t number){
	while(number != 0){
		__asm volatile("NOP");
		number--;
	}
}
