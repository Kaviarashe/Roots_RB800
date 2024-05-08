/*
 * motor_settings.c
 *
 * Created: 6/28/2016 10:18:23 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "motor_settings.h"

void initialize_RB_settings(void)
{
	//Check if the device settings have ever been set
	//Usually only needs to do this first time powering on
	if (eeprom_read_byte((const uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR) != DEVICE_SETTINGS_SET)
	{
		set_to_default();
		eeprom_update_byte((uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR, DEVICE_SETTINGS_SET);

		uint8_t temp[A3_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
		eeprom_busy_wait();

		//Read in A1 serial number
		eeprom_read_block((void *)A3_serial_num, (const void*)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
		A3_serial_num[A3_SERIAL_NUM_LEN] = '\0';
	}

	//Read in serial number
	eeprom_read_block((void *)A3_serial_num, (const void*)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
	if (A3_serial_num[0] != 0x52)// || A3_serial_num[0] > 0x39)
	{
		uint8_t temp[A3_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}
	eeprom_read_block((void *)A3_serial_num, (const void*)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
	A3_serial_num[A3_SERIAL_NUM_LEN] = '\0';

	//Read in the drive motor settings
	acceleration = eeprom_read_byte((const uint8_t *)EEP8_ACCELERATION_ADDR);
	deceleration = eeprom_read_byte((const uint8_t *)EEP8_DECELERATION_ADDR);
	brake_delay = eeprom_read_byte((const uint8_t *)EEP8_BRAKE_DELAY_ADDR);
	reverse_speed = eeprom_read_byte((const uint8_t *)EEP8_REVERSE_SPEED_ADDR);
	slow_speed = eeprom_read_byte((const uint8_t *)EEP8_SLOW_SPEED_ADDR);
	fast_speed = eeprom_read_byte((const uint8_t *)EEP8_FAST_SPEED_ADDR);
	rated_current = eeprom_read_byte((const uint8_t *)EEP8_CURRENT_RATING_ADDR);
	max_current = eeprom_read_byte((const uint8_t *)EEP8_MAX_CURRENT_ADDR);
	full_overload_time = eeprom_read_byte((const uint8_t *)EEP8_OVERLOAD_TIME_ADDR);
	pot_low_deadband = eeprom_read_word((const uint16_t *)EEP16_POT_LOW_DEADBAND_ADDR);
	pot_high_deadband = eeprom_read_word((const uint16_t *)EEP16_POT_HIGH_DEADBAND_ADDR);

	//Get and compare checksums
	//If they do not match reset the settings to the default
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	uint8_t stored_checksum = eeprom_read_byte((const uint8_t *)EEP8_CHECKSUM_ADDR);

	if (checksum != stored_checksum)
	{
		motor_parameter_error = TRUE;
		set_to_default();
	}
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

	over_current_rating = (uint8_t)((float)rated_current * 1.10);
	overload_time_constant = (uint32_t)(((float)max_current * max_current - (float)rated_current * rated_current) * full_overload_time);
}

void set_to_default(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_ACCELERATION_ADDR, ACCELERATION_RATE);		//Time to max acceleration in 100ms/unit = 3 sec
	eeprom_update_byte((uint8_t *)EEP8_DECELERATION_ADDR, DECELERATION_RATE);		//Time from max acceleration to stop in 100ms/unit = 500ms
	eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY_ADDR, BRAKE_DELAY);				//Time brake pedal press to stop in 100ms/unit = 1.2 sec
	eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED_ADDR, MAX_REVERSE_SPEED);		//Reverse speed in percentage of max speed = 40%
	eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED_ADDR, SLOW_PWM_PERCENTAGE);		//Slow forward speed in percentage of max speed = 60%
	eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED_ADDR, FAST_PWM_PERCENTAGE); 		//Fast forward speed in percentage of max speed = 100%
	eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING_ADDR, RATED_CURRENT);			//Default current rating in A/unit = 30A
	eeprom_update_byte((uint8_t *)EEP8_MAX_CURRENT_ADDR, MAX_RUNNING_CURRENT);		//Max working current rating in A/unit = 100A
	eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME_ADDR, MAX_OVERCURRENT_TIME);	//Max number of seconds allowed over working current rating in seconds = 45 seconds
	eeprom_update_word((uint16_t *)EEP16_POT_LOW_DEADBAND_ADDR, POT_LOW_DEADBAND);	//Voltage in mV of the end of the low side deadband of the pot = 0 V
	eeprom_update_word((uint16_t *)EEP16_POT_HIGH_DEADBAND_ADDR, POT_HIGH_DEADBAND);	//Voltage in mV of the start of the high side deadband of the pot = 2.048 V

	//Read in the drive motor settings
	acceleration = eeprom_read_byte((const uint8_t *)EEP8_ACCELERATION_ADDR);
	deceleration = eeprom_read_byte((const uint8_t *)EEP8_DECELERATION_ADDR);
	brake_delay = eeprom_read_byte((const uint8_t *)EEP8_BRAKE_DELAY_ADDR);
	reverse_speed = eeprom_read_byte((const uint8_t *)EEP8_REVERSE_SPEED_ADDR);
	slow_speed = eeprom_read_byte((const uint8_t *)EEP8_SLOW_SPEED_ADDR);
	fast_speed = eeprom_read_byte((const uint8_t *)EEP8_FAST_SPEED_ADDR);
	rated_current = eeprom_read_byte((const uint8_t *)EEP8_CURRENT_RATING_ADDR);
	max_current = eeprom_read_byte((const uint8_t *)EEP8_MAX_CURRENT_ADDR);
	full_overload_time = eeprom_read_byte((const uint8_t *)EEP8_OVERLOAD_TIME_ADDR);
	pot_low_deadband = eeprom_read_word((const uint16_t *)EEP16_POT_LOW_DEADBAND_ADDR);
	pot_high_deadband = eeprom_read_word((const uint16_t *)EEP16_POT_HIGH_DEADBAND_ADDR);
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}

void update_motor_settings(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_ACCELERATION_ADDR, acceleration);			//Time to max acceleration in 100ms/unit
	eeprom_update_byte((uint8_t *)EEP8_DECELERATION_ADDR, deceleration);			//Time from max acceleration to stop in 100ms/unit
	eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY_ADDR, brake_delay);				//Time brake pedal press to stop in 100ms/unit
	eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED_ADDR, reverse_speed);			//Reverse speed in percentage of max speed
	eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED_ADDR, slow_speed);				//Slow forward speed in percentage of max speed
	eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED_ADDR, fast_speed); 				//Fast forward speed in percentage of max speed
	eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING_ADDR, rated_current);		//Rated current in A/unit
	eeprom_update_byte((uint8_t *)EEP8_MAX_CURRENT_ADDR, max_current);				//Max working current rating in A/unit
	eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME_ADDR, full_overload_time);			//Max number of seconds allowed over working current rating in seconds
	eeprom_update_word((uint16_t *)EEP16_POT_LOW_DEADBAND_ADDR, pot_low_deadband);	//Voltage in mV of the end of the low side deadband of the pot
	eeprom_update_word((uint16_t *)EEP16_POT_HIGH_DEADBAND_ADDR, pot_high_deadband);	//Voltage in mV of the start of the high side deadband of the pot
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}

void update_accel_settings(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_ACCELERATION_ADDR, acceleration);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings

	sei();
}
void update_decel_settings(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_DECELERATION_ADDR, deceleration);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_break_delay_settings(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY_ADDR, brake_delay);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_reverse_speed(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED_ADDR, reverse_speed);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_fast_speed(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED_ADDR, fast_speed);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_slow_speed(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED_ADDR, slow_speed);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_current_rating(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING_ADDR, rated_current);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_max_current(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_MAX_CURRENT_ADDR, max_current);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}
void update_over_current_time(void)
{
	cli();
	//Set default settings for RB values
	eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME_ADDR, full_overload_time);			//Time to max acceleration in 100ms/unit
	eeprom_busy_wait();
	uint8_t checksum = acceleration ^ deceleration ^ brake_delay ^ reverse_speed ^ slow_speed ^ fast_speed
		^ rated_current ^ max_current ^ full_overload_time
		^ ((uint8_t)pot_low_deadband & 0xFF) ^ ((uint8_t)(pot_low_deadband >> 8))
		^ ((uint8_t)pot_high_deadband & 0xFF) ^ ((uint8_t)(pot_high_deadband >> 8));
	eeprom_update_byte((uint8_t *)EEP8_CHECKSUM_ADDR, checksum); // Checksum of settings
	sei();
}