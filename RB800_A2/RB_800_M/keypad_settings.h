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

 //Forward/Reverse Speed Level Settings
#define FAST	1
#define SLOW	0

//Water Pump/Detergent Flow Settings
#define	LOW			0
#define	LOW_MED		1
#define	MED			2
#define	MED_HIGH	3
#define	HIGH		4

#define LOW_DC		25
#define LOW_MED_DC	35
#define MED_DC		45
#define MED_HIGH_DC	55
#define HIGH_DC		65

//On/Off Settings for
//Horn/Headlights/Beacon Lamp/Vacuum
//Water Pump/Detergent/Extra Pressure
#define ON	1
#define OFF	0
#define TGL	2

uint8_t speed_settings[5];

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
uint8_t presweep;
uint8_t presweep_dc;
uint8_t solenoid_valve;

uint8_t fr_speed;

uint8_t new_auto_flag;

uint8_t ep_down;

uint8_t idle;

#endif /* SETTINGS_H_ */