/*
 * SPI_CAN.c
 *
 * Created: 7/1/2015 10:15:18 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "CAN.h"
int errorCount = 0;
#ifdef __SIM__
#include <emu_CAN.h>
#endif // __SIM__

//Initializes the CAN controller
void initialize_CAN(void)
{
	DBG("Init CAN");
	//Reset CAN controller
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(RESET_CAN);
	CAN_CHIP_SELECT_HIGH;

	uint8_t config_bytes[4] = { 0 };
	//TXRTSCTRL - Sets pin states of RTS pins
	//RTS pins are not used in this application
	config_bytes[0] = 0b00000000;
	write_bytes_CAN(TXRTSCTRL, config_bytes, 1);	//TXRTSCTRL

	//RXB0CTRL - Control for receive buffer 0
	//This buffer only receives only standard identifier messages
	//If this buffer is full messages will rollover to RXB1
	//RXB1CTRL - Control for receive buffer 0
	//This buffer only receives only standard identifier messages
	//BFPCTRL - Sets pin states of RXBF pins
	//RXBF pins not used
	config_bytes[0] = 0b00100100;	//RXB0CTRL
	config_bytes[1] = 0b00100000;	//RXB1CTRL
	config_bytes[2] = 0b00000000;	//BFPCTRL
	write_bytes_CAN(RXB0CTRL, config_bytes, 3);

	//CNF1 & CNF2 & CNF3
	//Used to setup the communication timing
	//Information about CAN bus transceiver bit timing
	//can be found in the MCP25625 data sheet pg. 21
	//Synchronized Jump Width - 4
	//Baud Rate Prescalar - 0
	//Sample Time Configuration - sample 3 times at sample point
	//PS1 Length - 4
	//PS2 Length - 4
	//Propagation Segment Length - 7
	/*config_bytes[0] = 0b11000000;	//CNF1
	config_bytes[1] = 0b11110011;	//CNF2
	config_bytes[2] = 0b00000011;	//CNF3*/ //153KBPS
	config_bytes[2] = 0b10000011;	//CNF1 /// BRP=3;SJW=3TQ
	config_bytes[1] = 0b01110000;	//CNF2 // BTLMODE=0;SAM=1;;PHSEG1=6;PRSEG=0; //125Kbps  //Received_125Kbps
	config_bytes[0] = 0b00000110;	//CNF3 // PHSEG2=6;*/
	write_bytes_CAN(CNF3, config_bytes, 3);

	//CANINTE - Enables the interrupts
	//Interrupts Enabled:
	//Message Error Interrupt
	//Transmit Buffer 0 Empty Interrupt
	//Receive Buffer 0 Full Interrupt
	//Receive Buffer 1 Full Interrupt
	config_bytes[0] = 0b10000011;	//CANINTE
	write_bytes_CAN(CANINTE, config_bytes, 1);

	config_bytes[0] = 0b00000000;
	write_bytes_CAN(TXB0CTRL, config_bytes, 1);
	write_bytes_CAN(TXB1CTRL, config_bytes, 1);

	//CANCTRL - Sets operation modes
	//Normal operation mode
	config_bytes[0] = 0b00000000;	//CANCTRL
	write_bytes_CAN(CANCTRL, config_bytes, 1);

	//Setup interrupt line for MCP25625
	PORTC_DIRCLR = (1 << CAN_INT);
	PORTC_INT0MASK = (1 << CAN_INT);	//Turn on interrupt
	PORTC_INTCTRL = 0x05;				//Low level interrupts
	PORTC_PIN3CTRL = 0x02;				//Falling Edge

	CAN_on = TRUE;
	can_functioning = UNTESTED;
	can_test_timer = ON;
	A1_ack_received = FALSE;
	A2_ack_received = FALSE;
}

//Initializes the CAN controller
void reinitialize_CAN(void)
{
	//stop_CAN();
	//IVA002: Standardized code
	CQ_initialize();
	CAN_on = FALSE;
	//Reset can
	PORTC_OUTCLR = (1 << CAN_RST);
	_delay_ms(10);
	PORTC_OUTSET = (1 << CAN_RST);

	//Reset CAN controller
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(RESET_CAN);
	CAN_CHIP_SELECT_HIGH;

	uint8_t config_bytes[4] = { 0 };

	//TXRTSCTRL - Sets pin states of RTS pins
	//RTS pins are not used in this application
	config_bytes[0] = 0b00000000;
	write_bytes_CAN(TXRTSCTRL, config_bytes, 1);	//TXRTSCTRL

	//RXB0CTRL - Control for receive buffer 0
	//This buffer only receives only standard identifier messages
	//If this buffer is full messages will rollover to RXB1
	//RXB1CTRL - Control for receive buffer 0
	//This buffer only receives only standard identifier messages
	//BFPCTRL - Sets pin states of RXBF pins
	//RXBF pins not used
	config_bytes[0] = 0b00100100;	//RXB0CTRL
	config_bytes[1] = 0b00100000;	//RXB1CTRL
	config_bytes[2] = 0b00000000;	//BFPCTRL
	write_bytes_CAN(RXB0CTRL, config_bytes, 3);

	//CNF1 & CNF2 & CNF3
	//Used to setup the communication timing
	//Information about CAN bus transceiver bit timing
	//can be found in the MCP25625 data sheet pg. 21
	//Synchronized Jump Width - 4
	//Baud Rate Prescalar - 0
	//Sample Time Configuration - sample 3 times at sample point
	//PS1 Length - 4
	//PS2 Length - 4
	//Propagation Segment Length - 7
	/*config_bytes[0] = 0b11000000;	//CNF1
	config_bytes[1] = 0b11110011;	//CNF2
	config_bytes[2] = 0b00000011;	//CNF3*/ //153KBPS

	/*config_bytes[2] = 0b10000001;	//CNF1 /// BRP=1;SJW=3TQ
	config_bytes[1] = 0b11101001;	//CNF2 // BTLMODE=1;SAM=1;;PHSEG1=5;PRSEG=1; //250KBPS   //Received_250Kbps
	config_bytes[0] = 0b00000110;	//CNF3 // PHSEG2=6;*/

	config_bytes[2] = 0b10000011;	//CNF1 /// BRP=3;SJW=3TQ
	config_bytes[1] = 0b01110000;	//CNF2 // BTLMODE=0;SAM=1;;PHSEG1=6;PRSEG=0; //125Kbps  //Received_125Kbps
	config_bytes[0] = 0b00000110;	//CNF3 // PHSEG2=6;

	write_bytes_CAN(CNF3, config_bytes, 3);

	//CANINTE - Enables the interrupts
	//Interrupts Enabled:
	//Message Error Interrupt
	//Transmit Buffer 0 Empty Interrupt
	//Receive Buffer 0 Full Interrupt
	//Receive Buffer 1 Full Interrupt
	config_bytes[0] = 0b10000011;	//CANINTE
	write_bytes_CAN(CANINTE, config_bytes, 1);

	config_bytes[0] = 0b00000000;
	write_bytes_CAN(TXB0CTRL, config_bytes, 1);
	write_bytes_CAN(TXB1CTRL, config_bytes, 1);

	//CANCTRL - Sets operation modes
	//Normal operation mode
	config_bytes[0] = 0b00000000;	//CANCTRL
	write_bytes_CAN(CANCTRL, config_bytes, 1);

	//Setup interrupt line for MCP25625
	PORTC_DIRCLR = (1 << CAN_INT);
	PORTC_INT0MASK = (1 << CAN_INT);	//Turn on interrupt
	PORTC_INTCTRL = 0x05;				//Low level interrupts
	PORTC_PIN3CTRL = 0x02;				//Falling Edge

	CAN_on = TRUE;
}


void stop_CAN(void)
{
	/*PORTC_DIRCLR &= ~(1 << CAN_INT);
	PORTC_INT0MASK &= ~(1 << CAN_INT);	//Turn off interrupt
	PORTC_INTCTRL &= ~0x05;				//Low level interrupts
	PORTC_PIN3CTRL &= ~0x02;			//Falling Edge*/

	CQ_initialize();
	CAN_on = FALSE;
}

//Writes the specified number of bytes to the specified address to the CAN controller
void write_bytes_CAN(uint8_t address, uint8_t bytes[], uint8_t num_bytes)
{
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(WRITE_CAN);
	write_byte_SPI(address);
	write_bytes_SPI(bytes, num_bytes);
	CAN_CHIP_SELECT_HIGH;
}

#ifndef  __SIM__
//Reads the specified number of bytes to the specified address to the CAN controller
void read_bytes_CAN(uint8_t address, uint8_t bytes[], uint8_t num_bytes)
{
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(READ_CAN);
	write_byte_SPI(address);
	read_bytes_SPI(bytes, num_bytes);
	CAN_CHIP_SELECT_HIGH;
}

//Reads the specified register from the CAN controller
uint8_t read_byte_CAN(uint8_t address)
{
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(READ_CAN);
	write_byte_SPI(address);
	uint8_t bytes[1];
	read_bytes_SPI(bytes, 1);
	CAN_CHIP_SELECT_HIGH;

	return bytes[0];
}

#endif // ! __SIM__

//Adds a CAN message on the Tx queue
void transmit_CAN(uint16_t dataID, uint8_t remote_transmission_request,
	uint8_t data_length, uint8_t data[])
{
	uint8_t buff[TX_BLOCK_SIZE];
	buff[0] = dataID;
	buff[1] = remote_transmission_request;
	buff[2] = data_length;
	for (uint8_t i = 0; i < data_length; i++)
		buff[i + 3] = data[i];
	CQTx_enqueue(buff);
}

//Transmits a CAN message on the CAN bus
void transmit_CAN_(uint8_t dataID, uint8_t remote_transmission_request,
	uint8_t data_length, uint8_t data[])
{
	uint8_t retry = 1;

jump_retry:
	Check_flag = 1;
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(LOAD_TX_BUFFER_CAN | LOAD_BUFFER_0);
	/*Write CAN bus communication to SPI*/
	//Arrange explicit parameters to the register map of the MCP25625
	//IVA002: J1939 Implementation
	//   0x10FFxxA3
	//   |<--- std --------------------->|<-------------- extended -------------------------->|
	//   10 ..................03.......00  .... .......................
	//   28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//    1  0  0  0  0  1  1  1  1  1  1  1  1  x  x  x  x  x  x  x  x  1  0  1  0  0  0  1  1

	//Bits 10-3 of standard identifier
	write_byte_SPI(0b10000111);

	//Bits 2-0 of standard identifier 
	//& extended identifier enable bit & bits 17-16 of extended identifier
	//Extended identifier is used
	write_byte_SPI(0b11101011);

	//Bits 15-8 of extended identifier
	write_byte_SPI(dataID);

	//Bits 7-0 of extended identifier
	write_byte_SPI(0xA3);

	//If remote transmission request is required
	//set RTR bit as 1, and set data length to 0
	if (remote_transmission_request)
	{
		uint8_t rtr_dlc = 0x40;
		write_byte_SPI(rtr_dlc);
	}
	//Else set data length and write the data bytes
	else
	{
		uint8_t rtr_dlc = data_length;
		write_byte_SPI(rtr_dlc);
		for (uint8_t i = 0; i < data_length; i++)
		{
			write_byte_SPI(data[i]);
		}
	}
	CAN_CHIP_SELECT_HIGH;

	//Send out data through CAN
	CAN_CHIP_SELECT_LOW;
	write_byte_SPI(WRITE_CAN);
	write_byte_SPI(TXB0CTRL);
	if (Check_flag)
	{
		write_byte_SPI(0x08);
	}
	else
	{
		if (retry == 1)
		{
			retry--;
			goto jump_retry;
		}
	}
	CAN_CHIP_SELECT_HIGH;
	Check_flag = 0;
}

//If CAN error has set reset the CAN controller
void reset_CAN_error(void)
{
	//IVA002: Fix for CAN SPI Corruption
	CAN_MX_LOCK();
	uint8_t t_error = read_byte_CAN(TEC);
	uint8_t r_error = read_byte_CAN(REC);

	if (t_error > 120 || r_error > 120)
	{
		reinitialize_CAN();
	}
	CAN_MX_UNLOCK();
}

ISR(CAN_INTERRUPT)
{
	if (Check_flag)
	{
		Check_flag = 0;
	}
	uint8_t int_flags;
	int_flags = read_byte_CAN(CANINTF);

	//If either error interrupt flag is raised, notify the user
	if (int_flags & ERRIF)
	{
		errorCount++;
	}
	if (int_flags & MERRF)
	{
		errorCount++;
	}

	//If either receive buffer is full read the data and perform accordingly
	if (int_flags & RXB0_FULL)
	{
		uint8_t data[8] = { 0 };
		//Read only bits 3-0 of RXB0DLC to get the data length code
		uint8_t data_length = read_byte_CAN(RXB0DLC) & 0x0F;
		if (data_length > 8) //IVA001: Incase of corruption do not process the data
		{
			DBG1("A3 CAN Corruption CH0 %d", data_length);
		}
		else
		{
			read_bytes_CAN(RXB0D0, data, data_length);

			uint8_t ids[4] = { 0 };
			read_bytes_CAN(RXB0SIDH, ids, 4);

			if ((ids[1] && 0b00001000) != 0) // ignore std ids
			{
				uint16_t can_id = ids[2];
				CQRx_enqueue(data, can_id, data_length);
			}
		}
	}
	if (int_flags & RXB1_FULL)
	{
		uint8_t data[8] = { 0 };
		uint8_t data_length = read_byte_CAN(RXB1DLC) & 0x0F;
		if (data_length > 8) //IVA001: Incase of corruption do not process the data
		{
			DBG1("A3 CAN Corruption CH1 %d", data_length);
		}
		else
		{
			read_bytes_CAN(RXB1D0, data, data_length);

			//IVA002: J1939 Implementation
			uint8_t ids[4] = { 0 };
			read_bytes_CAN(RXB1SIDH, ids, 4);
			if ((ids[1] && 0b00001000) != 0) // ignore std ids
			{
				uint16_t can_id = ids[2];
				CQRx_enqueue(data, can_id, data_length);
			}
		}
	}

	//Clear the flags
	uint8_t clear[1] = { 0x00 };
	write_bytes_CAN(CANINTF, clear, 1);
}
