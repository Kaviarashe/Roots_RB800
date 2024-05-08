/*
 * settings_menu.h
 *
 * Created: 9/17/2015 10:52:57 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef SETTINGS_MENU_H_
#define SETTINGS_MENU_H_

#include "RB_800.h"

#define MACHINE_DETAIL	"       RB800       "
#define A1_PCB			"A1 ASSEMBLY - V30418"
#define A2_PCB			"A2 ASSEMBLY - V30418"
#define A3_PCB			"A3 ASSEMBLY - V30418"
#define A1_FIRMWARE		"A1 FIRMWARE - 190902"
#define A2_FIRMWARE		"A2 FIRMWARE - 190902"
#define A3_FIRMWARE		"A3 FIRMWARE - 190902"

#define PRE_MENU		0x00
#define MOTOR_MENU		0x01
#define BATTERY_MENU	0x02
#define ABOUT_MENU		0x03
#define DEFAULT_MENU	0x04

#define ACCELERATION_MENU		0x10
#define DECELERATION_MENU		0x11
#define BRAKE_DELAY_MENU		0x12
#define REVERSE_SPEED_MENU		0x13
#define SLOW_SPEED_MENU			0x14
#define FAST_SPEED_MENU			0x15
#define CURRENT_RATING_MENU		0x16
#define MAX_CURRENT_MENU		0x17
#define OVERLOAD_TIME_MENU		0x18
#define FORWARD_SPEED_MENU		0x19
#define VACUUM_SETTINGS_MENU	0x1A
#define VACUUM_SLOW_START_MENU	0x1B
#define BRUSH_SETTINGS_MENU		0x1C
#define BRUSH_SLOW_START_MENU	0x1D
#define PRE_SWP_SETTINGS_MENU	0x1E
#define PRE_SWP_SLOW_START_MENU	0x1F

#define POTENTIOMETER_MENU			0x80
#define POT_TYPE_MENU				0x81
#define POT_CALIBRATION_MENU		0x82
#define POT_TYPE_VAL_MENU			0x83
#define POT_CALIBRATION_VAL_MENU	0x84
#define POT_RECALIBRATION_MENU		0x85
#define POT_VALUE_MENU				0x86
#define POT_MENU					0X87

#define ACCELERATION_VAL_MENU		0x20
#define DECELERATION_VAL_MENU		0x21
#define BRAKE_DELAY_VAL_MENU		0x22
#define REVERSE_SPEED_VAL_MENU		0x23
#define SLOW_SPEED_VAL_MENU			0x24
#define FAST_SPEED_VAL_MENU			0x25
#define CURRENT_RATING_VAL_MENU		0x26
#define MAX_CURRENT_VAL_MENU		0x27
#define OVERLOAD_TIME_VAL_MENU		0x28
#define VACUUM_SLOW_START_VAL_MENU	0x29
#define BRUSH_SLOW_START_VAL_MENU	0x2A
#define PRE_SWP_SLOW_START_VAL_MENU	0x2B

#define LEAD_MENU		0x30
#define AGM_MENU		0x31
#define GEL_MENU		0x32

#define PCB_MENU			0x40
#define FIRMWARE_MENU		0x41
#define PCB_VAL_MENU		0x42
#define FIRMWARE_VAL_MENU	0x43

#define DEFAULT_CONFIRM_MENU	0x50

#define SERIAL_NUM_MENU		0x60
#define SERIAL_NUM_VAL_MENU	0x61

#define A1_SERIAL_NUM_MENU			0x62
#define A1_SERIAL_NUM_VAL_MENU		0x63
#define A2_SERIAL_NUM_MENU			0x64
#define A2_SERIAL_NUM_VAL_MENU		0x65
#define A3_SERIAL_NUM_MENU			0x66
#define A3_SERIAL_NUM_VAL_MENU		0x67
#define RB800_SERIAL_NUM_MENU		0x68
#define RB800_SERIAL_NUM_VAL_MENU	0x69

#define RUN_TIME_MENU		0x70
#define RUN_TIME_VAL_MENU	0x71

#define SET_SERIAL_NUM_MENU		0x90
#define SET_SERIAL_NUM_VAL_MENU	0x91

#define SET_A1_SERIAL_NUM_MENU			0x92
#define SET_A1_SERIAL_NUM_VAL_MENU		0x93
#define SET_A2_SERIAL_NUM_MENU			0x94
#define SET_A2_SERIAL_NUM_VAL_MENU		0x95
#define SET_A3_SERIAL_NUM_MENU			0x96
#define SET_A3_SERIAL_NUM_VAL_MENU		0x97
#define SET_RB800_SERIAL_NUM_MENU		0x98
#define SET_RB800_SERIAL_NUM_VAL_MENU	0x99

#define STATISTICS_MENU				0xB0
#define BRUSH_STAT_MENU				0xB1
#define VACUUM_STAT_MENU			0xB2
#define PRESWEEP_STAT_MENU			0xB3
#define WATER_PUMP_STAT_MENU		0xB4
#define DETERGENT_PUMP_STAT_MENU	0xB5
#define BRUSH_HOURS_MENU			0xB6
#define BRUSH1_AVERAGE_CURRENT_MENU	0xB7
#define BRUSH1_MAX_CURRENT_MENU		0xB8
#define BRUSH2_AVERAGE_CURRENT_MENU 0xB9
#define BRUSH2_MAX_CURRENT_MENU		0xBA
#define VACUUM_HOURS_MENU			0xBB
#define VACUUM_AVERAGE_CURRENT_MENU	0xBC
#define VACUUM_MAX_CURRENT_MENU		0xBD
#define PRESWEEP_HOURS_MENU			0xBE
#define PRESWEEP_AVERAGE_CURRENT_MENU	0xBF
#define PRESWEEP_MAX_CURRENT_MENU	0xC0
#define WATER_PUMP_HOURS_MENU		0xC1
#define DETERGENT_PUMP_HOURS_MENU	0xC2
#define ECO_HOURS_MENU				0xC3

#define CLEAR_SD_CARD_MENU	0xD0
#define CLEAR_SD_CARD_CONFIRM_MENU 0xD1

#define ASCII_NUM	0x30

uint8_t current_menu;
uint8_t current_value;
uint8_t pot_calibrating;

uint16_t brush_hours;
uint16_t vacuum_hours;
uint16_t presweep_hours;
uint16_t eco_hours;
uint16_t water_pump_hours;
uint16_t detergent_pump_hours;

uint16_t brush1_average_current;
uint16_t brush2_average_current;
uint16_t vacuum_average_current;
uint16_t presweep_average_current;

uint16_t brush1_max_current;
uint16_t brush2_max_current;
uint16_t vacuum_max_current;
uint16_t presweep_max_current;

uint8_t admin_mode;
uint8_t admin_mode_count;

void start_s_menu(void);


#endif /* SETTINGS_MENU_H_ */