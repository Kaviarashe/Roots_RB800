/*
 * battery.h
 *
 * Created: 1/6/2016 11:13:20 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef ADC_H_
#define ADC_H_

#include "RB_800.h"
#include <math.h>

#define NUM_SAMPLES 10
#define ADC_NEG_ADJ 190
#define MAX_INPUT_VOLTAGE_READING 40
#define V_REF	2048 //2.048V
#define MAX_ADC_READING_COUNT	4096
#define MAX_ADC_ADJ_COUNT		(4096 - ADC_NEG_ADJ)

#define AV			20
#define R_SENSE_INV	1000

float v_off;

float moving_average_current;
uint8_t max_moving_average_current;

void initialize_ADC(void);
void measure_accelerator_pedal(void);
void measure_battery_voltage(void);
void measure_motor_current(void);
void measure_v_off(void);

#endif /* BATTERY_H_ */