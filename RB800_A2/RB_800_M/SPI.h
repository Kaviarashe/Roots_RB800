/*
 * SPI.h
 *
 * Created: 7/1/2015 11:23:19 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef SPI_H_
#define SPI_H_

#include "RB_800.h"

void initialize_SPI(void);
void write_byte_SPI_SD_CAN(uint8_t byte);
uint8_t write_read_byte_SPI_SD_CAN(uint8_t _byte);
void write_bytes_SPI_SD_CAN(uint8_t bytes[], uint8_t num_bytes);
void read_bytes_SPI_SD_CAN(uint8_t bytes[], uint8_t num_bytes);

#define SD_CAN_SPI_BUS SPIC

#define SD_CAN_MOSI		5//PC5
#define SD_CAN_MISO		6//PC6
#define SD_CAN_SCK		7//PC7
#define SD_CAN_INT		3//PC3


#endif /* SPI_H_ */