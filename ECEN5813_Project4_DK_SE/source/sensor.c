/*
 * sensor.c
 *
 *  Created on: Mar 18, 2020
 *      Author: Dhruva
 */

#include "i2c_poll.h"
#include "sensor.h"
#include "led_control.h"
#include "logger.h"
#include "global_defines.h"
#include <stdio.h>

// values for XYZ, min, max, avg values
static volatile int16_t acc_X = 0, acc_Y = 0, acc_Z = 0;
static volatile int16_t acc_Xh = 0, acc_Yh = 0, acc_Zh = 0;
static volatile int16_t acc_Xl = 0, acc_Yl = 0, acc_Zl = 0;
static volatile int16_t acc_Xa = 0, acc_Ya = 0, acc_Za = 0;
static int32_t avg_counter = 0;

// get i2c and sensor status
extern bool i2c_status;
extern bool sensor_status;

// initializes/enables mma8451 sensor. I2C has to already be enabled
bool Sensor_enable(void){
	if(i2c_status){
		i2c_write_byte(SLAVE_ADDRESS, REG_CTRL2, 0x40);		// reset device
		Delay(100);
		i2c_write_byte(SLAVE_ADDRESS, XYZ_DATA_CFG, 0x00);	// +/- 2g range
		i2c_write_byte(SLAVE_ADDRESS, REG_CTRL2, 0x02);		// high resolution mode
		i2c_write_byte(SLAVE_ADDRESS, REG_CTRL1, 0x3D);		// lowest ODR, reduced noise, high res active mode
		Log_string("Sensor enabled\r\n", LED_INIT, LOG_DEBUG);
		return true;
	} else{
		Log_string("ERROR: Failed to enable sensor: I2C not enabled\r\n", SENSOR_ENABLE, LOG_DEBUG);
		return false;
	}
}

// disables mma8451 sensor. I2C and sensor has to be already enabled
bool Sensor_disable(void){
	if(i2c_status && sensor_status){
		i2c_write_byte(SLAVE_ADDRESS, REG_CTRL1, 0x00);		// set to standby
		Log_string("Sensor disabled\r\n", SENSOR_DISABLE, LOG_DEBUG);
		// zero all values and set sensor_status to false
		avg_counter = 0;
		acc_X = 0, acc_Y = 0, acc_Z = 0;
		acc_Xh = 0, acc_Yh = 0, acc_Zh = 0;
		acc_Xl = 0, acc_Yl = 0, acc_Zl = 0;
		acc_Xa = 0, acc_Ya = 0, acc_Za = 0;
		sensor_status = false;
		LED_on(RED);
		return false;
	} else{
		Log_string("ERROR: Failed to disable sensor: I2C not enabled or sensor is already disabled\r\n", SENSOR_DISABLE, LOG_DEBUG);
		return sensor_status;
	}
}

// read xyz values from sensor
bool read_xyz(void){
	/* sign extend byte to 16 bits - need to cast to signed
	 * since function returns uint8_t which is unsigned
	 */
	// only get values if sensor is enabled
	if(sensor_status){
		acc_X = (int8_t) i2c_read_byte(SLAVE_ADDRESS, REG_XHI);
		Delay(100);
		acc_Y = (int8_t) i2c_read_byte(SLAVE_ADDRESS, REG_YHI);
		Delay(100);
		acc_Z = (int8_t) i2c_read_byte(SLAVE_ADDRESS, REG_ZHI);
		return true;
	} else{
		Log_string("Sensor disabled/disconnected: can't get new values\r\n", READ_XYZ, LOG_DEBUG);
		acc_X = 0;
		acc_Y = 0;
		acc_Z = 0;
		return false;
	}
}

// displays current XYZ values from sensor
void display_state(void){
	// get max and min values for XYZ values
	if(acc_X > acc_Xh){
		acc_Xh = acc_X;
	}
	if(acc_X < acc_Xl){
		acc_Xl = acc_X;
	}
	if(acc_Y > acc_Yh){
		acc_Yh = acc_Y;
	}
	if(acc_Y < acc_Yl){
		acc_Yl = acc_Y;
	}
	if(acc_Z > acc_Zh){
		acc_Xh = acc_X;
	}
	if(acc_Z < acc_Zl){
		acc_Zl = acc_Z;
	}
	// get average values for XYZ values
	avg_counter++;
	acc_Xa += acc_X;
	acc_Xa /= avg_counter;
	acc_Ya += acc_Y;
	acc_Ya /= avg_counter;
	acc_Za += acc_Z;
	acc_Za /= avg_counter;
	// display XYZ values
	PRINTF("X: %3d\tMax: %3d\tMin: %3d\tAvg: %3d\r\n", acc_X, acc_Xh, acc_Xl, acc_Xa);
	PRINTF("Y: %3d\tMax: %3d\tMin: %3d\tAvg: %3d\r\n", acc_Y, acc_Yh, acc_Yl, acc_Ya);
	PRINTF("Z: %3d\tMax: %3d\tMin: %3d\tAvg: %3d\r\n\r\n", acc_Z, acc_Zh, acc_Zl, acc_Za);
}


