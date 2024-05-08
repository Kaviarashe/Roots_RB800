/*
 * lcd_screen.h
 *
 * Created: 5/8/2014 4:24:51 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef LCD_SCREEN_H_
#define LCD_SCREEN_H_

#include "RB_800.h"
#include <stdio.h>
#include "twi_master_driver.h"
#include <avr/eeprom.h>

#define LCD_SLAVE_ADDRESS 0x3C
#define MAX_LCD_CHAR 21

#define BATT_LEFT_LCD			0x09
#define BATT_RIGHT_LCD			0x0A
#define BATT_MID_LCD			0x0B
#define TANK_FULL_LEFT_LCD		0x0C
#define TANK_FULL_RIGHT_LCD		0x0D
#define TANK_EMPTY_LEFT_LCD		0x0E
#define TANK_EMPTY_RIGHT_LCD	0x0F

void initialize_Screen(void);
void lcd_write(char *);
void lcd_write_new(char *);
void lcd_write_P(const char *);
void lcd_write_new_P(const char *);
void lcd_set_cursor(char line, char location);
void lcd_clear_screen(void);
void lcd_backspace(void);
void lcd_create_CGRAM(void);
void lcd_menu(void);
void lcd_custom(uint8_t special);
void lcd_write_hex(uint8_t val);

#endif /* LCD_SCREEN_H_ */