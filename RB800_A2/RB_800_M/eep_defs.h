//IVA003: EEPROM Location definitions for A2 board
#ifndef _EEP_DEFS_H_
#define _EEP_DEFS_H_

//#define EEP_AUTO_MODE_SET_ADDR			0x04
//#define AUTO_MODE_SET					0xAA
//#define EEP_AUTO_MODE_ADDR				0x05
//#define NUM_AUTO_SETTINGS				6


//Motor Settings Location
#define EEP8_VACUUM_SLOW_START			0x18
#define EEP8_BRUSH_SLOW_START			0x19
#define EEP8_PRE_SWP_SLOW_START			0x1A

//Address to check if device has ever been set up
#define EEP8_DEVICE_SETTINGS_SET_ADDR	0x20
#define DEVICE_SETTINGS_SET				0xAA

//Serial Num Location
#define	EEPxx_A2_SERIAL_NUM				0x40
#define A2_SERIAL_NUM_LEN				14

#define EEPxx_SD_CARD_MISSING_ADDR		0x50

//Motor statistics Location
#define EEP8_BRUSH_MINUTES_ADDR			0X70
#define EEP8_VACUUM_MINUTES_ADDR		0X71
#define EEP8_PRESWEEP_MINUTES_ADDR		0X72
#define EEP8_ECO_MINUTES_ADDR			0X73
#define EEP8_WATER_PUMP_MINUTES_ADDR	0X74
#define EEP8_DETERGENT_PUMP_MINUTES		0X75

// TESTING PURPOSER TO STORE THE FLAG VALUES//////////////////////////////reset issue
//#define EEP_PORF_cnt_location			0xc0
//#define EEP_EXTRF_cnt_location		0xc1
//#define EEP_BORF_cnt_location			0xc2
//#define EEP_WDRF_cnt_location			0xc3
//#define EEP_PDIRF_cnt_location		0xc4
//#define EEP_SRF_cnt_location			0xc5
//#define EEP_SDRF_cnt_location			0xc6
#endif//_EEP_DEFS_H_
