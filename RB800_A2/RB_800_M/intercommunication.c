/*
 * intercommunication.c
 *
 * Created: 11/24/2015 11:37:54 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "intercommunication.h"

void processCAN(uint16_t identifier, uint8_t* data)
{
	uint8_t message[21];
	uint16_t temp_pedal_level;

	DBG2("processCAN: %02X %02X", identifier, data[0]);
	//Process the communication depending on the type of communication
	switch (identifier)
	{
		//Update settings of all motors and peripherals
	case ALL_STATUS:
		update_all_settings(data);
		break;

		//Send the battery voltage to A1
	case BATTERY_CAN:
		send_battery_voltage();
		break;

		//Turn on ECO mode
	case ECO_CAN:
		if (emergency_off)
			return;
		/*//Log eco mode if it changes
		if(eco_mode && data[0] == OFF)
		{
			log_enqueue(ECO_OFF_EVENT);
		}
		else if(!eco_mode && data[0] == ON)
		{
			log_enqueue(ECO_ON_EVENT);
		}*/
		eco_mode = data[0];

		if (presweep && (accelerator == ACCEL_DN))
		{
			if (!runtimer_on)
			{
				send_runtime_timer(ON);
			}
			pre_swp_set(ON);
		}
		break;

		//Save motor start up info and send ACK
	case MOTOR_INFO_CAN:
		//vacuum_slow_start = data[0];
		//brush_slow_start = data[1];
		//pre_swp_slow_start = data[2];
		transmit_CAN(MOTOR_INFO_CAN, 0, 0, data);
		break;

		//Update pedal state and level
	case ACCEL_PDL_CAN:
	{
		uint8_t ack = 0xAC; //IVA003: Send ACK for the pedal state change
		transmit_CAN(ACCEL_PDL_CAN, 0, 1, &ack);
		if (emergency_off)
			return;
		if (data[0] == PEDAL_STATE)
		{
			if (data[1] == ACCEL_DN)
			{
				if (accelerator == ACCEL_UP)
				{
					//LED turns on when accelerator pedal is down
					PORTQ_OUTSET = (1 << 2);
					accelerator = ACCEL_DN;
					//If accelerator changed is already true,
					//then set it to false since the accelerator has gone back to its original position before the accelerator change code has run
					accelerator_changed = !accelerator_changed;
				}
			}
			else
			{
				if (accelerator == ACCEL_DN)
				{
					//LED turns off when accelerator pedal is down
					PORTQ_OUTCLR = (1 << 2);
					accelerator = ACCEL_UP;
					accelerator_changed = !accelerator_changed;
				}
			}
			__SIM_DOUT;
		}
		//Update the pumps if the pedal level changes
		else if (data[0] == PEDAL_LEVEL && accelerator == ACCEL_DN)
		{
			temp_pedal_level = data[1] << 8;
			temp_pedal_level += data[2];
			if (temp_pedal_level != pedal_level)
			{
				pedal_level = temp_pedal_level;
				update_pumps();
			}
		}
	}
	break;

	//Turn off all motors if TOP event occurs
	case TOP_CAN:
		if (data[0] == A3_CAN)
		{
			message[0] = A3_TOP_EVENT;
			message[1] = 0;
			log_event(message);
		}

		//Disable all 9201 devices
		PORTF_OUTSET = (1 << DISABLE_9201);
		//Disable all 8701 devices
		PORTE_OUTCLR = (1 << ENABLE_8701);

		water_pump_set(OFF, OFF);

		Solenoid_set(OFF);
		detergent_pump_set(OFF, OFF);

		send_top_flag = FALSE;

		emergency_off = TRUE;
		break;

		//Turn off horn in case it is stuck on after back safety failure
	case BACK_SAFETY_CAN:
		if (data[0] == BACK_SAFETY_UNRESOLVED)
			horn_set(OFF);
		break;

		//Log an event from A1 or A2
	case EVENT_LOG_CAN:
		if (data[0] >= FIRST_EVENT && data[0] <= LAST_EVENT)
		{
			switch (data[0])
			{
				/*case WATER_ON_EVENT:
				case DETERGENT_ON_EVENT:
					message[0] = data[1];
					log_enqueue_data(data[0], 1, message);
					break;*/

			default:
				log_enqueue(data[0]);
				break;
			}
		}
		break;

		//Get runtimer value from A1
	case RUNTIMER_CAN:
		runtimer_on = data[0];
		//runtimer_on = data[0] >> 4;
		//solenoid_valve = data[0] & 0x0F;
		hours = data[1] << 8 | data[2];
		minutes = data[3];
		event_logging = ON;
		check_sd_card = TRUE;
		break;

		//React to an alarm	
	case ALARM_CAN:
		if (data[0] == OVERVOLTAGE)
		{
			//Disable all 9201 devices
			PORTF_OUTSET = (1 << DISABLE_9201);
			//Disable all 8701 devices
			PORTE_OUTCLR = (1 << ENABLE_8701);

			water_pump_set(OFF, OFF);
			Solenoid_set(OFF);
			detergent_pump_set(OFF, OFF);

			emergency_off = TRUE;
		}
		break;

		//Respond to ACK frmo A1 with an ACK
	case ACK_CAN:
		if (data[0] == A1_CAN)
		{
			send_CAN_ACK();
		}
		can_functioning = TRUE;
		break;

		//Reset all values to default values
	case RESET_DEFAULT_CAN:
		set_to_default();
		break;
		//Emergency off received disable all the functions
	case EMERGENCY_OFF_CAN:
		emergency_off = TRUE;

		//Update A2 serial number
	case A2_SERIAL_NUM_CAN:
		if (data[0] == 1)
		{
			A2_serial_num[0] = data[1];
			A2_serial_num[1] = data[2];
			A2_serial_num[2] = data[3];
			A2_serial_num[3] = data[4];
			A2_serial_num[4] = data[5];
			A2_serial_num[5] = data[6];
		}
		else if (data[0] == 2)
		{
			A2_serial_num[6] = data[1];
			A2_serial_num[7] = data[2];
			A2_serial_num[8] = data[3];
			A2_serial_num[9] = data[4];
			A2_serial_num[10] = data[5];
			A2_serial_num[11] = data[6];
		}
		else if (data[0] == 3)
		{
			A2_serial_num[12] = data[1];
			A2_serial_num[13] = data[2];
			eeprom_update_block((const void*)A2_serial_num, (void *)EEPxx_A2_SERIAL_NUM, A2_SERIAL_NUM_LEN);
		}
		else
		{
			send_serial_num();
		}
		break;

		//Log drive current
	case DRIVE_CURRENT_CAN:
		log_enqueue_data(DRIVE_CURRENT, 1, data);
		break;

		//Log max drive current
	case MAX_DRIVE_CURRENT_CAN:
		log_enqueue_data(MAX_DRIVE_CURRENT, 1, data);
		break;

		//Send statistics if A1 goes into settings menu
	case REQUEST_STATISTICS:
		send_statistics();
		break;

		//Erase the SD card
	case ERASE_SD_CAN:
		erase_SD_card();
		break;
		//update vaccum slow start value
	case VACCUM_SLOW_START_CAN:
		vacuum_slow_start = data[0];
		update_vaccum_slow_start();
		break;
		//update brush slow start value
	case BRUSH_SLOW_START_CAN:
		brush_slow_start = data[0];
		update_brush_slow_start();
		break;
		//update presweep slow start value
	case PRESWEEP_SLOW_START_CAN:
		pre_swp_slow_start = data[0];
		update_pre_swp_slow_start();
		break;
		//Reset device if instructed to reset more than 10 seconds past turn on
	case RESET_DEVICE_CAN_A1:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			REBOOT();
		}
		break;
		//Reset device if instructed to reset more than 10 seconds past turn on
	case RESET_DEVICE_CAN_A3:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			REBOOT();
		}
		break;

	default:
		break;
	}
}

void update_all_settings(uint8_t* data)
{
	//Checksum to make sure data is not corrupted
	uint8_t i = 0;
	uint8_t checksum1 = 0xFF;
	uint16_t checksum2 = 0;

	for (i = 0; i < 7; i++)
	{
		checksum1 ^= data[i];
		checksum2 += data[i];
	}

	if ((checksum1 == current_checksum1) && (checksum2 == current_checksum2))
	{
		return;
	}
	else
	{
		current_checksum1 = checksum1;
		current_checksum2 = checksum2;
	}

	i = 0;

	//Separate out settings from data bytes
	//brush = data[i++];
	brush = data[i++];//>> 4;
	//solenoid_valve = data[i++] & 0x0F;	
	vacuum = data[i++];
	detergent_pump = data[i] >> 7;
	detergent_pump_dc = data[i++] & 0x7F;
	water_pump = data[i] >> 7;
	water_pump_dc = data[i++] & 0x7F;
	headlights = data[i] >> 4;
	solenoid_valve = (data[i] & 0x0c) >> 2;
	horn = data[i++] & 0x03;
	extra_pressure = data[i] >> 4;
	reverse = data[i++] & 0x0F;
	transport = data[i] >> 4;
	fr_speed = data[i++] & 0x0F;


	//Headlights and horn can be turned on immediately
	horn_set(horn);
	headlights_set(headlights);

	//Only headlights and horn work during emergency off
	if (emergency_off)
		return;

	extra_pressure_set(extra_pressure);

	//Switching between forward and reverse
	if (reverse && current_dir == FORWARD)
	{
		switch_motor_forward = FALSE;
		switch_motor_reverse = TRUE;

		if (squeegee_down)
		{
			SQ_UP;
			squeegee_timer = ON;
			squeegee_down = FALSE;
		}
	}
	else if (!reverse && current_dir == REVERSE)
	{
		switch_motor_reverse = FALSE;
		switch_motor_forward = TRUE;

		if (vacuum && !tank_full && !squeegee_down && !transport)
		{
			SQ_DN;
			squeegee_timer = ON;
			squeegee_down = TRUE;
		}
	}

	//Change drive motor speed
	if (fr_speed == SLOW)
	{
		SET_DRIVE_SPEED_SLOW;
	}
	else
	{
		SET_DRIVE_SPEED_FAST;
	}

	//In transport mode turn off everything
	if (transport)
	{
		transport_timer = ON;
		brush_state = BRUSH_OFF;
		update_brush_state();
		brush_motor_set(0);
		water_pump_set(OFF, water_pump_dc);
		Solenoid_set(OFF);
		detergent_pump_set(OFF, detergent_pump_dc);

		if (runtimer_on && accelerator == ACCEL_UP && vacuum_state == VACUUM_OFF && horn == OFF && headlights == OFF)
		{
			send_runtime_timer(OFF);
		}
		else if (!runtimer_on && (accelerator == ACCEL_DN || vacuum_state != VACUUM_OFF || horn != OFF || headlights != OFF))
		{
			send_runtime_timer(ON);
		}

		return;
	}

	//Vacuum motor can be turned on immediately
	if ((vacuum == VAC_ON_ECO || vacuum == VAC_ON_NORMAL) && !tank_full)
	{
		vacuum_motor_set(vacuum);

		if (!reverse && !squeegee_down)
		{
			SQ_DN;
			squeegee_timer = ON;
			squeegee_down = TRUE;
		}
		vacuum_off_timer = OFF;
		vacuum_state = VACUUM_RUNNING;
		update_vacuum_state();
	}

	//Turning off vacuum motor will start a 15 sec timer before turning it off
	else if (vacuum_off_timer == OFF && vacuum_state == VACUUM_RUNNING)
	{
		vacuum_state = VACUUM_RUNNING_OFF;
		update_vacuum_state();
		vacuum_off_timer = ON;
		if (squeegee_down)
		{
			SQ_UP;
			squeegee_timer = ON;
			squeegee_down = FALSE;
		}
	}

	//If the accelerator is already down, check for any changes to brush, water pump, or det pump
	if (accelerator == ACCEL_DN)
	{
		//Set brush motor
		brush_motor_set(brush);

		if (presweep)
		{
			//Record and set pre sweep
			if (presweep_state == OFF)
			{
				/*log_enqueue(PRE_SWP_ON_EVENT);*/
				presweep_state = ON;
			}
			pre_swp_set(ON);
		}

		//If brush is off turn off pumps, otherwise update the pumps
		if (!brush)
		{
			water_pump_set(OFF, water_pump_dc);
			Solenoid_set(OFF);
			detergent_pump_set(OFF, detergent_pump_dc);

			brush_state = BRUSH_OFF;
			update_brush_state();
		}
		else
		{
			update_pumps();
			if (!water_pump)
			{
				Solenoid_set(solenoid_valve);
			}
			brush_state = BRUSH_RUNNING;
			update_brush_state();
		}
	}

	//Check if runtimer should be off or on
	if (runtimer_on && accelerator == ACCEL_UP && vacuum_state == VACUUM_OFF && horn == OFF && headlights == OFF)
	{
		send_runtime_timer(OFF);
	}
	else if (!runtimer_on && (accelerator == ACCEL_DN || vacuum_state != VACUUM_OFF || horn != OFF || headlights != OFF))
	{
		send_runtime_timer(ON);
	}

}

//Send the brush state
void update_brush_state(void)
{
	uint8_t message[1];
	message[0] = brush_state;

	transmit_CAN(BRUSH_STATE, 0, 1, message);
}

//Send the vacuum state
void update_vacuum_state(void)
{
	uint8_t message[1];
	message[0] = vacuum_state;

	transmit_CAN(VACUUM_STATE, 0, 1, message);
}

//Send ACK
void send_CAN_ACK(void)
{
	uint8_t message[1];
	message[0] = A2_CAN;
	transmit_CAN(ACK_CAN, 0, 1, message);
}

//Send serial number to A1
void send_serial_num(void)
{
	uint8_t i = 0;
	uint8_t message[8];
	_delay_ms(50);
	message[i++] = 1;
	message[i++] = A2_serial_num[0];
	message[i++] = A2_serial_num[1];
	message[i++] = A2_serial_num[2];
	message[i++] = A2_serial_num[3];
	message[i++] = A2_serial_num[4];
	message[i++] = A2_serial_num[5];
	_delay_ms(20);
	transmit_CAN(A2_SERIAL_NUM_CAN, 0, 7, message);
	_delay_ms(20);

	i = 0;
	message[i++] = 2;
	message[i++] = A2_serial_num[6];
	message[i++] = A2_serial_num[7];
	message[i++] = A2_serial_num[8];
	message[i++] = A2_serial_num[9];
	message[i++] = A2_serial_num[10];
	message[i++] = A2_serial_num[11];
	_delay_ms(20);
	transmit_CAN(A2_SERIAL_NUM_CAN, 0, 7, message);
	_delay_ms(20);

	i = 0;
	message[i++] = 3;
	message[i++] = A2_serial_num[12];
	message[i++] = A2_serial_num[13];
	_delay_ms(20);
	transmit_CAN(A2_SERIAL_NUM_CAN, 0, 3, message);
	_delay_ms(20);
}

//Send reset command to other boards
void send_reset_CAN(void)
{
	uint8_t message[1];
	transmit_CAN(RESET_DEVICE_CAN_A2, 0, 1, message);
	_delay_ms(10);
}

void send_CAN_buffer_overflow(void)
{
	uint8_t message[1];
	message[0] = CAN_BUFFER_OVERFLOW;
	transmit_CAN(ALARM_CAN, 0, 1, message);
}

void send_SD_buffer_overflow(void)
{
	uint8_t message[1];
	message[0] = SD_BUFFER_OVERFLOW;
	transmit_CAN(ALARM_CAN, 0, 1, message);
}

//If the SD cards fails early, it will be considered missing
void send_SD_card_fail(void)
{
	uint8_t message[1];
	message[0] = SD_CARD_FAIL;
	transmit_CAN(ALARM_CAN, 0, 1, message);
}

//Send the RB800 statistics to the A1
void send_statistics(void)
{
	uint8_t message[8];

	message[0] = BRUSH_HOURS;
	message[1] = brush_hours >> 8;
	message[2] = brush_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = VACUUM_HOURS;
	message[1] = vacuum_hours >> 8;
	message[2] = vacuum_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = PRESWEEP_HOURS;
	message[1] = presweep_hours >> 8;
	message[2] = presweep_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = ECO_HOURS;
	message[1] = eco_hours >> 8;
	message[2] = eco_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = WATER_PUMP_HOURS;
	message[1] = water_pump_hours >> 8;
	message[2] = water_pump_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = DETERGENT_PUMP_HOURS;
	message[1] = detergent_pump_hours >> 8;
	message[2] = detergent_pump_hours;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = BRUSH1_AVERAGE_CURRENT;
	message[1] = brush1_average_current >> 8;
	message[2] = brush1_average_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = BRUSH2_AVERAGE_CURRENT;
	message[1] = brush2_average_current >> 8;
	message[2] = brush2_average_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = VACUUM_AVERAGE_CURRENT;
	message[1] = vacuum_average_current >> 8;
	message[2] = vacuum_average_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = PRESWEEP_AVERAGE_CURRENT;
	message[1] = presweep_average_current >> 8;
	message[2] = presweep_average_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = BRUSH1_MAX_CURRENT;
	message[1] = max_brush1_overall_current >> 8;
	message[2] = max_brush1_overall_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = BRUSH2_MAX_CURRENT;
	message[1] = max_brush2_overall_current >> 8;
	message[2] = max_brush2_overall_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = VACUUM_MAX_CURRENT;
	message[1] = max_vacuum_overall_current >> 8;
	message[2] = max_vacuum_overall_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);

	message[0] = PRESWEEP_MAX_CURRENT;
	message[1] = max_presweep_overall_current >> 8;
	message[2] = max_presweep_overall_current;
	transmit_CAN(STATISTICS_CAN, 0, 3, message);
}
//////testing purpose to check reset flag/////////////////////reset issue

void send_reset_flag_counts(void)
{
	uint8_t message[8];

	message[0] = PORF_cnt;
	message[1] = EXTRF_cnt;
	message[2] = BORF_cnt;
	message[3] = WDRF_cnt;
	message[4] = PDIRF_cnt;
	message[5] = SRF_cnt;
	message[6] = SDRF_cnt;
	_delay_ms(20);
	transmit_CAN(RESET_FLAG_COUNT_A2, 0, 7, message);
	_delay_ms(20);
}
///////////////////////////////////////////////////////////////////////////