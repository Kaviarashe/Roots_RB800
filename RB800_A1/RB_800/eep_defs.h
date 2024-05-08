//IVA003: EEPROM Location definitions for A1 board
#ifndef _EEP_DEFS_H_
#define _EEP_DEFS_H_

//Motor Setting Locations	
#define EEP8_BRAKE_DELAY				0x10
#define EEP8_REVERSE_SPEED				0x11
#define EEP8_FAST_SPEED					0x12
#define EEP8_SLOW_SPEED					0x13
#define EEP8_ACCELERATION				0x14
#define EEP8_DECELERATION				0x15
#define EEP8_CURRENT_RATING				0x16
#define EEP8_MAX_CURRENT				0x17
#define EEP8_OVERLOAD_TIME				0x18
#define EEP8_VACUUM_SLOW_START			0x19
#define EEP8_BRUSH_SLOW_START			0x1A
#define EEP8_PRE_SWP_SLOW_START			0x1B


//Address to check if device has ever been set up
#define EEP8_DEVICE_SETTINGS_SET_ADDR	0x20
#define DEVICE_SETTINGS_SET				0xAA

#define EEP8_BATTERY_TYPE				0x21
#define EEP8_BATTERY_LEVEL_ADDR			0x22
#define EEP8_MOTORS_STATUS_ADDR			0x23

//Serial Num Location
#define EEPxx_SERIAL_NUM				0x30
#define SERIAL_NUM_LEN					16

#define EEP8_MINUTES_ADDR				0x40
#define EEP8_MINUTES_CHKSM				0x41
#define EEP16_HOURS_ADDR				0x42
#define EEP16_HOURS_CHKSM				0x44

#define EEP8_MINUTES_BKP				0x50
#define EEP8_MINUTES_BKP_CKSM			0x51
#define EEP16_HOURS_BKP					0x52
#define EEP16_HOURS_BKP_CHKSM			0x54

//Auto mode settings
#define EEP8_AUTO_MODE_SET_ADDR			0x60
#define EEPxx_AUTO_MODE_ADDR			0x61
#define NUM_AUTO_SETTINGS				6
#define AUTO_MODE_SET					0xAA

#define	EEPxx_A1_SERIAL_NUM				0x70
#define A1_SERIAL_NUM_LEN 				14

#endif//_EEP_DEFS_H_
