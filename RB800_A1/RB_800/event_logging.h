/*
 * event_logging.h
 *
 * Created: 7/13/2016 11:09:55 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef EVENT_LOGGING_H_
#define EVENT_LOGGING_H_

#include "RB_800.h"

 //List of events
#define FIRST_EVENT				0x10
#define RB800_ON_EVENT			0x10
#define BRUSH_ON_EVENT			0x11
#define BRUSH_OFF_EVENT			0x12
#define VACUUM_ON_EVENT			0x13
#define VACUUM_OFF_EVENT		0x14
#define PRE_SWP_ON_EVENT		0x15
#define PRE_SWP_OFF_EVENT		0x16
#define BATTERY_VOLTAGE_EVENT	0x17
#define LOW_BATTERY_EVENT		0x18
#define ZERO_BATTERY_EVENT		0x19
#define TANK_FULL_EVENT			0x1A
#define SD_MISSING_EVENT		0x1B
#define FUSE_BLOWN_EVENT		0x1C
#define A3_ERROR_EVENT			0x1D
#define A2_TOP_EVENT			0x1E
#define A3_TOP_EVENT			0x1F
#define DETERGENT_ON_EVENT		0x20
#define DETERGENT_OFF_EVENT		0x21
#define WATER_ON_EVENT			0x22
#define WATER_OFF_EVENT			0x23
#define BRUSH_1_CURRENT			0x24
#define BRUSH_2_CURRENT			0x25
#define VACUUM_CURRENT			0x26
#define PRESWEEP_CURRENT		0x27
#define DRIVE_CURRENT			0x28
#define ECO_ON_EVENT			0x29
#define ECO_OFF_EVENT			0x2A
#define AUTO_MODE_EVENT			0x2B
#define TOP_EVENT				0x2C
#define TRANSPORT_ON_EVENT		0x2D
#define TRANSPORT_OFF_EVENT		0x2E
#define REVERSE_ON_EVENT		0x2F
#define REVERSE_OFF_EVENT		0x30
#define AUTO_MODE_CHANGED		0x31
#define PEDAL_DOWN_EVENT		0x32
#define PEDAL_UP_EVENT			0x33
#define MAX_BRUSH_1_CURRENT		0x34
#define MAX_BRUSH_2_CURRENT		0x35
#define MAX_VACUUM_CURRENT		0x36
#define MAX_PRESWEEP_CURRENT	0x37
#define MAX_DRIVE_CURRENT		0x38
#define LAST_EVENT				0x39

void log_event(uint8_t event_num);
void log_event_data(uint8_t event_code, uint8_t data[], uint8_t num_bytes);

#endif /* EVENT_LOGGING_H_ */