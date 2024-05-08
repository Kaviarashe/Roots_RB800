/*
 * settings_menu.c
 *
 * Created: 9/17/2015 10:52:46 AM
 *  Author: Anurag Vaddi Reddy
 */


#include "settings_menu.h"

 //Variables that can be used by the menu
char serial_num_temp[SERIAL_NUM_LEN + 1];
uint16_t run_time_hour;
char run_time_hour_arr[6];
uint8_t run_time_min;
char run_time_min_arr[3];
uint8_t cursor;
char temp[21];

void menu_next(void)
{
	switch (current_menu)
	{
		//Any button press will move pre-menu to the motor menu
	case PRE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/******************************************************************************************************************************/

		//In the motor menu users can adjust motor values
	case MOTOR_MENU:
		current_menu = ACCELERATION_MENU;
		lcd_write_new_P(PSTR("ACCELERATION"));
		break;

		/*-------------------------------------------------------------------------------*/

		//Time in seconds to zero acceleration to max acceleration
	case ACCELERATION_MENU:
		current_menu = ACCELERATION_VAL_MENU;
		current_value = acceleration;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

		//Time in seconds from max acceleration to zero acceleration
	case DECELERATION_MENU:
		current_menu = DECELERATION_VAL_MENU;
		current_value = deceleration;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

		//Time in seconds from pressing brake pedal to braking
	case BRAKE_DELAY_MENU:
		current_menu = BRAKE_DELAY_VAL_MENU;
		current_value = brake_delay;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

		//Two forward speed options available: slow and fast
	case FORWARD_SPEED_MENU:
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

		/*-------------------------------------------------------------------------------*/

		//% of max speed when in slow mode (tortoise)
	case SLOW_SPEED_MENU:
		current_menu = SLOW_SPEED_VAL_MENU;
		current_value = slow_speed;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

		//% of max speed when in fast mode (hare)
	case FAST_SPEED_MENU:
		current_menu = FAST_SPEED_VAL_MENU;
		current_value = fast_speed;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

		/*-------------------------------------------------------------------------------*/

		//% of max speed allowed when in reverse
	case REVERSE_SPEED_MENU:
		current_menu = REVERSE_SPEED_VAL_MENU;
		current_value = reverse_speed;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

		//Rated continuous current in amps
	case CURRENT_RATING_MENU:
		current_menu = CURRENT_RATING_VAL_MENU;
		current_value = current_rating;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

		//Max continuous current in amps
	case MAX_CURRENT_MENU:
		current_menu = MAX_CURRENT_VAL_MENU;
		current_value = max_current;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

		//Number seconds to allow motor to run above rated current before shutting off
	case OVERLOAD_TIME_MENU:
		current_menu = OVERLOAD_TIME_VAL_MENU;
		current_value = overload_time;
		sprintf(temp, "%d SECONDS", current_value);
		lcd_write_new(temp);
		break;

		/*--------------------------------------------------------------------------------*/

		//Choose type of pot and calibrate the pot
	case POTENTIOMETER_MENU:
		current_menu = POT_CALIBRATION_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CALIBRATION"));
		break;

		//Choose the type of pot being used
	case POT_TYPE_MENU:
		current_menu = POT_TYPE_VAL_MENU;
		lcd_write_new_P(PSTR("NOT YET IMPLEMENTED"));
		break;

	case POT_TYPE_VAL_MENU:
		break;

		//Display current calibration values, press next to recalibrate pot
	case POT_CALIBRATION_MENU:
		current_menu = POT_CALIBRATION_VAL_MENU;
		sprintf(temp, "LOW=%d.%03d|HIGH=%d.%03d", pot_low_deadband / 1000, pot_low_deadband % 1000, pot_high_deadband / 1000, pot_high_deadband % 1000);
		lcd_write_new(temp);
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("NEXT TO RECALIBRATE"));
		break;

	case POT_CALIBRATION_VAL_MENU:
		current_menu = POT_RECALIBRATION_MENU;
		lcd_write_new_P(PSTR("PRESS PEDAL UNTIL"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("PEDAL SWITCH CLOSES"));
		pot_calibrating = POT_CALIBRATING_LOW;
		send_pot_calibrating();
		break;

		//Begin recalibration
	case POT_RECALIBRATION_MENU:
		break;

		/*--------------------------------------------------------------------------------*/

		//Enter vacuum settings menu
	case VACUUM_SETTINGS_MENU:
		current_menu = VACUUM_SLOW_START_MENU;
		lcd_write_new_P(PSTR("VACUUM SLOW START"));
		break;

		//Choose rampup time for vacuum on
	case VACUUM_SLOW_START_MENU:
		current_menu = VACUUM_SLOW_START_VAL_MENU;
		current_value = vacuum_slow_start;
		if (vacuum_slow_start == FALSE)
			lcd_write_new_P(PSTR("OFF"));
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

		//Enter brush settings menu
	case BRUSH_SETTINGS_MENU:
		current_menu = BRUSH_SLOW_START_MENU;
		lcd_write_new_P(PSTR("BRUSH SLOW START"));
		break;

		//Choose rampup time for brush on
	case BRUSH_SLOW_START_MENU:
		current_menu = BRUSH_SLOW_START_VAL_MENU;
		current_value = brush_slow_start;
		if (brush_slow_start == FALSE)
			lcd_write_new_P(PSTR("OFF"));
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

		//Enter brush settings menu
	case PRE_SWP_SETTINGS_MENU:
		current_menu = PRE_SWP_SLOW_START_MENU;
		lcd_write_new_P(PSTR("PRE-SWEEP SLOW START"));
		break;

		//Choose rampup time for brush on
	case PRE_SWP_SLOW_START_MENU:
		current_menu = PRE_SWP_SLOW_START_VAL_MENU;
		current_value = pre_swp_slow_start;
		if (pre_swp_slow_start == FALSE)
			lcd_write_new_P(PSTR("OFF"));
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

		/*-------------------------------------------------------------------------------*/

		//This part of the menu allows the actual value to be changed
	case ACCELERATION_VAL_MENU:
		acceleration = current_value;
		//update_DM_settings();
		update_DM_accel();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_ACCELERATION, current_value);
		lcd_write_new_P(PSTR("ACCELERATION SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = ACCELERATION_MENU;
		lcd_write_new_P(PSTR("ACCELERATION"));
		break;

	case DECELERATION_VAL_MENU:
		deceleration = current_value;
		//update_DM_settings();
		update_DM_deaccel();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_DECELERATION, current_value);
		lcd_write_new_P(PSTR("DECELERATION SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = DECELERATION_MENU;
		lcd_write_new_P(PSTR("DECELERATION"));
		break;

	case BRAKE_DELAY_VAL_MENU:
		brake_delay = current_value;
		//update_DM_settings();
		update_DM_braekdelay();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_BRAKE_DELAY, current_value);
		lcd_write_new_P(PSTR("BRAKE DELAY SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = BRAKE_DELAY_MENU;
		lcd_write_new_P(PSTR("BRAKE DELAY"));
		break;

	case REVERSE_SPEED_VAL_MENU:
		reverse_speed = current_value;
		update_DM_reverse_speed();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_REVERSE_SPEED, current_value);
		lcd_write_new_P(PSTR("REVERSE SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;

	case SLOW_SPEED_VAL_MENU:
		slow_speed = current_value;
		update_DM_slow_speed();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_SLOW_SPEED, current_value);
		lcd_write_new_P(PSTR("SLOW SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

	case FAST_SPEED_VAL_MENU:
		fast_speed = current_value;
		update_DM_fast_speed();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_FAST_SPEED, current_value);
		lcd_write_new_P(PSTR("FAST SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

	case CURRENT_RATING_VAL_MENU:
		current_rating = current_value;
		update_DM_CURRENT_RATING();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING, current_value);
		lcd_write_new_P(PSTR("CURRENT RATING SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = CURRENT_RATING_MENU;
		lcd_write_new_P(PSTR("CURRENT RATING"));
		break;

	case MAX_CURRENT_VAL_MENU:
		max_current = current_value;
		update_DM_MAX_CURRENT();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_CURRENT_RATING, current_value);
		lcd_write_new_P(PSTR("MAX CURRENT SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("MAX CURRENT"));
		break;

	case OVERLOAD_TIME_VAL_MENU:
		overload_time = current_value;
		update_DM_OVER_LOAD_TIME();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_OVERLOAD_TIME, current_value);
		lcd_write_new_P(PSTR("OVERLOAD TIME SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = OVERLOAD_TIME_MENU;
		lcd_write_new_P(PSTR("OVERLOAD TIME"));
		break;

	case VACUUM_SLOW_START_VAL_MENU:
		vacuum_slow_start = current_value;
		update_A2_vaccum_slow_start();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_VACUUM_SLOW_START, current_value);
		lcd_write_new_P(PSTR("VACUUM SLOW START"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = VACUUM_SLOW_START_MENU;
		lcd_write_new_P(PSTR("VACUUM SLOW START"));
		break;

	case BRUSH_SLOW_START_VAL_MENU:
		brush_slow_start = current_value;
		update_A2_brush_slow_start();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_BRUSH_SLOW_START, current_value);
		lcd_write_new_P(PSTR("BRUSH SLOW START"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = BRUSH_SLOW_START_MENU;
		lcd_write_new_P(PSTR("BRUSH SLOW START"));
		break;

	case PRE_SWP_SLOW_START_VAL_MENU:
		pre_swp_slow_start = current_value;
		update_A2_presweep_slow_start();
		_delay_ms(100);
		eeprom_update_byte((uint8_t *)EEP8_PRE_SWP_SLOW_START, current_value);
		lcd_write_new_P(PSTR("PRE-SWEEP SLOW START"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = PRE_SWP_SLOW_START_MENU;
		lcd_write_new_P(PSTR("PRE-SWEEP SLOW START"));
		break;


		/******************************************************************************************************************************/
		//Three battery types are available to accurately determine battery capacity	
	case BATTERY_MENU:
		if (battery_type == LEAD)
		{
			current_menu = LEAD_MENU;
			lcd_write_new_P(PSTR("BATTERY: LEAD"));
		}
		else if (battery_type == AGM)
		{
			current_menu = AGM_MENU;
			lcd_write_new_P(PSTR("BATTERY: AGM"));
		}
		else
		{
			current_menu = GEL_MENU;
			lcd_write_new_P(PSTR("BATTERY: GEL"));
		}
		break;

		/*-------------------------------------------------------------------------------*/

		//This part of the menu allows the battery type to be set

	case LEAD_MENU:
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_TYPE, LEAD);
		battery_type = LEAD;
		lcd_write_new_P(PSTR("BATTERY TYPE SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

	case AGM_MENU:
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_TYPE, AGM);
		battery_type = AGM;
		lcd_write_new_P(PSTR("BATTERY TYPE SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

	case GEL_MENU:
		eeprom_update_byte((uint8_t *)EEP8_BATTERY_TYPE, GEL);
		battery_type = GEL;
		lcd_write_new_P(PSTR("BATTERY TYPE SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

		/******************************************************************************************************************************/
		//The serial number is 16 digits long and can be changed through the menu
	case SET_SERIAL_NUM_MENU:
		current_menu = SET_RB800_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET RB800 SERIAL NUM"));
		break;

	case SET_RB800_SERIAL_NUM_MENU:
		current_menu = SET_RB800_SERIAL_NUM_VAL_MENU;
		lcd_write_new_P(PSTR("SET RB800 SERIAL NUM"));
		lcd_clear_screen();
		strncpy((char *)serial_num_temp, (const char*)serial_num, SERIAL_NUM_LEN + 1);
		lcd_set_cursor(0, 2);
		lcd_write((char *)serial_num_temp);
		lcd_set_cursor(0, 2);
		lcd_cursor_on();
		cursor = 2;
		break;

	case SET_A1_SERIAL_NUM_MENU:
		current_menu = SET_A1_SERIAL_NUM_VAL_MENU;
		lcd_write_new_P(PSTR("SET A1 SERIAL NUM"));
		strncpy((char *)serial_num_temp, (const char*)A1_serial_num, A1_SERIAL_NUM_LEN + 1);
		lcd_write_new((char *)serial_num_temp);
		lcd_set_cursor(0, 2);
		lcd_cursor_on();
		cursor = 2;
		break;

	case SET_A2_SERIAL_NUM_MENU:
		current_menu = SET_A2_SERIAL_NUM_VAL_MENU;
		lcd_write_new_P(PSTR("SET A2 SERIAL NUM"));
		strncpy((char *)serial_num_temp, (const char*)A2_serial_num, A1_SERIAL_NUM_LEN + 1);
		lcd_write_new((char *)serial_num_temp);
		lcd_set_cursor(0, 2);
		lcd_cursor_on();
		cursor = 2;
		break;

	case SET_A3_SERIAL_NUM_MENU:
		current_menu = SET_A3_SERIAL_NUM_VAL_MENU;
		lcd_write_new_P(PSTR("SET A3 SERIAL NUM"));
		strncpy((char *)serial_num_temp, (const char*)A3_serial_num, A1_SERIAL_NUM_LEN + 1);
		lcd_write_new((char *)serial_num_temp);
		lcd_set_cursor(0, 2);
		lcd_cursor_on();
		cursor = 2;
		break;

		/*-------------------------------------------------------------------------------*/

		//This part of the menu displays the serial number and allows the number to be changed

	case SET_RB800_SERIAL_NUM_VAL_MENU:
		if (cursor == (SERIAL_NUM_LEN - 1) + 2)
		{
			if (!admin_mode)
				break;
			for (uint8_t i = 0; i < SERIAL_NUM_LEN; i++)
			{
				eeprom_update_byte((uint8_t *)EEPxx_SERIAL_NUM + i, serial_num_temp[i]);
			}
			lcd_write_new_P(PSTR("SERIAL NUMBER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("CHANGED"));
			_delay_ms(2000);
			clear_switch_pressed();
			eeprom_read_block((void *)serial_num, (const void*)EEPxx_SERIAL_NUM, SERIAL_NUM_LEN);
			serial_num[SERIAL_NUM_LEN] = '\0';
			current_menu = SET_RB800_SERIAL_NUM_VAL_MENU;
			lcd_clear_screen();
			strncpy((char *)serial_num_temp, (const char*)serial_num, SERIAL_NUM_LEN + 1); //IVA001: fixed memory corruption typo
			lcd_set_cursor(0, 2);
			lcd_write((char *)serial_num_temp);
			lcd_set_cursor(0, 2);
			lcd_cursor_on();
			cursor = 2;
		}
		else
		{
			cursor++;
			switch (cursor)
			{
			case 6:
			case 9:
			case 11:
			case 16:
				cursor++;
				break;
			default:
				break;
			}
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A1_SERIAL_NUM_VAL_MENU:
		if (cursor == (A1_SERIAL_NUM_LEN - 1))
		{
			if (!admin_mode)
				break;
			for (uint8_t i = 0; i < A1_SERIAL_NUM_LEN; i++)
			{
				eeprom_update_byte((uint8_t *)EEPxx_A1_SERIAL_NUM + i, serial_num_temp[i]);
			}
			lcd_write_new_P(PSTR("SERIAL NUMBER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("CHANGED"));
			_delay_ms(2000);
			clear_switch_pressed();
			eeprom_read_block((void *)A1_serial_num, (const void*)EEPxx_A1_SERIAL_NUM, A1_SERIAL_NUM_LEN);
			A1_serial_num[A1_SERIAL_NUM_LEN] = '\0';
			current_menu = SET_A1_SERIAL_NUM_VAL_MENU;
			strncpy((char *)serial_num_temp, (const char*)A1_serial_num, SERIAL_NUM_LEN + 1);
			lcd_write_new((char *)serial_num_temp);
			lcd_set_cursor(0, 2);
			lcd_cursor_on();
			cursor = 2;
		}
		else
		{
			cursor++;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor++;
					break;
				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A2_SERIAL_NUM_VAL_MENU:
		if (cursor == (A1_SERIAL_NUM_LEN - 1))
		{
			if (!admin_mode)
				break;
			send_serial_num(A2_CAN, serial_num_temp);
			request_A2_serial_num();
			lcd_write_new_P(PSTR("SERIAL NUMBER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("CHANGED"));
			cursor = 2;
			while (cursor < 20)
			{
				if (!CQRx_queueIsEmpty())
				{
					CQRx_dequeue();
					reset_CAN_error();
				}
				else
				{
					_delay_ms(100);
					cursor++;
				}
			}
			clear_switch_pressed();
			current_menu = SET_A2_SERIAL_NUM_VAL_MENU;
			strncpy((char *)serial_num_temp, (const char*)A2_serial_num, A1_SERIAL_NUM_LEN + 1);
			lcd_write_new((char *)serial_num_temp);
			lcd_set_cursor(0, 2);
			lcd_cursor_on();
			cursor = 2;
		}
		else
		{
			cursor++;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor++;
					break;
				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A3_SERIAL_NUM_VAL_MENU:
		if (cursor == (A1_SERIAL_NUM_LEN - 1))
		{
			if (!admin_mode)
				break;
			send_serial_num(A3_CAN, serial_num_temp);
			_delay_ms(50);
			request_A3_serial_num();
			lcd_write_new_P(PSTR("SERIAL NUMBER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("CHANGED"));
			cursor = 2;
			while (cursor < 20)
			{
				if (!CQRx_queueIsEmpty())
				{
					CQRx_dequeue();
					reset_CAN_error();
				}
				else
				{
					_delay_ms(100);
					cursor++;
				}
			}
			clear_switch_pressed();
			current_menu = SET_A3_SERIAL_NUM_VAL_MENU;
			strncpy((char *)serial_num_temp, (const char*)A3_serial_num, A1_SERIAL_NUM_LEN + 1);
			lcd_write_new((char *)serial_num_temp);
			lcd_set_cursor(0, 2);
			lcd_cursor_on();
			cursor = 2;
		}
		else
		{
			cursor++;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor++;
					break;
				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

		/******************************************************************************************************************************/
		//The run time shows the number of hours and minutes the RB800 has been running
		//It can be changed in case of board failure
	case RUN_TIME_MENU:
		current_menu = RUN_TIME_VAL_MENU;
		run_time_hour = hours;
		run_time_min = minutes;
		for (int8_t i = 4; i >= 0; i--)
		{
			run_time_hour_arr[i] = (run_time_hour % 10) + ASCII_NUM;
			run_time_hour /= 10;
		}
		run_time_hour_arr[5] = '\0';
		for (int8_t i = 1; i >= 0; i--)
		{
			run_time_min_arr[i] = (run_time_min % 10) + ASCII_NUM;
			run_time_min /= 10;
		}
		run_time_min_arr[2] = '\0';
		sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
		lcd_write_new(temp);
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("HOURS:MINUTES"));
		lcd_set_cursor(0, 0);
		lcd_cursor_on();
		cursor = 0;
		break;

		/*-------------------------------------------------------------------------------*/
		//This part of the menu displays the run time and allows it to be changed
	case RUN_TIME_VAL_MENU:
		if (cursor < 6)
		{
			cursor++;
			if (cursor < 5)
				lcd_set_cursor(0, cursor);
			else
				lcd_set_cursor(0, cursor + 1);
		}
		else
		{
			if (!admin_mode)
				break;
			run_time_hour = 0;
			run_time_hour += (run_time_hour_arr[0] - ASCII_NUM) * 10000;
			run_time_hour += (run_time_hour_arr[1] - ASCII_NUM) * 1000;
			run_time_hour += (run_time_hour_arr[2] - ASCII_NUM) * 100;
			run_time_hour += (run_time_hour_arr[3] - ASCII_NUM) * 10;
			run_time_hour += (run_time_hour_arr[4] - ASCII_NUM) * 1;
			hours = run_time_hour;

			run_time_min = 0;
			run_time_min += (run_time_min_arr[0] - ASCII_NUM) * 10;
			run_time_min += (run_time_min_arr[1] - ASCII_NUM) * 1;
			minutes = run_time_min;

			cli();
			write_hours();
			write_minutes();
			sei();

			lcd_write_new_P(PSTR("RUN TIME CHANGED"));
			_delay_ms(2000);
			clear_switch_pressed();

			current_menu = RUN_TIME_VAL_MENU;
			run_time_hour = hours;
			run_time_min = minutes;
			for (int8_t i = 4; i >= 0; i--)
			{
				run_time_hour_arr[i] = (run_time_hour % 10) + ASCII_NUM;
				run_time_hour /= 10;
			}
			run_time_hour_arr[5] = '\0';
			for (int8_t i = 1; i >= 0; i--)
			{
				run_time_min_arr[i] = (run_time_min % 10) + ASCII_NUM;
				run_time_min /= 10;
			}
			run_time_min_arr[2] = '\0';
			sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
			lcd_write_new(temp);
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("HOURS:MINUTES"));
			lcd_set_cursor(0, 0);
			lcd_cursor_on();
			cursor = 0;
		}
		break;

		/******************************************************************************************************************************/

	case STATISTICS_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_STAT_MENU:
		current_menu = BRUSH_HOURS_MENU;
		lcd_write_new_P(PSTR("BRUSH HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush_hours);
		lcd_write(temp);
		break;

	case VACUUM_STAT_MENU:
		current_menu = VACUUM_HOURS_MENU;
		lcd_write_new_P(PSTR("VACUUM HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", vacuum_hours);
		lcd_write(temp);
		break;

	case PRESWEEP_STAT_MENU:
		current_menu = PRESWEEP_HOURS_MENU;
		lcd_write_new_P(PSTR("PRESWEEP HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", presweep_hours);
		lcd_write(temp);
		break;

	case WATER_PUMP_STAT_MENU:
		current_menu = WATER_PUMP_HOURS_MENU;
		lcd_write_new_P(PSTR("WATER PUMP HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", water_pump_hours);
		lcd_write(temp);
		break;

	case DETERGENT_PUMP_STAT_MENU:
		current_menu = DETERGENT_PUMP_HOURS_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", detergent_pump_hours);
		lcd_write(temp);
		break;

		/******************************************************************************************************************************/

	case DEFAULT_MENU:
		current_menu = DEFAULT_CONFIRM_MENU;
		lcd_write_new_P(PSTR("CONFIRM RESET"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("DOWN TO CONFIRM"));
		break;

		/*-------------------------------------------------------------------------------*/

	case DEFAULT_CONFIRM_MENU:
		lcd_write_new_P(PSTR("RESET OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

		/******************************************************************************************************************************/

	case CLEAR_SD_CARD_MENU:
		current_menu = CLEAR_SD_CARD_CONFIRM_MENU;
		lcd_write_new_P(PSTR("CONFIRM CLEAR"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("DOWN TO CONFIRM"));
		break;

	case CLEAR_SD_CARD_CONFIRM_MENU:
		lcd_write_new_P(PSTR("CLEAR OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = CLEAR_SD_CARD_MENU;
		lcd_write_new_P(PSTR("CLEAR SD CARD"));
		break;

		/******************************************************************************************************************************/

	case ABOUT_MENU:
		current_menu = RB800_MENU;
		lcd_write_new_P(PSTR("ABOUT RB800"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_MENU:
		current_menu = RB800_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("RB800 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(serial_num);
		break;

	case A1_MENU:
		current_menu = A1_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A1 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A1_serial_num);
		break;

	case A2_MENU:
		current_menu = A2_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A2 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A2_serial_num);
		break;

	case A3_MENU:
		current_menu = A3_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A3 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A3_serial_num);
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_SERIAL_NUM_MENU:
		break;

	case A1_SERIAL_NUM_MENU:
		break;

	case A1_PCB_MENU:
		break;

	case A1_FIRMWARE_MENU:
		break;

	case A2_SERIAL_NUM_MENU:
		break;

	case A2_PCB_MENU:
		break;

	case A2_FIRMWARE_MENU:
		break;

	case A3_SERIAL_NUM_MENU:
		break;

	case A3_PCB_MENU:
		break;

	case A3_FIRMWARE_MENU:
		break;

		/******************************************************************************************************************************/

	default: break;
	}
}

void menu_back(void)
{
	switch (current_menu)
	{
	case PRE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case DECELERATION_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case BRAKE_DELAY_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case FORWARD_SPEED_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case SLOW_SPEED_MENU:
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;

	case FAST_SPEED_MENU:
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;

	case REVERSE_SPEED_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case CURRENT_RATING_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case MAX_CURRENT_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case OVERLOAD_TIME_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/*--------------------------------------------------------------------------------*/

		//Choose type of pot and calibrate the pot
	case POTENTIOMETER_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		//Choose the type of pot being used
	case POT_TYPE_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
		break;

	case POT_TYPE_VAL_MENU:
		current_menu = POT_TYPE_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER TYPE"));
		break;

	case POT_CALIBRATION_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
		break;

	case POT_CALIBRATION_VAL_MENU:
		current_menu = POT_CALIBRATION_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CALIBRATION"));
		break;


	case POT_RECALIBRATION_MENU:
		lcd_write_new_P(PSTR("RECALIBRATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		pot_calibrating = POT_CALIBRATING_CANCELLED;
		send_pot_calibrating();
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = POT_CALIBRATION_VAL_MENU;
		sprintf(temp, "LOW=%d.%03d|HIGH=%d.%03d", pot_low_deadband / 1000, pot_low_deadband % 1000, pot_high_deadband / 1000, pot_high_deadband % 1000);
		lcd_write_new(temp);
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("NEXT TO RECALIBRATE"));
		break;

		/*--------------------------------------------------------------------------------*/

	case VACUUM_SETTINGS_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case VACUUM_SLOW_START_MENU:
		current_menu = VACUUM_SETTINGS_MENU;
		lcd_write_new_P(PSTR("VACUUM SETTINGS"));
		break;

	case BRUSH_SETTINGS_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case BRUSH_SLOW_START_MENU:
		current_menu = BRUSH_SETTINGS_MENU;
		lcd_write_new_P(PSTR("BRUSH SETTINGS"));
		break;

	case PRE_SWP_SETTINGS_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case PRE_SWP_SLOW_START_MENU:
		current_menu = PRE_SWP_SETTINGS_MENU;
		lcd_write_new_P(PSTR("PRE-SWEEP SETTINGS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_VAL_MENU:
		current_menu = ACCELERATION_MENU;
		lcd_write_new_P(PSTR("ACCELERATION"));
		break;

	case DECELERATION_VAL_MENU:
		current_menu = DECELERATION_MENU;
		lcd_write_new_P(PSTR("DECELERATION"));
		break;

	case BRAKE_DELAY_VAL_MENU:
		current_menu = BRAKE_DELAY_MENU;
		lcd_write_new_P(PSTR("BRAKE DELAY"));
		break;

	case REVERSE_SPEED_VAL_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;

	case SLOW_SPEED_VAL_MENU:
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

	case FAST_SPEED_VAL_MENU:
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

	case CURRENT_RATING_VAL_MENU:
		current_menu = CURRENT_RATING_MENU;
		lcd_write_new_P(PSTR("CURRENT RATING"));
		break;

	case MAX_CURRENT_VAL_MENU:
		current_menu = MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("MAX CURRENT"));
		break;

	case OVERLOAD_TIME_VAL_MENU:
		current_menu = OVERLOAD_TIME_MENU;
		lcd_write_new_P(PSTR("OVERLOAD TIME"));
		break;

	case VACUUM_SLOW_START_VAL_MENU:
		current_menu = VACUUM_SLOW_START_MENU;
		lcd_write_new_P(PSTR("VACUUM SLOW START"));
		break;

	case BRUSH_SLOW_START_VAL_MENU:
		current_menu = BRUSH_SLOW_START_MENU;
		lcd_write_new_P(PSTR("BRUSH SLOW START"));
		break;

	case PRE_SWP_SLOW_START_VAL_MENU:
		current_menu = PRE_SWP_SLOW_START_MENU;
		lcd_write_new_P(PSTR("PRE-SWEEP SLOW START"));
		break;

		/******************************************************************************************************************************/

	case BATTERY_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case LEAD_MENU:
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

	case AGM_MENU:
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

	case GEL_MENU:
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

		/******************************************************************************************************************************/

	case STATISTICS_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_STAT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case VACUUM_STAT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case PRESWEEP_STAT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case WATER_PUMP_STAT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case DETERGENT_PUMP_STAT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case ECO_HOURS_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

	case WATER_PUMP_HOURS_MENU:
		current_menu = WATER_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;

	case DETERGENT_PUMP_HOURS_MENU:
		current_menu = DETERGENT_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;


		/*-------------------------------------------------------------------------------*/

	case BRUSH_HOURS_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

	case BRUSH1_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

	case BRUSH1_MAX_CURRENT_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

	case BRUSH2_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

	case BRUSH2_MAX_CURRENT_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));;
		break;

		/*-------------------------------------------------------------------------------*/

	case VACUUM_HOURS_MENU:
		current_menu = VACUUM_STAT_MENU;
		lcd_write_new_P(PSTR("VACUUM STATISTICS"));
		break;

	case VACUUM_AVERAGE_CURRENT_MENU:
		current_menu = VACUUM_STAT_MENU;
		lcd_write_new_P(PSTR("VACUUM STATISTICS"));
		break;

	case VACUUM_MAX_CURRENT_MENU:
		current_menu = VACUUM_STAT_MENU;
		lcd_write_new_P(PSTR("VACUUM STATISTICS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case PRESWEEP_HOURS_MENU:
		current_menu = PRESWEEP_STAT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP STATISTICS"));
		break;

	case PRESWEEP_AVERAGE_CURRENT_MENU:
		current_menu = PRESWEEP_STAT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP STATISTICS"));
		break;

	case PRESWEEP_MAX_CURRENT_MENU:
		current_menu = PRESWEEP_STAT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP STATISTICS"));
		break;

		/******************************************************************************************************************************/

	case DEFAULT_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case DEFAULT_CONFIRM_MENU:
		lcd_write_new_P(PSTR("RESET OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

		/******************************************************************************************************************************/

	case CLEAR_SD_CARD_MENU:
		break;

	case CLEAR_SD_CARD_CONFIRM_MENU:
		lcd_write_new_P(PSTR("CLEAR OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = CLEAR_SD_CARD_MENU;
		lcd_write_new_P(PSTR("CLEAR SD CARD"));
		break;

		/******************************************************************************************************************************/

	case SET_SERIAL_NUM_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case SET_RB800_SERIAL_NUM_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		lcd_cursor_off();
		break;
	case SET_A1_SERIAL_NUM_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		lcd_cursor_off();
		break;
	case SET_A2_SERIAL_NUM_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		lcd_cursor_off();
		break;
	case SET_A3_SERIAL_NUM_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		lcd_cursor_off();
		break;
		/*if(cursor == 2)
		{
			current_menu = SET_SERIAL_NUM_MENU;
			lcd_write_new_P(PSTR("SET SERIAL NUM"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			switch(cursor)
			{
				case 6:
				case 9:
				case 11:
				case 16:
					cursor--;
					break;
				default:
					break;
			}
			lcd_set_cursor(0, cursor);
		}
		break;*/

	case SET_RB800_SERIAL_NUM_VAL_MENU:
		if (cursor == 2)
		{
			current_menu = SET_RB800_SERIAL_NUM_MENU;
			lcd_write_new_P(PSTR("SET RB800 SERIAL NUM"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			switch (cursor)
			{
			case 6:
			case 9:
			case 11:
			case 16:
				cursor--;
				break;

			default:
				break;
			}
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A1_SERIAL_NUM_VAL_MENU:
		if (cursor == 2)
		{
			current_menu = SET_A1_SERIAL_NUM_MENU;
			lcd_write_new_P(PSTR("SET A1 SERIAL NUM"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor--;
					break;

				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A2_SERIAL_NUM_VAL_MENU:
		if (cursor == 2)
		{
			current_menu = SET_A2_SERIAL_NUM_MENU;
			lcd_write_new_P(PSTR("SET A2 SERIAL NUM"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor--;
					break;

				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

	case SET_A3_SERIAL_NUM_VAL_MENU:
		if (cursor == 2)
		{
			current_menu = SET_A3_SERIAL_NUM_MENU;
			lcd_write_new_P(PSTR("SET A3 SERIAL NUM"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			/*switch(cursor)
			{
				case 4:
				case 7:
				case 9:
				case 14:
					cursor--;
					break;

				default:
					break;
			}*/
			lcd_set_cursor(0, cursor);
		}
		break;

		/******************************************************************************************************************************/

	case RUN_TIME_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case RUN_TIME_VAL_MENU:
		if (cursor == 0)
		{
			current_menu = RUN_TIME_MENU;
			lcd_write_new_P(PSTR("RUN TIME"));
			lcd_cursor_off();
		}
		else
		{
			cursor--;
			if (cursor < 5)
			{
				lcd_set_cursor(0, cursor);
			}
			else
			{
				lcd_set_cursor(0, cursor + 1);
			}
		}
		break;


		/******************************************************************************************************************************/

	case ABOUT_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

	case A1_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

	case A2_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

	case A3_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_SERIAL_NUM_MENU:
		current_menu = RB800_MENU;
		lcd_write_new_P(PSTR("ABOUT RB800"));
		break;

	case A1_SERIAL_NUM_MENU:
		current_menu = A1_MENU;
		lcd_write_new_P(PSTR("ABOUT A1"));
		break;

	case A1_PCB_MENU:
		current_menu = A1_MENU;
		lcd_write_new_P(PSTR("ABOUT A1"));
		break;

	case A1_FIRMWARE_MENU:
		current_menu = A1_MENU;
		lcd_write_new_P(PSTR("ABOUT A1"));
		break;

	case A2_SERIAL_NUM_MENU:
		current_menu = A2_MENU;
		lcd_write_new_P(PSTR("ABOUT A2"));
		break;

	case A2_PCB_MENU:
		current_menu = A2_MENU;
		lcd_write_new_P(PSTR("ABOUT A2"));
		break;

	case A2_FIRMWARE_MENU:
		current_menu = A2_MENU;
		lcd_write_new_P(PSTR("ABOUT A2"));
		break;

	case A3_SERIAL_NUM_MENU:
		current_menu = A3_MENU;
		lcd_write_new_P(PSTR("ABOUT A3"));
		break;

	case A3_PCB_MENU:
		current_menu = A3_MENU;
		lcd_write_new_P(PSTR("ABOUT A3"));
		break;

	case A3_FIRMWARE_MENU:
		current_menu = A3_MENU;
		lcd_write_new_P(PSTR("ABOUT A3"));
		break;

		/******************************************************************************************************************************/
	default: break;
	}

}

void menu_up(void)
{
	switch (current_menu)
	{
	case PRE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_MENU:
		break;

	case DECELERATION_MENU:
		current_menu = ACCELERATION_MENU;
		lcd_write_new_P(PSTR("ACCELERATION"));
		break;

	case BRAKE_DELAY_MENU:
		current_menu = DECELERATION_MENU;
		lcd_write_new_P(PSTR("DECELERATION"));
		break;

	case FORWARD_SPEED_MENU:
		current_menu = BRAKE_DELAY_MENU;
		lcd_write_new_P(PSTR("BRAKE DELAY"));
		break;

	case SLOW_SPEED_MENU:
		break;

	case FAST_SPEED_MENU:
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

	case REVERSE_SPEED_MENU:
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;

	case CURRENT_RATING_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;

	case MAX_CURRENT_MENU:
		current_menu = CURRENT_RATING_MENU;
		lcd_write_new_P(PSTR("CURRENT RATING"));
		break;

	case OVERLOAD_TIME_MENU:
		current_menu = MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("MAX CURRENT"));
		break;

		/*--------------------------------------------------------------------------------*/

		//Choose type of pot and calibrate the pot
	case POTENTIOMETER_MENU:
		current_menu = OVERLOAD_TIME_MENU;
		lcd_write_new_P(PSTR("OVERLOAD TIME"));
		break;

		//Choose the type of pot being used
	case POT_TYPE_MENU:
		break;

	case POT_TYPE_VAL_MENU:
		break;

	case POT_CALIBRATION_MENU:
		/*current_menu = POT_TYPE_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER TYPE"));*/
		break;

	case POT_CALIBRATION_VAL_MENU:
		break;

	case POT_RECALIBRATION_MENU:
		lcd_write_new_P(PSTR("RECALIBRATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		pot_calibrating = POT_CALIBRATING_CANCELLED;
		send_pot_calibrating();
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = POT_CALIBRATION_VAL_MENU;
		sprintf(temp, "LOW=%d.%03d|HIGH=%d.%03d", pot_low_deadband / 1000, pot_low_deadband % 1000, pot_high_deadband / 1000, pot_high_deadband % 1000);
		lcd_write_new(temp);
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("NEXT TO RECALIBRATE"));
		break;

		/*--------------------------------------------------------------------------------*/

	case VACUUM_SETTINGS_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
		break;

	case VACUUM_SLOW_START_MENU:
		break;

	case BRUSH_SETTINGS_MENU:
		current_menu = VACUUM_SETTINGS_MENU;
		lcd_write_new_P(PSTR("VACUUM SETTINGS"));
		break;

	case BRUSH_SLOW_START_MENU:
		break;

	case PRE_SWP_SETTINGS_MENU:
		current_menu = BRUSH_SETTINGS_MENU;
		lcd_write_new_P(PSTR("BRUSH SETTINGS"));
		break;

	case PRE_SWP_SLOW_START_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_VAL_MENU:
		if (current_value < MAX_ACCELERATION)
			current_value++;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case DECELERATION_VAL_MENU:
		if (current_value < MAX_DECELERATION)
			current_value++;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case BRAKE_DELAY_VAL_MENU:
		if (current_value < MAX_BRAKE_DELAY)
			current_value++;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case REVERSE_SPEED_VAL_MENU:
		if (current_value < MAX_REVERSE)
			current_value++;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case SLOW_SPEED_VAL_MENU:
		if (current_value < MAX_SLOW)
			current_value++;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case FAST_SPEED_VAL_MENU:
		if (current_value < MAX_FAST)
			current_value++;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case CURRENT_RATING_VAL_MENU:
		if (current_value < MAX_CURRENT_RATING)
			current_value++;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

	case MAX_CURRENT_VAL_MENU:
		if (current_value < MAX_MAX_CURRENT)
			current_value++;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

	case OVERLOAD_TIME_VAL_MENU:
		if (current_value < MAX_OVERLOAD_TIME)
			current_value++;
		sprintf(temp, "%d SECONDS", current_value);
		lcd_write_new(temp);
		break;

	case VACUUM_SLOW_START_VAL_MENU:
		if (current_value < MAX_VACUUM_RAMPUP_TIME)
			current_value++;
		sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
		lcd_write_new(temp);
		break;

	case BRUSH_SLOW_START_VAL_MENU:
		if (current_value < MAX_BRUSH_RAMPUP_TIME)
			current_value++;
		sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
		lcd_write_new(temp);
		break;

	case PRE_SWP_SLOW_START_VAL_MENU:
		if (current_value < MAX_PRE_SWP_RAMPUP_TIME)
			current_value++;
		sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
		lcd_write_new(temp);
		break;

		/******************************************************************************************************************************/

	case BATTERY_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case LEAD_MENU:
		break;

	case AGM_MENU:
		current_menu = LEAD_MENU;
		lcd_write_new_P(PSTR("BATTERY: LEAD"));
		break;

	case GEL_MENU:
		current_menu = AGM_MENU;
		lcd_write_new_P(PSTR("BATTERY: AGM"));
		break;

		/******************************************************************************************************************************/

	case SET_SERIAL_NUM_MENU:
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

	case SET_RB800_SERIAL_NUM_MENU:
		break;

	case SET_A1_SERIAL_NUM_MENU:
		current_menu = SET_RB800_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET RB800 SERIAL NUM"));
		break;

	case SET_A2_SERIAL_NUM_MENU:
		current_menu = SET_A1_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET A1 SERIAL NUM"));
		break;

	case SET_A3_SERIAL_NUM_MENU:
		current_menu = SET_A2_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET A2 SERIAL NUM"));
		break;

		/*-------------------------------------------------------------------------------*/

	case SET_RB800_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor - 2] != '9')// -2 IS TO START THE ARRAY FROM 0th position(cursor value will be 2)
			serial_num_temp[cursor - 2]++;
		lcd_set_cursor(0, 2);
		lcd_write(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A1_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '9')
			serial_num_temp[cursor]++;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A2_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '9')
			serial_num_temp[cursor]++;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A3_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '9')
			serial_num_temp[cursor]++;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

		/******************************************************************************************************************************/

	case RUN_TIME_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RUN_TIME_VAL_MENU:
		if (!admin_mode)
			break;
		if (cursor < 5)
		{
			if (cursor == 0 && run_time_hour_arr[0] == '5')
			{
				break;
			}
			if (run_time_hour_arr[cursor] == '9')
			{
				break;
			}
			run_time_hour_arr[cursor]++;
			sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
			lcd_write_new(temp);
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("HOURS:MINUTES"));
			lcd_set_cursor(0, cursor);
		}
		else
		{
			if (cursor == 5 && run_time_min_arr[0] == '5')
			{
				break;
			}
			if (run_time_min_arr[cursor - 5] == '9')
			{
				break;
			}
			run_time_min_arr[cursor - 5]++;
			sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
			lcd_write_new(temp);
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("HOURS:MINUTES"));
			lcd_set_cursor(0, cursor + 1);
		}
		break;

		/******************************************************************************************************************************/

	case STATISTICS_MENU:
		current_menu = RUN_TIME_MENU;
		lcd_write_new_P(PSTR("RUN TIME"));
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_STAT_MENU:
		break;

	case VACUUM_STAT_MENU:
		current_menu = BRUSH_STAT_MENU;
		lcd_write_new_P(PSTR("BRUSH STATISTICS"));
		break;

	case PRESWEEP_STAT_MENU:
		current_menu = VACUUM_STAT_MENU;
		lcd_write_new_P(PSTR("VACUUM STATISTICS"));
		break;

	case WATER_PUMP_STAT_MENU:
		current_menu = PRESWEEP_STAT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP STATISTICS"));
		break;

	case DETERGENT_PUMP_STAT_MENU:
		current_menu = WATER_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;

	case ECO_HOURS_MENU:
		current_menu = DETERGENT_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_HOURS_MENU:
		break;

	case BRUSH1_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH_HOURS_MENU;
		lcd_write_new_P(PSTR("BRUSH HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush_hours);
		lcd_write(temp);
		break;

	case BRUSH1_MAX_CURRENT_MENU:
		current_menu = BRUSH1_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 1 AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush1_average_current);
		lcd_write(temp);
		break;

	case BRUSH2_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH1_MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 1 MAX CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush1_max_current);
		lcd_write(temp);
		break;

	case BRUSH2_MAX_CURRENT_MENU:
		current_menu = BRUSH2_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 2 AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush2_average_current);
		lcd_write(temp);
		break;

		/*-------------------------------------------------------------------------------*/

	case VACUUM_HOURS_MENU:
		break;

	case VACUUM_AVERAGE_CURRENT_MENU:
		current_menu = VACUUM_HOURS_MENU;
		lcd_write_new_P(PSTR("VACUUM HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", vacuum_hours);
		lcd_write(temp);
		break;

	case VACUUM_MAX_CURRENT_MENU:
		current_menu = VACUUM_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("VACUUM AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", vacuum_average_current);
		lcd_write(temp);
		break;

		/*-------------------------------------------------------------------------------*/

	case PRESWEEP_HOURS_MENU:
		break;

	case PRESWEEP_AVERAGE_CURRENT_MENU:
		current_menu = PRESWEEP_HOURS_MENU;
		lcd_write_new_P(PSTR("PRESWEEP HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", presweep_hours);
		lcd_write(temp);
		break;

	case PRESWEEP_MAX_CURRENT_MENU:
		current_menu = PRESWEEP_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", presweep_average_current);
		lcd_write(temp);
		break;

		/******************************************************************************************************************************/

	case DEFAULT_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case DEFAULT_CONFIRM_MENU:
		lcd_write_new_P(PSTR("RESET OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

		/******************************************************************************************************************************/

	case CLEAR_SD_CARD_MENU:
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

	case CLEAR_SD_CARD_CONFIRM_MENU:
		lcd_write_new_P(PSTR("CLEAR OPERATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = CLEAR_SD_CARD_MENU;
		lcd_write_new_P(PSTR("CLEAR SD CARD"));
		break;

		/******************************************************************************************************************************/

	case ABOUT_MENU:
		current_menu = CLEAR_SD_CARD_MENU;
		lcd_write_new_P(PSTR("CLEAR SD CARD"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_MENU:
		break;

	case A1_MENU:
		current_menu = RB800_MENU;
		lcd_write_new_P(PSTR("ABOUT RB800"));
		break;

	case A2_MENU:
		current_menu = A1_MENU;
		lcd_write_new_P(PSTR("ABOUT A1"));
		break;

	case A3_MENU:
		current_menu = A2_MENU;
		lcd_write_new_P(PSTR("ABOUT A2"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_SERIAL_NUM_MENU:
		break;

	case A1_SERIAL_NUM_MENU:
		break;

	case A1_PCB_MENU:
		current_menu = A1_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A1 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A1_serial_num);
		break;

	case A1_FIRMWARE_MENU:
		current_menu = A1_PCB_MENU;
		lcd_write_new(A1_PCB);
		break;

	case A2_SERIAL_NUM_MENU:
		break;

	case A2_PCB_MENU:
		current_menu = A2_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A2 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A2_serial_num);
		break;

	case A2_FIRMWARE_MENU:
		current_menu = A2_PCB_MENU;
		lcd_write_new(A2_PCB);
		break;

	case A3_SERIAL_NUM_MENU:
		break;

	case A3_PCB_MENU:
		current_menu = A3_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("A3 SERIAL NUM"));
		lcd_set_cursor(1, 0);
		lcd_write(A3_serial_num);
		break;

	case A3_FIRMWARE_MENU:
		current_menu = A3_PCB_MENU;
		lcd_write_new(A3_PCB);
		break;

		/******************************************************************************************************************************/

	default: break;
	}
}

void menu_down(void)
{
	switch (current_menu)
	{
	case PRE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_MENU:
		current_menu = BATTERY_MENU;
		lcd_write_new_P(PSTR("BATTERY TYPE"));
		break;

		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_MENU:
		current_menu = DECELERATION_MENU;
		lcd_write_new_P(PSTR("DECELERATION"));
		break;

	case DECELERATION_MENU:
		current_menu = BRAKE_DELAY_MENU;
		lcd_write_new_P(PSTR("BRAKE DELAY"));
		break;

	case BRAKE_DELAY_MENU:
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;

	case FORWARD_SPEED_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;

	case SLOW_SPEED_MENU:
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

	case FAST_SPEED_MENU:
		break;

	case REVERSE_SPEED_MENU:
		current_menu = CURRENT_RATING_MENU;
		lcd_write_new_P(PSTR("CURRENT RATING"));
		break;

	case CURRENT_RATING_MENU:
		current_menu = MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("MAX CURRENT"));
		break;

	case MAX_CURRENT_MENU:
		current_menu = OVERLOAD_TIME_MENU;
		lcd_write_new_P(PSTR("OVERLOAD TIME"));
		break;

	case OVERLOAD_TIME_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
		break;

		/*--------------------------------------------------------------------------------*/

		//Choose type of pot and calibrate the pot
	case POTENTIOMETER_MENU:
		current_menu = VACUUM_SETTINGS_MENU;
		lcd_write_new_P(PSTR("VACUUM SETTINGS"));
		break;

		//Choose the type of pot being used
	case POT_TYPE_MENU:
		current_menu = POT_CALIBRATION_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CALIBRATION"));
		break;

	case POT_TYPE_VAL_MENU:
		break;

	case POT_CALIBRATION_MENU:
		break;

	case POT_CALIBRATION_VAL_MENU:
		break;

	case POT_RECALIBRATION_MENU:
		lcd_write_new_P(PSTR("RECALIBRATION"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("CANCELLED"));
		pot_calibrating = POT_CALIBRATING_CANCELLED;
		send_pot_calibrating();
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = POT_CALIBRATION_VAL_MENU;
		sprintf(temp, "LOW=%d.%03d|HIGH=%d.%03d", pot_low_deadband / 1000, pot_low_deadband % 1000, pot_high_deadband / 1000, pot_high_deadband % 1000);
		lcd_write_new(temp);
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("NEXT TO RECALIBRATE"));
		break;

		/*--------------------------------------------------------------------------------*/

	case VACUUM_SETTINGS_MENU:
		current_menu = BRUSH_SETTINGS_MENU;
		lcd_write_new_P(PSTR("BRUSH SETTINGS"));
		break;

	case VACUUM_SLOW_START_MENU:
		break;

	case BRUSH_SETTINGS_MENU:
		current_menu = PRE_SWP_SETTINGS_MENU;
		lcd_write_new_P(PSTR("PRE-SWEEP SETTINGS"));
		break;

	case BRUSH_SLOW_START_MENU:
		break;

	case PRE_SWP_SETTINGS_MENU:
		break;

	case PRE_SWP_SLOW_START_MENU:
		break;
		/*-------------------------------------------------------------------------------*/

	case ACCELERATION_VAL_MENU:
		if (current_value > MIN_ACCELERATION)
			current_value--;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case DECELERATION_VAL_MENU:
		if (current_value > MIN_DECELERATION)
			current_value--;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case BRAKE_DELAY_VAL_MENU:
		if (current_value > MIN_BRAKE_DELAY)
			current_value--;
		sprintf(temp, "%d.%d SECONDS", current_value / 10, current_value % 10);
		lcd_write_new(temp);
		break;

	case REVERSE_SPEED_VAL_MENU:
		if (current_value > MIN_REVERSE)
			current_value--;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case SLOW_SPEED_VAL_MENU:
		if (current_value > MIN_SLOW)
			current_value--;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case FAST_SPEED_VAL_MENU:
		if (current_value > MIN_FAST)
			current_value--;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

	case CURRENT_RATING_VAL_MENU:
		if (current_value > MIN_CURRENT_RATING)
			current_value--;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

	case MAX_CURRENT_VAL_MENU:
		if (current_value > MIN_MAX_CURRENT)
			current_value--;
		sprintf(temp, "%d AMPS", current_value);
		lcd_write_new(temp);
		break;

	case OVERLOAD_TIME_VAL_MENU:
		if (current_value > MIN_OVERLOAD_TIME)
			current_value--;
		sprintf(temp, "%d SECONDS", current_value);
		lcd_write_new(temp);
		break;

	case VACUUM_SLOW_START_VAL_MENU:
		if (current_value > MIN_VACUUM_RAMPUP_TIME)
			current_value--;
		if (current_value == 0)
		{
			lcd_write_new_P(PSTR("OFF"));
		}
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

	case BRUSH_SLOW_START_VAL_MENU:
		if (current_value > MIN_BRUSH_RAMPUP_TIME)
			current_value--;
		if (current_value == 0)
		{
			lcd_write_new_P(PSTR("OFF"));
		}
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

	case PRE_SWP_SLOW_START_VAL_MENU:
		if (current_value > MIN_PRE_SWP_RAMPUP_TIME)
			current_value--;
		if (current_value == 0)
		{
			lcd_write_new_P(PSTR("OFF"));
		}
		else
		{
			sprintf(temp, "%d MILLISECONDS", (uint16_t)current_value * 250);
			lcd_write_new(temp);
		}
		break;

		/******************************************************************************************************************************/

	case BATTERY_MENU:
		current_menu = SET_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET SERIAL NUM"));
		break;

		/*-------------------------------------------------------------------------------*/

	case LEAD_MENU:
		current_menu = AGM_MENU;
		lcd_write_new_P(PSTR("BATTERY: AGM"));
		break;

	case AGM_MENU:
		current_menu = GEL_MENU;
		lcd_write_new_P(PSTR("BATTERY: GEL"));
		break;

	case GEL_MENU:
		break;

		/******************************************************************************************************************************/

	case SET_SERIAL_NUM_MENU:
		current_menu = RUN_TIME_MENU;
		lcd_write_new_P(PSTR("RUN TIME"));
		break;

	case SET_RB800_SERIAL_NUM_MENU:
		current_menu = SET_A1_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET A1 SERIAL NUM"));
		break;

	case SET_A1_SERIAL_NUM_MENU:
		current_menu = SET_A2_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET A2 SERIAL NUM"));
		break;

	case SET_A2_SERIAL_NUM_MENU:
		current_menu = SET_A3_SERIAL_NUM_MENU;
		lcd_write_new_P(PSTR("SET A3 SERIAL NUM"));
		break;

	case SET_A3_SERIAL_NUM_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case SET_RB800_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor - 2] != '0')// -2 IS TO START THE ARRAY FROM 0th position(cursor value will be 2)
			serial_num_temp[cursor - 2]--;
		lcd_set_cursor(0, 2);
		lcd_write(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A1_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '0')
			serial_num_temp[cursor]--;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A2_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '0')
			serial_num_temp[cursor]--;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

	case SET_A3_SERIAL_NUM_VAL_MENU:
		if (!admin_mode)
			break;
		if (serial_num_temp[cursor] != '0')
			serial_num_temp[cursor]--;
		lcd_write_new(serial_num_temp);
		lcd_set_cursor(0, cursor);
		break;

		/******************************************************************************************************************************/

	case RUN_TIME_MENU:
		current_menu = STATISTICS_MENU;
		lcd_write_new_P(PSTR("STATISTICS"));
		break;

		/*-------------------------------------------------------------------------------*/

	case RUN_TIME_VAL_MENU:
		if (!admin_mode)
			break;
		if (cursor < 5)
		{
			if (run_time_hour_arr[cursor] == '0')
			{
				break;
			}
			run_time_hour_arr[cursor]--;
			sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
			lcd_write_new(temp);
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("HOURS:MINUTES"));
			lcd_set_cursor(0, cursor);
		}
		else
		{
			if (run_time_min_arr[cursor - 5] == '0')
			{
				break;
			}
			run_time_min_arr[cursor - 5]--;
			sprintf(temp, "%s:%s", run_time_hour_arr, run_time_min_arr);
			lcd_write_new(temp);
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("HOURS:MINUTES"));
			lcd_set_cursor(0, cursor + 1);
		}
		break;

		/******************************************************************************************************************************/

	case STATISTICS_MENU:
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_STAT_MENU:
		current_menu = VACUUM_STAT_MENU;
		lcd_write_new_P(PSTR("VACUUM STATISTICS"));
		break;

	case VACUUM_STAT_MENU:
		current_menu = PRESWEEP_STAT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP STATISTICS"));
		break;

	case PRESWEEP_STAT_MENU:
		current_menu = WATER_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;

	case WATER_PUMP_STAT_MENU:
		current_menu = DETERGENT_PUMP_STAT_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("STATISTICS"));
		break;

	case DETERGENT_PUMP_STAT_MENU:
		current_menu = ECO_HOURS_MENU;
		lcd_write_new_P(PSTR("ECO MODE HOURS"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", eco_hours);
		lcd_write(temp);
		break;

	case ECO_HOURS_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case BRUSH_HOURS_MENU:
		current_menu = BRUSH1_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 1 AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush1_average_current);
		lcd_write(temp);
		break;

	case BRUSH1_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH1_MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 1 MAX CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush1_max_current);
		lcd_write(temp);
		break;

	case BRUSH1_MAX_CURRENT_MENU:
		current_menu = BRUSH2_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 2 AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush2_average_current);
		lcd_write(temp);
		break;

	case BRUSH2_AVERAGE_CURRENT_MENU:
		current_menu = BRUSH2_MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("BRUSH 2 MAX CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", brush2_max_current);
		lcd_write(temp);
		break;

	case BRUSH2_MAX_CURRENT_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case VACUUM_HOURS_MENU:
		current_menu = VACUUM_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("VACUUM AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", vacuum_average_current);
		lcd_write(temp);
		break;

	case VACUUM_AVERAGE_CURRENT_MENU:
		current_menu = VACUUM_MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("VACUUM MAX CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", vacuum_max_current);
		lcd_write(temp);
		break;

	case VACUUM_MAX_CURRENT_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case PRESWEEP_HOURS_MENU:
		current_menu = PRESWEEP_AVERAGE_CURRENT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP AVG CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", presweep_average_current);
		lcd_write(temp);
		break;

	case PRESWEEP_AVERAGE_CURRENT_MENU:
		current_menu = PRESWEEP_MAX_CURRENT_MENU;
		lcd_write_new_P(PSTR("PRESWEEP MAX CURRENT"));
		lcd_set_cursor(1, 0);
		sprintf(temp, "%d", presweep_max_current);
		lcd_write(temp);
		break;

	case PRESWEEP_MAX_CURRENT_MENU:
		break;

		/******************************************************************************************************************************/


	case DEFAULT_MENU:
		current_menu = CLEAR_SD_CARD_MENU;
		lcd_write_new_P(PSTR("CLEAR SD CARD"));
		break;

		/*-------------------------------------------------------------------------------*/

	case DEFAULT_CONFIRM_MENU:
		set_to_default();
		sei();
		lcd_write_new_P(PSTR("ALL SETTINGS RESET"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("TO DEFAULT VALUES"));
		_delay_ms(3000);
		clear_switch_pressed();
		current_menu = DEFAULT_MENU;
		lcd_write_new_P(PSTR("RESET TO DEFAULT"));
		break;

		/******************************************************************************************************************************/

	case CLEAR_SD_CARD_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

	case CLEAR_SD_CARD_CONFIRM_MENU:
		erase_SD_card();
		sei();
		lcd_write_new_P(PSTR("ERASING SD CARD"));
		_delay_ms(1000); lcd_write_P(PSTR("."));
		_delay_ms(1000); lcd_write_P(PSTR("."));
		_delay_ms(1000); lcd_write_P(PSTR("."));
		_delay_ms(3000);
		lcd_write_new_P(PSTR("SD CARD CLEARED"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("RESTART REQUIRED"));
		cli();
		WHILE_TRUE();
		break;

		/******************************************************************************************************************************/

	case ABOUT_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_MENU:
		current_menu = A1_MENU;
		lcd_write_new_P(PSTR("ABOUT A1"));
		break;

	case A1_MENU:
		current_menu = A2_MENU;
		lcd_write_new_P(PSTR("ABOUT A2"));
		break;

	case A2_MENU:
		current_menu = A3_MENU;
		lcd_write_new_P(PSTR("ABOUT A3"));
		break;

	case A3_MENU:
		break;

		/*-------------------------------------------------------------------------------*/

	case RB800_SERIAL_NUM_MENU:
		break;

	case A1_SERIAL_NUM_MENU:
		current_menu = A1_PCB_MENU;
		lcd_write_new(A1_PCB);
		break;

	case A1_PCB_MENU:
		current_menu = A1_FIRMWARE_MENU;
		lcd_write_new(A1_FIRMWARE);
		break;

	case A1_FIRMWARE_MENU:
		break;

	case A2_SERIAL_NUM_MENU:
		current_menu = A2_PCB_MENU;
		lcd_write_new(A2_PCB);
		break;

	case A2_PCB_MENU:
		current_menu = A2_FIRMWARE_MENU;
		lcd_write_new(A2_FIRMWARE);
		break;

	case A2_FIRMWARE_MENU:
		break;

	case A3_SERIAL_NUM_MENU:
		current_menu = A3_PCB_MENU;
		lcd_write_new(A3_PCB);
		break;

	case A3_PCB_MENU:
		current_menu = A3_FIRMWARE_MENU;
		lcd_write_new(A3_FIRMWARE);
		break;

	case A3_FIRMWARE_MENU:
		break;

		/******************************************************************************************************************************/

	default: break;
	}
}

//Admin mode allows user to change the serial number and runtime hours
void enter_admin_mode(uint8_t switch_pressed)
{
	switch (admin_mode_count)
	{
	case 0:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode_count++;
		}
		else
			admin_mode_count = 0;
		break;

	case 1:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode_count++;
		}
		else
			admin_mode_count = 0;
		break;

	case 2:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode_count++;
		}
		else
			admin_mode_count = 0;
		break;

	case 3:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode_count++;
		}
		else
			admin_mode_count = 0;
		break;

	case 4:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode_count++;
		}
		else
			admin_mode_count = 0;
		break;

	case 5:
		if (switch_pressed == S_SLO_NUM)
		{
			admin_mode = TRUE;
		}
		else
			admin_mode_count = 0;
		break;

	default:
		admin_mode = FALSE;
	}
}

void start_s_menu(void)
{
	stop_second_timer();
	user_settings_menu_enable = TRUE;
	pot_calibrating = FALSE;
	admin_mode = FALSE;
	admin_mode_count = 0;

	ack_counter = 0;

	lcd_write_new_P(PSTR("CHECKING CAN BUS..."));
	_delay_ms(1000);
	//Make sure CAN bus is functioning on all boards
	while (!can_functioning)
	{
		if (ack_counter >= 15 && !can_functioning)
		{
			if (!A2_ack_received & !A3_ack_received)
			{
				lcd_write_new_P(PSTR("NO CAN COMM"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("CHECK WIRING"));
			}
			else if (!A2_ack_received)
			{
				lcd_write_new_P(PSTR("NO A2 CAN COMM"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("CHECK WIRING"));
			}
			else if (!A3_ack_received)
			{
				lcd_write_new_P(PSTR("NO A3 CAN COMM"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("CHECK WIRING"));
			}
			
			
			cli();
			WHILE_TRUE();
		}
		if (!CQRx_queueIsEmpty())
		{
			CQRx_dequeue();
			reset_CAN_error();
		}
		else
		{
			ack_counter++;
			send_CAN_ACK();
			_delay_ms(1000);
		}
	}

	//_delay_ms(200);
	//request_DM_settings();
	_delay_ms(200);
	send_pot_calibrating();
	_delay_ms(200);
	request_A2_serial_num();
	_delay_ms(200);
	clear_switch_pressed();
	//_delay_ms(200);
	request_A3_serial_num();
	_delay_ms(200);
	lcd_set_cursor(0, 0);
	lcd_write_P(PSTR("EP - up  |RV - next"));
	lcd_set_cursor(1, 0);
	lcd_write_P(PSTR("BR - down|HL - back"));
	_delay_ms(200);
	request_statistics();
	current_menu = PRE_MENU;

	while (1)
	{
		//Check if any switches have been pressed
		for (int num = 0; num < NUM_SWITCHES; num++)
		{
			if (switch_pressed[num])
			{
				//Clear switch_pressed
				switch_pressed[num] = FALSE;
				//Perform action depending on switch
				switch (num)
				{
					/*case S_DU_NUM: menu_up();	break;
					case S_TR_NUM: menu_next();	break;
					case S_ECO_NUM: menu_back(); break;
					case S_DD_NUM: menu_down();	break;*/

				case S_EP_NUM: menu_up();   admin_mode_count = 0;	break;
				case S_RV_NUM: menu_next(); admin_mode_count = 0;	break;
				case S_HL_NUM: menu_back(); admin_mode_count = 0;   break;
				case S_BR_NUM: menu_down();	admin_mode_count = 0;   break;

				case S_DU_NUM: admin_mode_count = 0;
				case S_PU_NUM: admin_mode_count = 0;
				case S_SLO_NUM:enter_admin_mode(num); break;
				case S_TR_NUM: admin_mode_count = 0;
				case S_ECO_NUM:admin_mode_count = 0;
				case S_VAC_NUM:admin_mode_count = 0;
				case S_HRN_NUM:admin_mode_count = 0;
				case S_DD_NUM: admin_mode_count = 0;
				case S_ATO_NUM:admin_mode_count = 0;
				case S_PD_NUM: admin_mode_count = 0;

				default:  break;
				}
			}
		}

		if (!CQRx_queueIsEmpty())
		{
			CQRx_dequeue();
			reset_CAN_error();
		}

		YIELD();
	}
}