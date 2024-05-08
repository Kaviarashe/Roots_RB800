/*
 * SPI_CAN.h
 *
 * Created: 7/1/2015 10:15:40 AM
 *  Author: Anurag Vaddi Reddy
 */

#ifndef SPI_CAN_H_
#define SPI_CAN_H_

#include "RB_800.h"

 //Instructions listed in the MCP25625 data sheet (Table 5-1)
#define RESET_CAN			0b11000000
#define READ_CAN			0b00000011
#define READ_RX_BUFFER_CAN	0b10010000
#define WRITE_CAN			0b00000010
#define LOAD_TX_BUFFER_CAN	0b01000000
#define RTS_CAN				0b10000000
#define READ_STATUS_CAN		0b10100000
#define RX_STATUS_CAN		0b10110000
#define BIT_MODIFY_CAN		0b00000101

#define LOAD_BUFFER_0	0b000
#define LOAD_BUFFER_1	0b010
#define LOAD_BUFFER_2	0b100

#define RECEIVE_BUFFER_0	0b000
#define RECEIVE_BUFFER_1	0b100

//TX Registers
#define TXB0CTRL	0x30
#define TXB1CTRL	0x40
#define TXB2CTRL	0x50
#define TXRTSCTRL	0x0D
#define TXB0SIDH	0x31
#define TXB1SIDH	0x41
#define TXB2SIDH	0x51
#define TXB0SIDL	0x32
#define TXB1SIDL	0x42
#define TXB2SIDL	0x52
#define TXB0EID8	0x33
#define TXB1EID8	0x43
#define TXB2EID8	0x53
#define TXB0EID0	0x34
#define TXB1EID0	0x44
#define TXB2EID0	0x54
#define TXB0DLC		0x35
#define TXB1DLC		0x45
#define TXB2DLC		0x55
#define TXB0D0		0x36	/*Note: This is only the 1st byte out of 8. Simply treat 0x36/0x46/0x56 as the beginning of the			 */
#define TXB1D0		0x46	/*		array and add to access the other bytes. Or, write 8 bytes starting from 0x36/0x46/0x56 to		 */
#define TXB2D0		0x56	/*		access all the data bytes. There is also a command specifically for writing to the TX data bytes.*/

//RX Registers
#define RXB0CTRL	0x60
#define RXB1CTRL	0x70
#define BFPCTRL		0x0C
#define RXB0SIDH	0x61
#define RXB1SIDH	0x71
#define RXB0SIDL	0x62
#define RXB1SIDL	0x72
#define RXB0EID8	0x63
#define RXB1EID8	0x73
#define RXB0EID0	0x64
#define RXB1EID0	0x74
#define RXB0DLC		0x65
#define RXB1DLC		0x75
#define RXB0D0		0x66	/*Note: This is only the 1st byte out of 8. Simply treat 0x66/0x76 as the beginning of the array	*/
#define RXB1D0		0x76	/*		and add to access the other bytes. Or, read 8 bytes starting from 0x66/0x76 to access		*/
							/*		all the data bytes. There is also a command specifically for reading from the RX data bytes.*/

//Filter Registers
#define RXF0SIDH	0x00
#define RXF1SIDH	0x04
#define RXF2SIDH	0x08
#define RXF3SIDH	0x10
#define RXF4SIDH	0x14
#define RXF5SIDH	0x18
#define RXF0SIDL	0x01
#define RXF1SIDL	0x05
#define RXF2SIDL	0x09
#define RXF3SIDL	0x11
#define RXF4SIDL	0x15
#define RXF5SIDL	0x19
#define RXF0EID8	0x02
#define RXF1EID8	0x06
#define RXF2EID8	0x0A
#define RXF3EID8	0x12
#define RXF4EID8	0x16
#define RXF5EID8	0x1A
#define RXF0EID0	0x03
#define RXF1EID0	0x07
#define RXF2EID0	0x0B
#define RXF3EID0	0x13
#define RXF4EID0	0x17
#define RXF5EID0	0x1B

//Mask Registers
#define RXM0SIDH	0x20
#define RXM1SIDH	0x24
#define RXM0SIDL	0x21
#define RXM1SIDL	0x25
#define RXM0EID8	0x22
#define RXM1EID8	0x26
#define RXM0EID0	0x23
#define RXM1EID0	0x27

//Configuration Registers
#define CNF1		0x2A
#define CNF2		0x29
#define CNF3		0x28

//Error Detection Registers
#define TEC			0x1C
#define REC			0x1D
#define EFLG		0x2D

//Interrupt Registers
#define CANINTE		0x2B
#define CANINTF		0x2C
#define CANCTRL		0x0F
#define CANSTAT		0x0E

#define CAN_INTERRUPT	PORTC_INT0_vect
#define CAN_MOSI		5//PC5
#define CAN_MISO		6//PC6
#define CAN_SCK			7//PC7
#define CAN_CS			4//PC4
#define CAN_INT			3//PC3
#define CAN_RST			2//PC2

//IVA002: CAN Mutex
#define CAN_MX_LOCK()		cli()//PORTC_INT0MASK = 0;				// Halt interrupt
#define CAN_MX_UNLOCK()		sei()//PORTC_INT0MASK = 1<<CAN_INT;	// enable interrupt

#define CAN_SPI_BUS SPIC

#define CAN_CHIP_SELECT_HIGH	PORTC_DIRSET = (1 << CAN_CS); PORTC_OUTSET = (1 << CAN_CS);
#define CAN_CHIP_SELECT_LOW		PORTC_DIRSET = (1 << CAN_CS); PORTC_OUTCLR = (1 << CAN_CS);

//Interrupt flag bits
#define RXB0_FULL	0x01
#define RXB1_FULL	0x02
#define TXB0_EMPTY	0x04
#define MERRF		0x80
#define ERRIF		0x20

#define UNTESTED 3

void initialize_CAN(void);
void reinitialize_CAN(void);
void write_bytes_CAN(uint8_t address, uint8_t bytes[], uint8_t num_bytes);
void read_bytes_CAN(uint8_t address, uint8_t bytes[], uint8_t num_bytes);
uint8_t read_byte_CAN(uint8_t address);
void transmit_CAN(uint16_t dataID, uint8_t remote_transmission_request,
	uint8_t data_length, uint8_t data[]);
void transmit_CAN_(uint8_t dataID, uint8_t remote_transmission_request,
	uint8_t data_length, uint8_t data[]);
void reset_CAN_error(void);
void stop_CAN(void);


volatile uint8_t can_flag;
volatile uint8_t can_test_timer;
uint8_t CAN_on;
volatile uint8_t Check_flag;
#endif /* SPI_CAN_H_ */