/*
 * lcd_screen.c
 *
 * Created: 5/8/2014 3:09:24 PM
 *  Author: Anurag Vaddi Reddy
 */

#include "lcd_screen.h"

 //Initialize the LCD screen using the example setup in the LCD data sheet
void initialize_Screen(void)
{
	char message[10];

	message[0] = (0x00);													//Control bit with continuous data stream
	message[1] = (0x38);													//Function Set (8-bit data, 2 Line 5x8 character mode, Normal Instruction Table Chosen)
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 2);

	message[0] = (0x00);
	message[1] = (0x39);													//Function Set (8-bit data, 2 Line 5x8 character mode, Extension Instruction Table Chosen)
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 2);

	message[0] = (0x00);
	message[1] = (0x14);													//Set frame frequency to 192 Hz and Voltage Bias to 1/5
	message[2] = (0x70);													//Set contrast bits C3:0 to 8 (C5:0 - 0x28 *C5:4 is part of next data byte)
	message[3] = (0x5E);													//Turn on Icon Display and Booster Circuit and set C5:4 to 2 for contrast setting
	message[4] = (0x6D);													//Turn on internal follower circuit and adjust V0 generator amplified ratio (Rab2:0 - 2)
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 5);

	message[0] = (0x00);
	message[1] = (0x0F);													//Turn on display and turn on cursor and cursor blink
	message[2] = (0x01);													//Clear the display	- Entry Mode Set is required afterwards
	message[3] = (0x06);													//Entry Mode Set - Increment DDRAM Address (cursor) and do not shift display												
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 4);
}

#ifndef __SIM__
//Write a message to the lcd screen in the current position
void lcd_write(char *message)
{
	//Get the number of characters in the string
	uint8_t size = strlen(message);
	char complete_message[size + 3];
	complete_message[0] = (0x40);			//Control bit with R/S set high and continuous bytes

	//Write all bytes in message to the screen
	for (unsigned char i = 0; i < size; i++)
	{
		complete_message[i + 1] = message[i];
	}

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)complete_message, size + 1);
}

//Clear the screen and write a message from the top left of the screen
void lcd_write_new(char *message)
{
	//Clear the screen before writing
	lcd_clear_screen();
	//Write message to screen

	//Get the number of characters in the string
	uint8_t size = strlen(message);
	char complete_message[size + 3];
	complete_message[0] = (0x40);			//Control bit with R/S set high and continuous bytes

	//Write all bytes in message to the screen
	for (unsigned char i = 0; i < size; i++)
	{
		complete_message[i + 1] = message[i];
	}

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)complete_message, size + 1);
}

//Write a message to the lcd screen in the current position
void lcd_write_P(const char *message)
{
	//Get the number of characters in the string
	//uint8_t size = strlen(message);
	char complete_message[21 + 3];

	complete_message[0] = (0x40);			//Control bit with R/S set high and continuous bytes

	//Write all bytes in message to the screen
	uint8_t size = 0;
	for (; size < 21; size++)
	{
		complete_message[size + 1] = pgm_read_byte(message + size);
		if (complete_message[size + 1] == 0)
			break;
	}
	//complete_message[size + 1] = '\0';
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)complete_message, size + 2);
}

//Clear the screen and write a message from the top left of the screen
void lcd_write_new_P(const char *message)
{
	//Clear the screen before writing
	lcd_clear_screen();
	//Write message to screen

	//Get the number of characters in the string
	//uint8_t size = strlen(message);
	char complete_message[21 + 3];

	complete_message[0] = (0x40);			//Control bit with R/S set high and continuous bytes

	//Write all bytes in message to the screen
	uint8_t size = 0;
	for (; size < 21; size++)
	{
		complete_message[size + 1] = pgm_read_byte(message + size);
		if (complete_message[size + 1] == 0)
			break;
	}
	//complete_message[size + 1] = '\0';
	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)complete_message, size + 1);
}

//Line 0 is first line
//Location 0 is first character
//Set the cursor to a specified line & location
void lcd_set_cursor(char line, char location)
{
	char message[3];

	message[0] = (0x80);
	//Calculate memory address of line & location
	message[1] = 8 * 16 + line * 4 * 16 + location;

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 2);
}
//Clear the screen and sets the cursor to the top left
void lcd_clear_screen(void)
{
	char message[4];

	message[0] = (0x00);
	message[1] = (0x01);													//Clear the display	- Entry Mode Set is required afterwards
	message[2] = (0x06);													//Entry Mode Set - Increment DDRAM Address (cursor) and do not shift display

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 3);
}

//Performs a backspace operation on the lcd screen
void lcd_backspace(void)
{
	char message[3];

	message[0] = (0x00);
	message[1] = (0x04); //Set cursor to write to the left

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 2);

	lcd_write_P(PSTR(" "));	//Write a blank space

	message[1] = (0x06); //Set cursor to write to the right

	TWI_MasterWrite(&twiMaster, LCD_SLAVE_ADDRESS, (uint8_t *)message, 2);
}
#endif // !__SIM__

void lcd_write_hex(uint8_t val)
{
	char temp[5];
	sprintf(temp, "%02X ", val);
	lcd_write(temp);
}

//Print specially created characters
void lcd_custom(uint8_t custom_char_addr)
{
	char m[2];

	m[0] = custom_char_addr;
	m[1] = '\0';

	lcd_write(m);
}