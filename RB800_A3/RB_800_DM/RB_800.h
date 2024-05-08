/*
 * RB_800.h
 *
 * Created: 6/15/2016 3:17:10 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef RB_800_H_
#define RB_800_H_

#define F_CPU 32000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "eep_defs.h"
#include "timers.h"
#include "lcd_screen.h"
#include "twi_master_driver.h"
#include "motor.h"
#include "adc.h"
#include "SPI.h"
#include "CAN.h"
#include "can_queue.h"
#include "intercommunication.h"
#include "motor_settings.h"

//#define BENCH_TEST //uncomment this to run the code in the bench board (not in the machine)

 //TWI Settings
#define CPU_SPEED   32000000
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

#define ACTIVITY_LED	2//PD2

#define ACTIVITY_LED_ON		PORTD_OUTCLR = (1 << ACTIVITY_LED);__SIM_DOUT
#define ACTIVITY_LED_OFF	PORTD_OUTSET = (1 << ACTIVITY_LED);__SIM_DOUT
#define ACTIVITY_LED_TGL	PORTD_OUTTGL = (1 << ACTIVITY_LED);__SIM_DOUT

#define TRUE	1
#define FALSE	0

#define SENT 2

//TWI master global variable
TWI_Master_t twiMaster;

volatile uint8_t test8;
volatile uint16_t test16;
volatile uint16_t test16_1;
volatile float testf;

void initialize(void);

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

#ifndef __SIM__
#define DBG(s)
#define DBG1(s,v1)
#define DBG2(s,v1,v2)
#define YIELD()
#define YIELD_ADC(x,y)
#define REBOOT()	while(true)
#define IDLE_LOOP	while()
#define __SIM_DOUT
#define __SIM_SET_PWM(x,y)
#endif

#endif /* RB_800_H_ */