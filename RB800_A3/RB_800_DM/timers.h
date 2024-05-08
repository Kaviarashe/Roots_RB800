/*
 * timers.h
 *
 * Created: 6/15/2016 3:15:16 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef TIMERS_H_
#define TIMERS_H_

#include "RB_800.h"

#define _100_MS_TIMER_INT	TCC1_CCA_vect
#define SECOND_INT			TCC0_CCA_vect
#define CAN_TX_TIMER_INT	TCD1_CCA_vect

void initialize_timers(void);
void initialize_32Mhz(void);

volatile uint8_t overcurrent;
volatile uint8_t overcurrent_count;
volatile uint8_t emergency_stop_motor;
volatile uint8_t check_initial_switch_counter;
volatile uint8_t _100_ms_check;
volatile uint8_t reset_timer;
volatile uint8_t log_current_counter;
volatile uint16_t log_max_current_counter;
volatile uint8_t pedal_5_sec_timer;
volatile uint8_t pedal_5_sec_timer_on;

#endif /* TIMERS_H_ */