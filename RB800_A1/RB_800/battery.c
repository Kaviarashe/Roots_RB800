/*
 * battery.c
 *
 * Created: 1/11/2016 10:44:30 AM
 *  Author: Anurag Vaddi Reddy
 */


#include "battery.h"

void initialize_ADC(void)
{
	//Setup battery variables
	overvoltage_error = FALSE;
	overvoltage_count = 0;
	battery_status = BATTERY_UNKNOWN;

	//Read lowest recorded battery level for this charge cycle
	cli();
	eeprom_busy_wait();
	min_battery_level = eeprom_read_byte((const uint8_t *)EEP8_BATTERY_LEVEL_ADDR);
	if (min_battery_level > 99)
	{
		min_battery_level = 99;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_LEVEL_ADDR, 99);
	}
	sei();
}

void check_battery_voltage(uint16_t total)
{
	//Convert total provided to voltage value
	float voltage = (total - 195.0) / 100.0;
	uint8_t temp_battery_level = 0;

	//If voltage is 30 3 times in a row, overvoltage error occurs
	if (voltage >= 30)
	{
		if (overvoltage_count++ > 3)
		{
			overvoltage_error = TRUE;

			send_alarm(OVERVOLTAGE);

			lcd_write_new_P(PSTR("E13 - OVERVOLTAGE"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("TURN OFF KEY"));
		}
	}
	else
	{
		overvoltage_count = 0;
	}

	//Get battery voltage percentage relative to battery type
	//Equations were created using excel and the max/min voltage of the battery types below
	switch (battery_type)
	{
	case LEAD:
		if (voltage >= 25)
			temp_battery_level = 99;
		else if (voltage <= 20.2)
			temp_battery_level = 0;
		else
			temp_battery_level = (uint8_t)(20.833*voltage - 420.83);
		break;

	case AGM:
		if (voltage >= 25.2)
			temp_battery_level = 99;
		else if (voltage <= 22.8)
			temp_battery_level = 0;
		else
			temp_battery_level = (uint8_t)(41.667*voltage - 950.00);
		break;

	case GEL:
		if (voltage >= 24.5)
			temp_battery_level = 99;
		else if (voltage <= 21.8)
			temp_battery_level = 0;
		else
			temp_battery_level = (uint8_t)(36.733*voltage - 799.27);
		break;

	default:
		temp_battery_level = 0;
	}

	//If the motors have been shut off due to low battery and the battery has reached maximum charge
	//Turn motors back on
	if (motors_status == MOTORS_OFF && temp_battery_level == 99)
	{
		cli();
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_MOTORS_STATUS_ADDR, (uint8_t)MOTORS_ON);
		min_battery_level = 99;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_LEVEL_ADDR, 99);
		motors_status = MOTORS_ON;
		sei();
	}

	battery_level = temp_battery_level;

	//Compare battery level to minimum recorded battery level
	if (battery_level < min_battery_level)
	{
		min_battery_level = battery_level;
		cli();
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_LEVEL_ADDR, min_battery_level);
		sei();
	}
	//If large loads are running and battery level has significantly increased we can say that the minimum battery level has increased
	if (brush_state == BRUSH_RUNNING && vacuum_state == VACUUM_RUNNING && battery_level > (min_battery_level + BATTERY_LEVEL_HYSTERESIS))
	{
		min_battery_level = battery_level;
		cli();
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_LEVEL_ADDR, min_battery_level);
		sei();
	}
	if (min_battery_level < (99 - BATTERY_LEVEL_HYSTERESIS) && battery_level == 99)
	{
		min_battery_level = battery_level;
		cli();
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_LEVEL_ADDR, min_battery_level);
		sei();
	}

	//Check if battery status has changed and update accordingly 
	if (min_battery_level > 10 && battery_status != BATTERY_GOOD)
	{
		if (battery_status != BATTERY_LOW_CHECK && battery_status != BATTERY_OFF_CHECK)
		{
			LCD_LED_ON;
		}
		battery_status = BATTERY_GOOD;
	}
	else if (min_battery_level <= 10 && min_battery_level != 0 && battery_status != BATTERY_LOW && battery_status != BATTERY_LOW_CHECK)
	{
		battery_status = BATTERY_LOW_CHECK;
	}
	else if (min_battery_level <= 10 && min_battery_level != 0 && battery_status != BATTERY_LOW && battery_status == BATTERY_LOW_CHECK)
	{
		battery_status = BATTERY_LOW;
	}
	else if (min_battery_level == 0 && battery_status != BATTERY_OFF && battery_status != BATTERY_OFF_CHECK)
	{
		battery_status = BATTERY_OFF_CHECK;
	}
	else if (min_battery_level == 0 && battery_status != BATTERY_OFF && battery_status == BATTERY_OFF_CHECK)
	{
		battery_status = BATTERY_OFF;
	}
}

//Request for battery voltage from A2
void request_battery_can(void)
{
	uint8_t message[1];
	transmit_CAN(BATTERY_CAN, 0, 0, message);
	_delay_ms(10);
	//_delay_ms(20);
}