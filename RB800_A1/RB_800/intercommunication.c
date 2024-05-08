/*
* intercommunication.c
*
* Created: 11/20/2015 9:52:35 AM
*  Author: Anurag Vaddi Reddy
*/

#include "intercommunication.h"

void processCAN(uint16_t identifier, uint8_t* data)
{
	char message[21];
	uint16_t total;
	uint8_t i;
	can_functioning_timer = 0;
	DBG2("processCAN: %02X %02X", identifier, data[0]);
	//Process the communication depending on the type of communication
	switch (identifier)
	{
		//Inform user of error
	case ALARM_CAN:

		if (data[0] == KEY_SWITCH_OPEN)
		{
			printing_info = FALSE;
			lcd_write_new_P(PSTR("E17 - KEY INPUT OPEN"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("IN A3-CHECK WIRING"));

			cli();
			WHILE_TRUE()
				lcd_led_blink();
		}
		else if (data[0] == POT_FAIL)
		{
			printing_info = FALSE;
			lcd_write_new_P(PSTR("E09 - POT NOT"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("FUNCTIONING"));
			lcd_led_blink();
			print_menu = TRUE;
		}
		else if (data[0] == PEDAL_SWITCH_CLOSED && !pedal_switch_error)
		{
			printing_info = FALSE;
			pedal_switch_error = TRUE;
			if (!manual_switch_error)
			{
				lcd_write_new_P(PSTR("E10 - RELEASE"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("ACCELERATOR PEDAL"));
			}
		}
		else if (data[0] == MANUAL_SWITCH_CLOSED && !manual_switch_error)
		{
			printing_info = FALSE;
			manual_switch_error = TRUE;
			lcd_write_new_P(PSTR("E15-EMERGENCY DRIVE"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("SW-OUT OF NEUTRAL"));
		}
		else if (data[0] == MANUAL_SWITCH_OPEN && manual_switch_error)
		{
			printing_info = FALSE;
			manual_switch_error = FALSE;
			if (pedal_switch_error)
			{
				lcd_write_new_P(PSTR("E10 - RELEASE"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("ACCELERATOR PEDAL"));
			}
			else
			{
				clear_switch_pressed();
				print_menu = TRUE;
			}
		}
		else if (data[0] == PEDAL_SWITCH_OPEN && pedal_switch_error == TRUE)
		{
			printing_info = FALSE;
			pedal_switch_error = FALSE;
			clear_switch_pressed();
			print_menu = TRUE;
		}
		else if (data[0] == A3_OVERCURRENT)
		{
			lcd_write_new_P(PSTR("E12 - DRIVE MOTOR"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("OVERLOAD"));

			cli();
			WHILE_TRUE()
				lcd_led_blink();

		}
		else if (data[0] == MOTOR_PARAMETER)
		{
			lcd_write_new_P(PSTR("E18 - MOTOR SETTINGS"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("FAIL-DEFAULTS LOADED"));
			lcd_update_pause = 10;
		}
		else if (data[0] == CAN_BUFFER_OVERFLOW)
		{
			lcd_write_new_P(PSTR("CAN BUFFER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("OVERFLOW"));
			lcd_update_pause = 15;
		}
		else if (data[0] == SD_BUFFER_OVERFLOW)
		{
			lcd_write_new_P(PSTR("SD BUFFER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("OVERFLOW"));
			lcd_update_pause = 15;
		}
		else if (data[0] == SD_CARD_FAIL)
		{
			sd_card_unavailable = TRUE;
			if (reset_timer < 5)
			{
				lcd_write_new_P(PSTR("E19 - SD CARD"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("UNAVAILABLE"));
				lcd_update_pause = 5;
			}
			else
			{
				lcd_write_new_P(PSTR("E20 - SD CARD"));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("FAIL"));
				lcd_update_pause = 15;
			}
		}
		else if (data[0] == FUSE_FAIL)
		{
			printing_info = FALSE;
			fuse_error = TRUE;
			lcd_write_new_P(PSTR("E01 - FUSE BLOWN"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("TURN OFF KEY"));
		}
		else if (data[0] == BRAKE_SEAT_SWITCH_OPEN)
		{
			printing_info = FALSE;
			brake_seat_switch_error = TRUE;
			lcd_write_new_P(PSTR("E14 - SEAT/BRAKE"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("SWITCH OPEN"));
		}
		else if (data[0] == BRAKE_SEAT_SWITCH_CLOSED)
		{
			printing_info = FALSE;
			brake_seat_switch_error = FALSE;
			clear_switch_pressed();
			print_menu = TRUE;
		}
		else if (data[0] == FAULT_FAIL)
		{
			printing_info = FALSE;
			fault_error = TRUE;
			lcd_write_new_P(PSTR("E21 - 8701 FAULT"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("TURN OFF KEY"));
		}
		else if (data[0] == BM_OVERCURRENT)
		{
			printing_info = FALSE;
			bm_overcurrent_error = TRUE;
			lcd_write_new_P(PSTR("E22 - BM OVERLOAD"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("TURN OFF KEY"));
		}
		break;

		//Receive battery voltage from A2
	case BATTERY_CAN:
		total = data[0] + (data[1] << 8);
		check_battery_voltage(total);
		break;

		//Inform user of TOP error from A2 or A3
	case TOP_CAN:
		acknowledge_top();
		if (top_error == OFF)
		{
			top_error = data[0];
			if (top_error == 0xA2)
			{
				lcd_write_new_P(PSTR("E03 - A2 TOP ERROR"));
			}
			else if (top_error == 0xA3)
			{
				lcd_write_new_P(PSTR("E11 - A3 TOP ERROR"));
			}
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("TURN OFF KEY"));
		}
		break;

		//Turn off vacuum if tank is full
	case FLOAT_CAN:
		tank_full = TRUE;
		change_vacuum_state();
		break;

		//A2 or A3 can inform A1 that runtimer should be on/off
	case RUNTIMER_CAN:
		runtimer_on = data[0];
		break;

		//Updates the LED depending on the brush state on the A2 board
	case BRUSH_STATE:
		brush_state = data[0];
		if (data[0] == BRUSH_RUNNING)
		{
			L_BR_ON;
			/*if(solenoid_valve)
			{
			L_SLD_ON;
			//  blink_sld_led = FALSE;
			}
			else L_SLD_OFF;*/
			blink_brush_led = FALSE;
		}
		if (data[0] == BRUSH_OFF)
		{
			if (brush)
			{
				blink_brush_led = TRUE;
			}
			else
			{
				L_BR_OFF;
				blink_brush_led = FALSE;
			}
			if (solenoid_led)
			{
				L_SLD_ON;//blink_sld_led = TRUE;
			}
			else
			{
				L_SLD_OFF;
				//blink_sld_led = FALSE;
			}

		}
		break;

		//Updates the LED depending on the vacuum state on the A2 board
	case VACUUM_STATE:
		vacuum_state = data[0];
		if (data[0] == VACUUM_RUNNING)
		{
			L_VAC_ON;
			blink_vac_led = FALSE;
		}
		if (data[0] == VACUUM_RUNNING_OFF)
		{
			blink_vac_led = TRUE;
		}
		if (data[0] == VACUUM_OFF)
		{
			L_VAC_OFF;
			blink_vac_led = FALSE;
		}
		break;
		
		//Receive drive motor settings from A3
	case DM_SETTINGS_CAN:
		i = 1;
		if (data[0] == DM_SETTINGS_1)
		{
			acceleration = data[i++];
			deceleration = data[i++];
			brake_delay = data[i++];
			reverse_speed = data[i++];
		}
		else if (data[0] == DM_SETTINGS_2)
		{
			slow_speed = data[i++];
			fast_speed = data[i++];
			current_rating = data[i++];
			max_current = data[i++];
			overload_time = data[i++];
		}
		break;

	case A2_SETTINGS_CAN:
		break;

		//Inform user of back safety error
		/*case BACK_SAFETY_CAN:
		if(data[0] == BACK_SAFETY_ALERT)
		{
		if(reverse)
		{
		change_forward_reverse_state();
		}
		back_safety_alert = TRUE;
		turn_off_motors();
		lcd_write_new_P(PSTR("BACK SAFETY ALERT"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("RESOLVING"));
		}
		else if(data[0] == BACK_SAFETY_RESOLVED)
		{
		back_safety_alert = FALSE;
		LCD_LED_ON;
		clear_switch_pressed();
		print_menu = TRUE;
		}
		else if(data[0] == BACK_SAFETY_UNRESOLVED)
		{
		turn_off_motors();
		lcd_write_new_P(PSTR("E17 - BACK SAFETY"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("TURN OFF KEY"));
		cli();
		while(1)
		lcd_led_blink();
		}
		break;
		*/
		//When in settings menu, A3 will send pot calibration info to A1
	case POT_CALIBRATION_CAN:
		if (pot_calibrating == FALSE || pot_calibrating == POT_CALIBRATING_CANCELLED)
		{
			pot_low_deadband = (data[0] << 8) + data[1];
			pot_high_deadband = (data[2] << 8) + data[3];
		}
		else if (pot_calibrating == POT_CALIBRATING_LOW)
		{
			pot_low_deadband = (data[0] << 8) + data[1];

			lcd_write_new_P(PSTR("NOW FULLY PRESS"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("PEDAL TO FLOOR"));
			lcd_led_blink();
			lcd_led_blink();
			clear_switch_pressed();
			pot_calibrating = POT_CALIBRATING_HIGH;
			send_pot_calibrating();
		}
		else if (pot_calibrating == POT_CALIBRATING_HIGH)
		{
			pot_high_deadband = (data[0] << 8) + data[1];

			lcd_write_new_P(PSTR("CALIBRATION COMPLETE"));
			lcd_set_cursor(1, 0);
			sprintf(message, "LOW=%d.%03d|HIGH=%d.%03d", pot_low_deadband / 1000, pot_low_deadband % 1000, pot_high_deadband / 1000, pot_high_deadband % 1000);
			lcd_write(message);

			pot_calibrating = POT_CALIBRATING_COMPLETE;
			send_pot_calibrating();

			lcd_led_blink();
			_delay_ms(2000);
			clear_switch_pressed();

			current_menu = POT_CALIBRATION_MENU;
			lcd_write_new_P(PSTR("POTENTIOMETER"));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("CALIBRATION"));
		}
		break;

		//Get Pot value from the A3
	case POT_VALUE_CAN:
		pot_value = (data[0] << 8) + data[1];
		break;

		//Get pedal level from the A3
	case ACCEL_PDL_CAN:
		if (data[0] == PEDAL_LEVEL)
		{
			pedal_level = data[1] << 8;
			pedal_level += data[2];
			A3_can_functioning_timer = 0;
		}
		break;

		//Receive ACK from A2 and A3 when board starts up to make sure communication is working
	case ACK_CAN:
		if (data[0] == A2_CAN)
		{
			A2_ack_received = TRUE;
			send_motor_info();
		}
		else if (data[0] == A3_CAN)
		{
			A3_ack_received = TRUE;
		}

		if (A2_ack_received && A3_ack_received)
		{
			can_functioning = TRUE;
		}
		break;

		//Confirmation that motor info has been received from another board
	case MOTOR_INFO_CAN:
		motor_info_sent = TRUE;
		break;

		//Save serial number received from A2
	case A2_SERIAL_NUM_CAN:
		if (data[0] == 1)
		{
			A2_serial_num[0] = data[1];
			A2_serial_num[1] = data[2];
			A2_serial_num[2] = data[3];
			A2_serial_num[3] = data[4];
			A2_serial_num[4] = data[5];
			A2_serial_num[5] = data[6];
		}
		else if (data[0] == 2)
		{
			A2_serial_num[6] = data[1];
			A2_serial_num[7] = data[2];
			A2_serial_num[8] = data[3];
			A2_serial_num[9] = data[4];
			A2_serial_num[10] = data[5];
			A2_serial_num[11] = data[6];
		}
		else if (data[0] == 3)
		{
			A2_serial_num[12] = data[1];
			A2_serial_num[13] = data[2];
		}
		break;

		//Save serial number received by A3
	case A3_SERIAL_NUM_CAN:
		if (data[0] == 1)
		{
			A3_serial_num[0] = data[1];
			A3_serial_num[1] = data[2];
			A3_serial_num[2] = data[3];
			A3_serial_num[3] = data[4];
			A3_serial_num[4] = data[5];
			A3_serial_num[5] = data[6];
		}
		else if (data[0] == 2)
		{
			A3_serial_num[6] = data[1];
			A3_serial_num[7] = data[2];
			A3_serial_num[8] = data[3];
			A3_serial_num[9] = data[4];
			A3_serial_num[10] = data[5];
			A3_serial_num[11] = data[6];
		}
		else if (data[0] == 3)
		{
			A3_serial_num[12] = data[1];
			A3_serial_num[13] = data[2];
		}
		break;

		//Save statistics info provided by A2
	case STATISTICS_CAN:
		switch (data[0])
		{
		case BRUSH_HOURS:
			brush_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case VACUUM_HOURS:
			vacuum_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case PRESWEEP_HOURS:
			presweep_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case ECO_HOURS:
			eco_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case WATER_PUMP_HOURS:
			water_pump_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case DETERGENT_PUMP_HOURS:
			detergent_pump_hours = ((uint16_t)data[1] >> 8) + data[2]; break;
		case BRUSH1_AVERAGE_CURRENT:
			brush1_average_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case BRUSH2_AVERAGE_CURRENT:
			brush2_average_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case VACUUM_AVERAGE_CURRENT:
			vacuum_average_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case PRESWEEP_AVERAGE_CURRENT:
			presweep_average_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case BRUSH1_MAX_CURRENT:
			brush1_max_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case BRUSH2_MAX_CURRENT:
			brush2_max_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case VACUUM_MAX_CURRENT:
			vacuum_max_current = ((uint16_t)data[1] >> 8) + data[2]; break;
		case PRESWEEP_MAX_CURRENT:
			presweep_max_current = ((uint16_t)data[1] >> 8) + data[2]; break;

		default: break;
		}
		break;

		//Reset board if another board resets
	case RESET_DEVICE_CAN_A2:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			WHILE_TRUE();
		}
		break;
		//Reset board if another board resets
	case RESET_DEVICE_CAN_A3:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			WHILE_TRUE();
		}
		break;

	default:
		break;
	}
}

//Delays after transmit_CAN are required to prevent too many transmit from occurring at once
//Could possibly create a transmit queue run by timer to remove delays

//Send updated A2 settings to A2
void update_A2_settings(void)
{
	uint8_t message[15];

	uint8_t i = 0;

	message[i++] = brush;
	message[i++] = vacuum;

	message[i++] = (detergent_pump << 7) | detergent_pump_dc;
	message[i++] = (water_pump << 7) | water_pump_dc;
	message[i++] = (headlights << 4) | (solenoid_valve << 2) | horn;
	message[i++] = (extra_pressure << 4) | reverse;
	message[i++] = (transport << 4) | fr_speed;

	//transmit_tx1_CAN(ALL_STATUS, 0, 7, message);
	transmit_CAN(ALL_STATUS, 0, 7, message);
	_delay_ms(10);

	message[0] = runtimer_on;// (runtimer_on<<4)|solenoid_valve;
	message[1] = hours >> 8;
	message[2] = hours & 0xFF;
	message[3] = minutes;
	//transmit_tx2_CAN(RUNTIMER_CAN, 0, 4, message);
	transmit_CAN(RUNTIMER_CAN, 0, 4, message);
	_delay_ms(10);
}

//Get battery voltage from A2
void read_A2_status(void)
{
	uint8_t message[1];
	transmit_CAN(BATTERY_CAN, 1, 0, message);
	_delay_ms(10);
}

//Send the slow start info to A2
void send_motor_info(void)
{
	uint8_t message[3];
	message[0] = vacuum_slow_start;
	message[1] = brush_slow_start;
	message[2] = pre_swp_slow_start;
	transmit_CAN(MOTOR_INFO_CAN, 0, 3, message);
	_delay_ms(10);
}

//Request drive motor info from A3
void request_statistics(void)
{
	uint8_t message[1];
	message[0] = REQUEST_STATISTICS;
	transmit_CAN(REQUEST_STATISTICS, 0, 1, message);
	_delay_ms(10);
}



//Send updated drive motor settings to A3
void update_DM_settings(void)
{
	//Send all motor settings
	uint8_t message[8];
	message[0] = UPDATE_DM_SETTINGS;
	uint8_t i = 1;
	message[i++] = DM_SETTINGS_1;
	message[i++] = acceleration;
	message[i++] = deceleration;
	message[i++] = brake_delay;
	message[i++] = reverse_speed;
	transmit_CAN(DM_SETTINGS_CAN, 0, 6, message);
	_delay_ms(10);
	i = 1;
	message[i++] = DM_SETTINGS_2;
	message[i++] = slow_speed;
	message[i++] = fast_speed;
	message[i++] = current_rating;
	message[i++] = max_current;
	message[i++] = overload_time;
	transmit_CAN(DM_SETTINGS_CAN, 0, 7, message);
	_delay_ms(10);
}

void update_DM_accel(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = acceleration;
	transmit_CAN(ACCELERATION_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_deaccel(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = deceleration;
	transmit_CAN(DECELERATION_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_braekdelay(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = brake_delay;
	transmit_CAN(BREAK_DELAY_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_slow_speed(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = slow_speed;
	transmit_CAN(SLOW_SPEED_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_fast_speed(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = fast_speed;
	transmit_CAN(FAST_SPEED_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_reverse_speed(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = reverse_speed;
	transmit_CAN(REVERSE_SPEED_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_CURRENT_RATING(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = current_rating;
	transmit_CAN(CURRENT_RATING_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_MAX_CURRENT(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = max_current;
	transmit_CAN(MAX_CURRENT_CAN, 0, 1, message);
	_delay_ms(10);
}
void update_DM_OVER_LOAD_TIME(void)
{
	uint8_t message[8];
	//uint8_t i = 0;
	message[0] = overload_time;
	transmit_CAN(OVER_LOAD_TIME_CAN, 0, 1, message);
	_delay_ms(10);
}


//Send confirmation of TOP error received
void acknowledge_top(void)
{
	uint8_t message[1];
	message[0] = 0;
	transmit_CAN(TOP_CAN, 1, 0, message);
	_delay_ms(10);
}

//Ask for pot calibration info
void send_pot_calibrating(void)
{
	uint8_t message[1];
	message[0] = pot_calibrating;
	transmit_CAN(POT_CALIBRATION_CAN, 0, 1, message);
	_delay_ms(10);
}
//Ask for pot value FROM A3 board
void Request_pot_value(void)
{
	uint8_t message[1];
	message[0] = 0; //IVA001: Send a valid value 
	//message[0] = pot_calibrating;
	transmit_CAN(REQUEST_POT_VALUE_CAN, 0, 1, message);
	_delay_ms(10);
}

//Send an alarm to inform other boards
void send_alarm(uint8_t error)
{
	uint8_t message[1];
	message[0] = error;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(10);
}
//Send an alarm to inform A3 CAN communication error to A2 board to disable all functions
void send_emergency_off(void)
{
	uint8_t message[1];
	message[0] = 0; //A1_CAN;//IVA001: Send a valid value
	transmit_CAN(EMERGENCY_OFF_CAN, 0, 1, message);
	_delay_ms(10);
}

//Send preliminary ACK to other
void send_CAN_ACK(void)
{
	uint8_t message[1];
	message[0] = A1_CAN;
	transmit_CAN(ACK_CAN, 0, 1, message);
	_delay_ms(10);
}

//Send serial numbers to other boards to update them
void send_serial_num(uint8_t board, char *number)
{
	uint8_t can_data_type;
	switch (board)
	{
	case A1_CAN: can_data_type = A1_SERIAL_NUM_CAN; break;
	case A2_CAN: can_data_type = A2_SERIAL_NUM_CAN; break;
	case A3_CAN: can_data_type = A3_SERIAL_NUM_CAN; break;

	default: return;
	}

	uint8_t i = 0;
	uint8_t message[8];

	message[i++] = 1;
	message[i++] = number[0];
	message[i++] = number[1];
	message[i++] = number[2];
	message[i++] = number[3];
	message[i++] = number[4];
	message[i++] = number[5];

	transmit_CAN(can_data_type, 0, 7, message);
	_delay_ms(10);

	i = 0;
	message[i++] = 2;
	message[i++] = number[6];
	message[i++] = number[7];
	message[i++] = number[8];
	message[i++] = number[9];
	message[i++] = number[10];
	message[i++] = number[11];

	transmit_CAN(can_data_type, 0, 7, message);
	_delay_ms(10);

	i = 0;
	message[i++] = 3;
	message[i++] = number[12];
	message[i++] = number[13];

	transmit_CAN(can_data_type, 0, 3, message);
	_delay_ms(10);
}

//Ask for serial numbers from A2
void request_A2_serial_num(void)
{
	uint8_t message[1];
	message[0] = 0;
	transmit_CAN(A2_SERIAL_NUM_CAN, 0, 1, message);
	_delay_ms(100);
}
//Ask for serial numbers from A3
void request_A3_serial_num(void)
{
	uint8_t message[1];
	message[0] = 0;
	transmit_CAN(A3_SERIAL_NUM_CAN, 0, 1, message);
	_delay_ms(100);
}

//Instruct other boards to reset
void send_reset_CAN(void)
{
	uint8_t message[1];
	message[0] = 0; //IVA001: Send a valid value
	transmit_CAN(RESET_DEVICE_CAN_A1, 0, 1, message);
	_delay_ms(10);
}

//Tell A2 to erase its SD card
void erase_SD_card(void)
{
	uint8_t message[1];
	message[0] = 0; //IVA001: Send a valid value
	transmit_CAN(ERASE_SD_CAN, 0, 1, message);
}

void update_A2_brush_slow_start(void)
{
	uint8_t message[1];
	message[0] = brush_slow_start;
	transmit_CAN(BRUSH_SLOW_START_CAN, 0, 1, message);
	_delay_ms(10);
}

void update_A2_vaccum_slow_start(void)
{
	uint8_t message[1];
	message[0] = vacuum_slow_start;
	transmit_CAN(VACCUM_SLOW_START_CAN, 0, 1, message);
	_delay_ms(10);
}

void update_A2_presweep_slow_start(void)
{
	uint8_t message[1];
	message[0] = pre_swp_slow_start;
	transmit_CAN(PRESWEEP_SLOW_START_CAN, 0, 1, message);
	_delay_ms(10);
}