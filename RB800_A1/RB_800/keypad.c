/*
 * keypad.c
 *
 * Created: 7/13/2015 1:56:59 PM
 *  Author: Anurag Vaddi Reddy
 */

#include "keypad.h"

void initialize_keypad(void)
{
	//Set Direction of switches and LED's by PORT
	//Switches are inputs, LED's are outputs
	//LED's are active low so all are set to high
	//Turn on interrupts for all switches
	//Falling edge & low level interrupts
	//PORT A
	PORTA_DIRCLR = (1 << S_HL) | (1 << S_SLD);
	PORTA_INT0MASK = (1 << S_HL) | (1 << S_SLD);	//Turn on interrupt
	PORTA_INTCTRL = 0x05;			//Low level interrupts
	PORTA_PIN7CTRL = 0x02;			//Falling Edge
	PORTA_PIN3CTRL = 0x02;

	PORTA_DIRSET = (1 << L_P2);

	//PORT B
	PORTB_DIRCLR = (1 << S_PU) | (1 << S_SLO);
	PORTB_INT0MASK = (1 << S_PU) | (1 << S_SLO);
	PORTB_INTCTRL = 0x05;
	PORTB_PIN3CTRL = 0x02;
	PORTB_PIN5CTRL = 0x02;

	PORTB_DIRSET = (1 << L_HL) | (1 << L_P5) | (1 << L_P4) | (1 << L_P3) | (1 << L_SLO) | (1 << L_SLD);
	PORTB_OUTSET = (1 << L_HL) | (1 << L_P5) | (1 << L_P4) | (1 << L_P3) | (1 << L_SLO) | (1 << L_SLD);

	//PORT F
	//PORTF_DIRSET = (1 << 3);
	//PORT H
	PORTH_DIRCLR = (1 << S_ECO) | (1 << S_TR) | (1 << S_EP) | (1 << S_DU);
	PORTH_INT0MASK = (1 << S_ECO) | (1 << S_TR) | (1 << S_DU);
	PORTH_INT1MASK = (1 << S_EP);
	PORTH_INTCTRL = 0x05;
	PORTH_PIN1CTRL = 0x02; PORTH_PIN2CTRL = 0x02; PORTH_PIN5CTRL = 0x00; PORTH_PIN7CTRL = 0x02;

	PORTH_DIRSET = (1 << L_ECO) | (1 << L_EP) | (1 << L_TR) | (1 << L_D5);
	PORTH_OUTSET = (1 << L_ECO) | (1 << L_EP) | (1 << L_TR) | (1 << L_D5);

	//PORT J
	PORTJ_DIRCLR = (1 << S_RV) | (1 << S_DD) | (1 << S_VAC);
	PORTJ_INT0MASK = (1 << S_RV) | (1 << S_DD);
	PORTJ_INT1MASK = (1 << S_VAC);
	PORTJ_INTCTRL = 0x05;
	PORTJ_PIN0CTRL = 0x02; PORTJ_PIN5CTRL = 0x02; PORTJ_PIN7CTRL = 0x02;

	PORTJ_DIRSET = (1 << L_D4) | (1 << L_D3) | (1 << L_D2) | (1 << L_D1) | (1 << L_VAC);
	PORTJ_OUTSET = (1 << L_D4) | (1 << L_D3) | (1 << L_D2) | (1 << L_D1) | (1 << L_VAC);

	//PORT K
	PORTK_DIRCLR = (1 << S_HRN) | (1 << S_BR) | (1 << S_ATO) | (1 << S_PD);
	PORTK_INT0MASK = (1 << S_HRN);
	PORTK_INT1MASK = (1 << S_BR) | (1 << S_ATO) | (1 << S_PD);
	PORTK_INTCTRL = 0x05;
	PORTK_PIN1CTRL = 0x00; PORTK_PIN2CTRL = 0x02; PORTK_PIN4CTRL = 0x02; PORTK_PIN7CTRL = 0x02;

	PORTK_DIRSET = (1 << L_RV) | (1 << L_BR) | (1 << L_ATO) | (1 << L_P1);
	PORTK_OUTSET = (1 << L_RV) | (1 << L_BR) | (1 << L_ATO) | (1 << L_P1);

	//Initialize all the switch position to not being pressed
	for (int s_num = 0; s_num < NUM_SWITCHES; s_num++)
	{
		switch_down[s_num] = FALSE;
		switch_counter[s_num] = 0;
		switch_pressed[s_num] = FALSE;
		switch_debounce[s_num] = FALSE;
	}
}

//Check if a switch is low
uint8_t switch_low(uint8_t switch_num)
{
	switch (switch_num)
	{
	case S_SLD_NUM: return S_SLD_LOW;
	case S_HL_NUM: return S_HL_LOW;
	case S_PU_NUM: return S_PU_LOW;
	case S_SLO_NUM: return S_SLO_LOW;
	case S_ECO_NUM: return S_ECO_LOW;
	case S_TR_NUM: return S_TR_LOW;
	case S_EP_NUM: return S_EP_LOW;
	case S_DU_NUM: return S_DU_LOW;
	case S_RV_NUM: return S_RV_LOW;
	case S_DD_NUM: return S_DD_LOW;
	case S_VAC_NUM: return S_VAC_LOW;
	case S_HRN_NUM: return S_HRN_LOW;
	case S_BR_NUM: return S_BR_LOW;
	case S_ATO_NUM: return S_ATO_LOW;
	case S_PD_NUM: return S_PD_LOW;

	default: return FALSE;
	}
}

//Clear all switch pressed variables
//Usually used after a long period of delay when key presses will be ignored
void clear_switch_pressed(void)
{
	//Initialize all the switch position to not being pressed
	for (int s_num = 0; s_num < NUM_SWITCHES; s_num++)
	{
		switch_down[s_num] = FALSE;
		switch_counter[s_num] = 0;
		switch_pressed[s_num] = FALSE;
		switch_debounce[s_num] = FALSE;
	}
}

//Interrupts that set a switch pressed to true if switch down is detected
//Also included switch up for horn and extra pressure

 /*Function:
 *HRN - Turn on/off horn
 */
ISR(HL_SLD_SWITCH)
{
	if (S_HL_LOW)
	{
		if (!switch_down[S_HL_NUM])
		{
			switch_down[S_HL_NUM] = TRUE;
			switch_counter[S_HL_NUM] = 0;
		}
	}
	else if (S_SLD_LOW)
	{
		if (!switch_down[S_SLD_NUM])
		{
			switch_down[S_SLD_NUM] = TRUE;
			switch_counter[S_SLD_NUM] = 0;
		}
	}
}

/*Function:
*SLO - Set slow/fast moving speed
*PU - Increase water pump flow
*/
ISR(PU_SLO_SWITCH)
{
	if (S_SLO_LOW)
	{
		if (!switch_down[S_SLO_NUM])
		{
			switch_down[S_SLO_NUM] = TRUE;
			switch_counter[S_SLO_NUM] = 0;
		}
	}
	if (!(reverse || transport || top_error) && S_PU_LOW)
	{
		if (!switch_down[S_PU_NUM])
		{
			switch_down[S_PU_NUM] = TRUE;
			switch_counter[S_PU_NUM] = 0;
		}
	}
}

/*
 *Function:
 *ECO - Eco mode switch
 *TR  - Transport Mode switch
 *		Transport mode turns off all cleaning functions to allow movement without cleaning
 *DU - Increase detergent dosage
 */
ISR(ECO_TR_DU_SWITCH)
{
	if (!switch_down[S_ECO_NUM] && !top_error)
	{
		switch_down[S_ECO_NUM] = TRUE;
		switch_counter[S_ECO_NUM] = 0;
	}
	if (!switch_down[S_TR_NUM] && S_TR_LOW && !top_error)
	{
		switch_down[S_TR_NUM] = TRUE;
		switch_counter[S_TR_NUM] = 0;
	}
	if ((test_mode || !(reverse || transport || top_error)) && S_DU_LOW)
	{
		if (!switch_down[S_DU_NUM])
		{
			switch_down[S_DU_NUM] = TRUE;
			switch_counter[S_DU_NUM] = 0;
		}
	}
}

/*
 *Function:
 *EP -  Extra brush pressure
 */
ISR(EP_SWITCH)
{
	if ((reverse || transport || !brush || (brush_state == OFF) || top_error) && user_settings_menu_enable == FALSE)
	{
		return;
	}
	if (!user_settings_menu_enable)// while not in user or settings menu
	{
		if (S_EP_LOW && !(ep_down || ep_up))
		{
			switch_pressed[S_EP_NUM] = TRUE;
			switch_counter[S_EP_NUM] = 0;
			ep_down = TRUE;
			ep_down_counter = 0;
		}
		else if (ep_down && ep_down_counter >= _30_MS)
		{
			ep_up = TRUE;
			ep_down = FALSE;
		}
	}
	else   // while in user settings
	{
		if (S_EP_LOW)
		{
			if (!switch_down[S_EP_NUM])
			{
				switch_down[S_EP_NUM] = TRUE;
				switch_counter[S_EP_NUM] = 0;
			}
		}
	}
}



/*
 *Function:
 *RV - Set forward/reverse
 *DD - Decrease detergent dosage
 *
 */
ISR(RV_DD_SWITCH)
{
	if (S_RV_LOW)
	{
		if (!switch_down[S_RV_NUM])
		{
			switch_down[S_RV_NUM] = TRUE;
			switch_counter[S_RV_NUM] = 0;
		}
	}
	if ((test_mode || !(reverse || transport || top_error)) && S_DD_LOW)
	{
		if (!switch_down[S_DD_NUM])
		{
			switch_down[S_DD_NUM] = TRUE;
			switch_counter[S_DD_NUM] = 0;
		}
	}
}

/*
 *Function:
 *VAC - Turn on/off vacuum
 *
 */
ISR(VAC_SWITCH)
{
	if (S_VAC_LOW)
	{
		if (!switch_down[S_VAC_NUM] && !top_error)
		{
			switch_down[S_VAC_NUM] = TRUE;
			switch_counter[S_VAC_NUM] = 0;
		}
	}
}

/*
 *Function:
 *HRN - Turns on horn while pressed down
 *
 */
ISR(HRN_SWITCH)
{
	if (S_HRN_LOW && !(hrn_down || hrn_up))
	{
		switch_pressed[S_HRN_NUM] = TRUE;
		switch_counter[S_HRN_NUM] = 0;
		hrn_down = TRUE;
		hrn_down_counter = 0;
	}
	else if (hrn_down && hrn_down_counter >= _30_MS)
	{
		hrn_up = TRUE;
		hrn_down = FALSE;
	}
}

/*
 *Function:
 *ATO - Sets automatic/manual mode
 *PD - Decrease water pump flow
 *BR - Turn on brush
 */
ISR(ATO_PD_BR_SWITCH)
{
	if (!(reverse || transport || top_error) && S_ATO_LOW)
	{
		if (!switch_down[S_ATO_NUM])
		{
			switch_down[S_ATO_NUM] = TRUE;
			switch_counter[S_ATO_NUM] = 0;
		}
	}
	if (!(reverse || transport || top_error) && S_PD_LOW)
	{
		if (!switch_down[S_PD_NUM])
		{
			switch_down[S_PD_NUM] = TRUE;
			switch_counter[S_PD_NUM] = 0;
		}
	}
	if (!(reverse || transport || top_error) && S_BR_LOW)
	{
		if (!switch_down[S_BR_NUM])
		{
			switch_down[S_BR_NUM] = TRUE;
			switch_counter[S_BR_NUM] = 0;
		}
	}
}
