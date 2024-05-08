/*
 * SD_card.c
 *
 * Created: 4/18/2016 11:32:44 AM
 *  Author: Anurag Vaddi Reddy
 */
#include "RB_800.h" 

#ifdef __SIM__
#include <emu_SDCard.h>
#endif // __SIM__
void initialize_SD_card(void)
{
	sd_card_passed = FALSE;
	PORTD_PIN4CTRL |= PORT_OPC_PULLUP_gc;

	uint8_t counter = 0;

	//Write at least 74 clocks (each bit is a clock)
	for (uint8_t i = 0; i < 10; i++)
	{
		write_byte_SPI_SD_CAN(0xFF);
	}

	SD_CARD_CHIP_SELECT_LOW;

	response = 0xFF;

	for (uint8_t i = 0; i < 5; i++)
	{
		//Send CMD0 with CRC
		write_byte_SPI_SD_CAN(0x40);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x95);

		//Read R1 response
		do
		{
			response = write_read_byte_SPI_SD_CAN(0xFF);
		} while (response != 0x01 && counter++ < 8);
		if (response == 0x01)
			break;
	}

	if (response != 0x01)
	{
		SD_CARD_CHIP_SELECT_HIGH;
		return;
	}

	for (uint8_t i = 0; i < 5; i++)
	{
		//Send CMD8 with CRC
		write_byte_SPI_SD_CAN(0x48);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x01);
		write_byte_SPI_SD_CAN(0xAA);
		write_byte_SPI_SD_CAN(0x87);

		//Read R7 response
		counter = 0;
		do
		{
			response = write_read_byte_SPI_SD_CAN(0xFF);
		} while (response != 0x01 && counter++ < 8);
		if (response == 0x01)
		{
			if (write_read_byte_SPI_SD_CAN(0xFF) != 0x00)
			{
				SD_CARD_CHIP_SELECT_HIGH;
				return;
			}
			if (write_read_byte_SPI_SD_CAN(0xFF) != 0x00)
			{
				SD_CARD_CHIP_SELECT_HIGH;
				return;
			}
			if (write_read_byte_SPI_SD_CAN(0xFF) != 0x01)
			{
				SD_CARD_CHIP_SELECT_HIGH;
				return;
			}
			if (write_read_byte_SPI_SD_CAN(0xFF) != 0xAA)
			{
				SD_CARD_CHIP_SELECT_HIGH;
				return;
			}
		}
	}

	for (uint16_t i = 0; i < 2000; i++)
	{
		//Extra clocks to allow setup time
		write_byte_SPI_SD_CAN(0xFF);

		//Send CMD55, correct CRC not needed but sent anyway
		write_byte_SPI_SD_CAN(0x77);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x65);
		//Read R2 response
		counter = 0;
		do
		{
			response = write_read_byte_SPI_SD_CAN(0xFF);
		} while (response != 0x01 && counter++ < 8);

		if (response != 0x01)
			continue;

		//Extra clocks to allow setup time
		write_byte_SPI_SD_CAN(0xFF);

		//Send ACMD41 with HSC arg, correct CRC not needed but sent anyway
		write_byte_SPI_SD_CAN(0x69);
		write_byte_SPI_SD_CAN(0x40);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x00);
		write_byte_SPI_SD_CAN(0x77);
		//Read R1 response
		counter = 0;
		do
		{
			response = write_read_byte_SPI_SD_CAN(0xFF);
		} while (response != 0x01 && response != 0x00 && counter++ < 8);
		//0x01 response means try again
		if (response == 0x01)
		{
			continue;
		}
		//0x00 reponse means passed
		else if (response == 0x00)
		{
			break;
		}
		//Any other command means fail
		else
		{
			SD_CARD_CHIP_SELECT_HIGH;
			return;
		}
	}

	//Extra clocks to allow setup time
	write_byte_SPI_SD_CAN(0xFF);

	//Send CMD58, correct CRC not needed but sent anyway
	write_byte_SPI_SD_CAN(0x7A);
	write_byte_SPI_SD_CAN(0x00);
	write_byte_SPI_SD_CAN(0x00);
	write_byte_SPI_SD_CAN(0x00);
	write_byte_SPI_SD_CAN(0x00);
	write_byte_SPI_SD_CAN(0xFD);

	//Read R1 response
	counter = 0;
	do
	{
		response = write_read_byte_SPI_SD_CAN(0xFF);
	} while (response != 0x00 && counter++ < 8);
	//0x00 response means read OCR
	if (response == 0x00)
	{
		write_read_byte_SPI_SD_CAN(0xFF);
		write_read_byte_SPI_SD_CAN(0xFF);
		write_read_byte_SPI_SD_CAN(0xFF);
		if (!(write_read_byte_SPI_SD_CAN(0xFF) & 0x02))
		{
			//Extra clocks to allow setup time
			write_byte_SPI_SD_CAN(0xFF);

			//Send CMD16 to force block size to 128 bytes
			write_byte_SPI_SD_CAN(0x50);
			write_byte_SPI_SD_CAN(0x00);
			write_byte_SPI_SD_CAN(0x00);
			write_byte_SPI_SD_CAN(0x00);
			write_byte_SPI_SD_CAN(0x02);
			write_byte_SPI_SD_CAN(0x00);

			//Read R1 response
			counter = 0;
			do
			{
				response = write_read_byte_SPI_SD_CAN(0xFF);
			} while (response != 0x00 && counter++ < 8);
			//0x01 response means success
			if (response != 0x00)
			{
				SD_CARD_CHIP_SELECT_HIGH;
				return;
			}
		}
	}
	//Extra clocks to allow setup time
	write_byte_SPI_SD_CAN(0xFF);
	SD_CARD_CHIP_SELECT_HIGH;
	//Extra clocks so SD card releases MOSI line
	write_byte_SPI_SD_CAN(0xFF);
	sd_card_passed = TRUE;
}

uint8_t write_block_SD(uint32_t address)
{
	SD_CARD_CHIP_SELECT_LOW;
	//Make sure sd card is not busy
	while (write_read_byte_SPI_SD_CAN(0xFF) != 0xFF);
	//Send CMD24 to read block
	write_byte_SPI_SD_CAN(0x58);
	write_byte_SPI_SD_CAN((uint8_t)((address & 0xFF000000) >> 24));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x00FF0000) >> 16));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x0000FF00) >> 8));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x000000FF) >> 0));
	write_byte_SPI_SD_CAN(0x00);

	//Read R1 response
	uint16_t counter = 0;
	do
	{
		response = write_read_byte_SPI_SD_CAN(0xFF);
		_delay_ms(5);
	} while (response != 0x00 && counter++ < 8);
	//0x00 response means success
	if (response != 0x00)
	{
		SD_CARD_CHIP_SELECT_HIGH;
		return FALSE;
	}

	//Extra clocks to allow setup time
	write_byte_SPI_SD_CAN(0xFF);
	write_byte_SPI_SD_CAN(0xFF);
	write_byte_SPI_SD_CAN(0xFF);

	//Write 0xFE to indicate beginning of data
	write_byte_SPI_SD_CAN(0xFE);

	counter = 0;
	do
	{
		response = write_read_byte_SPI_SD_CAN(event_buffer[counter++]);
	} while (counter < ENTRY_BYTES_PER_BLOCK);

	do
	{
		response = write_read_byte_SPI_SD_CAN(0xFF);
	} while (response == 0xFF);


	//Read data response
	if ((response & 0x1F) != 0x05)
		return FALSE;

	//Extra clocks to allow setup time
	write_byte_SPI_SD_CAN(0xFF);
	SD_CARD_CHIP_SELECT_HIGH;

	return TRUE;
}

uint8_t read_block_SD(uint32_t address)
{
	SD_CARD_CHIP_SELECT_LOW;
	//Make sure sd card is not busy
	while (write_read_byte_SPI_SD_CAN(0xFF) != 0xFF);
	//Send CMD17 to read block
	write_byte_SPI_SD_CAN(0x51);
	write_byte_SPI_SD_CAN((uint8_t)((address & 0xFF000000) >> 24));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x00FF0000) >> 16));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x0000FF00) >> 8));
	write_byte_SPI_SD_CAN((uint8_t)((address & 0x000000FF) >> 0));
	write_byte_SPI_SD_CAN(0x00);

	//Read R1 response
	uint16_t counter = 0;
	do
	{
		response = write_read_byte_SPI_SD_CAN(0xFF);
		_delay_ms(5);
	} while (response != 0x00 && counter++ < 8);
	//0x00 response means success
	if (response != 0x00)
	{
		SD_CARD_CHIP_SELECT_HIGH;
		return FALSE;
	}

	counter = 0;
	do
	{
		response = write_read_byte_SPI_SD_CAN(0xFF);
		_delay_ms(5);
	} while (response != 0xFE && counter++ < 8);
	//0xFE response means data will begin
	if (response != 0xFE)
	{
		SD_CARD_CHIP_SELECT_HIGH;
		return FALSE;
	}

	counter = 0;
	do
	{
		event_buffer[counter++] = write_read_byte_SPI_SD_CAN(0xFF);
	} while (counter < ENTRY_BYTES_PER_BLOCK);

	for (; counter < 516; counter++)
		write_read_byte_SPI_SD_CAN(0xFF);

	//Extra clocks to allow setup time
	write_byte_SPI_SD_CAN(0xFF);
	SD_CARD_CHIP_SELECT_HIGH;
	return TRUE;
}

//Infinite loop to clear SD card
//Although clearing only the event logging block will work
void erase_SD_card(void)
{
	cli();
	clear_event_buffer();

	uint16_t counter = 0;
	while (1)
	{
		while (!write_block_SD(counter));
		counter++;
	}
}

