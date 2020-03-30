/*
 * i2c_interrupt.h
 *
 *  Created on: Mar 29, 2020
 *      Author: sagar
 */

#ifndef I2C_INTERRUPT_H_
#define I2C_INTERRUPT_H_



#endif /* I2C_INTERRUPT_H_ */
#include <stdint.h>

void i2c_starti(void);
void i2c_readsetupi(uint8_t dev, uint8_t address,uint8_t isLastRead);
void enbaleinterrupt(void);
void i2c_Transmit(void);

void dispaly_values(void);
