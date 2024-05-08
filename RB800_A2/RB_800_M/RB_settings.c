/*
 * RB_settings.c
 *
 * Created: 9/16/2015 11:40:43 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "RB_settings.h"

void initialize_RB_settings(void)
{
	//Check if the device settings have ever been set
	//Usually only needs to do this first time powering on
	if (eeprom_read_byte((const uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR) != DEVICE_SETTINGS_SET)
	{
		set_to_default();
		eeprom_update_byte((uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR, DEVICE_SETTINGS_SET);
		eeprom_busy_wait();

		uint8_t temp[A2_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
		eeprom_busy_wait();

		eeprom_update_byte((uint8_t *)EEP8_BRUSH_MINUTES_ADDR, 0); //update brush minutes as zero while first time powering on the board
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_VACUUM_MINUTES_ADDR, 0); //update vacuum minutes as zero while first time powering on the board
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_PRESWEEP_MINUTES_ADDR, 0); //update presweep minutes as zero while first time powering on the board
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_ECO_MINUTES_ADDR, 0); //update Eco minutes as zero while first time powering on the board
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_WATER_PUMP_MINUTES_ADDR, 0); //update water pump minutes as zero while first time powering on the board
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_DETERGENT_PUMP_MINUTES, 0); //update detergent pump minutes for as zero while first time powering on the board
		eeprom_busy_wait();

		//Read in A2 serial number
		eeprom_read_block((void *)A2_serial_num, (const void*)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
		A2_serial_num[A2_SERIAL_NUM_LEN] = '\0';
	}

	//Read in serial number
	eeprom_read_block((void *)A2_serial_num, (const void*)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
	if (A2_serial_num[0] != 0x52)// || A2_serial_num[0] > 0x39)
	{
		uint8_t temp[A2_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}
	else if ((A2_serial_num[2] < 0x30 || A2_serial_num[2] > 0x39) || (A2_serial_num[3] < 0x30 || A2_serial_num[3] > 0x39) || (A2_serial_num[4] < 0x30 || A2_serial_num[4] > 0x39) || (A2_serial_num[5] < 0x30 || A2_serial_num[5] > 0x39) || (A2_serial_num[6] < 0x30 || A2_serial_num[6] > 0x39) || (A2_serial_num[7] < 0x30 || A2_serial_num[7] > 0x39) || (A2_serial_num[8] < 0x30 || A2_serial_num[8] > 0x39) || (A2_serial_num[9] < 0x30 || A2_serial_num[9] > 0x39) || (A2_serial_num[10] < 0x30 || A2_serial_num[10] > 0x39) || (A2_serial_num[11] < 0x30 || A2_serial_num[11] > 0x39) || (A2_serial_num[12] < 0x30 || A2_serial_num[12] > 0x39) || (A2_serial_num[13] < 0x30 || A2_serial_num[13] > 0x39))
	{
		uint8_t temp[A2_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}
	eeprom_read_block((void *)A2_serial_num, (const void*)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
	A2_serial_num[A2_SERIAL_NUM_LEN] = '\0';

	//Read in the drive motor settings
	vacuum_slow_start = eeprom_read_byte((const uint8_t *)EEP8_VACUUM_SLOW_START);
	brush_slow_start = eeprom_read_byte((const uint8_t *)EEP8_BRUSH_SLOW_START);
	pre_swp_slow_start = eeprom_read_byte((const uint8_t *)EEP8_PRE_SWP_SLOW_START);


	//Read in the statistics
	brush_minutes = eeprom_read_byte((const uint8_t *)EEP8_BRUSH_MINUTES_ADDR);
	vacuum_minutes = eeprom_read_byte((const uint8_t *)EEP8_VACUUM_MINUTES_ADDR);
	presweep_minutes = eeprom_read_byte((const uint8_t *)EEP8_PRESWEEP_MINUTES_ADDR);
	eco_minutes = eeprom_read_byte((const uint8_t *)EEP8_ECO_MINUTES_ADDR);
	water_pump_minutes = eeprom_read_byte((const uint8_t *)EEP8_WATER_PUMP_MINUTES_ADDR);
	detergent_pump_minutes = eeprom_read_byte((const uint8_t *)EEP8_DETERGENT_PUMP_MINUTES);

	/*eeprom_busy_wait();
	PORF_cnt = eeprom_read_byte((const uint8_t *) EEP_PORF_cnt_location);
	eeprom_busy_wait();
	EXTRF_cnt = eeprom_read_byte((const uint8_t *) EEP_EXTRF_cnt_location);
	eeprom_busy_wait();
	BORF_cnt = eeprom_read_byte((const uint8_t *) EEP_BORF_cnt_location);
	eeprom_busy_wait();
	WDRF_cnt = eeprom_read_byte((const uint8_t *) EEP_WDRF_cnt_location);
	eeprom_busy_wait();
	PDIRF_cnt = eeprom_read_byte((const uint8_t *) EEP_PDIRF_cnt_location);
	eeprom_busy_wait();
	SRF_cnt = eeprom_read_byte((const uint8_t *) EEP_SRF_cnt_location);
	eeprom_busy_wait();
	SDRF_cnt = eeprom_read_byte((const uint8_t *) EEP_SDRF_cnt_location);
	eeprom_busy_wait();

	if (PORF_cnt==255 || EXTRF_cnt==255 || BORF_cnt==255 || WDRF_cnt==255)
	{
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_SDRF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_SRF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_PDIRF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_WDRF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_BORF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_EXTRF_cnt_location, 0);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_PORF_cnt_location, 0);
		eeprom_busy_wait();

		PORF_cnt = eeprom_read_byte((const uint8_t *) EEP_PORF_cnt_location);
		eeprom_busy_wait();
		EXTRF_cnt = eeprom_read_byte((const uint8_t *) EEP_EXTRF_cnt_location);
		eeprom_busy_wait();
		BORF_cnt = eeprom_read_byte((const uint8_t *) EEP_BORF_cnt_location);
		eeprom_busy_wait();
		WDRF_cnt = eeprom_read_byte((const uint8_t *) EEP_WDRF_cnt_location);
		eeprom_busy_wait();
		PDIRF_cnt = eeprom_read_byte((const uint8_t *) EEP_PDIRF_cnt_location);
		eeprom_busy_wait();
		SRF_cnt = eeprom_read_byte((const uint8_t *) EEP_SRF_cnt_location);
		eeprom_busy_wait();
		SDRF_cnt = eeprom_read_byte((const uint8_t *) EEP_SDRF_cnt_location);
		eeprom_busy_wait();
	}*/
}

void set_to_default(void)
{
	cli();
	eeprom_update_byte((uint8_t *)EEP8_VACUUM_SLOW_START, MAX_VACUUM_RAMPUP_TIME); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for vacuum motor
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_BRUSH_SLOW_START, MAX_BRUSH_RAMPUP_TIME); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for brush motor
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_PRE_SWP_SLOW_START, MAX_PRE_SWP_RAMPUP_TIME); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for preswp motor
	eeprom_busy_wait();

	//Read in the drive motor settings
	vacuum_slow_start = eeprom_read_byte((const uint8_t *)EEP8_VACUUM_SLOW_START);
	brush_slow_start = eeprom_read_byte((const uint8_t *)EEP8_BRUSH_SLOW_START);
	pre_swp_slow_start = eeprom_read_byte((const uint8_t *)EEP8_PRE_SWP_SLOW_START);


	eeprom_read_block((void *)A2_serial_num, (const void*)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
	sei();
}
void update_brush_slow_start(void)
{
	cli();
	eeprom_update_byte((uint8_t *)EEP8_BRUSH_SLOW_START, brush_slow_start);
	eeprom_busy_wait();
	sei();
}
void update_vaccum_slow_start(void)
{
	cli();
	eeprom_update_byte((uint8_t *)EEP8_VACUUM_SLOW_START, vacuum_slow_start);
	eeprom_busy_wait();
	sei();
}
void update_pre_swp_slow_start(void)
{
	cli();
	eeprom_update_byte((uint8_t *)EEP8_PRE_SWP_SLOW_START, pre_swp_slow_start);
	eeprom_busy_wait();
	sei();
}