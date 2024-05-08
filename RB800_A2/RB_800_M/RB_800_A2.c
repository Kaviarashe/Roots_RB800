/*
 * RB_800_A2.c
 *
 * Created: 7/1/2015 10:11:45 AM
 *  Author: Anurag Vaddi Reddy
 * IVA002: Renamed the file for easy identification
 */


#include "RB_800.h"

int main(void)
{
	brush_sns_out_timer = OFF;
	brush_sns_out_timer_count = 0;
	brush_sns_out_count = 30000;
#if PIN_TEST
	PORTA_DIR = 0xFF;
	PORTB_DIR = 0xFF;
	PORTC_DIR = 0xFF;
	PORTD_DIR = 0xFF;
	PORTE_DIR = 0xFF;
	PORTF_DIR = 0xFF;
	PORTH_DIR = 0xFF;
	PORTJ_DIR = 0xFF;
	PORTK_DIR = 0xFF;
	PORTQ_DIR = 0xFF;
	PORTR_DIR = 0xFF;
	while (1)
	{
		PORTA_OUTSET = 0xFF; _delay_ms(1); PORTA_OUTCLR = 0xFF; _delay_ms(1);
		PORTB_OUTSET = 0xFF; _delay_ms(1); PORTB_OUTCLR = 0xFF; _delay_ms(1);
		PORTC_OUTSET = 0xFF; _delay_ms(1); PORTC_OUTCLR = 0xFF; _delay_ms(1);
		PORTD_OUTSET = 0xFF; _delay_ms(1); PORTD_OUTCLR = 0xFF; _delay_ms(1);
		PORTE_OUTSET = 0xFF; _delay_ms(1); PORTE_OUTCLR = 0xFF; _delay_ms(1);
		PORTF_OUTSET = 0xFF; _delay_ms(1); PORTF_OUTCLR = 0xFF; _delay_ms(1);
		PORTH_OUTSET = 0xFF; _delay_ms(1); PORTH_OUTCLR = 0xFF; _delay_ms(1);
		PORTJ_OUTSET = 0xFF; _delay_ms(1); PORTJ_OUTCLR = 0xFF; _delay_ms(1);
		PORTK_OUTSET = 0xFF; _delay_ms(1); PORTK_OUTCLR = 0xFF; _delay_ms(1);
		PORTQ_OUTSET = 0xFF; _delay_ms(1); PORTQ_OUTCLR = 0xFF; _delay_ms(1);
		PORTR_OUTSET = 0xFF; _delay_ms(1); PORTR_OUTCLR = 0xFF; _delay_ms(1);
	}
#endif

	//Reset CAN controller
	/*Reset pin 1 and pin 2 since my board has uses a different pin than roots*/
	PORTA_DIRSET = (1 << CAN_RST) | (1 << (CAN_RST - 1));
	PORTA_OUTCLR = (1 << CAN_RST) | (1 << (CAN_RST - 1));

	initialize();
	brush_sns_out_count = 0;
	brush_sns_out_timer_count = 0;
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
	_delay_ms(50);*/
	//vacuum_slow_start=12;
	//brush_slow_start=12;
	//IDLE_LOOP
	while (true)
	{
		//Measure motor current if CAN queue is empty
		if (!CQRx_queueIsEmpty())
		{
			CQRx_dequeue();
			reset_CAN_error();
		}
		else if (measure_current)
		{
			measure_currents();

			measure_current = FALSE;
		}
		else if (check_max_current)
		{
			//Update session max current and overall max current
			if ((uint8_t)brush1_current > max_brush1_session_current)
			{
				max_brush1_session_current = (uint8_t)brush1_current;
				if (max_brush1_session_current > max_brush1_overall_current)
					max_brush1_overall_current = max_brush1_session_current;
			}
			if ((uint8_t)brush2_current > max_brush2_session_current)
			{
				max_brush2_session_current = (uint8_t)brush2_current;
				if (max_brush2_session_current > max_brush2_overall_current)
					max_brush2_overall_current = max_brush2_session_current;
			}
			if ((uint8_t)vacuum_current > max_vacuum_session_current)
			{
				max_vacuum_session_current = (uint8_t)vacuum_current;
				if (max_vacuum_session_current > max_vacuum_overall_current)
					max_vacuum_overall_current = max_vacuum_session_current;
			}
			if ((uint8_t)presweep_current > max_presweep_session_current)
			{
				max_presweep_session_current = (uint8_t)presweep_current;
				if (max_presweep_session_current > max_presweep_overall_current)
					max_presweep_overall_current = max_presweep_session_current;
			}

			check_max_current = FALSE;
		}

		if (emergency_off)
		{
			//Disable all 9201 devices
			PORTF_OUTSET = (1 << DISABLE_9201);
			//Disable all 8701 devices
			PORTE_OUTCLR = (1 << ENABLE_8701);
		}

		//If the accelerator has changed, perform the actions required
		if (accelerator_changed)
		{
			accelerator_changed = FALSE;
			if (accelerator == ACCEL_DN)
			{
				accelerator_down();
				/*log_enqueue(PEDAL_DOWN_EVENT);*/
			}
			else
			{
				accelerator_up();
				/*log_enqueue(PEDAL_UP_EVENT);*/
			}
		}

		//If the alarm flag has been set
		//Send the alarm code to the A1 board
		if (send_alarm_flag)
		{
			send_alarm(alarm_count);
			send_alarm_flag = FALSE;
			//Alarm sent prevents board from repeatedly sending the alarm code
			alarm_sent = TRUE;
		}

		//If the float flag is set inform the A1 board
		if (send_float_flag)
		{
			send_tank_full();
			send_float_flag = FALSE;
			vacuum_off_timer = ON;
			SQ_UP;

			log_enqueue(TANK_FULL_EVENT);
		}

		//If the fault flag is set inform the A1 board
		if (send_fault_flag)
		{
			send_fault_error();
			send_fault_flag = FALSE;
		}

		//If over current flag is set inform A1 board
		if (send_brush_overcurrent_alarm)
		{
			send_bm_overcurrent_error();
			send_brush_overcurrent_alarm = FALSE;
		}

		//If top flag is set inform the A1 board
		if (send_top_flag)
		{
			send_top_error();

			if (log_top_event)
			{
				uint8_t message[2];
				message[0] = A2_TOP_EVENT;
				message[1] = 0;
				log_event(message);
				log_top_event = FALSE;
			}

		}

		//If fuse fail flag is set inform the A1 board
		if (send_fuse_flag)
		{
			send_fuse_error();
			send_fuse_flag = FALSE;

			uint8_t message[2];
			message[0] = FUSE_BLOWN_EVENT;
			message[1] = 0;
			log_event(message);
		}

		//If is reset is TRUE and the 9201 devices are off
		//Reset the 9201's
		if (is_reset && (PORTF_OUT & (1 << DISABLE_9201)))
		{
			is_reset = FALSE;
			PORTF_OUTCLR = (1 << DISABLE_9201);
		}

		//Switching to reverse
		if (switch_motor_reverse)
		{
			SET_DRIVE_MOTOR_REVERSE;
			current_dir = REVERSE;
			switch_motor_reverse = FALSE;
		}

		//Switching to forward
		if (switch_motor_forward)
		{
			SET_DRIVE_MOTOR_FORWARD;
			current_dir = FORWARD;
			switch_motor_forward = FALSE;
		}

		/*Testing without log data*/
		//Every second write to the SD card
		if (second_timer_tick && !(PORTF_OUT & (1 << BH_PWM)))
		{
			log_dequeue();

			//If RB800 was powered on without an sd card previously, then record the runtimer hours and minutes of when the sd card was missing
			if (check_sd_card)
			{
				cli();
				if (!sd_card_passed)
				{
					if (eeprom_read_word((uint16_t*)EEPxx_SD_CARD_MISSING_ADDR) == 0 && eeprom_read_byte((uint8_t*)(EEPxx_SD_CARD_MISSING_ADDR + 2)) == 0)
					{
						eeprom_update_word((uint16_t*)EEPxx_SD_CARD_MISSING_ADDR, hours);
						eeprom_update_byte((uint8_t*)(EEPxx_SD_CARD_MISSING_ADDR + 2), minutes);
					}
				}
				else
				{
					uint16_t sd_hours = eeprom_read_word((uint16_t*)EEPxx_SD_CARD_MISSING_ADDR);
					uint8_t sd_minutes = eeprom_read_byte((uint8_t*)(EEPxx_SD_CARD_MISSING_ADDR + 2));
					if (sd_hours != 0 || sd_minutes != 0)
					{
						uint8_t message[3];
						message[0] = (uint8_t)(sd_hours << 8);
						message[1] = (uint8_t)sd_hours;
						message[2] = sd_minutes;
						log_enqueue_data(SD_MISSING_EVENT, 3, message);

						eeprom_update_word((uint16_t*)EEPxx_SD_CARD_MISSING_ADDR, 0);
						eeprom_update_byte((uint8_t*)(EEPxx_SD_CARD_MISSING_ADDR + 2), 0);
					}
				}
				sei();
				check_sd_card = FALSE;
			}

			//Make sure device is not losing power
			while (POWERING_DOWN) {};
			/* IVA002: Removed Unwanted problematic CAN Read
			//Check for any missed interrupts
			if (CAN_WAITING)
			{
				uint8_t int_flags;
				int_flags = read_byte_CAN(CANINTF);

				//If either error interrupt flag is raised, notify the user
				if (int_flags & ERRIF)
				{
					PORTQ_OUTSET = (1 << 2);
				}
				if (int_flags & MERRF)
				{
					PORTQ_OUTSET = (1 << 2);
				}

				//If either receive buffer is full read the data and perform accordingly
				if (int_flags & RXB0_FULL)
				{
					uint8_t data[8];
					//Read only bits 3-0 of RXB0DLC to get the data length code
					uint8_t data_length = read_byte_CAN(RXB0DLC) & 0x0F;
					read_bytes_CAN(RXB0D0, data, data_length);

					uint8_t setting_arr[2];
					read_bytes_CAN(RXB0SIDH, setting_arr, 2);
					uint16_t setting = (setting_arr[0] << 3) + (setting_arr[1] >> 5);

					CQRx_enqueue(data, setting, data_length);
				}
				if (int_flags & RXB1_FULL)
				{
					uint8_t data[8];
					uint8_t data_length = read_byte_CAN(RXB1DLC) & 0x0F;
					read_bytes_CAN(RXB1D0, data, data_length);

					uint8_t setting_arr[2];
					read_bytes_CAN(RXB1SIDH, setting_arr, 2);
					uint16_t setting = (setting_arr[0] << 3) + (setting_arr[1] >> 5);

					CQRx_enqueue(data, setting, data_length);
				}

				//Clear the flags
				uint8_t clear[1] = { 0x00 };
				write_bytes_CAN(CANINTF, clear, 1);
			}
*/
//If TOP pin is low, start a debounce timer of 500 ms to make sure pin went low
			if (PORTE_IN & (1 << TOP_PIN) && top_debounce_timer == OFF && top_debounce_timer <= 5)
			{
#ifndef BENCH_TEST //IVA002: Suppressed error in bench test
				top_debounce_timer = ON;
				top_debounce_count = 0;
				start_100_ms_timer();
#endif
			}

			//If FLT pin is low, start a 8 second timer to make sure float sensor is reading full tank
			if (!(PORTE_IN & (1 << FLOAT_PIN)) && float_sensor_timer == OFF)
			{
				float_sensor_timer = ON;
			}

			//Check if presweep state has changed
			if (!(PORTF_IN & (1 << PRE_SWP_SNS)))
			{
				if (presweep != ON)
				{
					presweep = ON;
					if (accelerator == ACCEL_DN)
					{
						pre_swp_set(ON);
					}
				}
			}
			else
			{
				if (presweep != OFF)
				{
					if (runtimer_on && accelerator == ACCEL_UP && vacuum_state == VACUUM_OFF && horn == OFF && headlights == OFF)
					{
						send_runtime_timer(OFF);
					}
					else if (!runtimer_on && (accelerator == ACCEL_DN || vacuum_state != VACUUM_OFF || horn != OFF || headlights != OFF))
					{
						send_runtime_timer(ON);
					}

					TCE0.CTRLB &= ~(0x80);
					PORTE_OUTCLR = (1 << PRE_SWP_ON);
					stop_25_ms_pre_swp_timer();
					presweep = OFF;
					presweep_dc = OFF;
					TCE0.CCD = 0;
					__SIM_SET_PWM(PWM_A2_PRES_MOTOR, TCE0.CCD);
				}
			}

			//Check the fuses every second
			if (!(PORTF_OUT & (1 << DISABLE_9201)))
				check_fuses();


			second_timer_tick = FALSE;
		}

		//Measure the current of running motors every 1 minute
		if (update_current)
		{
			if (seconds == 10 && brush_state == BRUSH_RUNNING)
			{
				uint8_t message[1];
				message[0] = (uint8_t)brush1_current;
				log_enqueue_data(BRUSH_1_CURRENT, 1, message);

				brush1_average_current = (brush1_average_current * 95 + brush1_current * 5) / 100;
			}
			else if (seconds == 20 && brush_state == BRUSH_RUNNING)
			{
				uint8_t message[1];
				message[0] = (uint8_t)brush2_current;
				log_enqueue_data(BRUSH_2_CURRENT, 1, message);

				brush2_average_current = (brush2_average_current * 95 + brush2_current * 5) / 100;
			}
			else if (seconds == 30 && (vacuum_state == VACUUM_RUNNING || vacuum_state == VACUUM_RUNNING_OFF))
			{
				uint8_t message[1];
				message[0] = (uint8_t)vacuum_current;
				log_enqueue_data(VACUUM_CURRENT, 1, message);

				vacuum_average_current = (vacuum_average_current * 95 + vacuum_current * 5) / 100;
			}
			else if (seconds == 40 && presweep)
			{
				uint8_t message[1];
				message[0] = (uint8_t)presweep_current;
				log_enqueue_data(PRESWEEP_CURRENT, 1, message);

				presweep_average_current = (presweep_average_current * 95 + presweep_current * 5) / 100;
			}

			if (update_eeprom_statistics && (brush_state == BRUSH_RUNNING || vacuum_state == VACUUM_RUNNING || (water_pump && brush_state == BRUSH_RUNNING) || (detergent_pump && brush_state == BRUSH_RUNNING) || (presweep_state == ON) || eco_mode))
			{
				cli();
				eeprom_update_byte((uint8_t *)EEP8_BRUSH_MINUTES_ADDR, brush_minutes); //update brush minutes for every one minute 
				eeprom_busy_wait();
				eeprom_update_byte((uint8_t *)EEP8_VACUUM_MINUTES_ADDR, vacuum_minutes); //update vacuum minutes for every one minute 
				eeprom_busy_wait();
				eeprom_update_byte((uint8_t *)EEP8_PRESWEEP_MINUTES_ADDR, presweep_minutes); //update presweep minutes for every one minute 
				eeprom_busy_wait();
				eeprom_update_byte((uint8_t *)EEP8_ECO_MINUTES_ADDR, eco_minutes); //update Eco minutes for every one minute 
				eeprom_busy_wait();
				eeprom_update_byte((uint8_t *)EEP8_WATER_PUMP_MINUTES_ADDR, water_pump_minutes); //update water pump minutes for every one minute 
				eeprom_busy_wait();
				eeprom_update_byte((uint8_t *)EEP8_DETERGENT_PUMP_MINUTES, detergent_pump_minutes); //update detergent pump minutes for every one minute 
				eeprom_busy_wait();
				sei();
			}
			update_current = FALSE;
			update_eeprom_statistics = FALSE;
		}

		//Update the max current measured and log the current battery voltage every 5 minute
		if (update_max_current)
		{
			if (minutes % 5 == 0 && max_brush1_session_current != 0)
			{
				uint8_t message[1];
				message[0] = (uint8_t)max_brush1_session_current;
				log_enqueue_data(MAX_BRUSH_1_CURRENT, 1, message);
			}
			else if (minutes % 5 == 1 && max_brush2_session_current != 0)
			{
				uint8_t message[1];
				message[0] = (uint8_t)max_brush2_session_current;
				log_enqueue_data(MAX_BRUSH_2_CURRENT, 1, message);
			}
			else if (minutes % 5 == 2 && max_vacuum_session_current != 0)
			{
				uint8_t message[1];
				message[0] = (uint8_t)max_vacuum_session_current;
				log_enqueue_data(MAX_VACUUM_CURRENT, 1, message);
			}
			else if (minutes % 5 == 3 && max_presweep_session_current != 0)
			{
				uint8_t message[1];
				message[0] = (uint8_t)max_presweep_session_current;
				log_enqueue_data(MAX_PRESWEEP_CURRENT, 1, message);
			}
			if (minutes % 5 == 4)
			{
				//Convert total provided to voltage value
				uint16_t voltage = ((float)battery_total - 195.0);
				uint8_t message[2];
				message[0] = (uint8_t)(voltage / 100);
				message[1] = (uint8_t)(voltage % 100);
				log_enqueue_data(BATTERY_VOLTAGE_EVENT, 2, message);
			}

			update_max_current = FALSE;
		}
	}
}

void initialize(void)
{
	brush_sns_out_timer = OFF;
	brush_sns_out_timer_count = 0;
	//Initialize test led
	PORTQ_DIRSET = (1 << 2);

	//Pin for checking if A3 or A2.5 board is being used
	PORTJ_PIN0CTRL = PORT_OPC_PULLUP_gc;

	initialize_32Mhz();
	//1 second delay to make sure power has settled
	//Without this delay EEPROM could get corrupted when initially reading
	_delay_ms(1000);
	PORTQ_OUTSET = (1 << 2);
	_delay_ms(1000);
	PORTQ_OUTCLR = (1 << 2);

	//Reset CAN
	/*Reset pin 1 and pin 2 since my board has uses a different pin than roots*/
	PORTA_OUTSET = (1 << CAN_RST) | (1 << (CAN_RST - 1));

	initialize_ADC();

	// Enable internal pull-up on PC0, PC1.. Uncomment if you don't have external pullups
	PORTCFG.MPCMASK = 0x03; // Configure several PINxCTRL registers at the same time
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches

	//Initialize the TWI master module using the TWIC port
	TWI_MasterInit(&twiMaster,
		&TWIC,
		TWI_MASTER_INTLVL_LO_gc,
		TWI_BAUDSETTING);

	initialize_RB_settings();
	initialize_SPI();

	sd_card_passed = FALSE;
	if (SD_CARD_IS_PRESENT)
	{
		initialize_SD_card();
		if (sd_card_passed)
		{
			initialize_event_logging();
			initialize_event_log_queue();
		}
	}

	initialize_timers();
	initialize_motors();
	initialize_alarms();
	CQ_initialize();

	//Enable low level interrupts
	PMIC_CTRL = 0x07;
	//Enable global interrupts
	sei();

	initialize_Screen();
	initialize_CAN();

#ifndef __SIM__ //IVA002: Donot send reset in simulation
	send_reset_CAN();
#endif // !__SIM__

	log_enqueue(RB800_ON_EVENT);

	measure_v_off();

	start_100_ms_timer();

	if (!sd_card_passed)
	{
		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		emergency_off = TRUE;
		_delay_ms(3000);
		send_SD_card_fail();
	}
}


//TWIE Master Interrupt vector
ISR(TWIC_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}
