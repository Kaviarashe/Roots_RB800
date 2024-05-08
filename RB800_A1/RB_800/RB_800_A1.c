/*
 * RB_800_A1.c
 *
 * Created: 7/1/2015 10:11:45 AM
 *  Author: Anurag Vaddi Reddy
 * IVA002: Renamed the file for easy identification
 */
 //int count=0;
 //int a=0, b=0;
#include "RB_800.h"

int main(void)
{
#if PIN_TEST
	PORTA_DIR = 0xFF;
	PORTB_DIR = 0xFF;
	PORTC_DIR = 0xFF;
	PORTD_DIR = 0xFF;
	PORTE_DIR = 0xFF;
	PORTF_DIR = 0xFF;
	PORTH_DIR = 0xFF;
	PORTJ_DIR = 0xFF;
	PORTK_DIR = 0xFF;
	PORTQ_DIR = 0xFF;
	PORTR_DIR = 0xFF;
	while (1)
	{
		PORTA_OUTSET = 0xFF; _delay_ms(1); PORTA_OUTCLR = 0xFF; _delay_ms(1);
		PORTB_OUTSET = 0xFF; _delay_ms(1); PORTB_OUTCLR = 0xFF; _delay_ms(1);
		PORTC_OUTSET = 0xFF; _delay_ms(1); PORTC_OUTCLR = 0xFF; _delay_ms(1);
		PORTD_OUTSET = 0xFF; _delay_ms(1); PORTD_OUTCLR = 0xFF; _delay_ms(1);
		PORTE_OUTSET = 0xFF; _delay_ms(1); PORTE_OUTCLR = 0xFF; _delay_ms(1);
		PORTF_OUTSET = 0xFF; _delay_ms(1); PORTF_OUTCLR = 0xFF; _delay_ms(1);
		PORTH_OUTSET = 0xFF; _delay_ms(1); PORTH_OUTCLR = 0xFF; _delay_ms(1);
		PORTJ_OUTSET = 0xFF; _delay_ms(1); PORTJ_OUTCLR = 0xFF; _delay_ms(1);
		PORTK_OUTSET = 0xFF; _delay_ms(1); PORTK_OUTCLR = 0xFF; _delay_ms(1);
		PORTQ_OUTSET = 0xFF; _delay_ms(1); PORTQ_OUTCLR = 0xFF; _delay_ms(1);
		PORTR_OUTSET = 0xFF; _delay_ms(1); PORTR_OUTCLR = 0xFF; _delay_ms(1);
	}
#endif

	//Reset LCD and CAN controller
	PORTF_DIRSET = (1 << LCD_CAN_RST);
	PORTF_OUTCLR = (1 << LCD_CAN_RST);
	_delay_ms(10);
	PORTF_OUTSET = (1 << LCD_CAN_RST);

	initialize();

	IDLE_LOOP
	{
		//PORTF_OUTSET = (1 << 3);
		if (ack_counter >= 6 && !can_functioning && !printing_info)
		{
			if (!A2_ack_received & !A3_ack_received)
			{
				if ((CanError & 0x01) == 0)
				{
					lcd_write_new_P(PSTR("E04 - NO CAN COMM"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("CHECK WIRING"));
					CanError |= 0x01;
				}
			}
			else if (!A2_ack_received)
			{
				if (((CanError >> 1) & 0x01) == 0)
				{
					lcd_write_new_P(PSTR("E04 - NO A2 CAN COMM"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("CHECK WIRING"));
					CanError |= (0x01 << 1);
				}
			}
			else if (!A3_ack_received)
			{
				if (((CanError >> 2) & 0x01) == 0)
				{
					lcd_write_new_P(PSTR("E04 - NO A3 CAN COMM"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("CHECK WIRING"));
					_delay_ms(100);
					send_emergency_off();
					_delay_ms(100);
					CanError |= (0x01 << 2);
				}
			}
			else
			{
				if (((CanError >> 3) & 0x01) == 0)
				{
					lcd_write_new_P(PSTR("E04 - NO CAN COMM"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("CHECK WIRING"));
					CanError |= (0x01 << 3);
				}
			}
			//cli();
			//while(1){}
			if (!CQRx_queueIsEmpty())
			{
				CQRx_dequeue();
				reset_CAN_error();
			}
		}
		else
		{//
			if (CanError)
			{
				print_menu = TRUE;
				CanError = 0;
			}

			if (sd_card_unavailable)
			{
				if (reset_timer < 5)
				{
					lcd_write_new_P(PSTR("E19 - SD CARD"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("UNAVAILABLE"));

					cli();
					WHILE_TRUE();
					//lcd_update_pause = 5;
				}
				else
				{
					lcd_write_new_P(PSTR("E20 - SD CARD"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("FAIL"));
					L_SLO_OFF;
					L_HL_OFF;
					L_EP_OFF;
					L_BR_OFF;
					L_RV_OFF;
					L_VAC_OFF;
					L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF;
					L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF;

					cli();
					WHILE_TRUE();
					//lcd_update_pause = 15;
				}
			}
			if (back_safety_alert || pedal_switch_error || manual_switch_error)
			{
				if (!CQRx_queueIsEmpty())
				{
					CQRx_dequeue();
					reset_CAN_error();
				}
				continue;
			}

			//Check if any switches have been pressed
			for (int num = 0; num < NUM_SWITCHES; num++)
			{
				if (switch_pressed[num])
				{
					/*if(printing_info)
					{
						lcd_clear_screen();
						printing_info = FALSE;
					}
					else*/
					//	lcd_update_pause = 3;
					//Clear switch_pressed
					switch_pressed[num] = FALSE;
					//Perform action depending on switch
					if (!printing_info && !fuse_error && !brake_seat_switch_error && !bm_overcurrent_error)
					{
						switch (num)
						{
						case S_SLD_NUM: change_solenoid_valve_state();		break;
						case S_HL_NUM: change_headlights_state();			break;
						case S_PU_NUM: increase_water_pump_level();			break;
						case S_SLO_NUM: change_speed();						break;
						case S_ECO_NUM: change_eco_mode();					break;
						case S_TR_NUM: change_transport_mode();				break;
						case S_EP_NUM: change_extra_pressure_state();		break;
						case S_DU_NUM: increase_detergent_pump_level();		break;
						case S_RV_NUM: change_forward_reverse_state();		break;
						case S_DD_NUM: decrease_detergent_pump_level();		break;
						case S_VAC_NUM: change_vacuum_state();				break;
						//case S_HRN_NUM: change_horn_state();				break;
						case S_BR_NUM: change_brush_state();				break;
						case S_ATO_NUM: change_auto_state();				break;
						case S_PD_NUM: decrease_water_pump_level();			break;

						default:  break;
						}

						//change_on_value =TRUE;
						update_A2_settings();
						periodic_send = FALSE;
						periodic_count = 0;
					}
					//Start the reset counter to return to menu screen
					start_input_reset();
				}
			}

			//If new auto flag is set store current settings to eeprom
			if (new_auto_flag)
			{
				change_auto_settings();
				L_ATO_ON;

				lcd_write_new_P(PSTR("AUTO MODE   "));
				lcd_set_cursor(1, 0);
				lcd_write_P(PSTR("CHANGED     "));
				lcd_update_pause = 2;
				//Start the reset counter to return to menu screen
				start_input_reset();

				new_auto_flag = FALSE;
			}
			if (update_hour)
			{
				write_hours();
				update_hour = FALSE;
			}
			//IVA002: Fix for EEPROM corruption
			if (update_minutes)
			{
				write_minutes();
				update_minutes = false;
			}

			if (!(PORTK_IN & (1 << S_HRN)) && horn_off)
			{
				//horn=ON;
				change_horn_state();
				//_delay_ms(50);
				update_A2_settings();
				periodic_send = FALSE;
				periodic_count = 0;
				//change_on_value =TRUE;
				horn_off = FALSE;

			}
			else if ((PORTK_IN & (1 << S_HRN)) && !horn_off)
			{
				//horn=OFF;
				change_horn_state();
				lcd_update_pause = 3;
				//_delay_ms(50);
				update_A2_settings();
				periodic_send = FALSE;
				periodic_count = 0;
				//change_on_value =TRUE;
				horn_off = TRUE;
			}
			/*if(sd_card_unavailable)
			{
				if(reset_timer < 5)
				{
					lcd_write_new_P(PSTR("E19 - SD CARD"));
					lcd_set_cursor(1,0);
					lcd_write_P(PSTR("UNAVAILABLE"));
					lcd_update_pause = 5;
				}
				else
				{
					lcd_write_new_P(PSTR("E20 - SD CARD"));
					lcd_set_cursor(1,0);
					lcd_write_P(PSTR("FAIL"));
					lcd_update_pause = 15;
				}
			}*/

			while (!CQRx_queueIsEmpty())
			{
				CQRx_dequeue();
				reset_CAN_error();
			}

			if (printing_info)
			{
				if (printing_info_counter % 2 == 0 && lcd_update_pause == 0)
					print_info();
				continue;
			}

			//If print menu flag is raised print the menu screen on the lcd screen
			if (print_menu)
			{
				if (top_error)
				{
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
				else if (bm_overcurrent_error)
				{
					lcd_write_new_P(PSTR("E22 - BM OVERLOAD"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("TURN OFF KEY"));
				}
				else if (fault_error)
				{
					lcd_write_new_P(PSTR("E21 - 8701 FAULT"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("TURN OFF KEY"));
				}
				else if (fuse_error)
				{
					lcd_write_new_P(PSTR("E01 - FUSE BLOWN"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("TURN OFF KEY"));
				}
				else if (overvoltage_error)
				{
					lcd_write_new_P(PSTR("E13 - OVERVOLTAGE"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("TURN OFF KEY"));
				}
				else if (pedal_switch_error)
				{
					lcd_write_new_P(PSTR("E10 - RELEASE"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("ACCELERATOR PEDAL"));
				}
				else if (manual_switch_error)
				{
					lcd_write_new_P(PSTR("E15 -EMERGENCY DRIVE"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("SW-OUT OF NEUTRAL"));
				}
				else if (brake_seat_switch_error)
				{
					lcd_write_new_P(PSTR("E14 - BRAKE/SEAT"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("SWITCH OPEN"));
				}
				else if (lcd_update_pause == 0)
				{
					lcd_menu();
					request_battery_can();
				}
				print_menu = FALSE;
			}

			//If update menu flag is raised only print the battery and hour
			if (update_menu && lcd_update_pause == 0)
			{
				if (!pedal_switch_error && !manual_switch_error && !overvoltage_error && !top_error && !fuse_error && !brake_seat_switch_error && !fault_error && !bm_overcurrent_error)
				{
					lcd_menu_update();
				}

				//while(a==1 || b==0)
				//{					
				request_battery_can();
				if (periodic_send == TRUE)
				{
					change_on_value = FALSE;
					periodic_send = FALSE;
					periodic_count = 0;
					update_A2_settings();
					update_menu = FALSE;
				}
				//update_A2_settings();
				//update_menu = FALSE;
				/*if(b==0)
				{
					break;
				}*/
				//}
			}

			/*if(count > 50)
			{
				a=1;
				b=1;
			}
			else
			{
						count++;
			}*/
			//If battery is low print battery low
			if (print_battery_low && battery_status != BATTERY_UNKNOWN && lcd_update_pause == 0)
			{
				lcd_set_cursor(0, 0);
				lcd_write_P(PSTR("BATTERY LOW!"));
				LCD_LED_TGL;
				print_battery_low = FALSE;
			}

			//If battery is too low to continue operating turn off motors and print charge battery
			if (print_battery_off && !print_menu && battery_status != BATTERY_UNKNOWN)
			{
				if (motors_status != MOTORS_OFF)
				{
					turn_off_motors();

					motors_status = MOTORS_OFF;
					cli();
					eeprom_busy_wait();
					eeprom_update_byte((uint8_t *)EEP8_MOTORS_STATUS_ADDR, (uint8_t)MOTORS_OFF);
					sei();
				}

				if (motors_status == MOTORS_OFF && lcd_update_pause == 0)
				{
					lcd_set_cursor(0, 0);
					lcd_write_P(PSTR("E16 - CHARGE"));
					lcd_set_cursor(1, 0);
					lcd_write_P(PSTR("THE BATTERY!"));
					LCD_LED_TGL;
					print_battery_off = FALSE;
				}
			}
		}//
		//PORTF_OUTCLR = (1 << 3);
	}
	return 0;
}

void initialize(void)
{
	change_on_value = TRUE;
	//For sending CAN data periodically
	periodic_send = FALSE;

	PORTA_DIRSET = (1 << LCD_LED);
	PORTA_OUTSET = (1 << LCD_LED);

	initialize_32Mhz();
	//1 second delay to make sure power has settled
	//Without this delay EEPROM could get corrupted when initially reading
	_delay_ms(500); //	_delay_ms(2000);
	CQ_initialize();

	//Initialize the TWI master module using the TWIE port
	TWI_MasterInit(&twiMaster,
		&TWIE,
		TWI_MASTER_INTLVL_LO_gc,
		TWI_BAUDSETTING);
	horn_off = TRUE;
	initialize_SPI();
	initialize_timers();
	initialize_runtime_reset_timer();
	initialize_keypad_settings();
	initialize_ADC();

	initialize_CAN();

	initialize_RB_settings();

	//Enable low level interrupts
	PMIC_CTRL = 0x07;
	//Enable global interrupts
	sei();

#if TEST_LED
	L_P2_ON
		L_HL_ON
		L_P5_ON
		L_P4_ON
		L_P3_ON
		L_SLO_ON
		//L_EXT_ON	
		L_ECO_ON
		L_EP_ON
		L_TR_ON
		L_D5_ON
		L_D4_ON
		L_D3_ON
		L_D2_ON
		L_D1_ON
		L_VAC_ON
		L_RV_ON
		L_BR_ON
		L_ATO_ON
		L_P1_ON
		;
#endif

	initialize_Screen();
	lcd_create_CGRAM();
	_delay_ms(50);

#ifndef  __SIM__
	send_reset_CAN();
#endif // ! __SIM__

	motor_info_sent = FALSE;

	/*if(S_ECO_LOW && S_TR_LOW)
	{
		send_motor_info();
		initialize_keypad();
		start_test_menu();
	}*/

	if (S_RV_LOW && S_SLO_LOW)
	{
		//send_motor_info();
		initialize_keypad();
		start_s_menu();
	}
	if (S_HL_LOW && S_HRN_LOW && S_RV_LOW)
	{
		//send_motor_info();
		initialize_keypad();
		start_u_menu();
	}
	//send_motor_info();
	initialize_keypad();
	_delay_ms(50);

	printing_info_counter = 0;
	printing_info = TRUE;
	printed_info = FALSE;
	print_info();
}


//Print out RB800 information
void print_info(void)
{

	lcd_write_new(MACHINE_DETAIL);
	_delay_ms(1000);

	if (battery_type == LEAD)
		lcd_write_new_P(PSTR("BATTERY TYPE: LEAD"));
	else if (battery_type == AGM)
		lcd_write_new_P(PSTR("BATTERY TYPE: AGM"));
	else
		lcd_write_new_P(PSTR("BATTERY TYPE: GEL"));
	lcd_set_cursor(1, 0);
	lcd_write(serial_num);
	_delay_ms(500);

	lcd_write_new(A1_PCB);
	lcd_set_cursor(1, 0);
	lcd_write(A1_FIRMWARE);
	_delay_ms(500);

	lcd_write_new(A2_PCB);
	lcd_set_cursor(1, 0);
	lcd_write(A2_FIRMWARE);
	if (!can_functioning)
	{
		send_CAN_ACK();
		ack_counter++;
	}
	_delay_ms(500);


	lcd_write_new(A3_PCB);
	lcd_set_cursor(1, 0);
	lcd_write(A3_FIRMWARE);
	if (!can_functioning)
	{
		send_CAN_ACK();
		ack_counter++;
	}
	_delay_ms(500);

	printing_info = FALSE;
	print_menu = TRUE;



	/*if(!printed_info && printing_info_counter == 0)
	{

		lcd_write_new(MACHINE_DETAIL);
		//lcd_set_cursor(1,0);
		//lcd_write(serial_num);
		printed_info = TRUE;
	}
	else if(!printed_info && printing_info_counter == 2)
	{
		if(battery_type == LEAD)
			lcd_write_new_P(PSTR("BATTERY TYPE: LEAD"));
		else if(battery_type == AGM)
			lcd_write_new_P(PSTR("BATTERY TYPE: AGM"));
		else
			lcd_write_new_P(PSTR("BATTERY TYPE: GEL"));
		lcd_set_cursor(1,0);
		lcd_write(serial_num);

		printed_info = TRUE;
	}
	else if(!printed_info && printing_info_counter == 4)
	{
		lcd_write_new(A1_PCB);
		lcd_set_cursor(1,0);
		lcd_write(A1_FIRMWARE);

		printed_info = TRUE;
	}
	else if(!printed_info && printing_info_counter == 6)
	{
		lcd_write_new(A2_PCB);
		lcd_set_cursor(1,0);
		lcd_write(A2_FIRMWARE);

		if(!can_functioning)
		{
			send_CAN_ACK();
			ack_counter++;
		}
		printed_info = TRUE;
	}
	else if(!printed_info && printing_info_counter == 8)
	{
		lcd_write_new(A3_PCB);
		lcd_set_cursor(1,0);
		lcd_write(A3_FIRMWARE);

		if(!can_functioning)
		{
			send_CAN_ACK();
			ack_counter++;
		}
		printed_info = TRUE;
	}
	else if(printing_info_counter == 10)
	{
		printing_info = FALSE;
		print_menu = TRUE;
	}*/
}

//IVA002: Removed stale code
//void check_CAN(void)
//{
//	uint8_t int_flags;
//	int_flags = read_byte_CAN(CANINTF);
//
//	//If either error interrupt flag is raised, notify the user
//	if (int_flags & ERRIF)
//	{
//		lcd_write_new_P(PSTR("CAN ERROR!!! - 1"));
//		lcd_update_pause = 5;
//	}
//	if (int_flags & MERRF)
//	{
//		lcd_write_new_P(PSTR("CAN ERROR!!! - 2"));
//		lcd_update_pause = 5;
//	}
//	//If either receive buffer is full read the data and perform accordingly
//	if (int_flags & RXB0_FULL)
//	{
//		uint8_t data[8];
//		//Read only bits 3-0 of RXB0DLC to get the data length code
//		uint8_t data_length = read_byte_CAN(RXB0DLC) & 0x0F;
//		read_bytes_CAN(RXB0D0, data, data_length);
//
//		uint8_t setting_arr[2];
//		read_bytes_CAN(RXB0SIDH, setting_arr, 2);
//		uint16_t setting = (setting_arr[0] << 3) + (setting_arr[1] >> 5);
//
//		processCAN(setting, data);
//	}
//	if (int_flags & RXB1_FULL)
//	{
//		uint8_t data[8];
//		uint8_t data_length = read_byte_CAN(RXB1DLC) & 0x0F;
//		read_bytes_CAN(RXB1D0, data, data_length);
//
//		uint8_t setting_arr[2];
//		read_bytes_CAN(RXB1SIDH, setting_arr, 2);
//		uint16_t setting = (setting_arr[0] << 3) + (setting_arr[1] >> 5);
//
//		processCAN(setting, data);
//	}
//
//	//Clear the flags
//	uint8_t clear[1] = { 0x00 };
//	write_bytes_CAN(CANINTF, clear, 1);
//
//	can_flag = FALSE;
//}
//

//TWIE Master Interrupt vector
ISR(TWIE_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}