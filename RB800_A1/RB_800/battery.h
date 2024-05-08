/*
 * battery.h
 *
 * Created: 1/11/2016 10:44:42 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef BATTERY_H_
#define BATTERY_H_

#include "RB_800.h"
#include <math.h>

#define NUM_SAMPLES 10

 //Battery Statuses
#define BATTERY_UNKNOWN			0x00
#define BATTERY_GOOD			0xAA
#define BATTERY_LOW				0xBB
#define BATTERY_OFF				0xCC
#define BATTERY_LOW_CHECK		0xDD
#define BATTERY_OFF_CHECK		0xEE


//Hysteresis to prevent constant change in battery states
#define BATTERY_LEVEL_HYSTERESIS	10

void initialize_ADC(void);
void check_battery_voltage(uint16_t total);
void request_battery_can(void);

uint8_t battery_level;
uint8_t min_battery_level;
uint8_t battery_status;
uint8_t overvoltage_count;
uint8_t overvoltage_error;

#endif /* BATTERY_H_ */