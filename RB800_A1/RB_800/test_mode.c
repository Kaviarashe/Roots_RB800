/*
 * test_mode.c
 *
 * Created: 12/14/2015 11:15:43 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "test_mode.h"

char message[20];

void test_menu_next(void)
{
	switch (current_menu)
	{
		//Any button press will move pre-menu to the test menu
	case PRE_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_MENU:
		current_menu = MOTOR_DIRECTION_VAL_MENU;
		if (reverse == ON)
		{
			lcd_write_new_P(PSTR("REVERSE"));
		}
		else
		{
			lcd_write_new_P(PSTR("FORWARD"));
		}
		break;

	case BRUSH_MENU:
		current_menu = BRUSH_VAL_MENU;
		if (brush >= 60)
		{
			sprintf(message, "BRUSH: %d%%", brush);
			lcd_write_new(message);
		}
		else
		{
			lcd_write_new_P(PSTR("BRUSH OFF"));
		}
		break;

	case VACUUM_MENU:
		current_menu = VACUUM_VAL_MENU;
		if (vacuum >= 60)
		{
			sprintf(message, "VACUUM: %d%%", vacuum);
			lcd_write_new(message);
		}
		else
		{
			lcd_write_new_P(PSTR("VACUUM OFF"));
		}
		break;

	case WATER_PUMP_MENU:
		current_menu = WATER_PUMP_VAL_MENU;
		if (water_pump_dc >= 5)
		{
			sprintf(message, "WATER PUMP: %d%%", water_pump_dc);
			lcd_write_new(message);
		}
		else
		{
			lcd_write_new_P(PSTR("WATER PUMP OFF"));
		}
		break;

	case DET_PUMP_MENU:
		current_menu = DET_PUMP_VAL_MENU;
		if (detergent_pump_dc >= 5)
		{
			detergent_pump = ON;
			sprintf(message, "DETERGENT PUMP: %d%%", detergent_pump_dc);
			lcd_write_new(message);
		}
		else
		{
			detergent_pump = OFF;
			lcd_write_new_P(PSTR("DETERGENT PUMP OFF"));
		}
		break;

	case EXTRA_PRESSURE_MENU:
		current_menu = EXTRA_PRESSURE_VAL_MENU;
		if (extra_pressure == ON)
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE ON"));
		}
		else
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE OFF"));
		}
		break;

	case HEADLIGHTS_MENU:
		current_menu = HEADLIGHTS_VAL_MENU;
		if (headlights == ON)
		{
			lcd_write_new_P(PSTR("HEADLIGHTS ON"));
		}
		else
		{
			lcd_write_new_P(PSTR("HEADLIGHTS OFF"));
		}
		break;

	case HORN_MENU:
		current_menu = HORN_VAL_MENU;
		if (horn == ON)
		{
			lcd_write_new_P(PSTR("HORN ON"));
		}
		else
		{
			lcd_write_new_P(PSTR("HORN OFF"));
		}
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_VAL_MENU:
		current_menu = MOTOR_DIRECTION_VAL_MENU;
		update_A2_settings();
		break;

	case BRUSH_VAL_MENU:
		current_menu = BRUSH_VAL_MENU;
		update_A2_settings();
		break;

	case VACUUM_VAL_MENU:
		current_menu = VACUUM_VAL_MENU;
		update_A2_settings();
		break;

	case WATER_PUMP_VAL_MENU:
		current_menu = WATER_PUMP_VAL_MENU;
		update_A2_settings();
		break;

	case DET_PUMP_VAL_MENU:
		current_menu = DET_PUMP_VAL_MENU;
		update_A2_settings();
		break;

	case EXTRA_PRESSURE_VAL_MENU:
		current_menu = EXTRA_PRESSURE_VAL_MENU;
		update_A2_settings();
		break;

	case HEADLIGHTS_VAL_MENU:
		current_menu = HEADLIGHTS_VAL_MENU;
		update_A2_settings();
		break;

	case HORN_VAL_MENU:
		current_menu = HORN_VAL_MENU;
		update_A2_settings();
		break;

	default: break;
	}
}

void test_menu_back(void)
{
	switch (current_menu)
	{
		//Any button press will move pre-menu to the test menu
	case PRE_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_VAL_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

	case BRUSH_VAL_MENU:
		current_menu = BRUSH_MENU;
		lcd_write_new_P(PSTR("BRUSH"));
		break;

	case VACUUM_VAL_MENU:
		current_menu = VACUUM_MENU;
		lcd_write_new_P(PSTR("VACUUM"));
		break;

	case WATER_PUMP_VAL_MENU:
		current_menu = WATER_PUMP_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		break;

	case DET_PUMP_VAL_MENU:
		current_menu = DET_PUMP_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		break;

	case EXTRA_PRESSURE_VAL_MENU:
		current_menu = EXTRA_PRESSURE_MENU;
		lcd_write_new_P(PSTR("EXTRA PRESSURE"));
		break;

	case HEADLIGHTS_VAL_MENU:
		current_menu = HEADLIGHTS_MENU;
		lcd_write_new_P(PSTR("HEADLIGHTS"));
		break;

	case HORN_VAL_MENU:
		current_menu = HORN_MENU;
		lcd_write_new_P(PSTR("HORN"));
		break;

	default: break;
	}

}

void test_menu_up(void)
{
	switch (current_menu)
	{
		//Any button press will move pre-menu to the test menu
	case PRE_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

	case BRUSH_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

	case VACUUM_MENU:
		current_menu = BRUSH_MENU;
		lcd_write_new_P(PSTR("BRUSH"));
		break;

	case WATER_PUMP_MENU:
		current_menu = VACUUM_MENU;
		lcd_write_new_P(PSTR("VACUUM"));
		break;

	case DET_PUMP_MENU:
		current_menu = WATER_PUMP_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		break;

	case EXTRA_PRESSURE_MENU:
		current_menu = DET_PUMP_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		break;

	case HEADLIGHTS_MENU:
		current_menu = EXTRA_PRESSURE_MENU;
		lcd_write_new_P(PSTR("EXTRA PRESSURE"));
		break;

	case HORN_MENU:
		current_menu = HEADLIGHTS_MENU;
		lcd_write_new_P(PSTR("HEADLIGHTS"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_VAL_MENU:
		current_menu = MOTOR_DIRECTION_VAL_MENU;
		if (reverse == ON)
		{
			lcd_write_new_P(PSTR("FORWARD"));
			reverse = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("REVERSE"));
			reverse = ON;
		}
		update_A2_settings();
		break;

	case BRUSH_VAL_MENU:
		current_menu = BRUSH_VAL_MENU;
		if (brush < 60)
		{
			brush = 60;
			sprintf(message, "BRUSH: %d%%", brush);
			lcd_write_new(message);
		}
		else if (brush <= 95)
		{
			brush += 5;
			sprintf(message, "BRUSH: %d%%", brush);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case VACUUM_VAL_MENU:
		current_menu = VACUUM_VAL_MENU;
		if (vacuum < 60)
		{
			vacuum = 60;
			sprintf(message, "VACUUM: %d%%", vacuum);
			lcd_write_new(message);
		}
		else if (vacuum <= 95)
		{
			vacuum += 5;
			sprintf(message, "VACUUM: %d%%", vacuum);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case WATER_PUMP_VAL_MENU:
		current_menu = WATER_PUMP_VAL_MENU;
		if (water_pump_dc <= 95)
		{
			water_pump_dc += 5;
			water_pump = ON;
			sprintf(message, "WATER PUMP: %d%%", water_pump_dc);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case DET_PUMP_VAL_MENU:
		current_menu = DET_PUMP_VAL_MENU;
		if (detergent_pump_dc <= 95)
		{
			detergent_pump_dc += 5;
			detergent_pump = ON;
			sprintf(message, "DETERGENT PUMP: %d%%", detergent_pump_dc);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case EXTRA_PRESSURE_VAL_MENU:
		current_menu = EXTRA_PRESSURE_VAL_MENU;
		if (extra_pressure == ON)
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE OFF"));
			extra_pressure = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE ON"));
			extra_pressure = ON;
		}
		update_A2_settings();
		break;

	case HEADLIGHTS_VAL_MENU:
		current_menu = HEADLIGHTS_VAL_MENU;
		if (headlights == ON)
		{
			lcd_write_new_P(PSTR("HEADLIGHTS OFF"));
			headlights = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("HEADLIGHTS ON"));
			headlights = ON;
		}
		update_A2_settings();
		break;

	case HORN_VAL_MENU:
		current_menu = HORN_VAL_MENU;
		if (horn == ON)
		{
			lcd_write_new_P(PSTR("HORN OFF"));
			horn = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("HORN ON"));
			horn = ON;
		}
		update_A2_settings();
		break;

	default: break;
	}
}

void test_menu_down(void)
{
	lcd_write_new_P(PSTR("DOWN"));
	switch (current_menu)
	{
		//Any button press will move pre-menu to the test menu
	case PRE_MENU:
		current_menu = MOTOR_DIRECTION_MENU;
		lcd_write_new_P(PSTR("MOTOR DIRECTION"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_MENU:
		current_menu = BRUSH_MENU;
		lcd_write_new_P(PSTR("BRUSH"));
		break;

	case BRUSH_MENU:
		current_menu = VACUUM_MENU;
		lcd_write_new_P(PSTR("VACUUM"));
		break;

	case VACUUM_MENU:
		current_menu = WATER_PUMP_MENU;
		lcd_write_new_P(PSTR("WATER PUMP"));
		break;

	case WATER_PUMP_MENU:
		current_menu = DET_PUMP_MENU;
		lcd_write_new_P(PSTR("DETERGENT PUMP"));
		break;

	case DET_PUMP_MENU:
		current_menu = EXTRA_PRESSURE_MENU;
		lcd_write_new_P(PSTR("EXTRA PRESSURE"));
		break;

	case EXTRA_PRESSURE_MENU:
		current_menu = HEADLIGHTS_MENU;
		lcd_write_new_P(PSTR("HEADLIGHTS"));
		break;

	case HEADLIGHTS_MENU:
		current_menu = HORN_MENU;
		lcd_write_new_P(PSTR("HORN"));
		break;

	case HORN_MENU:
		current_menu = HORN_MENU;
		lcd_write_new_P(PSTR("HORN"));
		break;

		/******************************************************************************************************************************/

	case MOTOR_DIRECTION_VAL_MENU:
		current_menu = MOTOR_DIRECTION_VAL_MENU;
		if (reverse == ON)
		{
			lcd_write_new_P(PSTR("FORWARD"));
			reverse = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("REVERSE"));
			reverse = ON;
		}
		update_A2_settings();
		break;

	case BRUSH_VAL_MENU:
		current_menu = BRUSH_VAL_MENU;
		if (brush <= 60)
		{
			lcd_write_new_P(PSTR("BRUSH OFF"));
			brush = OFF;
		}
		else
		{
			brush -= 5;
			sprintf(message, "BRUSH: %d%%", brush);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case VACUUM_VAL_MENU:
		current_menu = VACUUM_VAL_MENU;
		if (vacuum <= 60)
		{
			lcd_write_new_P(PSTR("VACUUM OFF"));
			vacuum = OFF;
		}
		else
		{
			vacuum -= 5;
			sprintf(message, "VACUUM: %d%%", vacuum);
			lcd_write_new(message);
		}
		update_A2_settings();
		break;

	case WATER_PUMP_VAL_MENU:
		current_menu = WATER_PUMP_VAL_MENU;
		if (water_pump_dc > 5)
		{
			water_pump_dc -= 5;
			water_pump = ON;
			sprintf(message, "WATER PUMP: %d%%", water_pump_dc);
			lcd_write_new(message);
		}
		else
		{
			water_pump_dc = 0;
			water_pump = OFF;
			lcd_write_new_P(PSTR("WATER PUMP OFF"));
		}
		update_A2_settings();
		break;

	case DET_PUMP_VAL_MENU:
		current_menu = DET_PUMP_VAL_MENU;
		if (detergent_pump_dc > 5)
		{
			detergent_pump_dc -= 5;
			detergent_pump = ON;
			sprintf(message, "DETERGENT PUMP: %d%%", detergent_pump_dc);
			lcd_write_new(message);
		}
		else
		{
			detergent_pump_dc = 0;
			detergent_pump = OFF;
			lcd_write_new_P(PSTR("DETERGENT PUMP OFF"));
		}
		update_A2_settings();
		break;

	case EXTRA_PRESSURE_VAL_MENU:
		current_menu = EXTRA_PRESSURE_VAL_MENU;
		if (extra_pressure == ON)
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE OFF"));
			extra_pressure = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("EXTRA PRESSURE ON"));
			extra_pressure = ON;
		}
		update_A2_settings();
		break;

	case HEADLIGHTS_VAL_MENU:
		current_menu = HEADLIGHTS_VAL_MENU;
		if (headlights == ON)
		{
			lcd_write_new_P(PSTR("HEADLIGHTS OFF"));
			headlights = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("HEADLIGHTS ON"));
			headlights = ON;
		}
		update_A2_settings();
		break;

	case HORN_VAL_MENU:
		current_menu = HORN_VAL_MENU;
		if (horn == ON)
		{
			lcd_write_new_P(PSTR("HORN OFF"));
			horn = OFF;
		}
		else
		{
			lcd_write_new_P(PSTR("HORN ON"));
			horn = ON;
		}
		update_A2_settings();
		break;

	default: break;
	}
}

void start_test_menu(void)
{
	lcd_write_new_P(PSTR("DU - up  |TR - next"));
	lcd_set_cursor(1, 0);
	lcd_write_P(PSTR("DD - down|EX - back"));
	current_menu = PRE_MENU;
	test_mode = TRUE;
	while (1)
	{
		//Check if any switches have been pressed
		for (int num = 0; num < NUM_SWITCHES; num++)
		{
			if (switch_pressed[num])
			{
				//Clear switch_pressed
				switch_pressed[num] = FALSE;
				//Perform action depending on switch
				switch (num)
				{
				case S_DU_NUM: test_menu_up();		break;
				case S_TR_NUM: test_menu_next();	break;
				case S_ECO_NUM: test_menu_back();	break;
				case S_DD_NUM: test_menu_down();	break;

				default:  break;
				}
			}
		}
	}
}