/*
 * battery.c
 *
 * Created: 1/6/2016 11:13:08 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "adc.h"

void initialize_ADC(void)
{
	//ADC A Setup
	//DMASEL - 00 - DMA Combined Requests off
	//CHSTART - 000 - Do not start any conversions
	//FLUSH - 0 - Do not flush
	//ENABLE - 1 - Enable the ADC
	ADCA_CTRLA = 0b00000001;

	//IMPMODE - 0 - High Impedance source
	//CURRLIMIT - 00 - No current limit
	//CONVMODE - 0 - ADC in unsigned mode
	//FREERUN - 0 - Freerun mode off
	//RESOLUTION - 00 - 12 bits
	//RESERVED - 0
	ADCA_CTRLB = 0b00000000;

	//RESERVED - 0
	//REFSEL - 010 - AREF pin on PORTA
	//RESERVED - 00
	//BANDGAP - 0 - Bandgap off
	//TEMPREF - 0 - Temperature reference disabled
	ADCA_REFCTRL = 0b00100000;

	//RSERVED - 00000
	//PRESCALAR - 101 - CLK /128 for ADC clock = 250KHz
	ADCA_PRESCALER = 0b00000101;

	//ADC CH0 - Battery Voltage
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCA_CH0_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 1/PA1
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCA_CH0_MUXCTRL = 0b00001000;

	//ADC CH1 - Accelerator Pedal
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCA_CH1_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 3/PA2
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCA_CH1_MUXCTRL = 0b00011000;

	//ADC CH2 - Motor Current
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCA_CH2_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 11
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCA_CH2_MUXCTRL = 0b01011000;
}

void measure_battery_voltage(void)
{
	uint16_t total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 0
		ADCA_CTRLA |= (1 << 2);
		//Wait for conversion to end
		while (ADCA_CTRLA & (1 << 2))YIELD_ADC("10_BATVOLT", ADCA_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCA_CH0RES;
	}
	//Get average of total
	total /= NUM_SAMPLES;
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		total = (uint16_t)((float)total * MAX_ADC_READING_COUNT / MAX_ADC_ADJ_COUNT);
	}

	testf = ((float)total * MAX_INPUT_VOLTAGE_READING) / 4096;
}

void measure_accelerator_pedal(void)
{
	uint16_t total = 0;

	for (uint8_t i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 1
		ADCA_CTRLA |= (1 << 3);
		//Wait for conversion to end
		while (ADCA_CTRLA & (1 << 3))YIELD_ADC("11_ACCPED", ADCA_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCA_CH1RES;
	}
	//Get average of total
	total /= NUM_SAMPLES;
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		total = (uint16_t)((float)total * MAX_ADC_READING_COUNT / MAX_ADC_ADJ_COUNT);

		//Adjust the pedal value for the deadband levels
		curr_pedal_level = total;

		if (curr_pedal_level <= low_deadband_level)
		{
			curr_pedal_level = 0;
		}
		else if (curr_pedal_level >= high_deadband_level)
		{
			//If the currrent pedal level is more than 0.25V than the upper bound there is an error
			if (curr_pedal_level >= (high_deadband_level + 500))
			{
				curr_pedal_level = 0;
			}
			else
			{
				curr_pedal_level = 4095;
				pedal_error_count = 0;
			}
		}
		else
		{
			if (deadband_level_difference > 0)//IVA001: Fix for div/0
			{
				curr_pedal_level = (uint16_t)(((uint32_t)curr_pedal_level - low_deadband_level) * 4096 / deadband_level_difference);
				pedal_error_count = 0;
			}
		}
	}
	else
	{
		curr_pedal_level = 0;
	}

	//If recheck pedal is true, then that means pedal switch is on but level is stuck at 0
	//Check if pedal is still stuck at 0
	if (recheck_pedal)
	{

		if (curr_pedal_level == 0)
		{
			pedal_5_sec_timer_on = TRUE;
			//if(pedal_error_count++ > 100)
			if (pedal_5_sec_timer > 4)
			{
				pedal_state = OFF;
				electrobrake_counter = ON;
				update_pedal_state();
				send_pedal_error(POT_FAIL);
				recheck_pedal = FALSE;
			}
		}
		else
		{
			pedal_state = ON;
			pedal_5_sec_timer_on = FALSE;
			pedal_5_sec_timer = 0;
			if (!emergency_stop_motor)
			{
				E_BRAKE_OFF;
				electrobrake_counter = OFF;
			}
			update_pedal_state();
			recheck_pedal = FALSE;
		}
	}
}

void measure_motor_current(void)
{
	float total = 0;

	for (uint8_t i = 0; i < NUM_SAMPLES * 20; i++)
	{
		//Start channel 2
		ADCA_CTRLA |= (1 << 4);
		//Wait for conversion to end
		while (ADCA_CTRLA & (1 << 4))YIELD_ADC("12_MTRCUR", ADCA_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCA_CH2RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES * 20);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		//If the shunt output voltage is less than the off voltage, set current to 0
		float so = total * 0.0005; //0.0005 = 2.048V / 4096 counts
		if (so < (v_off + 0.005)) // 5mV hysteresis
		{
			total = 0;
			motor_current = 0;
			test16 = (uint16_t)(v_off * 1000);
			testf = 0;
		}
		//Otherwise calculate the motor current
		else
		{
			motor_current = (so - v_off) / AV * R_SENSE_INV;

			test16 = (uint16_t)(v_off * 1000);
			testf = total;
		}
	}
	else
	{
		total = 0;
		motor_current = 0;
		test16 = (uint16_t)(v_off * 1000);
		testf = 0;
	}

	//A complementary filter to filter out noise from current readings
	//Also since current readings will vary largely when PWMing this filter will average out the readings
	moving_average_current = (moving_average_current * 99 + motor_current * 1) / 100;

	if (moving_average_current > max_moving_average_current)
		max_moving_average_current = moving_average_current;

	if (moving_average_current > over_current_rating)
	{
		overcurrent = TRUE;//IVA001: Fix for div/0
		float t = (moving_average_current*moving_average_current - (float)rated_current*rated_current);
		if (t > 0)
			overload_time = (uint8_t)(overload_time_constant / t);
	}
	else if (moving_average_current < rated_current)
	{
		overcurrent = FALSE;
		overcurrent_count = 0;
	}
}

//Measure v off to see what the zero current voltage is
void measure_v_off(void)
{
	float total = 0;

	for (uint16_t i = 0; i < NUM_SAMPLES * 200; i++)
	{
		//Start channel 2
		ADCA_CTRLA |= (1 << 4);
		//Wait for conversion to end
		while (ADCA_CTRLA & (1 << 4))YIELD_ADC("12_MTR_VOFF", ADCA_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCA_CH2RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES * 200);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		v_off = total * 0.0005; //0.0005 = 2.048V / 4096 counts
	}
	else
	{
		v_off = 0;
	}
}

