/*
 * RB_settings.c
 *
 * Created: 9/16/2015 11:50:15 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "RB_settings.h"
#include "intercommunication.h"

void initialize_RB_settings(void)
{
	//Check if the device settings have ever been set
	//Usually only needs to do this first time powering on
	eeprom_busy_wait();
	if (eeprom_read_byte((const uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR) != DEVICE_SETTINGS_SET)
	{
		set_to_default();
		eeprom_update_byte((uint8_t *)EEP8_DEVICE_SETTINGS_SET_ADDR, DEVICE_SETTINGS_SET);
		eeprom_busy_wait();

		uint8_t temp[SERIAL_NUM_LEN + 1] = "0000_00_0_0000_0";
		eeprom_update_block((const void*)temp, (void *)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
		eeprom_busy_wait();

		//Read in serial number
		eeprom_read_block((void *)serial_num, (const void*)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
		eeprom_busy_wait();
		serial_num[SERIAL_NUM_LEN] = '\0';
		uint8_t temp1[A1_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp1, (void *)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
		eeprom_busy_wait();

		//Read in A1 serial number
		eeprom_read_block((void *)A1_serial_num, (const void*)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
		eeprom_busy_wait();
		A1_serial_num[A1_SERIAL_NUM_LEN] = '\0';

		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_VACUUM_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for vacuum motor
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_BRUSH_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for brush motor
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_PRE_SWP_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for preswp motor
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_ACCELERATION, 30);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_DECELERATION, 8);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY, 12);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED, 40);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED, 100);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED, 60);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING, 30);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_MAX_CURRENT, 100);
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME, 45);

	}

	//Read in serial number
	eeprom_read_block((void *)serial_num, (const void*)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
	eeprom_busy_wait();
	if (serial_num[0] < 0x30 || serial_num[0] > 0x39)
	{
		uint8_t temp[SERIAL_NUM_LEN + 1] = "0000_00_0_0000_0";
		eeprom_update_block((const void*)temp, (void *)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}
	eeprom_read_block((void *)serial_num, (const void*)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
	eeprom_busy_wait();
	serial_num[SERIAL_NUM_LEN] = '\0';

	eeprom_read_block((void *)A1_serial_num, (const void*)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
	eeprom_busy_wait();
	if (A1_serial_num[0] != 0x52)//|| A1_serial_num[0] > 0x39)
	{
		uint8_t temp[A1_SERIAL_NUM_LEN + 1] = "RV000000000000";
		eeprom_update_block((const void*)temp, (void *)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}
	eeprom_read_block((void *)A1_serial_num, (const void*)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
	eeprom_busy_wait();
	A1_serial_num[A1_SERIAL_NUM_LEN] = '\0';

	//Read in the drive motor settings
	vacuum_slow_start = eeprom_read_byte((const uint8_t *)EEP8_VACUUM_SLOW_START);
	eeprom_busy_wait();
	brush_slow_start = eeprom_read_byte((const uint8_t *)EEP8_BRUSH_SLOW_START);
	eeprom_busy_wait();
	pre_swp_slow_start = eeprom_read_byte((const uint8_t *)EEP8_PRE_SWP_SLOW_START);
	eeprom_busy_wait();

	acceleration = eeprom_read_byte((const uint8_t *)EEP8_ACCELERATION);
	eeprom_busy_wait();
	deceleration = eeprom_read_byte((const uint8_t *)EEP8_DECELERATION);
	eeprom_busy_wait();
	brake_delay = eeprom_read_byte((const uint8_t *)EEP8_BRAKE_DELAY);
	eeprom_busy_wait();
	reverse_speed = eeprom_read_byte((const uint8_t *)EEP8_REVERSE_SPEED);
	eeprom_busy_wait();
	fast_speed = eeprom_read_byte((const uint8_t *)EEP8_FAST_SPEED);
	eeprom_busy_wait();
	slow_speed = eeprom_read_byte((const uint8_t *)EEP8_SLOW_SPEED);
	eeprom_busy_wait();
	current_rating = eeprom_read_byte((const uint8_t *)EEP8_CURRENT_RATING);
	eeprom_busy_wait();
	max_current = eeprom_read_byte((const uint8_t *)EEP8_MAX_CURRENT);
	eeprom_busy_wait();
	overload_time = eeprom_read_byte((const uint8_t *)EEP8_OVERLOAD_TIME);
	eeprom_busy_wait();

	//Read in the battery type
	battery_type = eeprom_read_byte((const uint8_t *)EEP8_BATTERY_TYPE);
	eeprom_busy_wait();
}

void save_motor_settings_to_eeprom() {
	eeprom_update_byte((uint8_t*)EEP8_ACCELERATION, acceleration);
	eeprom_update_byte((uint8_t*)EEP8_DECELERATION, deceleration);
	eeprom_update_byte((uint8_t*)EEP8_BRAKE_DELAY, brake_delay);
	eeprom_update_byte((uint8_t*)EEP8_REVERSE_SPEED, reverse_speed);
	eeprom_update_byte((uint8_t*)EEP8_SLOW_SPEED, slow_speed);
	eeprom_update_byte((uint8_t*)EEP8_FAST_SPEED, fast_speed);
	eeprom_update_byte((uint8_t*)EEP8_CURRENT_RATING, rated_current);
	eeprom_update_byte((uint8_t*)EEP8_MAX_CURRENT, max_current);
	eeprom_update_byte((uint8_t*)EEP8_OVERLOAD_TIME, full_overload_time);
}
void load_motor_settings_from_eeprom() {
	acceleration = eeprom_read_byte((uint8_t*)EEP8_ACCELERATION);
	deceleration = eeprom_read_byte((uint8_t*)EEP8_DECELERATION);
	brake_delay = eeprom_read_byte((uint8_t*)EEP8_BRAKE_DELAY);
	reverse_speed = eeprom_read_byte((uint8_t*)EEP8_REVERSE_SPEED);
	slow_speed = eeprom_read_byte((uint8_t*)EEP8_SLOW_SPEED);
	fast_speed = eeprom_read_byte((uint8_t*)EEP8_FAST_SPEED);
	rated_current = eeprom_read_byte((uint8_t*)EEP8_CURRENT_RATING);
	max_current = eeprom_read_byte((uint8_t*)EEP8_MAX_CURRENT);
	full_overload_time = eeprom_read_byte((uint8_t*)EEP8_OVERLOAD_TIME);
}
void send_motor_settings() {
	// Save motor settings to EEPROM
	save_motor_settings_to_eeprom();
}
// Send drive motor settings when A1 goes into settings menu
void handle_settings_command(uint8_t identifier, uint8_t* data){
switch(identifier){
case DM_SETTINGS_CAN:
if (data[0] == REQUEST_DM_SETTINGS) {
	// Load motor settings from EEPROM
	load_motor_settings_from_eeprom();
	// Send all motor settings
	send_motor_settings();
}
else if (data[0] == UPDATE_DM_SETTINGS) {
	uint8_t i = 2;
	if (data[1] == DM_SETTINGS_1) {
		acceleration = data[i++];
		deceleration = data[i++];
		brake_delay = data[i++];
		reverse_speed = data[i++];
	}
	else if (data[1] == DM_SETTINGS_2) {
		slow_speed = data[i++];
		fast_speed = data[i++];
		rated_current = data[i++];
		max_current = data[i++];	
		full_overload_time = data[i++];
	}
	// Save updated settings to EEPROM
	save_motor_settings_to_eeprom();
	// Update motor settings
	update_motor_settings();
}
break;
}
}


void set_to_default(void)
{
	cli();
	eeprom_update_byte((uint8_t *)EEP8_VACUUM_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for vacuum motor
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_BRUSH_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for brush motor
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_PRE_SWP_SLOW_START, 12); //Number of 250 ms for rampup time ex. value for 5 = 250*5 = 1250 ms rampup time for preswp motor
	eeprom_busy_wait();

	//Set default battery type
	eeprom_update_byte((uint8_t *)EEP8_BATTERY_TYPE, LEAD);	//Lead battery type is the default battery type. Other types: AGM, GEL
	eeprom_busy_wait();

	//Read in the drive motor settings
	vacuum_slow_start = eeprom_read_byte((const uint8_t *)EEP8_VACUUM_SLOW_START);
	eeprom_busy_wait();
	brush_slow_start = eeprom_read_byte((const uint8_t *)EEP8_BRUSH_SLOW_START);
	eeprom_busy_wait();
	pre_swp_slow_start = eeprom_read_byte((const uint8_t *)EEP8_PRE_SWP_SLOW_START);
	eeprom_busy_wait();

	//Read in the battery type
	battery_type = eeprom_read_byte((const uint8_t *)EEP8_BATTERY_TYPE);
	eeprom_busy_wait();

	eeprom_read_block((void *)serial_num, (const void*)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
	eeprom_busy_wait();
	if (serial_num[4] != '_')
	{
		serial_num[4] = '_';
		serial_num[7] = '_';
		serial_num[9] = '_';
		serial_num[14] = '_';
		serial_num[SERIAL_NUM_LEN] = '\0';
		eeprom_update_block((const void*)serial_num, (void *)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}

	eeprom_read_block((void *)A1_serial_num, (const void*)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
	eeprom_busy_wait();
	if (A1_serial_num[0] != 'R')
	{
		A1_serial_num[0] = 'R';
		A1_serial_num[1] = 'V';
		A1_serial_num[A1_SERIAL_NUM_LEN] = '\0'; //IVA001: Fixed incorrect array index
		eeprom_update_block((const void*)A1_serial_num, (void *)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
		eeprom_busy_wait();
	}

	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_ACCELERATION, 30);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_DECELERATION, 8);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY, 12);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED, 40);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED, 100);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED, 60);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING, 30);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_MAX_CURRENT, 100);
	eeprom_busy_wait();
	eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME, 45);
	eeprom_busy_wait();

	acceleration = eeprom_read_byte((const uint8_t *)EEP8_ACCELERATION);
	eeprom_busy_wait();
	deceleration = eeprom_read_byte((const uint8_t *)EEP8_DECELERATION);
	eeprom_busy_wait();
	brake_delay = eeprom_read_byte((const uint8_t *)EEP8_BRAKE_DELAY);
	eeprom_busy_wait();
	reverse_speed = eeprom_read_byte((const uint8_t *)EEP8_REVERSE_SPEED);
	eeprom_busy_wait();
	fast_speed = eeprom_read_byte((const uint8_t *)EEP8_FAST_SPEED);
	eeprom_busy_wait();
	slow_speed = eeprom_read_byte((const uint8_t *)EEP8_SLOW_SPEED);
	eeprom_busy_wait();
	current_rating = eeprom_read_byte((const uint8_t *)EEP8_CURRENT_RATING);
	eeprom_busy_wait();
	max_current = eeprom_read_byte((const uint8_t *)EEP8_MAX_CURRENT);
	eeprom_busy_wait();
	overload_time = eeprom_read_byte((const uint8_t *)EEP8_OVERLOAD_TIME);
	eeprom_busy_wait();
	_delay_ms(100);
	uint8_t message[1];
	transmit_CAN(RESET_DEFAULT_CAN, 1, 0, message);
	_delay_ms(10);
}