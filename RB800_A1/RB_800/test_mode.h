/*
 * test_mode.h
 *
 * Created: 12/14/2015 11:15:52 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef TEST_MODE_H_
#define TEST_MODE_H_

#include "RB_800.h"

#define PRE_MENU 0x00

#define MOTOR_DIRECTION_MENU	0x10
#define BRUSH_MENU				0x11
#define VACUUM_MENU				0x12
#define WATER_PUMP_MENU			0x13
#define DET_PUMP_MENU			0x14
#define EXTRA_PRESSURE_MENU		0x15
#define HEADLIGHTS_MENU			0x16
#define HORN_MENU				0x17

#define MOTOR_DIRECTION_VAL_MENU	0x20
#define BRUSH_VAL_MENU				0x21
#define VACUUM_VAL_MENU				0x22
#define WATER_PUMP_VAL_MENU			0x23
#define DET_PUMP_VAL_MENU			0x24
#define EXTRA_PRESSURE_VAL_MENU		0x25
#define HEADLIGHTS_VAL_MENU			0x26
#define HORN_VAL_MENU				0x27

void start_test_menu(void);
uint8_t test_mode;

#endif /* TEST_MODE_H_ */