/*
 * settings.h
 *
 * Created: 7/15/2015 10:19:58 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "RB_800.h"
#include <avr/eeprom.h>

#ifndef SETTINGS_H_
#define SETTINGS_H_

 //Turns the lcd information about the button presses on or off
#define LCD_INFO TRUE

//Forward/Reverse Speed Level Settings
#define FAST	1
#define SLOW	0

//Water Pump/Detergent Flow Settings
#define	LOW			0
#define	LOW_MED		1
#define	MED			2
#define	MED_HIGH	3
#define	HIGH		4

// dosage pump pwm level
#define LOW_DC_DOSAGE		15
#define LOW_MED_DC_DOSAGE	28
#define MED_DC_DOSAGE		45
#define MED_HIGH_DC_DOSAGE	66
#define HIGH_DC_DOSAGE		92
/// water pump pwm level
#define LOW_DC		40
#define LOW_MED_DC	55
#define MED_DC		70
#define MED_HIGH_DC	85
#define HIGH_DC		100

//On/Off Settings for
//Horn/Headlights/Beacon Lamp/Vacuum
//Water Pump/Detergent/Extra Pressure
#define ON	1
#define OFF	0

#define VAC_ON		100
#define BRUSH_ON	100

#define VAC_ON_ECO		75
#define BRUSH_ON_ECO	75

#define FORWARD 1
#define REVERSE 0


void initialize_keypad_settings(void);
void change_water_pump_state(uint8_t state);
void increase_water_pump_level(void);
void decrease_water_pump_level(void);
void set_detergent_pump_state(uint8_t state);
void increase_detergent_pump_level(void);
void decrease_detergent_pump_level(void);
void change_forward_reverse_state(void);
void change_vacuum_state(void);
void change_horn_state(void);
void change_headlights_state(void);
void change_beacon_lamp_state(void);
void change_extra_pressure_state(void);
void change_brush_state(void);
void change_auto_state(void);
void change_speed(void);
void change_transport_mode(void);
void transport_mode_on(void);
void transport_mode_off(void);
uint8_t is_reverse_transport(void);
void reverse_mode_on(void);
void reverse_mode_off(void);
void change_eco_mode(void);
void change_solenoid_valve_state(void);
void change_auto_settings(void);

uint8_t speed_settings_dosage[5];
uint8_t speed_settings_water[5];

uint8_t water_pump_sl;
uint8_t	detergent_pump_sl;
uint8_t water_pump_dc;
uint8_t	detergent_pump_dc;

uint8_t water_pump;
uint8_t detergent_pump;
uint8_t vacuum;
uint8_t horn;
uint8_t headlights;
uint8_t beacon_lamp;
uint8_t extra_pressure;
uint8_t reverse;
uint8_t brush;
uint8_t transport;
uint8_t eco_mode;
uint8_t solenoid_valve;
uint8_t solenoid_led;

uint8_t fr_speed;

uint8_t auto_settings[NUM_AUTO_SETTINGS];
uint8_t new_auto_flag;

uint8_t temp_wp;
uint8_t temp_dp;
uint8_t temp_b;

uint8_t blink_brush_led;
uint8_t blink_vac_led;
uint8_t blink_sld_led;

//Used for turning off motors when battery is low
#define MOTORS_OFF				0xAA
#define MOTORS_ON				0xBB
void turn_off_motors(void);
uint8_t motors_status;

#endif /* SETTINGS_H_ */