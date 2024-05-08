/*
* RB_800.h
*
* Created: 7/1/2015 10:15:31 AM
*  Author: Anurag Vaddi Reddy
*/

#ifndef RB_800_H_
#define RB_800_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "eep_defs.h"
#include "SPI.h"
#include "CAN.h"
#include "lcd_screen.h"
#include "twi_master_driver.h"
#include "timers.h"
#include "alarm.h"
#include "keypad.h"
#include "keypad_settings.h"
#include "RB_settings.h"
#include "settings_menu.h"
#include "intercommunication.h"
#include "test_mode.h"
#include "battery.h"
#include "can_queue.h"
#include "event_logging.h"
#include "user_menu.h"

//#define BENCH_TEST //uncomment this to run the code in the bench board (not in the machine)

//TWI Settings
#define CPU_SPEED   32000000
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

//TWI master global variable
TWI_Master_t twiMaster;

void initialize(void);
void check_CAN(void);
void print_info(void);

#define TRUE	1
#define FALSE	0

volatile uint8_t print_menu;
volatile uint8_t update_menu;
volatile uint8_t print_battery_low;
volatile uint8_t print_battery_off;
volatile uint8_t turn_off_motors_timer;
//For sending CAN data on change on any value
volatile uint8_t change_on_value;
//For sending CAN data periodically
volatile uint8_t periodic_send;
volatile uint8_t periodic_count;

volatile uint8_t battery_low_counter;
volatile uint8_t battery_off_counter;

volatile uint8_t printing_info_counter;
uint8_t printing_info;
uint16_t printed_info;

uint8_t fail;
uint8_t motor_info_sent;
uint8_t horn_off;

volatile uint8_t test8;

volatile uint8_t user_settings_menu_enable;
uint8_t CanError;

#ifndef __SIM__
#define DBG(s)
#define DBG1(s,v1)
#define DBG2(s,v1,v2)
#define DBG_IDLE(s)
#define YIELD()
#define WHILE_TRUE()	while(true)
#define IDLE_LOOP		while(true)
#define __SIM_DOUT
#define __SIM_SET_PWM(x,y)
#endif

#endif /* RB_800_H_ */