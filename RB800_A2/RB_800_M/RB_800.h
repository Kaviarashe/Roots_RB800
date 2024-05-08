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
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <math.h>

#include "eep_defs.h"
#include "SPI.h"
#include "CAN.h"
#include "lcd_screen.h"
#include "twi_master_driver.h"
#include "timers.h"
#include "alarm.h"
#include "keypad_settings.h"
#include "motor.h"
#include "intercommunication.h"
#include "adc.h"
#include "can_queue.h"
#include "SD_card.h"
#include "event_logging.h"
#include "event_log_queue.h"
#include "RB_settings.h"

//#define BENCH_TEST //uncomment this to run the code in the bench board (not in the machine)

 //TWI Settings
#define CPU_SPEED   32000000
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

//TWI master global variable
TWI_Master_t twiMaster;

void initialize(void);

void check_CAN(void);

#define TRUE	1
#define FALSE	0

#define DC_100_PERCENT	100
#define DC_0_PERCENT	0

///// for testing purpose for reset issue//////////////////////
volatile uint16_t status_reg_value;
volatile char PORF_cnt;
volatile char EXTRF_cnt;
volatile char BORF_cnt;
volatile char WDRF_cnt;
volatile char PDIRF_cnt;
volatile char SRF_cnt;
volatile char SDRF_cnt;
//////////////////////////////////////////////////////////////

uint8_t print_menu;
volatile uint8_t test8;
volatile uint16_t test16;
volatile uint16_t test16_1;
volatile uint16_t test16_2;
volatile uint16_t test16_3;
volatile uint16_t test16_4;
volatile uint16_t test16_5;
volatile uint32_t test32;
volatile float testf;

#ifndef __SIM__
#define DBG(s)
#define DBG1(s,v1)
#define DBG2(s,v1,v2)
#define YIELD()
#define YIELD_ADC(x,y)
#define REBOOT()	while(true)
#define IDLE_LOOP	while(true)
#define __SIM_DOUT
#define __SIM_SET_PWM(x,y)
#endif

#endif /* RB_800_H_ */