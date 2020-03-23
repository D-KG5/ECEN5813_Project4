/*
 * i2c.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Dhruva
 */

#include "i2c_poll.h"
#include "global_defines.h"
#include "logger.h"
#include <stdio.h>

// from dean book
int32_t lock_detect = 0;
bool i2c_lock = false;

bool I2C_init(void){
	// Enable clock to ports E and D
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;

	PORTE->PCR[SCL_LINE_POS] |= PORT_PCR_MUX(5);
	PORTE->PCR[SDA_LINE_POS] |= PORT_PCR_MUX(5);

	I2C0->F = (I2C_F_ICR(0x10) | I2C_F_MULT(0));

	I2C0->C1 |= I2C_C1_IICEN_MASK;

	I2C0->C2 |= (I2C_C2_HDRS_MASK);

	Log_string("I2C Initialized\r\n", I2C_INIT, LOG_DEBUG);
	return true;
}

void i2c_busy(void){
	// Start Signal
	lock_detect=0;
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C_TRAN;
	I2C_M_START;
	I2C0->C1 |= I2C_C1_IICEN_MASK;
	// Write to clear line
	I2C0->C1 |= I2C_C1_MST_MASK; // set MASTER mode
	I2C0->C1 |= I2C_C1_TX_MASK; // Set transmit (TX) mode
	I2C0->D = 0xFF;
	while ((I2C0->S & I2C_S_IICIF_MASK) == 0U) {
	} // wait interrupt
	I2C0->S |= I2C_S_IICIF_MASK; // clear interrupt bit

	// Clear arbitration error flag
	I2C0->S |= I2C_S_ARBL_MASK;

	// Send start
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_TX_MASK; // Set transmit (TX) mode
	I2C0->C1 |= I2C_C1_MST_MASK; // START signal generated

	I2C0->C1 |= I2C_C1_IICEN_MASK;
	// Wait until start is send

	// Send stop
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_MST_MASK;
	I2C0->C1 &= ~I2C_C1_MST_MASK; 	// set SLAVE mode
	I2C0->C1 &= ~I2C_C1_TX_MASK; 	// Set Rx
	I2C0->C1 |= I2C_C1_IICEN_MASK;
	// wait
	// Clear arbitration error & interrupt flag
	I2C0->S |= I2C_S_IICIF_MASK;
	I2C0->S |= I2C_S_ARBL_MASK;
	lock_detect = 0;
	i2c_lock = true;
}

void i2c_wait(void){
	lock_detect = 0;
	while(((I2C0->S & I2C_S_IICIF_MASK) == 0) & (lock_detect < 200)) {
		lock_detect++;
	}
	if (lock_detect >= 200)
		i2c_busy();
	I2C0->S |= I2C_S_IICIF_MASK;
}

//send start sequence
void i2c_start(void){
	I2C_TRAN;				// set to transmit mode
	I2C_M_START;			// send start
}

//send device and register addresses
void i2c_read_setup(uint8_t dev, uint8_t address){
	I2C0->D = dev;			// send dev address
	I2C_WAIT				// wait for completion

	I2C0->D =  address;		// send read address
	I2C_WAIT				// wait for completion

	I2C_M_RSTART;			// repeated start
	I2C0->D = (dev | READ);	// send dev address (read)
	I2C_WAIT				// wait for completion

	I2C_REC;				// set to receive mode

}

// funcs for reading and writing a single byte using 7bit addressing reads a byte from dev:address
uint8_t i2c_read_byte(uint8_t dev, uint8_t address){
	uint8_t data;

	I2C_TRAN;					// set to transmit mode
	I2C_M_START;				// send start
	I2C0->D = dev;				// send dev address
	I2C_WAIT					// wait for completion

	I2C0->D =  address;			// send read address
	I2C_WAIT					// wait for completion

	I2C_M_RSTART;				// repeated start
	I2C0->D = (dev | READ);		// send dev address (read)
	I2C_WAIT					// wait for completion

	I2C_REC;					// set to recieve mode
	NACK;						// set NACK after read

	data = I2C0->D;				// dummy read
	I2C_WAIT					// wait for completion

	I2C_M_STOP;					// send stop
	data = I2C0->D;				// read data

	return data;
}

// using 7bit addressing writes a byte data to dev:address
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data){
	I2C_TRAN;					// set to transmit mode
	I2C_M_START;				// send start
	I2C0->D = dev;				// send dev address
	I2C_WAIT					// wait for ack

	I2C0->D =  address;			// send write address
	I2C_WAIT

	I2C0->D = data;				// send data
	I2C_WAIT
	I2C_M_STOP;
}

