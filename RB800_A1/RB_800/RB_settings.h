/*
 * RB_settings.h
 *
 * Created: 9/16/2015 11:50:30 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef RB_SETTINGS_H_
#define RB_SETTINGS_H_

#include "RB_800.h"

//Battery Type Location
//Battery Types
#define LEAD	0x12
#define	AGM		0x23
#define GEL		0x34


//Min/Max Motor Values
#define MIN_ACCELERATION	5
#define MAX_ACCELERATION	50
#define MIN_DECELERATION	5
#define MAX_DECELERATION	50
#define MIN_REVERSE			5
#define MAX_REVERSE			80
#define MIN_FAST			5
#define MAX_FAST			100
#define MIN_SLOW			5
#define MAX_SLOW			80
#define MIN_BRAKE_DELAY		0
#define MAX_BRAKE_DELAY		100
#define MIN_CURRENT_RATING	0
#define MAX_CURRENT_RATING	40
#define MIN_MAX_CURRENT		0
#define MAX_MAX_CURRENT		100
#define MIN_OVERLOAD_TIME	0
#define MAX_OVERLOAD_TIME	60
#define MAX_VACUUM_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_VACUUM_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time
#define MAX_BRUSH_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_BRUSH_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time
#define MAX_PRE_SWP_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_PRE_SWP_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time

void initialize_RB_settings(void);
void set_to_default(void);
void save_motor_settings_to_eeprom();
void load_motor_settings_from_eeprom();
void handle_settings_command(uint8_t identifier, uint8_t* data);

char firmware[12];
char PCB_revision[12];

volatile uint8_t acceleration;
volatile uint8_t deceleration;
volatile uint8_t brake_delay;
volatile uint8_t reverse_speed;
volatile uint8_t slow_speed;
volatile uint8_t fast_speed;
volatile uint8_t max_current;
volatile uint8_t current_rating;
volatile uint8_t overload_time;
volatile uint16_t pot_low_deadband;
volatile uint16_t pot_high_deadband;
volatile uint16_t pot_value;
volatile uint8_t vacuum_slow_start;
volatile uint8_t brush_slow_start;
volatile uint8_t pre_swp_slow_start;

uint8_t battery_type;
uint8_t tank_full;

char serial_num[SERIAL_NUM_LEN + 1];
char A1_serial_num[A1_SERIAL_NUM_LEN + 1];
char A2_serial_num[A1_SERIAL_NUM_LEN + 1];
char A3_serial_num[A1_SERIAL_NUM_LEN + 1];


#endif /* RB_SETTINGS_H_ */