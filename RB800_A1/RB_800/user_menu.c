/*
 * user_menu.c
 *
 * Created: 8/24/2016 1:55:12 PM
 *  Author: Anurag Vaddi Reddy
 */

#include "user_menu.h"

 //Variables that can be used by the menu
uint8_t cursor;
char temp[21];

void user_menu_next(void)
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
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;

		/*-------------------------------------------------------------------------------*/

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
	case SLOW_SPEED_VAL_MENU:
		slow_speed = current_value;
		update_DM_settings();
		//eeprom_update_byte((uint8_t *) EEP8_SLOW_SPEED, current_value);
		lcd_write_new_P(PSTR("SLOW SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

	case FAST_SPEED_VAL_MENU:
		fast_speed = current_value;
		update_DM_settings();
		//eeprom_update_byte((uint8_t *) EEP8_FAST_SPEED, current_value);
		lcd_write_new_P(PSTR("FAST SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

		/*------------------------------------------------------------------------------------------------------------------------------*/
		//% of max speed allowed when in reverse
	case REVERSE_SPEED_MENU:
		current_menu = REVERSE_SPEED_VAL_MENU;
		current_value = reverse_speed;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;
		/*------------------------------------------------------------------------------------------------------------------------------*/
	case REVERSE_SPEED_VAL_MENU:
		reverse_speed = current_value;
		update_DM_settings();
		//eeprom_update_byte((uint8_t *) EEP8_REVERSE_SPEED, current_value);
		lcd_write_new_P(PSTR("REVERSE SPEED SET"));
		_delay_ms(2000);
		clear_switch_pressed();
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;
		/******************************************************************************************************************************/
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

	case POT_VALUE_MENU:
		current_menu = POT_MENU;
		Request_pot_value();
		break;

		//Begin recalibration
	case POT_RECALIBRATION_MENU:
		break;

	case POT_MENU:
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

void user_menu_back(void)
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

	case SLOW_SPEED_VAL_MENU:
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

	case FAST_SPEED_VAL_MENU:
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

	case REVERSE_SPEED_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;
	case REVERSE_SPEED_VAL_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;

		/******************************************************************************************************************************/
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

	case POT_VALUE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

	case POT_MENU:
		current_menu = POT_VALUE_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("VALUE"));
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

void user_menu_up(void)
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

	case FORWARD_SPEED_MENU:
		break;
	case REVERSE_SPEED_MENU:
		current_menu = FORWARD_SPEED_MENU;
		lcd_write_new_P(PSTR("FORWARD SPEED"));
		break;
	case SLOW_SPEED_MENU:
		break;

	case FAST_SPEED_MENU:
		current_menu = SLOW_SPEED_MENU;
		lcd_write_new_P(PSTR("SLOW SPEED"));
		break;

		/*--------------------------------------------------------------------------------*/

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
	case REVERSE_SPEED_VAL_MENU:
		if (current_value < MAX_REVERSE)
			current_value++;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;

		/******************************************************************************************************************************/
		/*--------------------------------------------------------------------------------*/

		//Choose type of pot and calibrate the pot
	case POTENTIOMETER_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;
	case POT_VALUE_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
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
	case ABOUT_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETINGS"));
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

void user_menu_down(void)
{
	switch (current_menu)
	{
	case PRE_MENU:
		current_menu = MOTOR_MENU;
		lcd_write_new_P(PSTR("MOTOR SETTINGS"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_MENU:
		current_menu = ABOUT_MENU;
		lcd_write_new_P(PSTR("ABOUT"));
		break;

		/*-------------------------------------------------------------------------------*/

	case FORWARD_SPEED_MENU:
		current_menu = REVERSE_SPEED_MENU;
		lcd_write_new_P(PSTR("REVERSE SPEED"));
		break;
	case REVERSE_SPEED_MENU:
		current_menu = POTENTIOMETER_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("SETTINGS"));
		break;
	case POTENTIOMETER_MENU:
		current_menu = POT_VALUE_MENU;
		lcd_write_new_P(PSTR("POTENTIOMETER"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("VALUE"));
		break;

	case SLOW_SPEED_MENU:
		current_menu = FAST_SPEED_MENU;
		lcd_write_new_P(PSTR("FAST SPEED"));
		break;

	case FAST_SPEED_MENU:
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
	case REVERSE_SPEED_VAL_MENU:
		if (current_value > MIN_REVERSE)
			current_value--;
		sprintf(temp, "%d%% OF MAX SPEED", current_value);
		lcd_write_new(temp);
		break;
		/*------------------------------------------------------------------------------------------------------*/
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

void start_u_menu(void)
{
	stop_second_timer();
	user_settings_menu_enable = TRUE;
	pot_calibrating = FALSE;

	ack_counter = 0;

	lcd_write_new_P(PSTR("CHECKING CAN BUS..."));
	_delay_ms(1000);
	while (!can_functioning)
	{
		if (ack_counter >= 5 && !can_functioning)
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

	//request_DM_settings();
	_delay_ms(200);
	send_pot_calibrating();
	_delay_ms(200);
	request_A2_serial_num();
	_delay_ms(200);
	clear_switch_pressed();
	request_A3_serial_num();
	_delay_ms(200);
	lcd_write_new_P(PSTR("EP - up  |RV - next"));
	lcd_set_cursor(1, 0);
	lcd_write_P(PSTR("BR - down|HL - back"));
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
				case S_EP_NUM: user_menu_up();	 break;
				case S_RV_NUM: user_menu_next(); break;
				case S_HL_NUM: user_menu_back(); break;
				case S_BR_NUM: user_menu_down(); break;

				default:  break;
				}
			}
		}
		if (current_menu == POT_MENU)
		{
			_delay_ms(10);
			Request_pot_value();
			_delay_ms(50);
			lcd_set_cursor(0, 0);
			sprintf(temp, "VALUE=%d.%03d  ", pot_value / 1000, pot_value % 1000);
			lcd_write(temp);
			lcd_set_cursor(1, 0);
			sprintf(temp, "        ");
			lcd_write(temp);
		}
		if (!CQRx_queueIsEmpty())
		{
			CQRx_dequeue();
			reset_CAN_error();
		}
	}
}