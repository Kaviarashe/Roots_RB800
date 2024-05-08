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

#define TANK_FULL_LEFT_LCD		0x0C
#define TANK_FULL_RIGHT_LCD		0x0D
#define TANK_EMPTY_LEFT_LCD		0x0E
#define TANK_EMPTY_RIGHT_LCD	0x0F

#define BAT_MID_LCD				0x09
#define BAT_RIGHT_LCD			0x0A
#define BAT_LEFT_LCD			0x0B

#define LCD_CAN_RST	1//PF1

void initialize_Screen(void);
void lcd_write_P(const char *);
void lcd_write_new_P(const char *);
void lcd_write(char *);
void lcd_write_new(char *);
void lcd_set_cursor(char line, char location);
void lcd_clear_screen(void);
void lcd_backspace(void);
void lcd_create_CGRAM(void);
void lcd_menu(void);
void lcd_menu_update(void);
void lcd_custom(uint8_t special);
void lcd_write_hex(uint8_t val);
void lcd_cursor_on(void);
void lcd_cursor_off(void);
void lcd_led_blink(void);

uint8_t colon_toggle;
char lcd_temp[21];

#endif /* LCD_SCREEN_H_ */