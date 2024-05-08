/*
 * timers.h
 *
 * Created: 7/9/2015 11:11:37 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef TIMERS_H_
#define TIMERS_H_

#include "RB_800.h"

#define SECOND_INT			TCC0_CCA_vect
#define DEBOUNCE_INT		TCC1_CCA_vect
#define CAN_TX_TIMER_INT	TCD1_CCA_vect

#define _30_MS		3
#define _50_MS		5
#define _150_MS		15
#define _350_MS		35
#define _750_MS		75
#define _1_5_SEC	150
#define _3_SEC		300

#define INPUT_RESET_SEC 2


 //Tracking Device
#define TRK	1
#define TRK_ON	PORTA_OUTSET = (1 << TRK);
#define TRK_OFF	PORTA_OUTCLR = (1 << TRK);

void initialize_timers(void);
void initialize_runtime_reset_timer(void);
void initialize_32Mhz(void);
void set_duty_cycle(uint8_t percent);
void write_minutes(void);
void write_hours(void);
void read_hours(void);
void read_minutes(void);
void start_input_reset(void);
void stop_second_timer(void);

uint8_t duty_cycle;
uint16_t global_counter;
uint16_t global_counter_60000;

uint8_t button_down_flag;
uint32_t button_down_counter;
uint8_t setting_changed;

uint8_t seconds;
uint8_t minutes;
volatile uint16_t hours;

uint8_t input_reset_flag;
uint8_t input_reset_sec;

volatile uint8_t runtimer_on;

volatile uint8_t horn_check_failed;
volatile uint8_t ep_check_failed;

volatile uint8_t CAN_check_counter;

volatile uint8_t lcd_update_pause;

volatile uint8_t reset_timer;

volatile uint8_t update_hour;

volatile uint8_t update_minutes; //IVA002: Fix for EEP Corruption

#endif /* TIMERS_H_ */