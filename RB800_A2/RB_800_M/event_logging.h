/*
* event_logging.h
*
* Created: 6/29/2016 3:51:53 PM
*  Author: Anurag Vaddi Reddy
*/


#ifndef EVENT_LOGGING_H_
#define EVENT_LOGGING_H_

#include "RB_800.h"

#define EVENT_LOGGING_INFO_ADDR 0x01
//#define SERIAL_LOGGING_INFO_ADDR 0x02
#define FIRST_ENTRY_ADDRESS_VAL 0x02

#define EVENT_LOGGING_HEADER	"OPUS SOLUTIONS"
//#define EVENT_LOGGING_HEADER	"OPUS"
#define SERIAL_HEADER	"ABCD"


//The max number of entries per block is 40 and each entry will be at least 7 bytes
//Setting a max number of entries per block makes data retrieval much simpler
//Each block is 512 bytes but only 500 is used
#define ENTRIES_PER_BLOCK			40
#define MINIMUM_BYTES_PER_ENTRY		7
#define ENTRY_BYTES_PER_BLOCK		500


#define STATISTICS_STARTING_POS		

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
#define LAST_EVENT				0x38

void initialize_event_logging(void);
void log_event(uint8_t* event_data);
void read_event(uint32_t num);
void send_event(uint32_t num);
void clear_event_buffer(void);
void update_statistics(void);

volatile uint32_t num_entries;
volatile char event_buffer[ENTRY_BYTES_PER_BLOCK];
volatile uint8_t event_logging;

volatile uint16_t current_position;

uint8_t check_sd_card;

uint8_t num_entries_pos;

volatile uint8_t brush_minutes;
volatile uint8_t vacuum_minutes;
volatile uint8_t presweep_minutes;
volatile uint8_t eco_minutes;
volatile uint8_t water_pump_minutes;
volatile uint8_t detergent_pump_minutes;

volatile uint16_t brush_hours;
volatile uint16_t vacuum_hours;
volatile uint16_t presweep_hours;
volatile uint16_t eco_hours;
volatile uint16_t water_pump_hours;
volatile uint16_t detergent_pump_hours;

volatile uint16_t brush1_average_current;
volatile uint16_t brush2_average_current;
volatile uint16_t vacuum_average_current;
volatile uint16_t presweep_average_current;

volatile uint16_t max_brush1_overall_current;
volatile uint16_t max_brush2_overall_current;
volatile uint16_t max_vacuum_overall_current;
volatile uint16_t max_presweep_overall_current;

uint8_t update_counter;


#endif /* EVENT_LOGGING_H_ */