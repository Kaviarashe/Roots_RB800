/*
 * motor.h
 *
 * Created: 7/23/2015 2:05:02 PM
 *  Author: Anurag Vaddi Reddy
 */
//IVA002: Adapted for Simulation


#ifndef MOTOR_H_
#define MOTOR_H_

#include "RB_800.h"

void on(void);
void off(void);
void set_motor_level(uint8_t percentage);
void set_up_motor(void);
void adjust_motor_level(void);

volatile uint16_t pedal_level;

void brush_motor_set(uint8_t status);
void vacuum_motor_set(uint8_t status);
void pre_swp_set(uint8_t level);
void detergent_pump_set(uint8_t on_off, uint8_t level);
void water_pump_set(uint8_t on_off, uint8_t level);
void headlights_set(uint8_t status);
void horn_set(uint8_t status);
void extra_pressure_set(uint8_t status);
void update_pumps(void);
void Solenoid_set(uint8_t status);

void accelerator_down(void);
void accelerator_up(void);

void initialize_motors(void);

#define MINIMUM_PUMP_PEDAL_LEVEL 300 //4095 is max possible level
#define TRANSPORT_SQ_TIME 1 //5 //Seconds
#define TRANSPORT_VAC_TIME   15   //ANURAG SET 10 //Seconds

#define BH_DIR		0 //PF0
#define BH_PWM		1 //PF1
#define SQ_DIR		2 //PF2
#define SQ_PWM		3 //PF3
#define WP_PWM		4 //PF4
#define DP_PWM		5 //PF5
#define DISABLE_9201	6 //PF6
#define PRE_SWP_SNS	7 //PF7

#define WP_SOL	0 //PK0
#define BCN		3 //PK3
#define HDL		1 //PK1
#define HRN		2 //PK2
#define BHA_MPS	6 //PK6


#define BM1_ON		0 //PE0
#define BM2_ON		1 //PE1
#define VAC_ON		2 //PE2
#define	PRE_SWP_ON	3 //PE3
#define ENABLE_8701	4 //PE4
#define	FAULT_8701	5 //PE5

#define FUSE_ALERT	2 //PB

#define PRE_SWP_DC		100
#define PRE_SWP_DC_ECO	75

#define PRE_SWP_INT		PORTF_INT0_vect
#define BR_HD_MID_INT	PORTK_INT0_vect

#define ACCEL_UP	0
#define ACCEL_DN	1
#define BEFORE_MIDDLE	0
#define MIDDLE			1
#define AFTER_MIDDLE	2

#define VACUUM_OFF_TIME	15

#define BR_HD_UP	PORTF_OUTCLR = (1 << BH_DIR); PORTF_OUTSET = (1 << BH_PWM);__SIM_DOUT
#define BR_HD_DN	PORTF_OUTSET = (1 << BH_DIR); PORTF_OUTSET = (1 << BH_PWM);__SIM_DOUT
#define BR_HD_OFF	PORTF_OUTCLR = (1 << BH_DIR) | (1 << BH_PWM);__SIM_DOUT

#define ACTUATOR_RESET_SEC 10

#define SQ_UP	PORTF_OUTCLR = (1 << SQ_DIR); PORTF_OUTSET = (1 << SQ_PWM);__SIM_DOUT
#define SQ_DN	PORTF_OUTSET = (1 << SQ_DIR); PORTF_OUTSET = (1 << SQ_PWM);__SIM_DOUT
#define SQ_OFF	PORTF_OUTCLR = (1 << SQ_DIR) | (1 << SQ_PWM);__SIM_DOUT

#define FWD_IN	4//PH4
#define REV_IN	2//PH2
#define MLT_MD	6//PH6
#define SET_DRIVE_MOTOR_FORWARD	PORTH_OUTCLR = (1 << REV_IN); PORTH_OUTSET = (1 << FWD_IN); __SIM_DOUT
#define SET_DRIVE_MOTOR_REVERSE	PORTH_OUTCLR = (1 << FWD_IN); PORTH_OUTSET = (1 << REV_IN); __SIM_DOUT

#define IS_DRIVE_MOTOR_FORWARD	!(PORTH_OUT & (1 << REV_IN)) && (PORTH_OUT & (1 << FWD_IN))
#define IS_DRIVE_MOTOR_REVERSE	!(PORTH_OUT & (1 << FWD_IN)) && (PORTH_OUT & (1 << REV_IN))

#define SET_DRIVE_SPEED_FAST	PORTH_OUTCLR = (1 << MLT_MD);__SIM_DOUT
#define SET_DRIVE_SPEED_SLOW	PORTH_OUTSET = (1 << MLT_MD);__SIM_DOUT

#define VAC_ON_NORMAL		100
#define BRUSH_ON_NORMAL		100

#define VAC_ON_ECO		75
#define BRUSH_ON_ECO	75

#define FORWARD	0
#define REVERSE	1

volatile uint8_t accelerator;
volatile uint8_t accelerator_changed;
volatile uint8_t brush_head_position;
volatile uint8_t extra_pressure_timer;
volatile uint8_t vacuum_off_timer;
volatile uint8_t brush_head_timer;
volatile uint8_t squeegee_timer;
volatile uint8_t switch_motor_reverse;
volatile uint8_t switch_motor_forward;
uint8_t current_dir;
uint8_t brush_state;
uint8_t vacuum_state;
uint8_t squeegee_down;
uint8_t eco_mode;
uint16_t vacuum_slow_start;
uint16_t brush_slow_start;
uint16_t pre_swp_slow_start;
float temp_vacuum;
float temp_brush;
float temp_pre_swp;

uint8_t presweep_state;
uint8_t w_pump_state;
uint8_t d_pump_state;

volatile uint8_t emergency_off;

uint16_t test_value1; // foe seeing local variable value in debug mode
uint16_t test_value2;

#endif /* MOTOR_H_ */