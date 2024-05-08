/*
 * alarm.c
 *
 * Created: 12/21/2015 2:54:53 PM
 *  Author: Anurag Vaddi Reddy
 */


#include "alarm.h"

void initialize_alarms(void)
{
	send_top_flag = FALSE;
	log_top_event = FALSE;

	//Set float pin and top pin interrupt
	PORTE_DIRCLR = (1 << TOP_PIN) | (1 << FLOAT_PIN) | (1 << FAULT_8701);
	if (PORTE_IN & (1 << TOP_PIN))
	{
#ifndef BENCH_TEST //IVA002: Suppress the error in bench testing
		top_debounce_timer = ON;
		top_debounce_count = 0;
		start_100_ms_timer();
#endif
	}
	if (!(PORTE_IN & (1 << FLOAT_PIN)))
	{
		float_sensor_timer = ON;
	}
	if (!(PORTE_IN & (1 << FAULT_8701)))
	{
		fault_timer = ON;
	}

	PORTE_INT0MASK = (1 << TOP_PIN) | (1 << FLOAT_PIN) | (1 << FAULT_8701);
	PORTE_INTCTRL = 0x05;
	PORTE_PIN5CTRL = 0x19;	//Pull up and falling edge
	PORTE_PIN6CTRL = 0x1A;	//Pull up and rising edge
	PORTE_PIN7CTRL = 0x19;	//Pull up and falling edge

	PORTB_DIRCLR = (1 << POWER_DOWN_PIN) | (1 << FUSE_FAIL_PIN);
	PORTB_INT0MASK = (1 << POWER_DOWN_PIN);
	PORTB_INTCTRL = 0x05;
	PORTB_PIN1CTRL = 0x01;	//Rising Edge

	//Set interrupt for IS pins from 9201
	PORTA_DIRSET = (1 << IS_1_PIN) | (1 << IS_2_PIN) | (1 << IS_3_PIN) | (1 << IS_4_PIN);
	PORTA_INT0MASK = (1 << IS_1_PIN) | (1 << IS_2_PIN) | (1 << IS_3_PIN) | (1 << IS_4_PIN);
	PORTA_INTCTRL = 0x05;
	PORTA_PIN1CTRL = 0x00; PORTA_PIN2CTRL = 0x00; PORTA_PIN3CTRL = 0x00; PORTA_PIN4CTRL = 0x00;	//Both edges

	PORTD_DIRCLR = (1 << SNS_OUT_BM1) | (1 << SNS_OUT_BM2) | (1 << SNS_OUT_VAC) | (1 << SNS_OUT_PRE);
	PORTD_INT0MASK = (1 << SNS_OUT_BM1) | (1 << SNS_OUT_BM2) | (1 << SNS_OUT_VAC) | (1 << SNS_OUT_PRE);
	PORTD_INTCTRL = 0x05;
	PORTD_PIN0CTRL = 0x01;	//Pull up and rising edge
	PORTD_PIN1CTRL = 0x01;	//Pull up and rising edge
	PORTD_PIN2CTRL = 0x01;	//Pull up and rising edge
	PORTD_PIN3CTRL = 0x01;	//Pull up and rising edge


}

//If an alarm goes off, inform other boards in CAN
void send_alarm(uint8_t alarm_num)
{
	uint8_t message[1];
	message[0] = alarm_num;
	transmit_CAN(ALARM_CAN, 0, 1, message);
}
//If float sense for water level goes low , inform other boards in CAN

//Start alarm timer for debouncing the alarms
void start_alarm_timer(void)
{
	start_100_ms_timer();
	alarm_timer_count = 0;
}

//If TOP error is detected, inform the other boards
void send_top_error(void)
{
	uint8_t message[1];
	message[0] = A2_CAN;
	transmit_CAN(TOP_CAN, 0, 1, message);
	_delay_ms(500);
}

//If fuse error is detected, inform the other boards
void send_fuse_error(void)
{
	uint8_t message[1];
	message[0] = FUSE_FAIL;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(500);
}

//If 8701 fault is detected, inform the other boards
void send_fault_error(void)
{
	uint8_t message[1];
	message[0] = FAULT_FAIL;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(500);
}

//If brush motor overcurrent is detected through sns_out pin
void send_bm_overcurrent_error(void)
{
	uint8_t message[1];
	message[0] = BM_OVERCURRENT;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(500);
}

//If tank full is detected, inform the other boards
void send_tank_full(void)
{
	uint8_t message[1];
	transmit_CAN(FLOAT_CAN, 0, 0, message);
}

ISR(TOP_FLOAT_INT)
{
	//If TOP pin is low, start a debounce timer of 500 ms to make sure pin went low
	if (PORTE_IN & (1 << TOP_PIN))
	{
#ifndef BENCH_TEST //IVA002: Suppress the error in bench testing
		top_debounce_timer = ON;
		top_debounce_count = 0;
		start_100_ms_timer();
#endif
	}
	//If FLOAT pin is low, start an 8 second timer to make sure float sensor is reading full tank
	if (!(PORTE_IN & (1 << FLOAT_PIN)))
	{
		float_sensor_timer = ON;
	}
	if (!(PORTE_IN & (1 << FAULT_8701)))
	{
		fault_timer = ON;
		fault_timer_count = 0;
	}
}

ISR(SNS_OUT_INT)
{
	//horn_set(TGL);
	if ((PORTD_IN & ((1 << SNS_OUT_BM1) | (1 << SNS_OUT_BM2))) && brush_sns_out_count <= MAX_SNS_OUT_COUNT)
	{
		if (brush_sns_out_count < MAX_SNS_OUT_COUNT)
			brush_sns_out_count++;
		brush_sns_out_timer = ON;
		if ((brush_sns_out_count >= MAX_SNS_OUT_COUNT) && (brush_sns_out_timer_count == 5))
		{
			send_brush_overcurrent_alarm = TRUE;
			brush_sns_out_count = 0;

			//Disable all 9201 devices
			PORTF_OUTSET = (1 << DISABLE_9201);

			//Disable all 8701 devices
			PORTE_OUTCLR = (1 << ENABLE_8701);

			water_pump_set(OFF, OFF);
			Solenoid_set(OFF);
			detergent_pump_set(OFF, OFF);

		}
		else if ((brush_sns_out_count < MAX_SNS_OUT_COUNT) && (brush_sns_out_timer_count == 5))
		{

			brush_sns_out_timer_count = 0;
			brush_sns_out_timer = OFF;
			brush_sns_out_count = 0;
			//horn_set(TGL);

		}
	}

}

//If pin that checks for very low voltage goes high, wait in this interrupt
ISR(POWER_DOWN_INT)
{
	while (POWERING_DOWN) {};
}

//Check current sense pins
ISR(IS_INT)
{
	is_1_flag = PORTA_IN & (1 << IS_1_PIN);
	is_2_flag = PORTA_IN & (1 << IS_2_PIN);
	is_3_flag = PORTA_IN & (1 << IS_3_PIN);
	is_4_flag = PORTA_IN & (1 << IS_4_PIN);
	is_reset = FALSE;
}


/*//If fault pin is high, start a 8 second timer to make sure fault is constant
	if(!(PORTE_IN & (1 << FAULT_8701)))
	{
		fault_timer = FAULT_SEC;
		fault_counter++;
		if(fault_counter == MAX_FAULT_COUNT)
		{
			send_fault_flag = TRUE;
			fault_timer = OFF;
			//Disable all 9201 devices
			PORTF_OUTSET = (1 << DISABLE_9201);

			//Disable all 8701 devices
			PORTE_OUTCLR = (1 << ENABLE_8701);

			water_pump_set(OFF, OFF);
			detergent_pump_set(OFF, OFF);
			fault_timer = OFF;
			fault_counter = OFF;
		}
	}*/