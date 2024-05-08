/*
 * keypad.h
 *
 * Created: 7/13/2015 1:57:08 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "RB_800.h"

 //Separate Controls
#define LCD_LED	4//PA4
#define LCD_LED_ON	PORTA_OUTSET = (1 << LCD_LED);
#define LCD_LED_OFF	PORTA_OUTCLR = (1 << LCD_LED);
#define LCD_LED_TGL	PORTA_OUTTGL = (1 << LCD_LED);

//PORT A
#define S_SLD	3
#define L_P2	6
#define S_HL	7

#define HL_SLD_SWITCH		PORTA_INT0_vect

#define S_SLD_NUM	0
#define S_HL_NUM	1

#define L_P2_NUM	0

#define S_SLD_LOW	!(PORTA_IN & (1 << S_SLD))
#define S_HL_LOW	!(PORTA_IN & (1 << S_HL))

#define L_P2_OFF	PORTA_OUTSET = (1 << L_P2);
#define L_P2_ON		PORTA_OUTCLR = (1 << L_P2);

//PORT B
#define L_HL	0
#define L_P5	1
#define L_P4	2
#define S_PU	3
#define	L_P3	4
#define S_SLO	5
#define L_SLO	6
#define L_SLD	7

#define	PU_SLO_SWITCH	PORTB_INT0_vect

#define S_PU_NUM	2
#define S_SLO_NUM	3

#define L_HL_NUM	1
#define L_P5_NUM	2
#define L_P4_NUM	3
#define	L_P3_NUM	4
#define L_SLO_NUM	5
#define L_SLD_NUM	6

#define S_PU_LOW	!(PORTB_IN & (1 << S_PU))
#define S_SLO_LOW	!(PORTB_IN & (1 << S_SLO))

//IVA002: __SIM_DOUT added for Similator adaptation
#define L_HL_OFF	PORTB_OUTSET = (1 << L_HL);__SIM_DOUT
#define L_P5_OFF	PORTB_OUTSET = (1 << L_P5);__SIM_DOUT
#define L_P4_OFF	PORTB_OUTSET = (1 << L_P4);__SIM_DOUT
#define L_P3_OFF	PORTB_OUTSET = (1 << L_P3);__SIM_DOUT
#define L_SLO_OFF	PORTB_OUTSET = (1 << L_SLO);__SIM_DOUT
#define L_SLD_OFF	PORTB_OUTSET = (1 << L_SLD);__SIM_DOUT

#define L_HL_ON		PORTB_OUTCLR = (1 << L_HL);__SIM_DOUT
#define L_P5_ON		PORTB_OUTCLR = (1 << L_P5);__SIM_DOUT
#define L_P4_ON		PORTB_OUTCLR = (1 << L_P4);__SIM_DOUT
#define L_P3_ON		PORTB_OUTCLR = (1 << L_P3);__SIM_DOUT
#define L_SLO_ON	PORTB_OUTCLR = (1 << L_SLO);__SIM_DOUT
#define L_SLD_ON	PORTB_OUTCLR = (1 << L_SLD);__SIM_DOUT

#define L_SLD_TGL	PORTB_OUTTGL = (1 << L_SLD);__SIM_DOUT
//PORT H
#define L_ECO	0
#define S_ECO	1
#define S_TR	2
#define L_EP	3
#define	L_TR	4
#define S_EP	5
#define L_D5	6
#define S_DU	7

#define ECO_TR_DU_SWITCH	PORTH_INT0_vect
#define EP_SWITCH			PORTH_INT1_vect

#define S_ECO_NUM	4
#define S_TR_NUM	5
#define S_EP_NUM	6
#define S_DU_NUM	7

#define L_ECO_NUM	7
#define L_EP_NUM	8
#define L_TR_NUM	9
#define L_D5_NUM	10

#define S_ECO_LOW	!(PORTH_IN & (1 << S_ECO))
#define	S_TR_LOW	!(PORTH_IN & (1 << S_TR))
#define S_EP_LOW	!(PORTH_IN & (1 << S_EP))
#define S_DU_LOW	!(PORTH_IN & (1 << S_DU))

#define L_ECO_OFF	PORTH_OUTSET = (1 << L_ECO);__SIM_DOUT
#define L_EP_OFF	PORTH_OUTSET = (1 << L_EP);__SIM_DOUT
#define L_TR_OFF	PORTH_OUTSET = (1 << L_TR);__SIM_DOUT
#define L_D5_OFF	PORTH_OUTSET = (1 << L_D5);__SIM_DOUT

#define L_ECO_ON	PORTH_OUTCLR = (1 << L_ECO);__SIM_DOUT
#define L_EP_ON		PORTH_OUTCLR = (1 << L_EP);__SIM_DOUT
#define L_TR_ON		PORTH_OUTCLR = (1 << L_TR);__SIM_DOUT
#define L_D5_ON		PORTH_OUTCLR = (1 << L_D5);__SIM_DOUT

//PORT J
#define S_RV	0
#define L_D4	1
#define	L_D3	2
#define L_D2	3
#define	L_D1	4
#define S_DD	5
#define L_VAC	6
#define S_VAC	7

#define RV_DD_SWITCH	PORTJ_INT0_vect
#define VAC_SWITCH		PORTJ_INT1_vect

#define	S_RV_NUM	8
#define	S_DD_NUM	9
#define S_VAC_NUM	10

#define L_D4_NUM	9
#define L_D3_NUM	10
#define L_D2_NUM	11
#define L_D1_NUM	12
#define L_VAC_NUM	13

#define S_RV_LOW	!(PORTJ_IN & (1 << S_RV))
#define S_DD_LOW	!(PORTJ_IN & (1 << S_DD))
#define	S_VAC_LOW	!(PORTJ_IN & (1 << S_VAC))

#define L_D4_OFF	PORTJ_OUTSET = (1 << L_D4);__SIM_DOUT
#define L_D3_OFF	PORTJ_OUTSET = (1 << L_D3);__SIM_DOUT
#define L_D2_OFF	PORTJ_OUTSET = (1 << L_D2);__SIM_DOUT
#define L_D1_OFF	PORTJ_OUTSET = (1 << L_D1);__SIM_DOUT
#define L_VAC_OFF	PORTJ_OUTSET = (1 << L_VAC);__SIM_DOUT

#define L_D4_ON		PORTJ_OUTCLR = (1 << L_D4);__SIM_DOUT
#define L_D3_ON		PORTJ_OUTCLR = (1 << L_D3);__SIM_DOUT
#define L_D2_ON		PORTJ_OUTCLR = (1 << L_D2);__SIM_DOUT
#define L_D1_ON		PORTJ_OUTCLR = (1 << L_D1);__SIM_DOUT
#define L_VAC_ON	PORTJ_OUTCLR = (1 << L_VAC);__SIM_DOUT

#define L_VAC_TGL	PORTJ_OUTTGL = (1 << L_VAC);__SIM_DOUT

//PORT K
#define L_RV	0
#define S_HRN	1
#define S_BR	2
#define L_BR	3
#define	S_ATO	4
#define L_ATO	5
#define L_P1	6
#define S_PD	7

#define HRN_SWITCH			PORTK_INT0_vect
#define ATO_PD_BR_SWITCH	PORTK_INT1_vect

#define S_HRN_NUM	11
#define S_BR_NUM	12
#define S_ATO_NUM	13
#define S_PD_NUM	14

#define L_RV_NUM	15
#define L_BR_NUM	16
#define L_ATO_NUM	17
#define L_P1_NUM	18

#define S_HRN_LOW	!(PORTK_IN & (1 << S_HRN))
#define S_BR_LOW	!(PORTK_IN & (1 << S_BR))
#define	S_ATO_LOW	!(PORTK_IN & (1 << S_ATO))
#define S_PD_LOW	!(PORTK_IN & (1 << S_PD))

#define L_RV_OFF	PORTK_OUTSET = (1 << L_RV);__SIM_DOUT
#define L_BR_OFF	PORTK_OUTSET = (1 << L_BR);__SIM_DOUT
#define L_ATO_OFF	PORTK_OUTSET = (1 << L_ATO);__SIM_DOUT
#define L_P1_OFF	PORTK_OUTSET = (1 << L_P1);__SIM_DOUT

#define L_RV_ON		PORTK_OUTCLR = (1 << L_RV);__SIM_DOUT
#define L_BR_ON		PORTK_OUTCLR = (1 << L_BR);__SIM_DOUT
#define L_ATO_ON	PORTK_OUTCLR = (1 << L_ATO);__SIM_DOUT
#define L_P1_ON		PORTK_OUTCLR = (1 << L_P1);__SIM_DOUT

#define L_BR_TGL	PORTK_OUTTGL = (1 << L_BR);__SIM_DOUT

#define NUM_SWITCHES	15
#define NUM_LED			19

void initialize_keypad(void);
uint8_t switch_low(uint8_t switch_num);
void clear_switch_pressed(void);

volatile uint8_t switch_down[NUM_SWITCHES];
volatile uint16_t switch_counter[NUM_SWITCHES];
volatile uint8_t switch_pressed[NUM_SWITCHES];
volatile uint8_t switch_debounce[NUM_SWITCHES];

volatile uint8_t ep_down;
volatile uint8_t ep_down_counter;
volatile uint8_t ep_up;

volatile uint8_t hrn_down;
volatile uint8_t hrn_down_counter;
volatile uint8_t hrn_up;

//volatile uint8_t led_state[NUM_LED];

#endif /* KEYPAD_H_ */