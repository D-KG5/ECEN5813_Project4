/*
 * i2c.h
 *
 *  Created on: Mar 18, 2020
 *      Author: Dhruva
 */

#ifndef I2C_POLL_H_
#define I2C_POLL_H_
#include <stdbool.h>
#include <stdint.h>

#define CS_LINE_POS (4)    // on port D
#define SDA_LINE_POS (25)	// on port E
#define SCL_LINE_POS (24)	// on port E

#define SLAVE_ADDRESS 0x3A

#define REG_XHI 0x01
#define REG_XLO 0x02
#define REG_YHI 0x03
#define REG_YLO 0x04
#define REG_ZHI	0x05
#define REG_ZLO 0x06

#define REG_WHOAMI 0x0D
#define XYZ_DATA_CFG 0x0E
#define REG_CTRL1  0x2A
#define REG_CTRL2  0x2B
#define REG_CTRL3  0x2C
#define REG_CTRL4  0x2D

#define WRITE	0x00
#define	READ	0x01

#define I2C_M_START 	I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP  	I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART 	I2C0->C1 |= I2C_C1_RSTA_MASK

#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK
#define I2C_REC				I2C0->C1 &= ~I2C_C1_TX_MASK

#define BUSY_ACK 	    while(I2C0->S & 0x01)
#define TRANS_COMP		while(!(I2C0->S & 0x80))
#define I2C_WAIT 			i2c_wait();

#define NACK 	        I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK

bool I2C_init(void);
void i2c_busy(void);
void i2c_wait(void);
void i2c_start(void);
void i2c_read_setup(uint8_t dev, uint8_t address);
uint8_t i2c_read_byte(uint8_t dev, uint8_t address);
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data);

#endif /* I2C_POLL_H_ */
