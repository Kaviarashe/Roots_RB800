/*
 * settings.c
 *
 * Created: 7/15/2015 10:10:52 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "keypad_settings.h"

void initialize_keypad_settings(void)
{
	//Setup all speed settings
	speed_settings_dosage[LOW] = LOW_DC_DOSAGE;
	speed_settings_dosage[LOW_MED] = LOW_MED_DC_DOSAGE;
	speed_settings_dosage[MED] = MED_DC_DOSAGE;
	speed_settings_dosage[MED_HIGH] = MED_HIGH_DC_DOSAGE;
	speed_settings_dosage[HIGH] = HIGH_DC_DOSAGE;

	//Setup all speed settings
	speed_settings_water[LOW] = LOW_DC;
	speed_settings_water[LOW_MED] = LOW_MED_DC;
	speed_settings_water[MED] = MED_DC;
	speed_settings_water[MED_HIGH] = MED_HIGH_DC;
	speed_settings_water[HIGH] = HIGH_DC;

	//Default level settings
	water_pump_sl = LOW; L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF;
	detergent_pump_sl = LOW; L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF;
	fr_speed = FAST; L_SLO_OFF;

	//Set everything off initially
	water_pump = OFF;
	solenoid_valve = OFF;		L_SLD_OFF;
	detergent_pump = OFF;
	vacuum = OFF;			L_VAC_OFF;
	horn = OFF;
	headlights = OFF;		L_HL_OFF;
	beacon_lamp = OFF;
	extra_pressure = OFF;	L_EP_OFF;
	reverse = OFF;			L_RV_OFF;
	brush = OFF;			L_BR_OFF;
	brush_state = BRUSH_OFF;
	vacuum_state = VACUUM_OFF;
	user_settings_menu_enable = FALSE;
	eeprom_busy_wait();
	//Check if auto mode has ever been set
	//!Should only run once in board's lifetime!
	if (eeprom_read_byte((const uint8_t *)EEP8_AUTO_MODE_SET_ADDR) != AUTO_MODE_SET)
	{
		//Sets values to factory settings
		const uint8_t settings[6] =
			// Water pump level, detergent pump level, water pump, detergent pump, vacuum, brush
		{ MED, MED, OFF, OFF, VAC_ON, BRUSH_ON };

		//Writes auto mode settings to eeprom
		eeprom_update_block((const void*)settings, (void*)EEPxx_AUTO_MODE_ADDR, NUM_AUTO_SETTINGS);
		//Updates byte so that auto mode does not need to set again
		eeprom_update_byte((uint8_t *)EEP8_AUTO_MODE_SET_ADDR, (uint8_t)AUTO_MODE_SET);
	}
	eeprom_busy_wait();
	//Read current auto mode settings from eeprom and write to SRAM array
	eeprom_read_block((void *)auto_settings, (const void*)EEPxx_AUTO_MODE_ADDR, NUM_AUTO_SETTINGS);

	eeprom_busy_wait();
	motors_status = eeprom_read_byte((const uint8_t *)EEP8_MOTORS_STATUS_ADDR);
}

void change_auto_state(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

#if LCD_INFO
	lcd_set_cursor(0, 0);
	lcd_write_P(PSTR("AUTO MODE   "));
#endif
	L_ATO_ON;

	//Set pump state
	water_pump = auto_settings[2];
	detergent_pump = auto_settings[3];
	solenoid_valve = ON;
	//Automatic level settings
	//If pump is on set LEDs according to level
	water_pump_sl = auto_settings[0];
	L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF;
	if (water_pump == ON)
	{
		switch (water_pump_sl)
		{
		case HIGH:		L_P5_ON;
		case MED_HIGH:	L_P4_ON;
		case MED:		L_P3_ON;
		case LOW_MED:	L_P2_ON;
		case LOW:		L_P1_ON;

		default: break;
		}
	}
	water_pump_dc = speed_settings_water[water_pump_sl];

	detergent_pump_sl = auto_settings[1];
	L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF;
	if (detergent_pump == ON)
	{
		switch (detergent_pump_sl)
		{
		case HIGH:		L_D5_ON;
		case MED_HIGH:	L_D4_ON;
		case MED:		L_D3_ON;
		case LOW_MED:	L_D2_ON;
		case LOW:		L_D1_ON;
		default: break;
		}
	}
	detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];

	//Set vacuum and brush states to auto state
	//Check current state of motors before restoring as well
	vacuum = auto_settings[4];
	if (vacuum == VAC_ON || vacuum == VAC_ON_ECO)
	{
		L_VAC_ON;
		blink_vac_led = FALSE;
		if (eco_mode)
			vacuum = VAC_ON_ECO;
		else
			vacuum = VAC_ON;
	}
	else if (vacuum_state == VACUUM_OFF) { L_VAC_OFF; blink_vac_led = FALSE; }

	brush = auto_settings[5];
	if (brush == BRUSH_ON || brush == BRUSH_ON_ECO)
	{
		if (eco_mode)
			brush = BRUSH_ON_ECO;
		else
			brush = BRUSH_ON;
	}
	if (brush_state == BRUSH_RUNNING)
	{
		L_BR_ON;
		blink_brush_led = FALSE;
	}
	if (brush_state == BRUSH_OFF)
	{
		if (brush)
		{
			blink_brush_led = TRUE;
		}
		else
		{
			L_BR_OFF;
			blink_brush_led = FALSE;
		}
	}

	log_event(AUTO_MODE_EVENT);
}

uint8_t is_reverse_transport(void)
{
	//If in reverse mode, inform user
	if (reverse)
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("REVERSE MODE"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		return TRUE;
	}
	//If in transport mode, inform user
	if (transport)
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("TRANSPORT MODE"));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		return TRUE;
	}

	return FALSE;
}

void change_transport_mode(void)
{
	//If in reverse mode simply switch to transport state
	if (reverse)
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("TRANSPORT   "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		reverse = OFF;
		transport = ON;
		L_RV_OFF;
		L_TR_ON;
		/*log_event(TRANSPORT_ON_EVENT);*/
		return;
	}

	//Switch idle mode on/off
	if (transport)
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("FORWARD      "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		transport_mode_off();
		transport = FALSE;
		/*log_event(TRANSPORT_OFF_EVENT);*/
	}
	else
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("TRANSPORT   "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		transport_mode_on();
		transport = TRUE;
		if (vacuum_state == VACUUM_RUNNING)
		{
			blink_vac_led = TRUE;
		}
		/*log_event(TRANSPORT_ON_EVENT);*/
	}
}

void transport_mode_on(void)
{
	//Turn off LEDs of functions that cannot run during idle mode
	//Settings are untouched so that they will return to original state once reverse is turned off
	L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF; temp_wp = water_pump; water_pump = OFF;
	L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF; temp_dp = detergent_pump; detergent_pump = OFF;
	L_BR_OFF; temp_b = brush; brush = OFF; blink_brush_led = FALSE;
	extra_pressure = OFF; L_EP_OFF;
	L_TR_ON;
}

void transport_mode_off(void)
{
	//Restore water pump state
	water_pump = temp_wp;
	if (water_pump)
	{
		switch (water_pump_sl)
		{
		case HIGH:		L_P5_ON;
		case MED_HIGH:	L_P4_ON;
		case MED:		L_P3_ON;
		case LOW_MED:	L_P2_ON;
		case LOW:		L_P1_ON;

		default: break;
		}
	}
	water_pump_dc = speed_settings_water[water_pump_sl];

	//Restore detergent pump state
	detergent_pump = temp_dp;
	if (detergent_pump == ON)
	{
		switch (detergent_pump_sl)
		{
		case HIGH:		L_D5_ON;
		case MED_HIGH:	L_D4_ON;
		case MED:		L_D3_ON;
		case LOW_MED:	L_D2_ON;
		case LOW:		L_D1_ON;
		default: break;
		}
	}
	detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];

	//Restore brush state
	brush = temp_b;
	if (brush == BRUSH_ON || brush == BRUSH_ON_ECO)
	{
		if (eco_mode)
			brush = BRUSH_ON_ECO;
		else
			brush = BRUSH_ON;
	}
	if (brush_state == BRUSH_RUNNING)
	{
		L_BR_ON;
		blink_brush_led = FALSE;
	}
	if (brush_state == BRUSH_OFF)
	{
		if (brush)
		{
			blink_brush_led = TRUE;
		}
		else
		{
			L_BR_OFF;
			blink_brush_led = FALSE;
		}
	}
	L_TR_OFF;
}

/*
 *Function: Switches between forward and reverse state
 *
 */
void change_forward_reverse_state(void)
{
	//If in idle mode simply switch to reverse state
	if (transport)
	{
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("REVERSE     "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		transport = OFF;
		reverse = ON;
		L_TR_OFF;
		L_RV_ON;
		/*log_event(REVERSE_ON_EVENT);*/
		return;
	}

	//Switch between forward/reverse
	if (reverse)
	{
		reverse = OFF;
		L_RV_OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("FORWARD     "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		reverse_mode_off();
		/*log_event(REVERSE_OFF_EVENT);*/
	}
	else
	{
		reverse = ON;
		L_RV_ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("REVERSE     "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		reverse_mode_on();
		/*log_event(REVERSE_ON_EVENT);*/
	}
}


void reverse_mode_on(void)
{
	//Turn off LEDs of functions that cannot run during reverse mode
	//Settings are untouched so that they will return to original state once reverse is turned off
	L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF; temp_wp = water_pump; water_pump = OFF;
	L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF; temp_dp = detergent_pump; detergent_pump = OFF;
	L_BR_OFF; temp_b = brush; brush = OFF; blink_brush_led = FALSE;
	extra_pressure = OFF; L_EP_OFF;
}

void reverse_mode_off(void)
{
	//Restore water pump state
	water_pump = temp_wp;
	if (water_pump)
	{
		switch (water_pump_sl)
		{
		case HIGH:		L_P5_ON;
		case MED_HIGH:	L_P4_ON;
		case MED:		L_P3_ON;
		case LOW_MED:	L_P2_ON;
		case LOW:		L_P1_ON;

		default: break;
		}
	}
	water_pump_dc = speed_settings_water[water_pump_sl];

	//Restore detergent pump state
	detergent_pump = temp_dp;
	if (detergent_pump == ON)
	{
		switch (detergent_pump_sl)
		{
		case HIGH:		L_D5_ON;
		case MED_HIGH:	L_D4_ON;
		case MED:		L_D3_ON;
		case LOW_MED:	L_D2_ON;
		case LOW:		L_D1_ON;
		default: break;
		}
	}
	detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];

	//Restore brush state
	brush = temp_b;
	if (brush == BRUSH_ON || brush == BRUSH_ON_ECO)
	{
		if (eco_mode)
			brush = BRUSH_ON_ECO;
		else
			brush = BRUSH_ON;
	}
	if (brush_state == BRUSH_RUNNING)
	{
		L_BR_ON;
		blink_brush_led = FALSE;
	}
	if (brush_state == BRUSH_OFF)
	{
		if (brush)
		{
			blink_brush_led = TRUE;
		}
		else
		{
			L_BR_OFF;
			blink_brush_led = FALSE;
		}
	}
}

void change_auto_settings(void)
{
	cli();

	//Gather the relevant settings for auto mode into an array
	uint8_t settings[6] =
	{ water_pump_sl, detergent_pump_sl, water_pump, detergent_pump, vacuum, brush };

	//Write the settings to the eeprom
	eeprom_busy_wait();
	solenoid_valve = ON;
	eeprom_update_block((const void*)settings, (void*)EEPxx_AUTO_MODE_ADDR, NUM_AUTO_SETTINGS);

	//Read current auto mode settings from eeprom and write to SRAM array
	//Also makes sure that the settings were set correctly
	eeprom_read_block((void *)auto_settings, (const void*)EEPxx_AUTO_MODE_ADDR, NUM_AUTO_SETTINGS);

	sei();
	log_event(AUTO_MODE_CHANGED);
}

/*
 *Function: Increase water pump level
 *
 */
void increase_water_pump_level(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	L_ATO_OFF;
#if LCD_INFO
	//lcd_write_P(PSTR("WATER PUMP "));
#endif

//If pump is off turn on
	if (water_pump == OFF)
	{
		L_P1_ON;
		water_pump = ON;
		water_pump_sl = LOW;
		water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW         "));
#endif
		/*uint8_t data[] = {water_pump_sl};
		log_event_data(WATER_ON_EVENT, data, 1);*/
		return;
	}
	//Else check the current state and increase unless at highest state
	switch (water_pump_sl)
	{
	case LOW: L_P2_ON; water_pump_sl++; water_pump_dc = speed_settings_water[water_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW-MED     "));
#endif
		break;

	case LOW_MED: L_P3_ON; water_pump_sl++; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED         "));
#endif
		break;

	case MED: L_P4_ON; water_pump_sl++; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED-HIGH     "));
#endif
		break;

	case MED_HIGH: L_P5_ON; water_pump_sl++; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("HIGH         "));
#endif
		break;

	case HIGH:
		break;

	default: break;
	}

	/*uint8_t data[] = {water_pump_sl};
	log_event_data(WATER_ON_EVENT, data, 1);*/
}

/*
 *Function: Decrease water pump level
 *
 */
void decrease_water_pump_level(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	L_ATO_OFF;
	//Decrease pump level until lowest level and then turn pump off
	switch (water_pump_sl)
	{
	case LOW: L_P1_OFF; water_pump = OFF;solenoid_valve = OFF;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("OFF         "));
#endif
		break;

	case LOW_MED: L_P2_OFF; water_pump_sl--; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW         "));
#endif
		break;

	case MED: L_P3_OFF; water_pump_sl--; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW-MED     "));
#endif
		break;

	case MED_HIGH: L_P4_OFF; water_pump_sl--; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED         "));
#endif 
		break;

	case HIGH: L_P5_OFF; water_pump_sl--; water_pump_dc = speed_settings_water[water_pump_sl];solenoid_valve = ON;
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("WATER PUMP  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED-HIGH     "));
#endif
		break;

	default: break;
	}

	/*if(water_pump == OFF)
	{
		log_event(WATER_OFF_EVENT);
	}
	else
	{
		uint8_t data[] = {water_pump_sl};
		log_event_data(WATER_ON_EVENT, data, 1);
	}*/
}

/*
 *Function: Increase detergent pump level
 *
 */
void increase_detergent_pump_level(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	L_ATO_OFF;

	//If pump is off turn on
	if (detergent_pump == OFF)
	{
		L_D1_ON;
		detergent_pump = ON;
		detergent_pump_sl = LOW;
		detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW         "));
#endif
		/*uint8_t data[] = {detergent_pump_sl};
		log_event_data(DETERGENT_ON_EVENT, data, 1);*/
		return;
	}
	//Else check the current state and increase unless at highest state
	switch (detergent_pump_sl)
	{
	case LOW: L_D2_ON; detergent_pump_sl++; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW-MED     "));
#endif
		break;

	case LOW_MED: L_D3_ON; detergent_pump_sl++; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED         "));
#endif
		break;

	case MED: L_D4_ON; detergent_pump_sl++; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED-HIGH     "));
#endif 
		break;

	case MED_HIGH: L_D5_ON; detergent_pump_sl++; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("HIGH         "));
#endif 
		break;

	case HIGH:
		break;

	default: break;
	}

	/*uint8_t data[] = {detergent_pump_sl};
	log_event_data(DETERGENT_ON_EVENT, data, 1);*/
}

/*
 *Function: Decrease detergent pump level
 *
 */
void decrease_detergent_pump_level(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	L_ATO_OFF;
	//Decrease pump level until lowest level and then turn pump off
	switch (detergent_pump_sl)
	{
	case LOW: L_D1_OFF; detergent_pump = OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("OFF         "));
#endif
		break;

	case LOW_MED: L_D2_OFF; detergent_pump_sl--; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW         "));
#endif 
		break;

	case MED: L_D3_OFF; detergent_pump_sl--; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("LOW-MED     "));
#endif
		break;

	case MED_HIGH: L_D4_OFF; detergent_pump_sl--; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED         "));
#endif 
		break;

	case HIGH: L_D5_OFF; detergent_pump_sl--; detergent_pump_dc = speed_settings_dosage[detergent_pump_sl];
#if LCD_INFO 
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("DET PUMP    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("MED-HIGH     "));
#endif
		break;

	default: break;
	}

	/*if(detergent_pump == OFF)
	{
		log_event(DETERGENT_OFF_EVENT);
	}
	else
	{
		uint8_t data[] = {detergent_pump_sl};
		log_event_data(DETERGENT_ON_EVENT, data, 1);
	}*/
}

/*
 *Function: Turns vacuum on and off
 *
 */
void change_vacuum_state(void)
{
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;
	if (tank_full)
	{
		L_ATO_OFF;
		vacuum = OFF;
		L_VAC_OFF;
		return;
	}

	runtimer_on = TRUE;

	L_ATO_OFF;
	if (vacuum)
	{
		vacuum = OFF;
		L_VAC_OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("VACUUM OFF  "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		/*log_event(VACUUM_OFF_EVENT);*/
	}
	else
	{
		if (eco_mode)
			vacuum = VAC_ON_ECO;
		else
			vacuum = VAC_ON;
		L_VAC_ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("VACUUM ON   "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif
		/*log_event(VACUUM_ON_EVENT);*/
	}
}

/*
 *Function: Turns horn on and off
 *
 */
void change_horn_state(void)
{
	if (horn)
	{
		horn = OFF;
		//L_EXT_OFF;	/*TEST L_EXT TO SHOW HORN FUNCTION*/
#if LCD_INFO
		if (!top_error)
		{
			lcd_set_cursor(0, 0);
			lcd_write_P(PSTR("HORN OFF    "));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("            "));
		}
		else
		{
			lcd_write_new_P(PSTR("HORN OFF    "));
		}
#endif
	}
	else
	{
		horn = ON;
		//L_EXT_ON;	/*TEST L_EXT TO SHOW HORN FUNCTION*/
#if LCD_INFO
		if (!top_error)
		{
			lcd_set_cursor(0, 0);
			lcd_write_P(PSTR("HORN ON     "));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("            "));
		}
		else
		{
			lcd_write_new_P(PSTR("HORN ON     "));
		}
#endif
	}
}

/*
 *Function: Turns headlights on and off
 *
 */
void change_headlights_state(void)
{
	if (headlights)
	{
		headlights = OFF;
		L_HL_OFF;
#if LCD_INFO
		if (!top_error)
		{
			lcd_set_cursor(0, 0);
			lcd_write_P(PSTR("HLIGHT  OFF "));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("            "));
		}
		else
		{
			lcd_write_new_P(PSTR("HLIGHT  OFF "));
		}
#endif
	}
	else
	{
		headlights = ON;
		L_HL_ON;
#if LCD_INFO
		if (!top_error)
		{
			lcd_set_cursor(0, 0);
			lcd_write_P(PSTR("HLIGHT  ON  "));
			lcd_set_cursor(1, 0);
			lcd_write_P(PSTR("            "));
		}
		else
		{
			lcd_write_new_P(PSTR("HLIGHT  ON  "));
		}
#endif
	}
}


/*
 *Function: Turns solenoid valve on and off depends on the pump level
 *
 */
void change_solenoid_valve_state(void)
{
	if (solenoid_valve && water_pump == OFF)
	{
		solenoid_valve = OFF;
		solenoid_led = OFF;
		L_SLD_OFF;
		//blink_sld_led=FALSE;	

	}
	else if (!solenoid_valve && water_pump == OFF)
	{
		solenoid_valve = ON;
		solenoid_led = ON;
		L_SLD_ON;
		//blink_sld_led=TRUE;		
	}
	if (water_pump != OFF && solenoid_led == OFF)
	{
		solenoid_led = ON;
		L_SLD_ON;
	}
	else if (water_pump != OFF && solenoid_led == ON)
	{
		solenoid_led = OFF;
		L_SLD_OFF;
	}
}

/*
 *Function: Turns beacon lamp on and off
 *
 */
void change_beacon_lamp_state(void)
{
	if (beacon_lamp)
	{
		beacon_lamp = OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("BEACON LAMP OFF"));
#endif
	}
	else
	{
		beacon_lamp = ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("BEACON LAMP ON"));
#endif
	}
}

/*
 *Function: Turns extra pressure on and off
 *
 */
void change_extra_pressure_state(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	if (extra_pressure)
	{
		extra_pressure = OFF;
		L_EP_OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("EXTRA       "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("PRESSURE OFF"));
#endif
	}
	else
	{
		extra_pressure = ON;
		L_EP_ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("EXTRA       "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("PRESSURE ON "));
#endif
	}
}

/*
 *Function: Turn brush on and off
 *
 */
void change_brush_state(void)
{
	//Do not perform button action if in reverse or idle mode
	if (is_reverse_transport() || motors_status == MOTORS_OFF)
		return;

	L_ATO_OFF;
	if (brush)
	{
		brush = OFF;
		L_BR_OFF;
		blink_brush_led = FALSE;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("BRUSH OFF   "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
		/*log_event(BRUSH_OFF_EVENT);*/
#endif
	}
	else
	{
		if (eco_mode)
			brush = BRUSH_ON_ECO;
		else
			brush = BRUSH_ON;
		L_BR_ON;
		blink_brush_led = TRUE;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("BRUSH ON    "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
		/*log_event(BRUSH_ON_EVENT);*/
#endif
	}
}

/*
 *Function: Change forward/reverse speed from fast to slow or vice versa
 *
 */
void change_speed(void)
{
	//Turn on auto mode light
	L_ATO_OFF;
	if (fr_speed == FAST)
	{
		fr_speed = SLOW;
		L_SLO_ON;
		lcd_menu();
	}
	else
	{
		fr_speed = FAST;
		L_SLO_OFF;
		lcd_menu();
	}
}

void change_eco_mode(void)
{
	if (motors_status == MOTORS_OFF)
		return;
	eco_mode = !eco_mode;
	if (eco_mode)
	{
		L_ECO_ON;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("ECO ON      "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif

		if (vacuum == VAC_ON)
			vacuum = VAC_ON_ECO;
		if (brush == BRUSH_ON)
			brush = BRUSH_ON_ECO;

		uint8_t message[1] = { ON };
		transmit_CAN(ECO_CAN, 0, 1, message);
		_delay_ms(10);
	}
	else
	{
		L_ECO_OFF;
#if LCD_INFO
		lcd_set_cursor(0, 0);
		lcd_write_P(PSTR("ECO OFF     "));
		lcd_set_cursor(1, 0);
		lcd_write_P(PSTR("            "));
#endif

		if (vacuum == VAC_ON_ECO)
			vacuum = VAC_ON;
		if (brush == BRUSH_ON_ECO)
			brush = BRUSH_ON;

		uint8_t message[1] = { OFF };
		transmit_CAN(ECO_CAN, 0, 1, message);
		_delay_ms(10);
	}
}

//Turn off motors if zero percent battery is reached
//Save this state to EEPROM so RB800 must be fully charged before continuing operation
void turn_off_motors(void)
{
	L_P1_OFF; L_P2_OFF; L_P3_OFF; L_P4_OFF; L_P5_OFF; water_pump = OFF;
	L_D1_OFF; L_D2_OFF; L_D3_OFF; L_D4_OFF; L_D5_OFF; detergent_pump = OFF;
	L_BR_OFF; brush = OFF; blink_brush_led = FALSE;
	extra_pressure = OFF; L_EP_OFF;
	L_VAC_OFF; vacuum = OFF; blink_vac_led = OFF;
	L_ATO_OFF;
	L_SLD_OFF;solenoid_valve = OFF;
	update_A2_settings();
}
