/*
 * motor_settings.h
 *
 * Created: 6/28/2016 10:18:36 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef MOTOR_SETTINGS_H_
#define MOTOR_SETTINGS_H_

#include "RB_800.h"

////////////////////////////////////////////////////////////////////////////

void initialize_RB_settings(void);
void set_to_default(void);
void update_motor_settings(void);
void update_accel_settings(void);
void update_decel_settings(void);
void update_break_delay_settings(void);
void update_reverse_speed(void);
void update_fast_speed(void);
void update_slow_speed(void);
void update_current_rating(void);
void update_max_current(void);
void update_over_current_time(void);

volatile uint8_t acceleration;
volatile uint8_t deceleration;
volatile uint8_t brake_delay;
volatile uint8_t reverse_speed;
volatile uint8_t slow_speed;
volatile uint8_t fast_speed;
volatile uint8_t rated_current;
volatile uint8_t over_current_rating;
volatile uint8_t max_current;
volatile uint8_t overload_time;
volatile uint8_t full_overload_time;
volatile uint32_t overload_time_constant;
volatile uint16_t pot_low_deadband;
volatile uint16_t pot_high_deadband;
volatile uint8_t motor_parameter_error;

char A3_serial_num[A3_SERIAL_NUM_LEN + 1];

#endif /* MOTOR_SETTINGS_H_ */