//IVA003: EEPROM Location definitions for A3 board
#ifndef _EEP_DEFS_H_
#define _EEP_DEFS_H_

 //Motor Setting Locations
#define EEP8_BRAKE_DELAY_ADDR				0x10
#define EEP8_REVERSE_SPEED_ADDR				0x11
#define EEP8_FAST_SPEED_ADDR				0x12
#define EEP8_SLOW_SPEED_ADDR				0x13
#define EEP8_ACCELERATION_ADDR				0x14
#define EEP8_DECELERATION_ADDR				0x15
#define EEP8_CURRENT_RATING_ADDR			0x16
#define EEP8_MAX_CURRENT_ADDR				0x17
#define EEP8_OVERLOAD_TIME_ADDR				0x18
#define EEP16_POT_LOW_DEADBAND_ADDR			0x19 //2 bytes
#define EEP16_POT_HIGH_DEADBAND_ADDR		0x1B //2 bytes
#define EEP8_CHECKSUM_ADDR					0x1F

//Address to check if device has ever been set up
#define EEP8_DEVICE_SETTINGS_SET_ADDR		0x20
#define DEVICE_SETTINGS_SET					0xAA

//Serial Num Location
#define	EEPxx_A3_SERIAL_NUM					0x40
#define A3_SERIAL_NUM_LEN					14

//// TESTING PURPOSER TO STORE THE FLAG VALUES//////////////////////////////reset issue
//#define EEP_PORF_cnt_location 0xc0
//#define EEP_EXTRF_cnt_location 0xc1
//#define EEP_BORF_cnt_location 0xc2
//#define EEP_WDRF_cnt_location 0xc3
//#define EEP_PDIRF_cnt_location 0xc4
//#define EEP_SRF_cnt_location 0xc5
//#define EEP_SDRF_cnt_location 0xc6
#endif//_EEP_DEFS_H_
