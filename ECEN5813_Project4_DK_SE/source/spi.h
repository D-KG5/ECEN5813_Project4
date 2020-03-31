/*
 * spi.h
 *
 *  Created on: Mar 20, 2020
 *      Author: Dhruva
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include <stdint.h>

#define MISO_LINE_POS (1)	// on port E
#define SCK_LINE_POS (2)	// on port E
#define MOSI_LINE_POS (3)	// on port E
#define PCS_LINE_POS (4)    // on port E

bool SPI_init(void);
uint8_t SPI_send(uint8_t d_out);
bool SPI_Loopback(void);
void display_SPI(int8_t counter);

#endif /* SPI_H_ */
