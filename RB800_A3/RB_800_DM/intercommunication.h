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
#define KEY_SWITCH_OPEN			17

#define PEDAL_STATE 0xAA
#define PEDAL_LEVEL	0xBB


void processCAN(uint16_t identifier, uint8_t* data);
void update_pedal_state(void);
void update_pedal_level(void);
void send_back_safety_error(uint8_t state);
void send_top_error(void);
void send_pedal_error(uint8_t error);
void send_CAN_ACK(void);
void send_serial_num(void);
void send_overcurrent_error(void);
void send_reset_CAN(void);
void send_motor_parameter_error(void);
void send_CAN_buffer_overflow(void);
void send_drive_motor_current(void);
void send_max_drive_motor_current(void);
void send_brake_seat_error(uint8_t error);
void send_key_error(uint8_t error);

volatile uint8_t can_functioning;
uint8_t A1_ack_received;
uint8_t A2_ack_received;

volatile uint8_t send_pedal_switch_error;
volatile uint8_t send_manual_switch_error;
volatile uint8_t send_brake_seat_switch_error;
volatile uint8_t send_key_switch_error;

uint16_t temp1;
uint16_t temp2;


#endif /* INTERCOMMUNICATION_H_ */