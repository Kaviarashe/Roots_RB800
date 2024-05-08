/*
 * alarm.h
 *
 * Created: 7/14/2015 2:17:55 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef ALARM_H_
#define ALARM_H_

#include "RB_800.h"

#define TOP_PIN		6//PE6
#define FLOAT_PIN		7//PE7
#define TOP_FLOAT_INT	PORTE_INT0_vect
#define FLOAT_SNS_SEC	8
#define FAULT_SEC		8
#define MAX_FAULT_COUNT	5000

#define POWER_DOWN_PIN	1 //PB1
#define POWERING_DOWN	PORTB_IN & (1 << POWER_DOWN_PIN)
#define POWER_DOWN_INT	PORTB_INT0_vect

#define FUSE_FAIL_PIN	2//PB2
#define FUSE_FAIL_INT	PORTB_INT1_vect

#define IS_1_PIN	1//PA1
#define IS_2_PIN	2//PA2
#define IS_3_PIN	3//PA3
#define IS_4_PIN	4//PA4
#define IS_INT		PORTA_INT0_vect

#define SNS_OUT_INT	PORTD_INT0_vect
#define SNS_OUT_BM1	0//PD0
#define SNS_OUT_BM2	1//PD1
#define SNS_OUT_VAC	2//PD2
#define SNS_OUT_PRE	3//PD3
#define MAX_SNS_OUT_COUNT	15000//Current chopping during short results in 33khz pwm on sns_out pin, so 15000 -> ~500 msecond of constant chopping


volatile uint8_t alarm_count;
volatile uint8_t second_alarm_count;
volatile uint8_t alarm_timer;
volatile uint8_t alarm_timer_count;
volatile uint8_t send_alarm_flag;
volatile uint8_t second_reading;
volatile uint8_t alarm_sent;

volatile uint8_t send_float_flag;
volatile uint8_t send_fault_flag;
volatile uint8_t send_top_flag;
volatile uint8_t log_top_event;
volatile uint8_t top_debounce_timer;
volatile uint8_t top_debounce_count;
volatile uint8_t float_sensor_timer;
volatile uint8_t fault_timer;
volatile uint8_t fault_timer_count;


volatile uint8_t send_fuse_flag;
volatile uint8_t fuse_debounce_timer;
volatile uint8_t fuse_debounce_count;

volatile uint8_t tank_full;

volatile uint8_t is_1_flag;
volatile uint8_t is_2_flag;
volatile uint8_t is_3_flag;
volatile uint8_t is_4_flag;
volatile uint8_t is_reset;

volatile  float brush_sns_out_count;
volatile uint8_t brush_sns_out_timer;
volatile uint8_t brush_sns_out_timer_count;
volatile uint8_t send_brush_overcurrent_alarm;


void initialize_alarms(void);
void start_alarm_timer(void);
void send_alarm(uint8_t alarm_num);
void send_tank_full(void);
void send_top_error(void);
void send_fuse_error(void);
void send_fault_error(void);
void send_bm_overcurrent_error(void);

#endif /* ALARM_H_ */