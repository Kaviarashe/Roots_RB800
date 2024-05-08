/*
 * intercommunication.h
 *
 * Created: 11/20/2015 9:53:57 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef INTERCOMMUNICATION_H_
#define INTERCOMMUNICATION_H_

#include "RB_800.h"

#include "../../Common/CANIds.h"

#define POT_CALIBRATING_LOW			0xAA
#define POT_CALIBRATING_HIGH		0xBB
#define POT_CALIBRATING_COMPLETE	0xCC
#define POT_CALIBRATING_CANCELLED	0xDD

#define DM_SETTINGS_1		0x11
#define DM_SETTINGS_2		0x22
#define REQUEST_DM_SETTINGS	0x12
#define UPDATE_DM_SETTINGS	0x34
#define REQUEST_STATISTICS	0x47

#define BACK_SAFETY_ALERT		0xAA
#define BACK_SAFETY_RESOLVED	0xBB
#define BACK_SAFETY_UNRESOLVED	0xCC

#define ALL_STATUS	0xCC

#define BRUSH_RUNNING		0xAA
#define BRUSH_OFF			0xBB
#define VACUUM_RUNNING		0xAA
#define VACUUM_RUNNING_OFF	0xBB
#define VACUUM_OFF			0xCC

#define A1_CAN		0xA1
#define A2_CAN		0xA2
#define A3_CAN		0xA3


#define POT_FAIL				1
#define PEDAL_SWITCH_CLOSED		2
#define PEDAL_SWITCH_OPEN		3
#define OVERVOLTAGE				4
#define MANUAL_SWITCH_CLOSED	5
#define MANUAL_SWITCH_OPEN		6
#define A3_OVERCURRENT			7
#define MOTOR_PARAMETER			8
#define CAN_BUFFER_OVERFLOW		9
#define SD_BUFFER_OVERFLOW		10
#define SD_CARD_FAIL			11
#define FUSE_FAIL				12
#define BRAKE_SEAT_SWITCH_OPEN	13
#define BRAKE_SEAT_SWITCH_CLOSED	14
#define FAULT_FAIL				15
#define BM_OVERCURRENT			16
#define FLOAT_SENSE_WATER		17

#define PEDAL_STATE 0xAA
#define PEDAL_LEVEL	0xBB

#define BRUSH_HOURS					1
#define VACUUM_HOURS				2
#define PRESWEEP_HOURS				3
#define ECO_HOURS					4
#define WATER_PUMP_HOURS			5
#define DETERGENT_PUMP_HOURS		6
#define BRUSH1_AVERAGE_CURRENT		7
#define BRUSH2_AVERAGE_CURRENT		8
#define VACUUM_AVERAGE_CURRENT		9
#define PRESWEEP_AVERAGE_CURRENT	10
#define BRUSH1_MAX_CURRENT			11
#define BRUSH2_MAX_CURRENT			12
#define VACUUM_MAX_CURRENT			13
#define PRESWEEP_MAX_CURRENT		14


void update_all_settings(uint8_t* data);
void processCAN(uint16_t identifier, uint8_t* data);
void update_brush_state(void);
void update_vacuum_state(void);
void update_pre_swp_state(void);
void update_w_pump_state(void);
void update_d_pump_state(void);
void send_CAN_ACK(void);
void send_serial_num(void);
void send_reset_CAN(void);
void send_CAN_buffer_overflow(void);
void send_SD_buffer_overflow(void);
void send_SD_card_fail(void);
void send_statistics(void);

uint8_t current_checksum1;
uint16_t current_checksum2;

uint8_t can_functioning;
uint8_t A1_ack_received;
uint8_t A3_ack_received;

#endif /* INTERCOMMUNICATION_H_ */