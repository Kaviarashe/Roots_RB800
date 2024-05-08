/*
 * SD_card.h
 *
 * Created: 4/18/2016 11:32:57 AM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef SD_CARD_H_
#define SD_CARD_H_

#define SD_CARD_CS		2 //PC2
#define SD_CARD_MOSI	5 //PC5
#define SD_CARD_MISO	6 //PC6
#define SD_CARD_SCK		7 //PC7
#define SD_CARD_PRESENT	4 //PD4

#define SD_CARD_CHIP_SELECT_LOW		PORTC_DIRSET = (1 << SD_CARD_CS); PORTC_OUTCLR = (1 << SD_CARD_CS);
#define SD_CARD_CHIP_SELECT_HIGH	PORTC_DIRSET = (1 << SD_CARD_CS); PORTC_OUTSET = (1 << SD_CARD_CS);

#define SD_CARD_IS_PRESENT	!(PORTD_IN & (1 << SD_CARD_PRESENT))

void initialize_SD_card(void);
uint8_t read_block_SD(uint32_t address);
uint8_t write_block_SD(uint32_t address);
//Debug function to clear sd card
void erase_SD_card(void);

volatile uint8_t response;
uint8_t sd_card_passed;
volatile uint16_t temp_counter;

#endif /* SD_CARD_H_ */