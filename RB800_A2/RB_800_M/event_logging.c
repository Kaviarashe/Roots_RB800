/*
 * event_logging.c
 *
 * Created: 6/29/2016 3:51:39 PM
 *  Author: Anurag Vaddi Reddy
 */

#include "event_logging.h"

//IVA002: Standarized library functions. TODO move these to a common file
uint32_t ReadU32BE(uint8_t* buff)
{
	uint32_t ret = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		ret <<= 8;
		ret += buff[i];
	}
	return ret;
}

uint16_t ReadU16BE(uint8_t* buff)
{
	uint16_t ret = 0;
	for (uint8_t i = 0; i < 2; i++)
	{
		ret <<= 8;
		ret += buff[i];
	}
	return ret;
}

void WriteU32BE(uint8_t* buff, uint32_t v)
{
	for (int8_t i = 3; i >= 0; i--)//IVA003: Fixed ++ typo
	{
		buff[i] = v & 0xff;
		v >>= 8;
	}
}

void WriteU16BE(uint8_t* buff, uint16_t v)
{
	for (int8_t i = 1; i >= 0; i--)
	{
		buff[i] = v & 0xff;
		v >>= 8;
	}
}

void initialize_event_logging(void)
{
	///*Testing without log data*/
	//return;

	//Read first block of SD card which contains logging info
	uint8_t counter = 0;
	while (!read_block_SD(EVENT_LOGGING_INFO_ADDR) && counter < 3)
	{
		_delay_ms(25);
		counter++;
	}
	//If reading the SD card fails 3 times, label it non functioning
	if (counter == 3)
	{
		sd_card_passed = FALSE;
		return;
	}

	//Get the position of the number of entries byte
	num_entries_pos = (uint8_t)strlen(EVENT_LOGGING_HEADER) + 1;// +strlen(SERIAL_HEADER) + 1;

	//Check if the OPUS SOLUTIONS header exists in the SD Card
	char header[21];
	sprintf(header, "%.20s", event_buffer);
	if (strcmp(header, EVENT_LOGGING_HEADER) != 0)
	{
		//Otherwise clear block and write the header
		clear_event_buffer();
		sprintf((char*)event_buffer, EVENT_LOGGING_HEADER);
		//uint8_t i = 0;
		/*event_buffer[i++ + 4] = 1;
		event_buffer[i++ + 4] = 2;
		event_buffer[i++ + 4] = 3;
		event_buffer[i++ + 4] = 4;
		event_buffer[i++ + 4] = 5;
		event_buffer[i++ + 4] = 1;
		event_buffer[i++ + 4] = 2;
		event_buffer[i++ + 4] = 3;
		event_buffer[i++ + 4] = 4;
		event_buffer[i++ + 4] = 5;	*/
		current_position = 0;

		counter = 0;
		while (!write_block_SD(EVENT_LOGGING_INFO_ADDR) && counter < 3)
		{
			_delay_ms(25);
			counter++;
		}

		if (counter == 3)
		{
			sd_card_passed = FALSE;
			return;
		}

		/*sprintf(event_buffer, SERIAL_HEADER);
		current_position = 0;

		counter = 0;
		while (!write_block_SD(SERIAL_LOGGING_INFO_ADDR) && counter < 3)
		{
			_delay_ms(25);
			counter++;
		}

		if(counter == 3)
		{
			sd_card_passed = FALSE;
			return;
		}*/

	}
	else
	{
		//Read total num entries and current position
		num_entries = ReadU32BE((uint8_t*)&event_buffer[num_entries_pos]);		//IVA002: Code standardization
		current_position = ReadU16BE((uint8_t*)&event_buffer[num_entries_pos]);	//IVA002: Code standardization
	}


	uint8_t position = 23;

	//Read all the statistics from the SD card
	brush_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	vacuum_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	presweep_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	eco_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	water_pump_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	detergent_pump_hours = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;

	brush1_average_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	brush2_average_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	vacuum_average_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	presweep_average_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;

	max_brush1_overall_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	max_brush2_overall_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	max_vacuum_overall_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;
	max_presweep_overall_current = ((uint16_t)event_buffer[position] << 8) + event_buffer[position + 1]; position += 2;

	event_logging = OFF;
}

/*Log event should eventually write multiple events at a time if there are more events waiting the queue*/
void log_event(uint8_t* event_data)
{
	///*Testing without log data*/
	//return;

	//Clear interrupts otherwise SD card eventually fails during an interrupt
	cli();
	uint8_t counter = 0;
	while (!read_block_SD(FIRST_ENTRY_ADDRESS_VAL + num_entries / ENTRIES_PER_BLOCK) && counter < 3)
	{
		_delay_ms(15);
		counter++;
	}

	if (counter == 3)
	{
		sd_card_passed = FALSE;
		sei();

		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		emergency_off = TRUE;

		send_SD_card_fail();
		return;
	}

	uint8_t i = 0;
	//Copy over event code and number of data bytes
	event_buffer[i + current_position] = event_data[i];
	i++;
	event_buffer[i + current_position] = event_data[i];
	i++;
	uint8_t num_bytes = event_data[i - 1];
	for (uint8_t j = 0; j < num_bytes; j++, i++)
	{
		event_buffer[i + current_position] = event_data[i];
	}

	//Log the current run timer and number of seconds since board has turned on
	uint16_t total_sec = RTC.CNT;
	event_buffer[i++ + current_position] = (uint8_t)(total_sec >> 8);
	event_buffer[i++ + current_position] = (uint8_t)total_sec;
	event_buffer[i++ + current_position] = (uint8_t)(hours >> 8); 
	event_buffer[i++ + current_position] = (uint8_t)hours;
	event_buffer[i++ + current_position] = minutes;

	counter = 0;
	//Write to the SD Card
	while (!write_block_SD(FIRST_ENTRY_ADDRESS_VAL + num_entries / ENTRIES_PER_BLOCK) && counter < 3)
	{
		_delay_ms(15);
		counter++;
	}


	if (counter == 3)
	{
		sd_card_passed = FALSE;
		sei();

		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		emergency_off = TRUE;

		send_SD_card_fail();
		return;
	}

	counter = 0;
	//Read the logging info block
	while (!read_block_SD(EVENT_LOGGING_INFO_ADDR) && counter < 3)
	{
		_delay_ms(15);
		counter++;
	}

	if (counter == 3)
	{
		sd_card_passed = FALSE;
		sei();

		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		emergency_off = TRUE;

		send_SD_card_fail();
		return;
	}

	//Log the updated number of entries
	num_entries++;
	event_buffer[num_entries_pos] = (uint8_t)(num_entries >> 24);
	event_buffer[num_entries_pos + 1] = (uint8_t)(num_entries >> 16);
	event_buffer[num_entries_pos + 2] = (uint8_t)(num_entries >> 8);
	event_buffer[num_entries_pos + 3] = (uint8_t)(num_entries);
	if (num_entries % 40 == 0)
	{
		current_position = 0;
		event_buffer[num_entries_pos + 5] = 0;
		event_buffer[num_entries_pos + 6] = 0;
	}
	else
	{
		current_position += MINIMUM_BYTES_PER_ENTRY + num_bytes;
		event_buffer[num_entries_pos + 5] = current_position >> 8;
		event_buffer[num_entries_pos + 6] = current_position & 0xFF;
	}

	//Update board statistics
	uint8_t position = 23;// 27;
	event_buffer[position++] = brush_hours >> 8;
	event_buffer[position++] = brush_hours & 0xFF;

	event_buffer[position++] = vacuum_hours >> 8;
	event_buffer[position++] = vacuum_hours & 0xFF;

	event_buffer[position++] = presweep_hours >> 8;
	event_buffer[position++] = presweep_hours & 0xFF;

	event_buffer[position++] = eco_hours >> 8;
	event_buffer[position++] = eco_hours & 0xFF;

	event_buffer[position++] = water_pump_hours >> 8;
	event_buffer[position++] = water_pump_hours & 0xFF;

	event_buffer[position++] = detergent_pump_hours >> 8;
	event_buffer[position++] = detergent_pump_hours & 0xFF;


	event_buffer[position++] = brush1_average_current >> 8;
	event_buffer[position++] = brush1_average_current & 0xFF;

	event_buffer[position++] = brush2_average_current >> 8;
	event_buffer[position++] = brush2_average_current & 0xFF;

	event_buffer[position++] = vacuum_average_current >> 8;
	event_buffer[position++] = vacuum_average_current & 0xFF;

	event_buffer[position++] = presweep_average_current >> 8;
	event_buffer[position++] = presweep_average_current & 0xFF;


	event_buffer[position++] = max_brush1_overall_current >> 8;
	event_buffer[position++] = max_brush1_overall_current & 0xFF;

	event_buffer[position++] = max_brush2_overall_current >> 8;
	event_buffer[position++] = max_brush2_overall_current & 0xFF;

	event_buffer[position++] = max_vacuum_overall_current >> 8;
	event_buffer[position++] = max_vacuum_overall_current & 0xFF;

	event_buffer[position++] = max_presweep_overall_current >> 8;
	event_buffer[position++] = max_presweep_overall_current & 0xFF;


	counter = 0;
	//Write statistics and num entries
	while (!write_block_SD(EVENT_LOGGING_INFO_ADDR) && counter < 3)
	{
		_delay_ms(15);
		counter++;
	}

	if (counter == 3)
	{
		sd_card_passed = FALSE;
		sei();

		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		emergency_off = TRUE;

		send_SD_card_fail();
		return;
	}
	sei();
}

//Read an event beginning with number
void read_event(uint32_t num)
{
	///*Testing without log data*/
	//return;

	if (num <= num_entries)
	{
		uint8_t counter = 0;
		while (!read_block_SD(FIRST_ENTRY_ADDRESS_VAL + num / ENTRIES_PER_BLOCK) && counter < 3)
		{
			_delay_ms(25);
			counter++;
		}

		if (counter == 3)
		{
			sd_card_passed = FALSE;
			return;
		}
	}
}

//Clear the event buffer array
void clear_event_buffer(void)
{
	for (uint16_t i = 0; i < ENTRY_BYTES_PER_BLOCK; i++)
	{
		event_buffer[i] = 0;
	}
}
