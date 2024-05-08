/*
 * timers.c
 *
 * Created: 7/9/2015 11:11:25 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "timers.h"
uint8_t test_value;

void initialize_timers(void)
{
	//10 ms timer
	//Debounce timer for switches
	//Enable timer 1 with /64 prescalar
	TCC1.CTRLA = 0x05;
	//Enable Compare A interrupt
	TCC1.INTCTRLB = 0x01;
	//Compare at count of 1600
	TCC1.CCA = 5000;
	//IVA002: Added for CAN Tx Queue
		//25 ms CAN TX timer
		//Enable timer 0 with /1024 prescalar
	TCD1.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCD1.INTCTRLB = 0x01;
	//	TCD1.CCA = 3130;
	TCD1.CCA = 781;
}

void initialize_runtime_reset_timer(void)
{
	runtimer_on = FALSE;

	//Second timer for the run time of the RB
	//Enable timer 0 with /1024 prescalar
	TCC0.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCC0.INTCTRLB = 0x01;
	TCC0.CCA = 31250;

	read_hours();
	read_minutes();

	//Turn on tracking device
	PORTA_DIRSET = (1 << TRK);
	TRK_ON;
}

void stop_second_timer(void)
{
	TCC0.CTRLA = 0;
}

//Change the duty cycle for the PWM output
void set_duty_cycle(uint8_t percent)
{
	TCC0.CCA = (400 * percent) / 100;
}

//Enabling the 32 Mhz clock will change the system clock to 8 Mhz
//The 32 Mhz clock goes through a /4 before the system clock
void initialize_32Mhz(void)
{
	//Enable the 32 Mhz clock
	OSC.CTRL |= 0x02;
	//Wait for the clock to stabilize
	while (!(OSC.STATUS & 0x02));
	//Protect the Io registers
	CCP = CCP_IOREG_gc;
	//Set the clock to the 32 Mhz clock
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
}

void write_hours(void)
{
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)EEP16_HOURS_ADDR, hours);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)EEP16_HOURS_CHKSM, ~hours);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)EEP16_HOURS_BKP, hours);
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)EEP16_HOURS_BKP_CHKSM, ~hours);
	eeprom_busy_wait();
}

void write_minutes(void)
{
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)EEP8_MINUTES_ADDR, minutes);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)EEP8_MINUTES_CHKSM, ~minutes);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)EEP8_MINUTES_BKP, minutes);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)EEP8_MINUTES_BKP_CKSM, ~minutes);
	eeprom_busy_wait();
}

void read_hours(void)
{
	uint16_t v1, v2;
	eeprom_busy_wait();
	v1 = eeprom_read_word((const uint16_t*)EEP16_HOURS_ADDR);
	eeprom_busy_wait();
	v2 = eeprom_read_word((const uint16_t*)EEP16_HOURS_CHKSM);
	if ((v1 ^ v2) == 0xFFFF)
	{
		hours = v1;
		return;
	}
	eeprom_busy_wait();
	v1 = eeprom_read_word((const uint16_t*)EEP16_HOURS_BKP);
	eeprom_busy_wait();
	v2 = eeprom_read_word((const uint16_t*)EEP16_HOURS_BKP_CHKSM);
	if ((v1 ^ v2) == 0xFFFF)
	{
		hours = v1;
		return;
	}

	hours = 0;
}

void read_minutes(void)
{
	uint8_t v1, v2;
	eeprom_busy_wait();
	v1 = eeprom_read_byte((uint8_t *)EEP8_MINUTES_ADDR);
	eeprom_busy_wait();
	v2 = eeprom_read_byte((uint8_t *)EEP8_MINUTES_CHKSM);
	if ((v1 ^ v2) == 0xFF)
	{
		minutes = v1;
		return;
	}
	eeprom_busy_wait();
	v1 = eeprom_read_byte((uint8_t *)EEP8_MINUTES_BKP);
	eeprom_busy_wait();
	v2 = eeprom_read_byte((uint8_t *)EEP8_MINUTES_BKP_CKSM);
	if ((v1 ^ v2) == 0xFF)
	{
		minutes = v1;
		return;
	}

	minutes = 0;
}


void start_input_reset(void)
{
	input_reset_flag = TRUE;
	input_reset_sec = 0;
}

//Each interrupt occurs at 1 second
ISR(SECOND_INT)
{
	//Reset counter
	TCC0.CNT = 0;

	/*if(!motor_info_sent)
	{
		send_motor_info();
	}*/

	if (!can_functioning && !printing_info)
	{
		send_CAN_ACK();
		ack_counter++;
	}

	if (top_error || back_safety_alert || fuse_error)
	{
		LCD_LED_TGL;
		//If input flag is set, count to some time and print the menu
		if (input_reset_flag == TRUE && (++input_reset_sec > INPUT_RESET_SEC))
		{
			input_reset_flag = FALSE;
			print_menu = TRUE;
		}
		return;
	}

	//Counter for the lifetime usage of the cleaner
	if (runtimer_on)
	{
		if (++seconds >= 60)
		{
			if (++minutes >= 60)
			{
				hours++;
				update_hour = TRUE;
				minutes = 0;
			}
			//IVA002: Fix for EEP corruption
			update_minutes = true;
			seconds = 0;
		}
	}

	//Checks if the horn button state is incorrect
	if ((S_HRN_LOW && horn == OFF) || (!S_HRN_LOW && horn == ON))
	{
		if (horn_check_failed >= 1)
		{
			switch_pressed[S_HRN_NUM] = TRUE;
			horn_check_failed = FALSE;
		}
		else
		{
			horn_check_failed++;
		}
	}

#ifndef  __SIM__
	//Checks if the extra pressure button state is incorrect
	if ((S_EP_LOW && extra_pressure == OFF) || (!S_EP_LOW && extra_pressure == ON))
	{
		if (ep_check_failed >= 2)
		{
			switch_pressed[S_EP_NUM] = TRUE;
			ep_check_failed = FALSE;
		}
		else
		{
			ep_check_failed++;
		}
	}
#endif // ! __SIM_

	//If input flag is set, count to some time and print the menu
	if (input_reset_flag == TRUE && (++input_reset_sec > INPUT_RESET_SEC))
	{
		input_reset_flag = FALSE;
		print_menu = TRUE;
	}

	//Blink indefinitely when tank is full
	if (tank_full || alarm_on)
	{
		LCD_LED_TGL;
	}

	//Blinks the brush led
	if (blink_brush_led)
	{
		L_BR_TGL;
	}

	//Blinks the vacuum led
	if (blink_vac_led)
	{
		L_VAC_TGL;

	}
	/*if (blink_sld_led)
	{
		L_SLD_TGL;
	}*/

	//Pauses the lcd screen from updating for a while so message can be read
	if (lcd_update_pause > 1)
	{
		lcd_update_pause--;
}
	else if (lcd_update_pause == 1)
	{
		lcd_update_pause--;
		print_menu = TRUE;
	}
	else
		update_menu = TRUE;

	if (motors_status == MOTORS_OFF && battery_status != BATTERY_UNKNOWN)
	{
		print_battery_off = TRUE;
	}
	//Clear screen first then print battery low
	else if (battery_status == BATTERY_LOW)
	{
		if (battery_low_counter < 2)
		{
			print_menu = TRUE;
			battery_low_counter++;
		}
		else
		{
			print_battery_low = TRUE;
		}
	}
	//Clear screen first then print battery off
	else if (battery_status == BATTERY_OFF)
	{
		battery_low_counter = 0;
		if (battery_off_counter < 2)
		{
			print_menu = TRUE;
			battery_off_counter++;
		}
		else
		{
			print_battery_off = TRUE;
		}
	}
	//Reset counters
	else
	{
		battery_low_counter = 0;
		battery_off_counter = 0;
	}

	if (printing_info && lcd_update_pause == 0)
	{
		printing_info_counter++;
		if (printing_info_counter % 2 == 0)
			printed_info = FALSE;
	}

	if (can_functioning && (can_functioning_timer++ > 3))
	{
		can_functioning = FALSE;
	}

	if (can_functioning && (A3_can_functioning_timer++ > 3))
	{
		can_functioning = FALSE;
		A3_ack_received = FALSE;
		ack_counter = 0;
	}

	if (reset_timer < 10)
		reset_timer++;
}

ISR(DEBOUNCE_INT)
{
	//PORTF_OUTSET = (1 << 3);
	//Extra pressure debounce
	if (!user_settings_menu_enable)// while not in user menu
	{
		if (ep_down)
			ep_down_counter++;

		if (ep_up)
		{
			if (ep_down_counter >= _750_MS)
			{
				switch_pressed[S_EP_NUM] = TRUE;
				ep_up = FALSE;
			}
			else
				ep_down_counter++;
		}
	}
	//Horn debounce
	if (hrn_down)
		hrn_down_counter++;

	if (hrn_up)
	{
		if (hrn_down_counter >= _150_MS)
		{
			switch_pressed[S_HRN_NUM] = TRUE;
			hrn_up = FALSE;
		}
		else
			hrn_down_counter++;
	}

	//Check all switches
	for (int num = 0; num < NUM_SWITCHES; num++)
	{
		//Check if switch has been pressed
		if (switch_down[num])
		{
			//Auto switch can be held down for some time to change auto mode
			//So it require special operation
			if (num == S_ATO_NUM)
			{
				//Count up to a certain time
				if (switch_counter[num] <= _50_MS)
				{
					//Increment the counter
					switch_counter[num]++;
				}
				else
				{
					//Auto switch can be held down for some time to set new auto mode
					if (!S_ATO_LOW)
					{
						//If held for long than 3 seconds
						if (switch_counter[num] < _3_SEC)
						{
							switch_down[num] = FALSE;
							switch_counter[num] = 0;
							switch_debounce[num] = FALSE;
							switch_pressed[num] = TRUE;
						}
						else
						{
							switch_down[num] = FALSE;
							switch_counter[num] = 0;
							switch_debounce[num] = FALSE;
						}
					}
					else
					{
						if (switch_debounce[num] == FALSE)
						{
							//Increment the counter
							switch_counter[num]++;
							if (switch_counter[num] >= _3_SEC)
							{
								switch_debounce[num] = TRUE;
								new_auto_flag = TRUE;
							}
						}

					}
				}
				continue;
			}

			//Increment the counter
			switch_counter[num]++;

			//After some time check if the switch is still low and has not already been pressed
			if (!switch_debounce[num] && !switch_pressed[num] && (switch_counter[num] > _50_MS) && switch_low(num))
			{
				switch_pressed[num] = TRUE;
				switch_debounce[num] = TRUE;
			}

			//After enough time passes allow the switch to be pushed again
			if (switch_counter[num] > _350_MS)
			{
				//Pump switches can be held to increase/decrease so they are reset after some time
				switch (num)
				{
				case S_PU_NUM:
					//If the switch has been let go simply reset
					if (!S_PU_LOW)
					{
						switch_down[num] = FALSE;
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					//If it's still being held down, wait for 750 ms before resetting
					//Do not reset switch_down since switch is already being held down
					else if (switch_counter[num] > _750_MS)
					{
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					break;

				case S_PD_NUM:
					if (!S_PD_LOW)
					{
						switch_down[num] = FALSE;
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					else if (switch_counter[num] > _750_MS)
					{
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					break;

				case S_DU_NUM:
					if (!S_DU_LOW)
					{
						switch_down[num] = FALSE;
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					else if (switch_counter[num] > _750_MS)
					{
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					break;

				case S_DD_NUM:
					if (!S_DD_LOW)
					{
						switch_down[num] = FALSE;
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					else if (switch_counter[num] > _750_MS)
					{
						switch_counter[num] = 0;
						switch_debounce[num] = FALSE;
					}
					break;

				default:
					switch_down[num] = FALSE;
					switch_counter[num] = 0;
					switch_debounce[num] = FALSE;
					break;
				}
			}
		}
	}

	periodic_count++;
	if (periodic_count > 1)
	{
		periodic_count = 0;
		periodic_send = TRUE;
	}

	//PORTF_OUTCLR = (1 << 3);	
	//Counter is reset to start timer from the beginning so compare interrupt can happen correctly
	TCC1.CNT = 0;
}

//IVA002: Added for CAN Tx Queue
ISR(CAN_TX_TIMER_INT)
{
	CQTx_dequeue();
	TCD1.CNT = 0;
}
