/*
 * timers.h
 *
 * Created: 7/9/2015 11:11:37 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef TIMERS_H_
#define TIMERS_H_

#include "RB_800.h"
#include <math.h>

#define SECOND_INT			TCD0_CCA_vect
#define _100_MS_TIMER_INT	TCE1_CCA_vect

#define _25_MS_VACUUM_TIMER_INT		TCF0_CCA_vect
#define _25_MS_BRUSH_TIMER_INT		TCC1_CCA_vect
#define _25_MS_PRE_SWP_TIMER_INT	TCD1_CCA_vect
#define CAN_TX_TIMER_INT			TCC0_CCA_vect

#define VACUUM_TIMER_IS_ON			TCF0.CTRLA != 0
#define BRUSH_TIMER_IS_ON			TCC1.CTRLA != 0
#define PRE_SWP_TIMER_IS_ON			TCD1.CTRLA != 0

#define SECONDS_PER_HOUR	3600
#define SECONDS_PER_MIN		60

void initialize_timers(void);
void initialize_32Mhz(void);
void send_runtime_timer(uint8_t status);
void start_100_ms_timer(void);
void stop_100_ms_timer(void);
void start_25_ms_vacuum_timer(void);
void start_25_ms_brush_timer(void);
void start_25_ms_pre_swp_timer(void);
void stop_25_ms_vacuum_timer(void);
void stop_25_ms_brush_timer(void);
void stop_25_ms_pre_swp_timer(void);

volatile uint8_t runtimer_on;
volatile uint8_t event_counter;
volatile uint8_t seconds;
volatile uint8_t minutes;
volatile uint16_t hours; //IVA003: Fixed hours
volatile uint8_t transport_timer;
volatile uint8_t reset_timer;
volatile uint8_t second_timer_tick;

volatile uint8_t update_current;
volatile uint8_t update_max_current;
volatile uint8_t update_eeprom_statistics;

#endif /* TIMERS_H_ */