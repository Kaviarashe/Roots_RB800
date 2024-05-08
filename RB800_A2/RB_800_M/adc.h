/*
 * adc.h
 *
 * Created: 1/6/2016 11:13:20 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef ADC_H_
#define ADC_H_

#include "RB_800.h"
#include <math.h>

#define NUM_SAMPLES 10
#define NUM_SAMPLES_VOFF 100
#define ADC_NEG_ADJ 190
#define MAX_INPUT_VOLTAGE_READING 40
#define V_REF	2048 //2.048V
#define MAX_ADC_READING_COUNT	4096
#define MAX_ADC_ADJ_COUNT		(4096 - ADC_NEG_ADJ)

#define V_OFF		0.020
#define AV			20
#define R_SENSE_INV	500

#define BRUSH_MOTOR_1_CURRENT_PIN	4//PB4
#define BRUSH_MOTOR_2_CURRENT_PIN	5//PB5
#define VACUUM_CURRENT_PIN			6//PB6
#define PRESWEEP_CURRENT_PIN		7//PB7

void initialize_ADC(void);
void send_battery_voltage(void);
void check_fuses(void);
void measure_v_off(void);
void measure_currents(void);

float motor_current;
volatile uint8_t measure_current;
volatile uint8_t check_max_current;
uint16_t battery_total;

float brush1_current;
float brush2_current;
float vacuum_current;
float presweep_current;

uint8_t max_brush1_session_current;
uint8_t max_brush2_session_current;
uint8_t max_vacuum_session_current;
uint8_t max_presweep_session_current;

float brush_1_v_off;
float brush_2_v_off;
float vacuum_v_off;
float presweep_v_off;

uint16_t batt_vol[8];
uint8_t batt_index;

#endif /* BATTERY_H_ */