/*
 * sensor.h
 *
 *  Created on: Mar 18, 2020
 *      Author: Dhruva
 */

#ifndef SENSOR_H_
#define SENSOR_H_
#include <stdbool.h>
#include <stdint.h>

bool Sensor_poll_enable(void);
bool Sensor_poll_disable(void);
bool read_xyz_poll(void);
void display_state_poll(void);

#endif /* SENSOR_H_ */
