/*
 * alarm.h
 *
 * Created: 7/14/2015 2:17:55 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef ALARM_H_
#define ALARM_H_

#include "RB_800.h"

#define POT_FAIL				1
#define PEDAL_SWITCH_CLOSED		2
#define PEDAL_SWITCH_OPEN		3
#define OVERVOLTAGE				4
#define MANUAL_SWITCH_CLOSED	5
#define MANUAL_SWITCH_OPEN		6

uint8_t top_error;
uint8_t fuse_error;
uint8_t fault_error;
uint8_t bm_overcurrent_error;
uint8_t back_safety_alert;
volatile uint8_t alarm_on;
volatile uint8_t pedal_switch_error;
volatile uint8_t manual_switch_error;
volatile uint8_t brake_seat_switch_error;

#endif /* ALARM_H_ */