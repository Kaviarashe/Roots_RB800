/*
 * timers.c
 *
 * Created: 7/9/2015 11:11:25 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "timers.h"

void initialize_timers(void)
{

	//1 sec timer
	//Enable timer 0 with /1024 prescalar
	TCD0.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCD0.INTCTRLB = 0x01;
	TCD0.CCA = 31250;

	event_counter = FIRST_EVENT;

	//RTC timer
	//Get 1.024 kHz from external 32.768 kHz osc
	//Enable RTC
	CLK.RTCCTRL = 0b00000011;
	//use /1024 prescalar for 1 sec per count
	RTC.CTRL = 0b00000111;

//IVA002: Added for CAN Tx Queue
	//25 ms CAN TX timer
	//Enable timer 0 with /1024 prescalar
	TCC0.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCC0.INTCTRLB = 0x01;
	TCC0.CCA = 781;
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

void start_100_ms_timer(void)
{
	//100 ms timer
	//Enable timer 0 with /1024 prescalar
	TCE1.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCE1.INTCTRLB = 0x01;
	TCE1.CCA = 3130;
}

void stop_100_ms_timer(void)
{
	TCE1.CTRLA = 0;
	TCE1.INTCTRLB = 0;
}

void start_25_ms_vacuum_timer(void)
{
	//25 ms timer
	//Enable timer 0 with /1024 prescalar
	TCF0.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCF0.INTCTRLB = 0x01;
	TCF0.CCA = 781;
}

void start_25_ms_brush_timer(void)
{
	//25 ms timer
	//Enable timer 0 with /1024 prescalar
	TCC1.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCC1.INTCTRLB = 0x01;
	TCC1.CCA = 781;
}

void start_25_ms_pre_swp_timer(void)
{
	//25 ms timer
	//Enable timer 0 with /1024 prescalar
	TCD1.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCD1.INTCTRLB = 0x01;
	TCD1.CCA = 781;
}

void stop_25_ms_vacuum_timer(void)
{
	TCF0.CTRLA = 0;
	TCF0.INTCTRLB = 0;
}

void stop_25_ms_brush_timer(void)
{
	TCC1.CTRLA = 0;
	TCC1.INTCTRLB = 0;
}

void stop_25_ms_pre_swp_timer(void)
{
	TCD1.CTRLA = 0;
	TCD1.INTCTRLB = 0;
}

//Send runtimer status to A1
void send_runtime_timer(uint8_t status)
{
	runtimer_on = status;
	uint8_t message[1];
	message[0] = status;
	transmit_CAN(RUNTIMER_CAN, 0, 1, message);
}

//25 ms timers to slow start the motors

ISR(_25_MS_VACUUM_TIMER_INT)
{
	if (temp_vacuum < vacuum)
	{
		if (vacuum_slow_start != 0) //IVA001: Fix for div/0
		{
			temp_vacuum += ((float)vacuum) / vacuum_slow_start / 10.0;
			uint32_t pwm = ((uint32_t)temp_vacuum * 4000) / 100;
			TCE0.CCC = (uint16_t)pwm;
		}
	}
	else
	{
		stop_25_ms_vacuum_timer();
	}

	TCF0.CNT = 0;
}

ISR(_25_MS_BRUSH_TIMER_INT)
{
	if (temp_brush < brush)
	{
		if (brush_slow_start != 0)//IVA001: Fix for div/0
		{
			temp_brush += ((float)brush) / brush_slow_start / 10.0;
			uint32_t pwm = ((uint32_t)temp_brush * 4000) / 100;
			TCE0.CCA = (uint16_t)pwm;
			TCE0.CCB = (uint16_t)pwm;
		}
	}
	else
	{
		stop_25_ms_brush_timer();
	}

	TCC1.CNT = 0;
}

ISR(_25_MS_PRE_SWP_TIMER_INT)
{
	if (temp_pre_swp < presweep_dc)
	{
		if (pre_swp_slow_start != 0)//IVA001: Fix for div/0
		{
			temp_pre_swp += ((float)presweep_dc) / pre_swp_slow_start / 10.0;
			uint32_t pwm = ((uint32_t)temp_pre_swp * 4000) / 100;
			TCE0.CCD = (uint16_t)pwm;
			__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		}
	}
	else
	{
		stop_25_ms_pre_swp_timer();
	}

	TCD1.CNT = 0;
}

ISR(_100_MS_TIMER_INT)
{
	if (brush_sns_out_timer)
	{
		brush_sns_out_timer_count++;
		if (brush_sns_out_timer_count > 20)
		{
			brush_sns_out_timer = OFF;
			brush_sns_out_count = 0;

			brush_sns_out_timer_count = 0;
			//Reset the counter if the alarm was not trigger
			if (!send_brush_overcurrent_alarm)
			{
				brush_sns_out_count = 0;
			}
		}
	}
	if (top_debounce_timer)
	{
		//Check after 500 ms if the TOP pin is still low
		if (++top_debounce_count > 5)
		{
			if (PORTE_IN & (1 << TOP_PIN))
			{
				//Disable all 9201 devices
				PORTF_OUTSET = (1 << DISABLE_9201);

				//Disable all 8701 devices
				PORTE_OUTCLR = (1 << ENABLE_8701);

				water_pump_set(OFF, OFF);
				Solenoid_set(OFF);
				detergent_pump_set(OFF, OFF);

				send_top_flag = TRUE;
				log_top_event = TRUE;
			}
			top_debounce_timer = OFF;
		}
	}

	if (fuse_debounce_timer)
	{
		check_fuses();
		//Check after 500 ms if the fuse pin ratio is still less than 20
		if (++fuse_debounce_count > 5)
		{
			//Disable all 9201 devices
			PORTF_OUTSET = (1 << DISABLE_9201);

			//Disable all 8701 devices
			PORTE_OUTCLR = (1 << ENABLE_8701);

			water_pump_set(OFF, OFF);
			Solenoid_set(OFF);
			detergent_pump_set(OFF, OFF);

			send_fuse_flag = TRUE;
			fuse_debounce_timer = OFF;
		}
	}

	measure_current = TRUE;

	TCE1.CNT = 0;
}

ISR(SECOND_INT)
{
	second_timer_tick = TRUE;
	check_max_current = TRUE;
	//Counter for the lifetime usage of the cleaner
	if (runtimer_on)
	{
		if (++seconds >= 60)
		{
			if (++minutes >= 60)
			{
				hours++;
				minutes = 0;
			}
			update_max_current = TRUE;
			update_eeprom_statistics = TRUE;
			seconds = 0;

			//Update statistic counters for running motors/modes
			if (brush_state == BRUSH_RUNNING)
			{
				if (++brush_minutes >= 60)
				{
					brush_hours++;
					brush_minutes = 0;
				}
			}
			if (vacuum_state == VACUUM_RUNNING)
			{
				if (++vacuum_minutes >= 60)
				{
					vacuum_hours++;
					vacuum_minutes = 0;
				}
			}
			if (presweep_state == ON)
			{
				if (++presweep_minutes >= 60)
				{
					presweep_hours++;
					presweep_minutes = 0;
				}
			}
			if (eco_mode)
			{
				if (++eco_minutes >= 60)
				{
					eco_hours++;
					eco_minutes = 0;
				}
			}
			if (water_pump && brush_state == BRUSH_RUNNING)
			{
				if (++water_pump_minutes >= 60)
				{
					water_pump_hours++;
					water_pump_minutes = 0;
				}
			}
			if (detergent_pump && brush_state == BRUSH_RUNNING)
			{
				if (++detergent_pump_minutes >= 60)
				{
					detergent_pump_hours++;
					detergent_pump_minutes = 0;
				}
			}
		}
		update_current = TRUE;
	}

	if (vacuum_off_timer != OFF)
	{
		//30 seconds after vacuum switch is turned off, turn off vacuum motor
		if (++vacuum_off_timer > VACUUM_OFF_TIME)
		{
			vacuum_motor_set(OFF);
			vacuum_state = VACUUM_OFF;
			if (runtimer_on && accelerator == ACCEL_UP && horn == OFF && headlights == OFF)
			{
				send_runtime_timer(OFF);
			}
			update_vacuum_state();
			vacuum_off_timer = OFF;
		}
	}
	//If current flag has been set from 9201 driver
	if (is_1_flag | is_2_flag | is_3_flag | is_4_flag)
	{
		//First time around set the is_reset flag
		//Second time, turn off the 9201 devices
		//They will be turned back on in the main loop
		if (is_reset)
		{
			PORTF_OUTSET = (1 << DISABLE_9201);
		}
		else
		{
			is_reset = TRUE;
		}
	}

	if (brush_head_timer != OFF)
	{
		//Brush head will turn off completely after 10 seconds
		if (brush_head_timer++ > ACTUATOR_RESET_SEC)
		{
			BR_HD_OFF;
			brush_head_timer = OFF;
		}
	}

	if (squeegee_timer != OFF)
	{
		//Squeegee will stay off for 10 seconds before moving again
		if (squeegee_timer++ > ACTUATOR_RESET_SEC)
		{
			SQ_OFF;
			squeegee_timer = OFF;
		}
	}

	if (float_sensor_timer != OFF)
	{
		//Check if float sensor is on for the entire duration
		if (!(PORTE_IN & (1 << FLOAT_PIN)))
		{
			//If float sensor is on for 8 seconds
			if (float_sensor_timer++ > FLOAT_SNS_SEC)
			{
				send_float_flag = TRUE;
				tank_full = TRUE;
				float_sensor_timer = OFF;
				vacuum_motor_set(OFF);
			}
		}
		//Otherwise reset the float sensor timer
		else
		{
			float_sensor_timer = OFF;
		}
	}

	if (fault_timer)
	{
		//Check if fault is on for the entire duration
		if (!(PORTE_IN & (1 << FAULT_8701)))
		{
			fault_timer_count++;
			if (fault_timer_count > 5)
			{
				send_fault_flag = TRUE;
				fault_timer = OFF;
				//Disable all 9201 devices
				PORTF_OUTSET = (1 << DISABLE_9201);

				//Disable all 8701 devices
				PORTE_OUTCLR = (1 << ENABLE_8701);

				water_pump_set(OFF, OFF);
				Solenoid_set(OFF);
				detergent_pump_set(OFF, OFF);
			}
		}
		else
		{
			fault_timer = OFF;
		}
	}

	//Count up to 3 seconds to before resetting counter
	/*if(brush_sns_out_timer)
	{
		brush_sns_out_timer_count++;
		if(brush_sns_out_timer_count > 2)
		{
			brush_sns_out_timer = OFF;
			brush_sns_out_count = 0;

			brush_sns_out_timer_count=0;
			//Reset the counter if the alarm was not trigger
			if(!send_brush_overcurrent_alarm)
			{
				brush_sns_out_count = 0;
			}
					}
	}*/

	if (transport_timer != OFF)
	{
		if (!transport)
			transport_timer = OFF;

		//Some time after switching to transport turn off squeegee
		if (transport_timer == TRANSPORT_SQ_TIME)
		{
			if (squeegee_down)
			{
				SQ_UP;
				squeegee_down = FALSE;
				squeegee_timer = ON;
			}
		}
		//Some time after switching to transport turn off vacuum
		if (transport_timer++ == TRANSPORT_VAC_TIME)
		{
			vacuum_state = VACUUM_OFF;
			update_vacuum_state();
			vacuum_motor_set(0);
			transport_timer = OFF;
		}
	}

	//Reset timer to see if reset CAN communication should be obeyed
	if (reset_timer < 10)
		reset_timer++;

	TCD0.CNT = 0;
}

//IVA002: Added for CAN Tx Queue
ISR(CAN_TX_TIMER_INT)
{
	CQTx_dequeue();
	TCC0.CNT = 0;
}
