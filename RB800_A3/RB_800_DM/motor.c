/*
 * motor.c
 *
 * Created: 6/16/2016 2:12:07 PM
 *  Author: Anurag Vaddi Reddy
 */
//IVA002: Adaptation for simulator done

#include "motor.h"


void initialize_motor(void)
{
	//Motor input setup
	PORTD_DIRSET = (1 << FR_DIR) | (1 << PWM);

	//Reverse buzzer setup
	PORTB_DIRSET = (1 << REV_BUZZ) | (1 << E_BRK);

	E_BRAKE_ON;
	REVERSE_BUZZER_OFF;

	//Motor Controller
	//Prescalar /8
	TCD0.CTRLA = 0x01;  //0x04

	//Enable pin A & B with single slope PWM
	TCD0.CTRLB = 0x23;
	//Set period to 1000 counts ~500Hz
	TCD0.PER = 4090;//4090;    
	SET_MOTOR_SPEED(0);

	//Accelerator switch and Brake/Seat switch setup
	PORTE.PIN0CTRL |= PORT_OPC_PULLUP_gc;
	PORTE.PIN3CTRL |= PORT_OPC_PULLUP_gc;
	PORTE.INT0MASK |= (1 << PDL_SW) | (1 << BRK_SEAT_SW);
	PORTE.INTCTRL |= PORT_INT0LVL_LO_gc;

	//Forward/reverse switch setup
	PORTA.PIN5CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.PIN6CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.PIN4CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.INT0MASK |= (1 << FWR_SW) | (1 << REV_SW) | (1 << TOP_PIN);
	PORTA.INTCTRL |= PORT_INT0LVL_LO_gc;
	//Back safety and slow switch setup
	PORTE.PIN2CTRL |= PORT_OPC_PULLUP_gc;
	PORTE.PIN1CTRL |= PORT_OPC_PULLUP_gc;
	PORTE.INT1MASK |= (1 << KEY_SW) | (1 << SLOW_SW);
	PORTE.INTCTRL |= PORT_INT1LVL_LO_gc;

	//DAC setup for DRV8701 current chopping Vref
	//Turn on channel 0 DAC and turn on DAC
	DACB.CTRLA = 0b00000101;
	//Single channel operation on DAC0
	DACB.CTRLB = 0b00000000;
	//Use 2.048V on Aref for reference voltage
	DACB.CTRLC = 0b00010000;
	//Setup DAC depending on max current 
	if (max_current >= 100)
		DACB.CH0DATA = 0xFFFF;
	else
		DACB.CH0DATA = (uint16_t)(((float)max_current * AV / R_SENSE_INV + v_off) / 2.048 * MAX_ADC_READING_COUNT);

	_delay_ms(200);

	direction = UNTESTED;
	//Check motor direction switch
	if (!(PORTA_IN & (1 << FWR_SW)) || !(PORTA_IN & (1 << REV_SW)))
	{
		manual_mode = TRUE;
	}
	else
	{
		manual_mode = FALSE;
	}
	speed = FAST;
	fr_transition = FALSE;
	stop_motor = FALSE;
	emergency_stop_motor = FALSE;
	back_safety_adjust = FALSE;
	//Check if the pedal is currently activated
	if ((PORTE_IN & (1 << KEY_SW)))
	{
#ifndef BENCH_TEST //IVA002: Supress the error in bench test
		send_key_switch_error = TRUE;
		emergency_stop_motor = TRUE;
#endif
	}

	if (!(PORTE_IN & (1 << PDL_SW)))
	{
#ifndef BENCH_TEST //IVA002: Supress the error in bench test
		send_pedal_switch_error = TRUE;
#endif
	}
	//Check if the key switch input is connected


	pedal_state = OFF;

	curr_pedal_level = OFF;


	//Calculate motor setting values
	if (fast_speed > 0 && slow_speed > 0 && reverse_speed > 0) //IVA001: Fix for div/0
	{
		fast_level_increment = (uint16_t)((float)ACCELERATION_LEVEL_INCREMENT / ((float)fast_speed / 100));
		slow_level_increment = (uint16_t)((float)ACCELERATION_LEVEL_INCREMENT / ((float)slow_speed / 100));
		reverse_level_increment = (uint16_t)((float)fast_level_increment / ((float)reverse_speed / 100));
		pwm_increment = ACCELERATION_PWM_INCREMENT;

		fast_level_decrement = (uint16_t)((float)DECELERATION_LEVEL_DECREMENT / ((float)fast_speed / 100));
		slow_level_decrement = (uint16_t)((float)DECELERATION_LEVEL_DECREMENT / ((float)slow_speed / 100));
		reverse_level_decrement = (uint16_t)((float)fast_level_decrement / ((float)reverse_speed / 100));
		pwm_decrement = DECELERATION_PWM_DECREMENT;

		fast_level_pwm_ratio = (float)LEVEL_PWM_RATIO * ((float)fast_speed / 100);
		slow_level_pwm_ratio = (float)LEVEL_PWM_RATIO * ((float)slow_speed / 100);
		reverse_level_pwm_ratio = (float)fast_level_pwm_ratio * ((float)reverse_speed / 100);

		minimum_pwm = (uint16_t)(4096 * ((float)MINIMUM_PWM_PERCENTAGE / 100));
		max_reverse_pwm = (uint16_t)(4096 * ((float)reverse_speed / 100));

		low_deadband_level = pot_low_deadband * 2; // 2 = (4096 count / 2048 mV)
		high_deadband_level = pot_high_deadband * 2; // 2 = (4096 count / 2048 mV)
		deadband_level_difference = high_deadband_level - low_deadband_level;
	}
	pedal_level = OFF;
	electrobrake_counter = OFF;
	motor_off_time = ENERGY_RELEASE_TIME + 1;
}

//Interrupt for the accelerator pedal switch
ISR(PEDAL_SWITCH_INT)
{
	//Check to see if the brake/seat switch is closed otherwise inform user
	if (!(PORTE_IN & (1 << PDL_SW)) && ((PORTE_IN & (1 << BRK_SEAT_SW)) || (PORTA_IN & (1 << TOP_PIN))))
	{
		send_brake_seat_error(BRAKE_SEAT_SWITCH_OPEN);
		send_brake_seat_switch_error = SENT;
	}
	//If pedal is released or brake/seat switch is closed, inform user
	else if (((PORTE_IN & (1 << PDL_SW)) || !(PORTE_IN & (1 << BRK_SEAT_SW))) && send_brake_seat_switch_error == SENT)
	{
		send_brake_seat_error(BRAKE_SEAT_SWITCH_CLOSED);
		send_brake_seat_switch_error = FALSE;
	}

	//If both switches are low then the pedal state is on otherwise pedal is off
	if (!(PORTE_IN & (1 << BRK_SEAT_SW)) && !(PORTE_IN & (1 << PDL_SW)) && !(PORTA_IN & (1 << TOP_PIN)) && can_functioning != UNTESTED)
	{
		if (curr_pedal_level == 0 && !emergency_stop_motor)
		{
			recheck_pedal = TRUE;
			pedal_5_sec_timer_on = TRUE;
			pedal_5_sec_timer = 0;
			if (pedal_error_count >= 100)
			{
				pedal_error_count = 0;
			}
		}
		else
		{
			pedal_state = ON;
			update_pedal_state();
			if (!emergency_stop_motor)
			{
				E_BRAKE_OFF;
				electrobrake_counter = OFF;
			}
		}
	}
	else
	{
		//If the pedal state changed set motor off time to 0
		if (pedal_state != OFF)
		{
			motor_off_time = 0;
		}
		pedal_state = OFF;
		recheck_pedal = FALSE;
		pedal_5_sec_timer_on = FALSE;
		pedal_5_sec_timer = 0;
		update_pedal_state();
		if (send_pedal_switch_error == SENT && (PORTE_IN & (1 << PDL_SW)))
		{
			send_pedal_error(PEDAL_SWITCH_OPEN);
			send_pedal_switch_error = FALSE;
		}
		electrobrake_counter = ON;
	}
}

//Interrupt for the forward reverse emergency control switch
ISR(FORWARD_REVERSE_SWITCH_INT)
{
	//If the forward switch is selected, the forward direction will be set
	//First make sure can is not functioning before accepting emergency switch
	//If CAN is functioning send error
	//Otherwise allow emergency switch to function

	//Check to see if the top switch is closed otherwise inform user
	if (!(PORTE_IN & (1 << PDL_SW)) && (PORTA_IN & (1 << TOP_PIN)))
	{
		send_brake_seat_error(BRAKE_SEAT_SWITCH_OPEN);
		send_brake_seat_switch_error = SENT;
	}
	if (!(PORTA_IN & (1 << FWR_SW)))
	{
		if (can_functioning == UNTESTED)
		{
			manual_mode = TRUE;
		}
		else if (can_functioning == TRUE)
		{
			emergency_stop_motor = TRUE;
			//If the pedal state changed set motor off time to 0
			if (pedal_state != OFF)
			{
				motor_off_time = 0;
			}
			pedal_state = OFF;
			pedal_level = OFF;
			electrobrake_counter = ON;
			update_pedal_state();
			send_pedal_error(MANUAL_SWITCH_CLOSED);
			if (send_pedal_switch_error)
			{
				send_pedal_error(PEDAL_SWITCH_CLOSED);
				send_pedal_switch_error = FALSE;
			}
		}
		else
		{
			direction = FORWARD;
			manual_mode = TRUE;
			fr_transition = !fr_transition;
			stop_motor = FALSE;
		}
	}
	else if (!(PORTA_IN & (1 << REV_SW)))
	{
		if (can_functioning == UNTESTED)
		{
			manual_mode = TRUE;
		}
		else if (can_functioning == TRUE)
		{
			emergency_stop_motor = TRUE;
			//If the pedal state changed set motor off time to 0
			if (pedal_state != OFF)
			{
				motor_off_time = 0;
			}
			pedal_state = OFF;
			pedal_level = OFF;
			electrobrake_counter = ON;
			update_pedal_state();
			send_pedal_error(MANUAL_SWITCH_CLOSED);
			if (send_pedal_switch_error)
			{
				send_pedal_error(PEDAL_SWITCH_CLOSED);
				send_pedal_switch_error = FALSE;
			}
		}
		else
		{
			direction = REVERSE;
			manual_mode = TRUE;
			fr_transition = !fr_transition;
			stop_motor = FALSE;
		}
	}
	else
	{
		//If the pedal state changed set motor off time to 0
		if (pedal_state != OFF)
		{
			motor_off_time = 0;
		}
		pedal_state = OFF;
		update_pedal_state();
		direction = NEUTRAL;
		manual_mode = FALSE;
		send_pedal_error(MANUAL_SWITCH_OPEN);
		REVERSE_BUZZER_OFF;
		electrobrake_counter = ON;
		if (emergency_stop_motor != PERMANENT)
		{
			emergency_stop_motor = FALSE;
		}
	}
}

//Interrupt for back safety and slow switch
ISR(BACK_SAFETY_SWITCH_INT)
{
	//If the key switch is turned off
	if (PORTE_IN & (1 << KEY_SW))
	{
		//If the pedal is being pressed or less than 900 ms have passed since the pedal has been released
		//Keep the motor speed high for 800 ms so it can lose most of its energy and then turn off the motor
		if (pedal_state == ON || motor_off_time <= ENERGY_RELEASE_TIME)
		{
			pedal_level = 3000; //About 75% pwm
			SET_MOTOR_SPEED(pedal_level * fast_level_pwm_ratio + minimum_pwm);
			_delay_ms(800);
			pedal_state = OFF; SET_MOTOR_SPEED(0);
			_delay_ms(1000);
			electrobrake_counter = ON;
		}
	}
	//If the slow switch is triggered, change to slow mode and update pwm
	if (!(PORTE_IN & (1 << SLOW_SW)))
	{
		speed = SLOW_MANUAL;
		if (direction == FORWARD)
		{
			if (GET_MOTOR_SPEED() >= minimum_pwm)
				pedal_level = (GET_MOTOR_SPEED() - minimum_pwm) / slow_level_pwm_ratio;
			else
				pedal_level = 0;
		}
	}
	//Otherwise change back to fast mode and update pwm
	else if (speed == SLOW_MANUAL)
	{
		speed = FAST;
		if (direction == FORWARD)
		{
			if (GET_MOTOR_SPEED() >= minimum_pwm)
				pedal_level = (GET_MOTOR_SPEED() - minimum_pwm) / fast_level_pwm_ratio;
			else
				pedal_level = 0;
		}
	}

	/*if(direction == NEUTRAL || pedal_state == OFF)
		return;

	//If back safety switch is enabled, move away in the opposite direction of movement
	if(!(PORTE_IN & (1 << BCK_SFTY_SW)))
	{
		TCD0.CCB = OFF;
		pedal_level = OFF;
		pedal_state = OFF;
		E_BRAKE_OFF;
		update_pedal_state();

		//send_back_safety_error(BACK_SAFETY_UNRESOLVED);

		_delay_ms(500);
		if(direction == REVERSE)
		{
			direction = FORWARD;
			SET_MOTOR_FORWARD;
		}
		else
		{
			direction = REVERSE;
			SET_MOTOR_REVERSE;
		}

		back_safety_adjust = TRUE;
		back_safety_adjust_counter = 0;
		send_back_safety_error(BACK_SAFETY_ALERT);
	}*/
}

