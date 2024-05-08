/*
 * RB_800_A3.c
 *
 * Created: 6/15/2016 1:51:34 PM
 * Author : Anurag Vaddi Reddy
 * IVA002: Renamed the file for easy identification
 */

#include "RB_800.h"


int main(void)
{
	//Reset CAN controller
	PORTC_DIRSET = (1 << CAN_RST);
	PORTC_OUTCLR = (1 << CAN_RST);

	initialize();
	/*status_reg_value=RST.STATUS;
	if(status_reg_value & RST_SDRF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_SDRF_bm); SDRF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_SDRF_cnt_location, SDRF_cnt);
	}
	if(status_reg_value & RST_SRF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_SRF_bm);SRF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_SRF_cnt_location, SRF_cnt);
	}
	if(status_reg_value & RST_PDIRF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_PDIRF_bm);PDIRF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_PDIRF_cnt_location, PDIRF_cnt);
	}
	if(status_reg_value & RST_WDRF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_WDRF_bm);WDRF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_WDRF_cnt_location, WDRF_cnt);
	}
	if(status_reg_value & RST_BORF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_BORF_bm);BORF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_BORF_cnt_location, BORF_cnt);
	}
	if(status_reg_value & RST_EXTRF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_EXTRF_bm);EXTRF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_EXTRF_cnt_location, EXTRF_cnt);
	}
	if(status_reg_value & RST_PORF_bm)
	{
		RST.STATUS=(RST.STATUS | RST_PORF_bm); PORF_cnt++;
		eeprom_busy_wait();
		eeprom_update_byte((uint8_t *) EEP_PORF_cnt_location, PORF_cnt);
	}
	_delay_ms(50);
	send_reset_flag_counts();
	_delay_ms(50);
	*/
	//IDLE_LOOP
	while (true)
	{
		//Measure pedal level and current constantly
		measure_accelerator_pedal();
		measure_motor_current();

		//Update the pedal level every 200 ms
		if (_100_ms_check > 2 && can_functioning == TRUE && can_test_timer == OFF && emergency_stop_motor == FALSE)
		{
			update_pedal_level();
			_100_ms_check = FALSE;
		}

		if (!CQRx_queueIsEmpty() && CAN_on)
		{
			CQRx_dequeue();
			reset_CAN_error();
		}

		//Log drive current every minute
		if (log_current_counter >= 60 && pedal_state == ON)
		{
			send_drive_motor_current();
			log_current_counter = 0;
		}

		//Log max drive current every 5 minutes
		if (log_max_current_counter >= 302)
		{
			send_max_drive_motor_current();
			log_max_current_counter = 0;
		}
	}
}

void initialize(void)
{
	initialize_SPI();
	initialize_32Mhz();
	//1 second delay to make sure power has settled
	//Without this delay EEPROM could get corrupted when initially reading
	_delay_ms(1000);

	PORTD_DIRSET = (1 << ACTIVITY_LED);
	ACTIVITY_LED_ON;
	_delay_ms(1000);
	ACTIVITY_LED_OFF;

	//Reset CAN controller
	PORTC_OUTSET = (1 << CAN_RST);

	CQ_initialize();
	initialize_ADC();
	measure_v_off();
	_delay_ms(10);

	// Enable internal pull-up on PC0, PC1
	PORTCFG.MPCMASK = 0x03; // Configure several PINxCTRL registers at the same time
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches

	//Initialize the TWI master module using the TWIC port
	TWI_MasterInit(&twiMaster,
		&TWIC,
		TWI_MASTER_INTLVL_LO_gc,
		TWI_BAUDSETTING);

	initialize_RB_settings();

	//Enable low level interrupts
	PMIC_CTRL = 0x07;
	//Enable global interrupts
	sei();

	initialize_CAN();
	initialize_Screen();

	initialize_motor();
	initialize_timers();

	check_initial_switch_counter = ON;
#ifndef __SIM__ //IVA002: Donot send reset CAN in simulator
	send_reset_CAN();
#endif // !__SIM__
}

ISR(TWIC_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

