/*
 * motor.c
 *
 * Created: 7/23/2015 2:04:52 PM
 *  Author: Anurag Vaddi Reddy
 */
//IVA002: Adapted for Simulation

#include "motor.h"
void initialize_motors(void)
{
	//Set presweep switch interrupt w/ pullup
	PORTF_DIRCLR = (1 << PRE_SWP_SNS);
	PORTF_INT0MASK |= (1 << PRE_SWP_SNS);
	PORTF_INTCTRL = 0x05;
	PORTF_PIN7CTRL = 0x18;

	//Set brush head middle detector interrupt w/ pullup
	PORTK_DIRCLR = (1 << BHA_MPS);
	PORTK_INT0MASK |= (1 << BHA_MPS);
	PORTK_INTCTRL = 0x05;
	PORTK_PIN6CTRL = 0x19;

	//Enable all 9201 devices
	PORTF_DIRSET = 0x7F;
	PORTF_OUTCLR = (1 << DISABLE_9201);

	//Enable all 8701 devices
	PORTE_DIRSET = 0x1F;
	PORTE_OUTSET = (1 << ENABLE_8701);

	//Turn on beacon light
	PORTK_DIRSET = 0x0F;
	PORTK_OUTSET = (1 << BCN);

	//Set italsea board outputs
	PORTH_DIRSET = (1 << FWD_IN) | (1 << REV_IN) | (1 << MLT_MD);

	brush_motor_set(OFF);
	vacuum_motor_set(OFF);
	horn_set(OFF);
	headlights_set(OFF);
	water_pump_set(OFF, OFF);
	Solenoid_set(OFF);
	detergent_pump_set(OFF, OFF);
	extra_pressure_set(OFF);
	SET_DRIVE_MOTOR_FORWARD;
	SET_DRIVE_SPEED_FAST;
	SQ_UP;
	BR_HD_UP;
	brush_head_position = BEFORE_MIDDLE;
	brush_head_timer = ON;
	squeegee_timer = ON;
	vacuum_state = VACUUM_OFF;
	accelerator = ACCEL_UP;

	brush_state = BRUSH_OFF;
	vacuum_state = VACUUM_OFF;
	presweep_state = OFF;
}

void brush_motor_set(uint8_t level)
{
	if (level != OFF && brush_head_position == BEFORE_MIDDLE && brush_head_position != AFTER_MIDDLE)
	{
		//Begin bringing the brush head down
		BR_HD_DN;
		brush_head_position = MIDDLE;
		brush_head_timer = ON;
		_delay_ms(100);
	}
	else if (level == OFF && brush_head_position != BEFORE_MIDDLE)
	{
		//Begin pulling brush head back up
		BR_HD_UP;
		brush_head_position = BEFORE_MIDDLE;
		brush_head_timer = ON;
		_delay_ms(100);
	}

	//Prescalar /8
	TCE0.CTRLA |= 0x04;
	//Enable pin A & B with single slope PWM
	TCE0.CTRLB |= 0x33;
	//Set period to 1000 counts ~500Hz
	TCE0.PER = 4000;

	//If brush is to be turned off, turn off
	if (level == 0)
	{
		stop_25_ms_brush_timer();
		TCE0.CCA = 0;
		TCE0.CCB = 0;
		__SIM_SET_PWM(PWM_A2_BR1_MOTOR, TCE0.CCA);
		__SIM_SET_PWM(PWM_A2_BR2_MOTOR, TCE0.CCB);
	}
	//If brush does not need to start slowly, start up immmediately
	else if (brush_slow_start == 0)
	{
		uint32_t pwm = ((uint32_t)level * 4000) / 100;
		test_value2 = (uint16_t)level;
		test_value1 = (uint16_t)pwm;
		TCE0.CCA = (uint16_t)pwm;
		TCE0.CCB = (uint16_t)pwm;
		__SIM_SET_PWM(PWM_A2_BR1_MOTOR, TCE0.CCA);
		__SIM_SET_PWM(PWM_A2_BR2_MOTOR, TCE0.CCB);
	}
	else if (BRUSH_TIMER_IS_ON)
	{
		return;
	}
	//If brush is already on, eco mode is changed
	else if (TCE0.CCA != 0)
	{
		//If eco mode is turned off slowly turn up to maximum pwm
		if (!eco_mode && brush_slow_start != 0 && temp_brush < 100)
		{
			temp_brush = 75;
			start_25_ms_brush_timer();
		}
		else
		{
			temp_brush = level;
			uint32_t pwm = ((uint32_t)level * 4000) / 100;
			TCE0.CCA = (uint16_t)pwm;
			TCE0.CCB = (uint16_t)pwm;
			__SIM_SET_PWM(PWM_A2_BR1_MOTOR, TCE0.CCA);
			__SIM_SET_PWM(PWM_A2_BR2_MOTOR, TCE0.CCB);
		}
	}
	//Slowly turn on brush
	else
	{
		temp_brush = 0;
		start_25_ms_brush_timer();
	}
}

void vacuum_motor_set(uint8_t level)
{
	//Prescalar /8
	TCE0.CTRLA |= 0x04;
	//Enable pin C with single slope PWM
	TCE0.CTRLB |= 0x43;
	//Set period to 1000 counts ~500Hz
	TCE0.PER = 4000;

	//If vacuum is to be turned off, turn off
	if (level == 0 || tank_full)
	{
		stop_25_ms_vacuum_timer();
		TCE0.CCC = 0;
		__SIM_SET_PWM(PWM_A2_VAC_MOTOR, TCE0.CCC);
	}
	//If vacuum does not need to start slowly, start up immmediately
	else if (vacuum_slow_start == 0)
	{
		uint32_t pwm = ((uint32_t)level * 4000) / 100;
		TCE0.CCC = (uint16_t)pwm;
		__SIM_SET_PWM(PWM_A2_VAC_MOTOR, TCE0.CCC);
	}
	else if (VACUUM_TIMER_IS_ON)
	{
		return;
	}
	//If vacuum is already on, eco mode is changed
	else if (TCE0.CCC != 0)
	{
		//If eco mode is turned off slowly turn up to maximum pwm
		if (!eco_mode && vacuum_slow_start != 0 && temp_vacuum < 100)
		{
			temp_vacuum = 75;
			start_25_ms_vacuum_timer();
		}
		else
		{
			temp_vacuum = level;
			uint32_t pwm = ((uint32_t)level * 4000) / 100;
			TCE0.CCC = (uint16_t)pwm;
			__SIM_SET_PWM(PWM_A2_VAC_MOTOR, TCE0.CCC);
		}
	}
	//Slowly turn on vacuum
	else
	{
		temp_vacuum = 0;
		start_25_ms_vacuum_timer();
	}
}

void pre_swp_set(uint8_t level)
{
	if (!presweep)
	{
		return;
	}

	if (level == OFF)
	{
		TCE0.CTRLB &= ~(0x80);
		PORTE_OUTCLR = (1 << PRE_SWP_ON);
		stop_25_ms_pre_swp_timer();
		presweep_dc = OFF;
		TCE0.CCD = 0;
		__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		return;
	}

	//Prescalar /8
	TCE0.CTRLA |= 0x04;
	//Enable pin D with single slope PWM
	TCE0.CTRLB |= 0x83;
	//Set period to 1000 counts ~500Hz
	TCE0.PER = 4000;

	if (!eco_mode)
	{
		presweep_dc = PRE_SWP_DC;
	}
	else
	{
		presweep_dc = PRE_SWP_DC_ECO;
	}

	//If pre_swp does not need to start slowly, start up immediately
	if (pre_swp_slow_start == 0)
	{
		uint32_t pwm = ((uint32_t)presweep_dc * 4000) / 100;
		TCE0.CCD = (uint16_t)pwm;
		__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
	}
	else if (PRE_SWP_TIMER_IS_ON)
	{
		return;
	}
	//If pre_swp is already on, eco mode is changed
	else if (TCE0.CCD != 0)
	{
		//If eco mode is turned off slowly turn up to maximum pwm
		if (!eco_mode && pre_swp_slow_start != 0 && temp_pre_swp < 100)
		{
			temp_pre_swp = 75;
			start_25_ms_pre_swp_timer();
		}
		else
		{
			temp_pre_swp = presweep_dc;
			uint32_t pwm = ((uint32_t)presweep_dc * 4000) / 100;
			TCE0.CCD = (uint16_t)pwm;
			__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		}
	}
	//Slowly turn on pre_swp
	else
	{
		temp_pre_swp = 0;
		start_25_ms_pre_swp_timer();
	}
}

void detergent_pump_set(uint8_t on_off, uint8_t level)
{
	if (on_off == ON)
	{
		//Prescalar /8
		TCF1.CTRLA |= 0x04;
		//Enable pin B with single slope PWM
		TCF1.CTRLB |= 0x23;
		//Set period to 1000 counts ~500Hz
		TCF1.PER = 4000;

		uint32_t pwm = ((uint32_t)level * 4000) / 100;
		TCF1.CCB = (uint16_t)pwm;
		
		__SIM_SET_PWM(PWM_A2_DET_PUMP, TCF1.CCB);
	}
	else
	{
		//Disable pin B
		TCF1.CTRLB &= ~(0x20);
		PORTK_OUTCLR = (1 << DP_PWM);
		__SIM_SET_PWM(PWM_A2_DET_PUMP, 0);
	}

	__SIM_DOUT;
}

void water_pump_set(uint8_t on_off, uint8_t level)
{
	if (on_off == ON)
	{
		//Prescalar /64
		TCF1.CTRLA |= 0x04;
		//Enable pin A with single slope PWM
		TCF1.CTRLB |= 0x13;
		//Set period to 1000 counts ~500Hz
		TCF1.PER = 4000;

		//Open or close the water solenoid depends on the pump state
		if (solenoid_valve)PORTK_OUTSET = (1 << WP_SOL);
		else PORTK_OUTCLR = (1 << WP_SOL);

		uint32_t pwm = ((uint32_t)level * 4000) / 100;
		TCF1.CCA = (uint16_t)pwm;

		__SIM_SET_PWM(PWM_A2_WATER_PUMP, TCF1.CCA);
	}
	else
	{
		//Disable pin A
		TCF1.CTRLB &= ~(0x10);
		PORTK_OUTCLR = (1 << WP_PWM);

		//Open or close the water solenoid depends on the pump state
		if (solenoid_valve)PORTK_OUTSET = (1 << WP_SOL);
		else PORTK_OUTCLR = (1 << WP_SOL);

		__SIM_SET_PWM(PWM_A2_WATER_PUMP, 0);
	}
	__SIM_DOUT;
}

//Change headlight state depending on CAN bus info
void headlights_set(uint8_t status)
{
	if (status == ON)
	{
		PORTK_OUTSET = (1 << HDL);
	}
	else if (status == TGL)
	{
		PORTK_OUTTGL = (1 << HDL);
	}
	else
	{
		PORTK_OUTCLR = (1 << HDL);
	}
	__SIM_DOUT;
}

//Change horn state depending on CAN bus info
void horn_set(uint8_t status)
{
	if (status == ON)
	{
		PORTK_OUTSET = (1 << HRN);
	}
	else if (status == TGL)
	{
		PORTK_OUTTGL = (1 << HRN);
	}
	else
	{
		PORTK_OUTCLR = (1 << HRN);
	}
	__SIM_DOUT;
}

void Solenoid_set(uint8_t status)
{
	if (status == ON)
	{
		//Open water solenoid
		PORTK_OUTSET = (1 << WP_SOL);
	}
	else
	{
		//Close water solenoid
		PORTK_OUTCLR = (1 << WP_SOL);
	}

	__SIM_DOUT;
}

void extra_pressure_set(uint8_t status)
{
	//Prescalar /8
	TCE0.CTRLA |= 0x04;
	//Enable pin D with single slope PWM
	TCE0.CTRLB |= 0x83;
	//Set period to 1000 counts ~500Hz
	TCE0.PER = 4000;
	if (transport || reverse)
		status = OFF;
	if (status == ON && (brush_head_position == BEFORE_MIDDLE || brush_head_position == MIDDLE) && accelerator == ACCEL_DN && brush_state == BRUSH_RUNNING)
	{
		uint32_t pwm = ((uint32_t)PRE_SWP_DC * 4000) / 100;
		TCE0.CCD = (uint16_t)pwm;
		__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		BR_HD_DN;
		brush_head_position = AFTER_MIDDLE;
		brush_head_timer = ON;
	}
	else if ((status == OFF || accelerator == ACCEL_UP || brush_state == OFF) && brush_head_position == AFTER_MIDDLE)
	{
		uint32_t pwm = ((uint32_t)PRE_SWP_DC_ECO * 4000) / 100;
		TCE0.CCD = (uint16_t)pwm;
		__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		BR_HD_UP;
		brush_head_position = MIDDLE;
		brush_head_timer = ON;
	}
}

void accelerator_up(void)
{
	if (vacuum_state == VACUUM_OFF && horn == OFF && headlights == OFF)
	{
		send_runtime_timer(OFF);
	}

	//Turn off brush motor
	brush_motor_set(OFF);
	_delay_ms(50);

	water_pump_set(OFF, 0);
	Solenoid_set(OFF);
	_delay_ms(50);

	detergent_pump_set(OFF, 0);
	_delay_ms(50);

	//Record and set pre sweep
	if (presweep_state == ON)
	{
		/*log_enqueue(PRE_SWP_OFF_EVENT);*/
		presweep_state = OFF;
	}
	pre_swp_set(OFF);

	brush_state = BRUSH_OFF;
	update_brush_state();
}

void accelerator_down(void)
{
	if (transport || reverse)
	{
		if (!runtimer_on)
		{
			send_runtime_timer(ON);
		}
		return;
	}

	//Turn on brush motor
	if (brush)
	{
		brush_motor_set(brush);
		_delay_ms(100);
	}

	//If accelerator is pushed then tell the A1 board to turn on the runtimer
	if (!runtimer_on)
	{
		send_runtime_timer(ON);
	}

	if (presweep)
	{
		//Record and set pre sweep
		if (presweep_state == OFF)
		{
			/*log_enqueue(PRE_SWP_ON_EVENT);*/
			presweep_state = ON;
		}
		pre_swp_set(ON);
	}

	if (brush)
	{
		//Record brush
		/*if(brush_state == BRUSH_OFF)
		{
			log_enqueue(BRUSH_ON_EVENT);
		}*/
		brush_state = BRUSH_RUNNING;
		update_brush_state();
	}
	update_pumps();
}

void update_pumps(void)
{
	//If the brush is on and the pedal is pressed past the minimum level, update the pump speed
	if (pedal_level >= MINIMUM_PUMP_PEDAL_LEVEL && brush_state == BRUSH_RUNNING)
	{
		uint8_t water_pump_level = (uint8_t)(water_pump_dc * ((float)pedal_level / 4096));
		uint8_t detergent_pump_level = (uint8_t)(detergent_pump_dc * ((float)pedal_level / 4096));

		water_pump_set(water_pump, water_pump_level);
		detergent_pump_set(detergent_pump, detergent_pump_level);
	}
	//Otherwise turn it off
	else
	{
		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);
	}
}

ISR(PRE_SWP_INT)
{
	//If presweep switch is low turn on presweep
	if (!(PORTF_IN & (1 << PRE_SWP_SNS)))
	{
		presweep = ON;
		if (accelerator == ACCEL_DN)
		{
			pre_swp_set(ON);
			/*log_enqueue(PRE_SWP_ON_EVENT);*/
		}
	}
	//Otherwise update runtimer and turn off presweep
	else
	{
		if (runtimer_on && accelerator == ACCEL_UP && vacuum_state == VACUUM_OFF && horn == OFF && headlights == OFF)
		{
			send_runtime_timer(OFF);
		}
		else if (!runtimer_on && (accelerator == ACCEL_DN || vacuum_state != VACUUM_OFF || horn != OFF || headlights != OFF))
		{
			send_runtime_timer(ON);
		}

		TCE0.CTRLB &= ~(0x80);
		PORTE_OUTCLR = (1 << PRE_SWP_ON);
		stop_25_ms_pre_swp_timer();
		presweep = OFF;
		presweep_dc = OFF;
		TCE0.CCD = 0;
		__SIM_DOUT;
		__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
		/*log_enqueue(PRE_SWP_OFF_EVENT);*/
	}
}

ISR(BR_HD_MID_INT)
{
	//If brush has been turned off go back to the top
	if (!brush || brush_state == BRUSH_OFF)
	{
		BR_HD_UP;
		brush_head_position = BEFORE_MIDDLE;
		brush_head_timer = ON;
	}
	//If extra pressure is not set stop in the middle
	else if (!extra_pressure)
	{
		BR_HD_OFF;
		brush_head_position = MIDDLE;
	}
	//Otherwise proceed to bottom
	else
	{
		BR_HD_DN;
		brush_head_position = AFTER_MIDDLE;
		brush_head_timer = ON;
	}
}

