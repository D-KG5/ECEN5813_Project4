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

bool Sensor_enable(void);
bool Sensor_disable(void);
bool read_xyz(void);
void display_state(void);

#endif /* SENSOR_H_ */
