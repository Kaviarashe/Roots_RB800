/*
 * battery.c
 *
 * Created: 1/6/2016 11:13:08 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "adc.h"

static void measure_brush_1_motor_current(void);
static void measure_brush_2_motor_current(void);
static void measure_vacuum_motor_current(void);
static void measure_presweep_motor_current(void);

void initialize_ADC(void)
{
	//ADC A Setup
	//DMASEL - 00 - DMA Combined Requests off
	//CHSTART - 0000 - Do not start any conversions
	//FLUSH - 0 - Do not flush
	//ENABLE - 1 - Enable the ADC
	ADCA_CTRLA = 0b0000001;

	//IMPMODE - 0 - High Impedance source
	//CURRLIMIT - 00 - No current limit
	//CONVMODE - 0 - ADC in unsigned mode
	//FREERUN - 0 - Freerun mode off
	//RESOLUTION - 00 - 12 bits
	//RESERVED - 0
	ADCA_CTRLB = 0b0000000;

	//RESERVED - 0
	//REFSEL - 010 - AREF pin on PORTA
	//RESERVED - 00
	//BANDGAP - 0 - Bandgap off
	//TEMPREF - 0 - Temperature reference disabled
	ADCA_REFCTRL = 0b00100000;

	//RSERVED - 00000
	//PRESCALAR - 101 - CLK /128 for ADC clock = 250KHz
	ADCA_PRESCALER = 0b00000101;


	//ADC B Setup
	//DMASEL - 00 - DMA Combined Requests off
	//CHSTART - 0000 - Do not start any conversions
	//FLUSH - 0 - Do not flush
	//ENABLE - 1 - Enable the ADC
	ADCB_CTRLA = 0b0000001;

	//IMPMODE - 0 - High Impedance source
	//CURRLIMIT - 00 - No current limit
	//CONVMODE - 0 - ADC in unsigned mode
	//FREERUN - 0 - Freerun mode off
	//RESOLUTION - 00 - 12 bits
	//RESERVED - 0
	ADCB_CTRLB = 0b0000000;

	//RESERVED - 0
	//REFSEL - 010 - AREF pin on PORTA
	//RESERVED - 00
	//BANDGAP - 0 - Bandgap off
	//TEMPREF - 0 - Temperature reference disabled
	ADCB_REFCTRL = 0b00100000;

	//RSERVED - 00000
	//PRESCALAR - 101 - CLK /128 for ADC clock = 250KHz
	ADCB_PRESCALER = 0b00000101;


	//ADC A CH0 - Battery Voltage
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCA_CH0_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 11/PB3
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCA_CH0_MUXCTRL = 0b01011000;

	//ADC A CH0 - Battery Voltage
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCA_CH1_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 10/PB2
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCA_CH1_MUXCTRL = 0b01010000;


	//ADC B CH0 - Brush 1 Motor Current
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCB_CH0_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 12
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCB_CH0_MUXCTRL = 0b00100000;

	//ADC B CH1 - Brush 2 Motor Current
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCB_CH1_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 13
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCB_CH1_MUXCTRL = 0b00101000;

	//ADC B CH2 - Vacuum Motor Current
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCB_CH2_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 14
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCB_CH2_MUXCTRL = 0b00110000;

	//ADC B CH3 - Presweep Motor Current
	//START - 0 - Do not start conversion
	//RESERVED - 00
	//GAIN - 000 - x1 Gain
	//INPUTMODE - 01 - Single-ended input measurement
	ADCB_CH3_CTRL = 0b00000001;

	//RESERVED - 0
	//MUXPOS - XXXX - Read from ADC 15
	//MUXNEG - 000 - Not used for single-ended measurement
	ADCB_CH3_MUXCTRL = 0b00111000;
}

void send_battery_voltage(void)
{
	uint16_t total = 0;
	uint32_t total_vol = 0;

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
	if (batt_index != 8)
	{
		batt_vol[batt_index] = total;
		batt_index++;
	}
	else
	{
		for (int i = 1; i < 8; i++)
		{
			batt_vol[i - 1] = batt_vol[i];
		}
		batt_vol[7] = total;

	}
	for (int i = 0; i < batt_index; i++)
	{
		total_vol += batt_vol[i];
	}
	total = total_vol / batt_index;
	if (battery_total > (total + 5) || battery_total < (total - 5))
	{
		battery_total = total;
	}
	else
	{
		battery_total = total;
		//Send the average ADC value
		uint8_t message[2];
		message[0] = total;
		message[1] = total >> 8;
		transmit_CAN(BATTERY_CAN, 0, 2, message);
	}
}

void check_fuses(void)
{
	uint16_t total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 1
		ADCA_CTRLA |= (1 << 3);
		//Wait for conversion to end
		while (ADCA_CTRLA & (1 << 3))YIELD_ADC("11_FUSES", ADCA_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCA_CH1RES;
	}
	//Get average of total
	total /= NUM_SAMPLES;

	if (total == 0) return; //IVA001: check for div/0

	//If fuse fail pin is shows a ratio less than 20 to battery voltage, start a debounce timer of 500 ms to make sure ratio is correct
	if (((uint16_t)battery_total * 20 / total) > 22)
	{
		if (fuse_debounce_timer == OFF)
		{
			fuse_debounce_timer = ON;
			fuse_debounce_count = 0;
			start_100_ms_timer();
		}
	}
	else
	{
		fuse_debounce_timer = OFF;
		fuse_debounce_count = 0;
	}

	/*char message[21];
	sprintf(message, "%d", ((uint16_t)battery_total * 20 / total));
	lcd_write_new(message);*/
}

void measure_currents(void)
{
	measure_brush_1_motor_current();
	measure_brush_2_motor_current();
	measure_vacuum_motor_current();
	measure_presweep_motor_current();
}

static void measure_brush_1_motor_current(void)
{
	float total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 0
		ADCB_CTRLA |= (1 << 2);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 2))YIELD_ADC("20_BRMTR1", ADCB_CTRLA);
		//Add result to total
		total += ADCB_CH0RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		//If the shunt output voltage is less than the off voltage, set current to 0
		float so = total * 0.0005; // 0.0005 = 2.048V/4096 counts
		test16_1 = total / 2;
		if (so < (brush_1_v_off + 0.005))
		{
			total = 0;
			motor_current = 0;
			testf = 0;
		}
		//Otherwise calculate the motor current
		else
		{
			motor_current = (so - brush_1_v_off) / AV * R_SENSE_INV;

			test16 = (uint16_t)(motor_current * 1000);
			testf = total;
		}
	}
	else
	{
		total = 0;
		motor_current = 0;
		testf = 0;
	}

	//A complementary filter to filter out noise from current readings
	//Also since current readings will vary largely when PWMing this filter will average out the readings
	if (eco_mode)
	{
		brush1_current = (brush1_current * 95 + motor_current * 5) / 100;
	}
	else
	{
		brush1_current = (brush1_current * 90 + motor_current * 10) / 100;
	}

}

void measure_brush_2_motor_current(void)
{
	float total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 1
		ADCB_CTRLA |= (1 << 3);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 3))YIELD_ADC("21_BRMTR2", ADCB_CTRLA);
		//Add result to total
		total += ADCB_CH1RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		//If the shunt output voltage is less than the off voltage, set current to 0
		float so = total * 0.0005; // 0.0005 = 2.048V/4096 counts
		test16_2 = total / 2;
		if (so < (brush_2_v_off + 0.005))
		{
			total = 0;
			motor_current = 0;
			testf = 0;
		}
		//Otherwise calculate the motor current
		else
		{
			motor_current = (so - brush_2_v_off) / AV * R_SENSE_INV;

			test16 = (uint16_t)(motor_current * 1000);
			testf = total;
		}
	}
	else
	{
		total = 0;
		motor_current = 0;
		testf = 0;
	}

	//A complementary filter to filter out noise from current readings
	//Also since current readings will vary largely when PWMing this filter will average out the readings
	if (eco_mode)
	{
		brush2_current = (brush2_current * 95 + motor_current * 5) / 100;
	}
	else
	{
		brush2_current = (brush2_current * 90 + motor_current * 10) / 100;
	}
}

void measure_vacuum_motor_current(void)
{
	float total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 2
		ADCB_CTRLA |= (1 << 4);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 4))YIELD_ADC("22_VACMTR", ADCB_CTRLA);
		//Add result to total
		total += ADCB_CH2RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;

		//If the shunt output voltage is less than the off voltage, set current to 0
		float so = total * 0.0005; // 0.0005 = 2.048V/4096 counts
		test16_3 = total / 2;
		if (so < (vacuum_v_off + 0.005))
		{
			total = 0;
			motor_current = 0;
			testf = 0;
		}
		//Otherwise calculate the motor current
		else
		{
			motor_current = (so - vacuum_v_off) / AV * R_SENSE_INV;

			test16 = (uint16_t)(motor_current * 1000);
			testf = total;
		}
	}
	else
	{
		total = 0;
		motor_current = 0;
		testf = 0;
	}

	//A complementary filter to filter out noise from current readings
	//Also since current readings will vary largely when PWMing this filter will average out the readings
	if (eco_mode)
	{
		vacuum_current = (vacuum_current * 95 + motor_current * 5) / 100;
	}
	else
	{
		vacuum_current = (vacuum_current * 90 + motor_current * 10) / 100;
	}
}

void measure_presweep_motor_current(void)
{
	float total = 0;

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		//Start channel 3
		ADCB_CTRLA |= (1 << 5);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 5))YIELD_ADC("23_PRSWPMTR", ADCB_CTRLA);
		//Add result to total
		total += ADCB_CH3RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		//If the shunt output voltage is less than the off voltage, set current to 0
		float so = total * 0.0005; // 0.0005 = 2.048V/4096 counts
		test16_4 = total / 2;
		if (so < (presweep_v_off + 0.005))
		{
			total = 0;
			motor_current = 0;
			testf = 0;
		}
		//Otherwise calculate the motor current
		else
		{
			motor_current = (so - presweep_v_off) / AV * R_SENSE_INV;

			test16 = (uint16_t)(motor_current * 1000);
			testf = total;
		}
	}
	else
	{
		total = 0;
		motor_current = 0;
		testf = 0;
	}

	//A complementary filter to filter out noise from current readings
	//Also since current readings will vary largely when PWMing this filter will average out the readings
	if (eco_mode)
	{
		presweep_current = (presweep_current * 95 + motor_current * 5) / 100;
	}
	else
	{
		presweep_current = (presweep_current * 90 + motor_current * 10) / 100;
	}
}

void measure_v_off(void)
{
	float total = 0;

	//Measure voff for brush 1
	for (uint16_t i = 0; i < NUM_SAMPLES_VOFF; i++)
	{
		//Start channel 0
		ADCB_CTRLA |= (1 << 2);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 2))YIELD_ADC("20_BR1_VOFF", ADCB_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCB_CH0RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES_VOFF);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		brush_1_v_off = total * 0.0005; //0.0005 = 2.048V / 4096 counts
	}
	else
	{
		brush_1_v_off = 0;
	}

	total = 0;

	//Measure voff for brush 2
	for (uint16_t i = 0; i < NUM_SAMPLES_VOFF; i++)
	{
		//Start channel 2
		ADCB_CTRLA |= (1 << 3);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 3))YIELD_ADC("21_BR2_VOFF", ADCB_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCB_CH1RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES_VOFF);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		brush_2_v_off = total * 0.0005; //0.0005 = 2.048V / 4096 counts
	}
	else
	{
		brush_2_v_off = 0;
	}

	total = 0;

	//Measure voff for vacuum
	for (uint16_t i = 0; i < NUM_SAMPLES_VOFF; i++)
	{
		//Start channel 2
		ADCB_CTRLA |= (1 << 4);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 4))YIELD_ADC("22_VAC_VOFF", ADCB_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCB_CH2RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES_VOFF);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		vacuum_v_off = total * 0.0005; //0.0005 = 2.048V / 4096 counts
	}
	else
	{
		vacuum_v_off = 0;
	}

	total = 0;

	//Measure voff for presweep
	for (uint16_t i = 0; i < NUM_SAMPLES_VOFF; i++)
	{
		//Start channel 3
		ADCB_CTRLA |= (1 << 5);
		//Wait for conversion to end
		while (ADCB_CTRLA & (1 << 5))YIELD_ADC("23_PRSWP_VOFF", ADCB_CTRLA);
		//Add result to total
		_delay_us(40);
		total += ADCB_CH3RES;
	}
	//Get average of total
	total /= (NUM_SAMPLES_VOFF);
	//Make sure that the total is above the negative voltage range
	if (total > ADC_NEG_ADJ)
	{
		total -= ADC_NEG_ADJ;
		presweep_v_off = total * 0.0005; //0.0005 = 2.048V / 4096 counts
	}
	else
	{
		presweep_v_off = 0;
	}
}
