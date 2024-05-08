/*
 * timers.c
 *
 * Created: 6/15/2016 3:15:02 PM
 *  Author: Anurag Vaddi Reddy
 */
//IVA002: Adaptation for simulator

#include "timers.h"

void initialize_timers(void)
{
	//Second timer
	//Enable timer 0 with /1024 prescalar
	TCC0.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCC0.INTCTRLB = 0x01;
	TCC0.CCA = 31250;

	//Motor Update & e-Brake Timer
	//100 msec timer
	//Enable timer 0 with /256 prescalar
	TCC1.CTRLA = 0x06;
	//Enable Compare A interrupt
	TCC1.INTCTRLB = 0x01;
	TCC1.CCA = 12500;

//IVA002: Added for CAN Tx Queue
	//25 ms CAN TX timer
	//Enable timer 0 with /1024 prescalar
	TCD1.CTRLA = 0x07;
	//Enable Compare A interrupt
	TCD1.INTCTRLB = 0x01;
	TCD1.CCA = 781;

	overcurrent = FALSE;
	overcurrent_count = 0;
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

ISR(SECOND_INT)
{
	TCC0.CNT = 0;

	log_current_counter++;
	log_max_current_counter++;
	//pedal not functioning means check for 5 sec then indicate error to A1 board 
	if (pedal_5_sec_timer_on)
	{
		pedal_5_sec_timer++;
	}
	//The last current measurement was over the rated limit begin counting to see how long the overcurrent persists
	if (overcurrent)
	{
		//If the time spent during overcurrent surpasses the maximum set time for overcurrent
		//Stop the motor
		if (++overcurrent_count > overload_time)
		{
			emergency_stop_motor = PERMANENT;
			send_overcurrent_error();
		}
	}
	else
	{
		overcurrent_count = 0;
	}
	//Send pedal error if it occurs
	if (manual_mode && can_functioning == TRUE)
	{
		send_pedal_error(MANUAL_SWITCH_CLOSED);
		emergency_stop_motor = TRUE;
		electrobrake_counter = ON;
		REVERSE_BUZZER_OFF;
		pedal_level = OFF;
		pedal_state = OFF;
		update_pedal_state();
		manual_mode = FALSE;
	}
	else if (manual_mode && can_functioning == FALSE && direction == UNTESTED)
	{
		if (!(PORTA_IN & (1 << FWR_SW)))
		{
			manual_mode = TRUE;
			direction = FORWARD;
			SET_MOTOR_FORWARD;
			REVERSE_BUZZER_OFF;
		}
		else if (!(PORTA_IN & (1 << REV_SW)))
		{
			manual_mode = TRUE;
			direction = REVERSE;
			SET_MOTOR_REVERSE;
			REVERSE_BUZZER_ON;
		}
		else
		{
			direction = NEUTRAL;
			if (emergency_stop_motor != PERMANENT)
			{
				emergency_stop_motor = FALSE;
			}
		}
	}

	//Send key switch error if it occurs
	if (send_key_switch_error)
	{
		if ((PORTE_IN & (1 << KEY_SW)))
		{
			send_key_error(KEY_SWITCH_OPEN);
		}
		send_key_switch_error = FALSE;
	}

	//Send pedal switch error if it occurs
	if (send_pedal_switch_error)
	{
		if (!(PORTE_IN & (1 << PDL_SW)))
		{
			send_pedal_error(PEDAL_SWITCH_CLOSED);
		}
		send_pedal_switch_error = SENT;
	}

	//Give the CAN bus about 5 seconds to begin working
	if (can_test_timer != OFF && can_test_timer++ > 5)
	{
		if (can_functioning == UNTESTED)
			can_functioning = FALSE;
		can_test_timer = OFF;
	}

	if (reset_timer < 10)
		reset_timer++;
}

ISR(_100_MS_TIMER_INT)
{
	TCC1.CNT = 0;

	_100_ms_check++;

	//This counter counts the 100s of millseconds that have passed since the pedal was released
	if (motor_off_time <= ENERGY_RELEASE_TIME && pedal_state == OFF)
		motor_off_time++;

	//Turn on the electrobrake after some time defined in the settings
	if (electrobrake_counter)
	{
		electrobrake_counter++;
		if (electrobrake_counter > brake_delay)
		{
			electrobrake_counter = OFF;
			E_BRAKE_ON;
		}
	}

	if (!emergency_stop_motor)
	{
		if (back_safety_adjust)
		{
			if (direction == FORWARD)
			{
				//If the difference between the pedal level and the current pedal level is greater than the level increment
				//Increase the pedal level and pwm by the preset increment
				if (pedal_level < (4090 + fast_level_increment))
				{
					pedal_level += fast_level_increment;
					SET_MOTOR_SPEED(GET_MOTOR_SPEED() + pwm_increment);
				}
				//If the difference between the pedal level and the current pedal level is less than the level increment
				//Increase the pedal level and pwm to the current pedal level
				else if (pedal_level <= (4090 - PEDAL_LEVEL_HYSTERESIS) || 4090 > (4096 - PEDAL_LEVEL_HYSTERESIS))
				{
					pedal_level = 4090;
					SET_MOTOR_SPEED(pedal_level * fast_level_pwm_ratio + minimum_pwm);
				}
			}
			else
			{
				//If the difference between the pedal level and the current pedal level is greater than the level increment
				//Increase the pedal level and pwm by the preset increment
				if (pedal_level < (4090 + reverse_level_increment))
				{
					pedal_level += reverse_level_increment;
					SET_MOTOR_SPEED(GET_MOTOR_SPEED() + pwm_increment);
				}
				//If the difference between the pedal level and the current pedal level is less than the level increment
				//Increase the pedal level and pwm to the current pedal level
				else if (pedal_level <= (4090 - PEDAL_LEVEL_HYSTERESIS) || 4090 > (4096 - PEDAL_LEVEL_HYSTERESIS))
				{
					pedal_level = 4090;
					SET_MOTOR_SPEED(pedal_level * reverse_level_pwm_ratio + minimum_pwm);
				}
			}

			if (back_safety_adjust_counter++ > 10)
			{
				if (!(PORTE_IN & (1 << KEY_SW)))
				{
					emergency_stop_motor = PERMANENT;
					send_back_safety_error(BACK_SAFETY_UNRESOLVED);
				}
				else
				{
					back_safety_adjust = FALSE;
					send_back_safety_error(BACK_SAFETY_RESOLVED);
				}
			}
			return;
		}

		if (fr_transition == TRUE)
		{
			if (direction == REVERSE)
			{
				if (speed == FAST)
				{
					//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
					//Turn off the motor
					if (pedal_level <= fast_level_decrement)
					{
						pedal_level = 0;
						SET_MOTOR_SPEED(OFF);
						fr_transition = FALSE;
						REVERSE_BUZZER_ON;
						SET_MOTOR_REVERSE;
					}
					//If the difference between the pedal level and the current pedal level is greater than the level increment
					//Decrease the pedal level and pwm by the preset increment
					else if (pedal_level > (0 + fast_level_decrement))
					{
						pedal_level -= fast_level_decrement;
						SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
					}
				}
				else
				{
					//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
					//Turn off the motor
					if (pedal_level <= slow_level_decrement)
					{
						pedal_level = 0;
						SET_MOTOR_SPEED(OFF);
						fr_transition = FALSE;
						REVERSE_BUZZER_ON;
						SET_MOTOR_REVERSE;
					}
					//If the difference between the pedal level and the current pedal level is greater than the level increment
					//Decrease the pedal level and pwm by the preset increment
					else if (pedal_level > (0 + slow_level_decrement))
					{
						pedal_level -= slow_level_decrement;
						SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
					}
				}
			}
			else if (direction == FORWARD)
			{
				//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
				//Turn off the motor
				if (pedal_level <= reverse_level_decrement)
				{
					pedal_level = 0;
					SET_MOTOR_SPEED(OFF);
					fr_transition = FALSE;
					REVERSE_BUZZER_OFF;
					SET_MOTOR_FORWARD;
				}
				//If the difference between the pedal level and the current pedal level is greater than the level increment
				//Decrease the pedal level and pwm by the preset increment
				else if (pedal_level > (0 + reverse_level_decrement))
				{
					pedal_level -= reverse_level_decrement;
					SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
				}
			}
			return;
		}
	}

	//If the pedal switch turns off or the direction is set to neutral the motor will decelerate to 0
	if (pedal_state == OFF || direction == NEUTRAL || direction == UNTESTED || emergency_stop_motor || stop_motor == TRUE)
	{
		if (direction == REVERSE)
		{
			//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
			//Turn off the motor
			if (pedal_level <= reverse_level_decrement)
			{
				pedal_level = 0;
				SET_MOTOR_SPEED(OFF);
				if (stop_motor)
				{
					stop_motor = FALSE;
				}

			}
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			else if (pedal_level > reverse_level_decrement)
			{
				pedal_level -= reverse_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
		}
		else if (speed == FAST)
		{
			//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
			//Turn off the motor
			if (pedal_level <= fast_level_decrement)
			{
				pedal_level = 0;
				SET_MOTOR_SPEED(OFF);
				if (stop_motor)
				{
					stop_motor = FALSE;
				}
			}
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			else if (pedal_level > fast_level_decrement)
			{
				pedal_level -= fast_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
		}
		else
		{
			//If the current pedal level is less than the minimum pedal level and either the pedal level is already less than the minimum or within a level increment
			//Turn off the motor
			if (pedal_level <= slow_level_decrement)
			{
				pedal_level = 0;
				SET_MOTOR_SPEED(OFF);
				if (stop_motor)
				{
					stop_motor = FALSE;
				}
			}
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			else if (pedal_level > slow_level_decrement)
			{
				pedal_level -= slow_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
		}
		return;
	}

	//If the pedal level is less than the current pedal level
	if (pedal_level < curr_pedal_level)
	{
		if (direction == REVERSE)
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Increase the pedal level and pwm by the preset increment
			if (pedal_level < (curr_pedal_level + reverse_level_increment))
			{
				pedal_level += reverse_level_increment;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() + pwm_increment);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Increase the pedal level and pwm to the current pedal level
			else if (pedal_level <= (curr_pedal_level - PEDAL_LEVEL_HYSTERESIS) || curr_pedal_level >= (4096 - PEDAL_LEVEL_HYSTERESIS))
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * reverse_level_pwm_ratio + minimum_pwm);
			}
		}
		else if (speed == FAST)
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Increase the pedal level and pwm by the preset increment
			if (pedal_level < (curr_pedal_level + fast_level_increment))
			{
				pedal_level += fast_level_increment;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() + pwm_increment);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Increase the pedal level and pwm to the current pedal level
			else if (pedal_level <= (curr_pedal_level - PEDAL_LEVEL_HYSTERESIS) || curr_pedal_level > (4096 - PEDAL_LEVEL_HYSTERESIS))
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * fast_level_pwm_ratio + minimum_pwm);
			}
		}
		else
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Increase the pedal level and pwm by the preset increment
			if (pedal_level < (curr_pedal_level + slow_level_increment))
			{
				pedal_level += slow_level_increment;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() + pwm_increment);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Increase the pedal level and pwm to the current pedal level
			else if (pedal_level <= (curr_pedal_level - PEDAL_LEVEL_HYSTERESIS) || curr_pedal_level >= (4096 - PEDAL_LEVEL_HYSTERESIS))
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * slow_level_pwm_ratio + minimum_pwm);
			}
		}
	}
	//If the pedal level is greater than the current pedal level
	else if (pedal_level > curr_pedal_level)
	{
		if (direction == REVERSE)
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			if (pedal_level > (curr_pedal_level + reverse_level_decrement))
			{
				pedal_level -= reverse_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Decrease the pedal level and pwm to the current pedal level
			else if (pedal_level >= (curr_pedal_level + PEDAL_LEVEL_HYSTERESIS) && pedal_level > PEDAL_LEVEL_HYSTERESIS)
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * reverse_level_pwm_ratio + minimum_pwm);
			}
			else if (curr_pedal_level <= PEDAL_LEVEL_HYSTERESIS)
			{
				if (pedal_state == OFF)
				{
					pedal_level = OFF;
					SET_MOTOR_SPEED(minimum_pwm);
				}
			}
		}
		else if (speed == FAST)
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			if (pedal_level > (curr_pedal_level + fast_level_decrement))
			{
				pedal_level -= fast_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Decrease the pedal level and pwm to the current pedal level
			else if (pedal_level >= (curr_pedal_level + PEDAL_LEVEL_HYSTERESIS) && pedal_level > PEDAL_LEVEL_HYSTERESIS)
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * fast_level_pwm_ratio + minimum_pwm);
			}
			else if (curr_pedal_level <= PEDAL_LEVEL_HYSTERESIS)
			{
				if (pedal_state == OFF)
				{
					pedal_level = OFF;
					SET_MOTOR_SPEED(minimum_pwm);
				}
			}
		}
		else
		{
			//If the difference between the pedal level and the current pedal level is greater than the level increment
			//Decrease the pedal level and pwm by the preset increment
			if (pedal_level > (curr_pedal_level + slow_level_decrement))
			{
				pedal_level -= slow_level_decrement;
				SET_MOTOR_SPEED(GET_MOTOR_SPEED() - pwm_decrement);
			}
			//If the difference between the pedal level and the current pedal level is less than the level increment
			//Decrease the pedal level and pwm to the current pedal level
			else if (pedal_level >= (curr_pedal_level + PEDAL_LEVEL_HYSTERESIS) && pedal_level > PEDAL_LEVEL_HYSTERESIS)
			{
				pedal_level = curr_pedal_level;
				SET_MOTOR_SPEED(pedal_level * slow_level_pwm_ratio + minimum_pwm);
			}
			else if (curr_pedal_level <= PEDAL_LEVEL_HYSTERESIS)
			{
				if (pedal_state == OFF)
				{
					pedal_level = OFF;
					SET_MOTOR_SPEED(minimum_pwm);
				}
			}
		}

	}
}

//IVA002: Added for CAN Tx Queue
ISR(CAN_TX_TIMER_INT)
{
	CQTx_dequeue();
	TCD1.CNT = 0;
}
