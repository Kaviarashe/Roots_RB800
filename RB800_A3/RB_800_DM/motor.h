/*
 * motor.h
 *
 * Created: 6/16/2016 2:12:16 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef MOTOR_H_
#define MOTOR_H_

#include "RB_800.h"
#include <math.h>

#define OFF	0
#define ON	1

#define FR_DIR	0//PD0
#define PWM		1//PD1

#define SNS_OUT	3//PD3

#define PDL_SW		0//PE0
#define TOP_PIN     4//PA4
#define FWR_SW		5//PA5
#define REV_SW		6//PA6
#define REV_BUZZ	1//PB1
#define E_BRK		0//PB0
#define KEY_SW		2//PE2
#define BRK_SEAT_SW	3//PE3
#define SLOW_SW		1//PE1

#define FORWARD			0xAA
#define REVERSE			0xBB
#define NEUTRAL			0xCC

#define PEDAL_SWITCH_INT				PORTE_INT0_vect
#define BACK_SAFETY_SWITCH_INT			PORTE_INT1_vect
#define FORWARD_REVERSE_SWITCH_INT		PORTA_INT0_vect

#define SET_MOTOR_FORWARD	PORTD_OUTSET = (1 << FR_DIR); __SIM_DOUT
#define SET_MOTOR_REVERSE	PORTD_OUTCLR = (1 << FR_DIR); __SIM_DOUT
#ifndef  __SIM__
#define SET_MOTOR_SPEED(x)	TCD0.CCB = x
#define GET_MOTOR_SPEED()	TCD0.CCB
#else
#define SET_MOTOR_SPEED(x)	if(TCD0.CCB != (x))__SIM_SET_PWM(PWM_A3_MOTOR, x);TCD0.CCB = x
#define GET_MOTOR_SPEED()	TCD0.CCB
#endif // ! __SIM__


#define REVERSE_BUZZER_ON	PORTB_OUTSET = (1 << REV_BUZZ)
#define REVERSE_BUZZER_OFF	PORTB_OUTCLR = (1 << REV_BUZZ)

#define E_BRAKE_ON			PORTB_OUTCLR = (1 << E_BRK);
#define E_BRAKE_OFF			PORTB_OUTSET = (1 << E_BRK);
#define E_BRAKE_TGL			PORTB_OUTTGL = (1 << E_BRK);

#define PEDAL_SWITCH_ON		PORTE.INT0MASK |= (1 << PDL_SW);
#define PEDAL_SWITCH_OFF	PORTE.INT0MASK &= ~(1 << PDL_SW);

 //Default Motor Settings
#define ACCELERATION_RATE				30	//3 seconds from 0 to max
#define DECELERATION_RATE				8	//0.8 seconds from max to 0
#define MAXIMUM_PWM_PERCENTAGE			100	//100% maximum rate
#define MINIMUM_PWM_PERCENTAGE			10	//10% minimum rate
#define ACCELERATION_LEVEL_INCREMENT	((uint16_t) ((float)4096)/acceleration)
#define ACCELERATION_PWM_INCREMENT		((uint16_t) ((float) ACCELERATION_LEVEL_INCREMENT * (MAXIMUM_PWM_PERCENTAGE - MINIMUM_PWM_PERCENTAGE)/100.0))
#define DECELERATION_LEVEL_DECREMENT	((uint16_t) ((float)4096)/deceleration)
#define DECELERATION_PWM_DECREMENT		((uint16_t) ((float) DECELERATION_LEVEL_DECREMENT * (MAXIMUM_PWM_PERCENTAGE - MINIMUM_PWM_PERCENTAGE)/100.0))
#define MAX_REVERSE_SPEED				60	//60% of fast forward speed
#define BRAKE_DELAY						12	//1.2 seconds from releasing accelerator to activating electro-brake
#define RATED_CURRENT					30	//30 amps rated current
#define MAX_RUNNING_CURRENT				100	//100 Amps max running current
#define MAX_OVERCURRENT_TIME			45	//Allow 45 seconds of current draw above max running current
#define POT_LOW_DEADBAND				0	//0 Volts end of low side deadband on the potentiometer
#define POT_HIGH_DEADBAND				2048 //2.048 Volts start of high side deadband on the potentiometer
#define FAST_PWM_PERCENTAGE				100 //100% pwm in fast mode
#define SLOW_PWM_PERCENTAGE				60	//60% pwm in slow mode
#define LEVEL_PWM_RATIO					((4096 * ((float)MAXIMUM_PWM_PERCENTAGE/100) - 4096 * ((float)MINIMUM_PWM_PERCENTAGE/100)) / 4096)

#define ENERGY_RELEASE_TIME	8	//Time in 100s of ms from releasing the pedal for the motor to lose enough energy slowing down to not destroy diodes/mosfets when stopped immediately

//Forward/Reverse Speed Level Settings
#define SLOW_MANUAL	2
#define FAST		1
#define SLOW		0

#define PERMANENT	3

#define PEDAL_LEVEL_HYSTERESIS			100

void initialize_motor(void);

volatile uint8_t direction;
volatile uint8_t speed;
volatile uint16_t pedal_level;
volatile uint16_t pedal_state;
volatile uint16_t curr_pedal_level;
volatile uint16_t curr_pedal_on_off;
volatile uint16_t electrobrake_counter;
float motor_current;
volatile uint8_t manual_mode;
volatile uint8_t stop_motor;
volatile uint8_t back_safety_adjust;
volatile uint8_t back_safety_adjust_counter;


uint16_t fast_level_increment;
uint16_t slow_level_increment;
uint16_t reverse_level_increment;
uint16_t pwm_increment;
uint16_t fast_level_decrement;
uint16_t slow_level_decrement;
uint16_t reverse_level_decrement;
uint16_t pwm_decrement;
float fast_level_pwm_ratio;
float slow_level_pwm_ratio;
float reverse_level_pwm_ratio;
uint16_t minimum_pwm;
uint16_t max_reverse_pwm;
uint16_t max_electrobrake_count;
uint8_t fr_transition;
uint16_t low_deadband_level;
uint16_t high_deadband_level;
uint16_t deadband_level_difference;

volatile uint8_t top_debounce_timer;
volatile uint8_t top_debounce_count;
volatile uint8_t send_top_flag;

volatile uint8_t pedal_error_count;
volatile uint8_t recheck_pedal;

volatile uint8_t motor_off_time;

#endif /* MOTOR_H_ */