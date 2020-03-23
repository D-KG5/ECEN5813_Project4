/*
 * spi.c
 *
 *  Created on: Mar 20, 2020
 *      Author: Dhruva
 */


#include "spi.h"
#include "global_defines.h"
#include "led_control.h"
#include "logger.h"
#include <stdio.h>
#include <stdbool.h>

uint8_t out='A', in;

bool SPI_init(void){
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;// enable clock to SPI1
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SPI1->C1 &= ~SPI_C1_SPE_MASK;// disable SPI1 to allow configuration

	PORTE->PCR[SCK_LINE_POS] &= ~PORT_PCR_MUX_MASK;   // set SPI1_SCK -- ALT2
	PORTE->PCR[SCK_LINE_POS] |= PORT_PCR_MUX(2);
	PORTE->PCR[MOSI_LINE_POS] &= ~PORT_PCR_MUX_MASK;   // set SPI1_MOSI -- ALT5
	PORTE->PCR[MOSI_LINE_POS] |= PORT_PCR_MUX(5);
	PORTE->PCR[MISO_LINE_POS] &= ~PORT_PCR_MUX_MASK;   // set SPI1_MISO -- ALT5
	PORTE->PCR[MISO_LINE_POS] |= PORT_PCR_MUX(5);
	PORTE->PCR[PCS_LINE_POS] &= ~PORT_PCR_MUX_MASK;   // set SPI1_PCS0 -- ALT2
	PORTE->PCR[PCS_LINE_POS] |= PORT_PCR_MUX(2);

	SPI1->C1 = SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;  // Select master mode, enable SS output
	SPI1->C2 = SPI_C2_MODFEN_MASK;
	SPI1->C1 &= ~SPI_C1_CPHA_MASK;	// Select active high clock, first edge sample
	SPI1->C1 &= ~SPI_C1_CPOL_MASK;

	SPI1->BR = SPI_BR_SPPR(1) | SPI_BR_SPR(3);  //   BaudRate = BusClock / ((SPPR+1)*2^(SPR+1))

	SPI1->C1 |= SPI_C1_SPE_MASK;// enable SPI1
	Log_string("SPI Initialized\r\n", SPI_INIT, LOG_DEBUG);
	return true;
}

uint8_t SPI_send(uint8_t d_out){
	while(!(SPI1->S & SPI_S_SPTEF_MASK))
		;  //Wait for transmit buffer empty, put data on data buffer
	SPI1->D = d_out;
	while (!(SPI1->S & SPI_S_SPRF_MASK))
		; // wait for receive buffer full, read from data buffer
	return SPI1->D;
}

// send ascii character and recieve it back
bool SPI_Loopback(void){
	static bool connected = true;
	connected = true;
	in = SPI_send(out);
	if(in != out){
		LED_on(RED); // Red: error, data doesn't match
		connected = false;
		return connected;
	}else{
		LED_on(GREEN); // Green: data matches
	}
	out++;
	if(out > 'z'){
		out = 'A';
	}
	return connected;
}

void display_SPI(void){
	PRINTF("In: %c\t Out: %c\r\n", in, out);
}
