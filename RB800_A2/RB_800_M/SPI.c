/*
 * SPI.c
 *
 * Created: 7/1/2015 11:23:10 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "SPI.h"

 //Initializes the SPI bus
void initialize_SPI(void)
{
	//These pins must be setup before enabling SPI
	CAN_CHIP_SELECT_HIGH;
	SD_CARD_CHIP_SELECT_HIGH;
	PORTC_DIRSET = (1 << SD_CAN_MOSI);
	PORTC_DIRSET = (1 << SD_CAN_SCK);

	//SPI Setup - Load
	//CLK2X - 1 - Clock speed x2
	//ENABLE - 1 - SPI enabled
	//DORD - 0 - MSB first transmission
	//MASTER - 1 - Master mode on
	//MODE - 00 - Mode 0 transmission
	//PRESCALAR - 10 - Clock speed /64 Combined with clock speed x2 = /32
	SD_CAN_SPI_BUS.CTRL = 0b11010010;
}

//Writes a single byte to SPI bus and waits for the bus to finish processing
void write_byte_SPI_SD_CAN(uint8_t byte)
{
	SD_CAN_SPI_BUS.DATA = byte;
	uint16_t counter = 0;
	while (!(SD_CAN_SPI_BUS.STATUS & 0x80))
	{
#ifndef __SIM__
		counter++;
		//In case SPI is stuck, counter to break out of loop
		if (counter > 65000)
		{
			break;
		}
#else 
		break;
#endif
	}
}

//Reads a single byte from the SPI bus
uint8_t write_read_byte_SPI_SD_CAN(uint8_t _byte)
{
	write_byte_SPI_SD_CAN(_byte);
	uint8_t byte = SD_CAN_SPI_BUS.DATA;
	return byte;
}

//Writes the number of specified byte from the array to the SPI bus
void write_bytes_SPI_SD_CAN(uint8_t bytes[], uint8_t num_bytes)
{
	for (int i = 0; i < num_bytes; i++)
	{
		write_byte_SPI_SD_CAN(bytes[i]);
	}
}

//Reads the number of specified bytes to the array from the SPI bus
void read_bytes_SPI_SD_CAN(uint8_t bytes[], uint8_t num_bytes)
{
	for (int i = 0; i < num_bytes; i++)
	{
		bytes[i] = write_read_byte_SPI_SD_CAN(0x00);
	}
}