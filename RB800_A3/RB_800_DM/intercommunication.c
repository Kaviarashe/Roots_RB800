/*
 * intercommunication.c
 *
 * Created: 11/20/2015 9:52:35 AM
 *  Author: Anurag Vaddi Reddy
 */

#include "intercommunication.h"
//#include "CANopen.h"

bool pedal_ack_pending;
bool pedal_upd_pending;
uint8_t pedal_state_q;
//IVA003: Send new state only after previous change is handled by reading the ack
void filter_pedal_state(bool chkAck, uint8_t st)
{
	if (chkAck && pedal_ack_pending)
	{
		pedal_state_q = st;
		pedal_upd_pending = true;
		return;
	}

	uint8_t message[2];
	message[0] = PEDAL_STATE;
	message[1] = st;
	pedal_ack_pending = true;
	transmit_CAN(ACCEL_PDL_CAN, 0, 2, message);
}

void processCAN(uint16_t identifier, uint8_t* data)
{
	uint8_t message[20];
	uint16_t total;
	uint8_t i;

	//Process the communication depending on the type of communication
	switch (identifier)
	{
		//Update status of drive motor using all settings
	case ALL_STATUS:
		if (manual_mode)
			break;
		if ((data[5] & 0x0F) == ON)
		{
			if (direction == FORWARD)
			{
				fr_transition = !fr_transition;
			}
			else if (direction == NEUTRAL || direction == UNTESTED)
			{
				SET_MOTOR_REVERSE;
			}
			else if (direction == REVERSE && !(PORTB_OUT & (1 << REV_BUZZ)))
			{
				REVERSE_BUZZER_ON;
			}

			direction = REVERSE;

		}
		else if ((data[5] & 0x0F) == OFF)
		{
			if (direction == REVERSE)
			{
				fr_transition = !fr_transition;
			}
			else if (direction == NEUTRAL || direction == UNTESTED)
			{
				SET_MOTOR_FORWARD;
			}
			else if (direction == FORWARD && (PORTB_OUT & (1 << REV_BUZZ)))
			{
				REVERSE_BUZZER_OFF;
			}

			direction = FORWARD;
		}
		//Check motor speed
		if (speed != SLOW_MANUAL)
		{
			if ((data[6] & 0x0F) == SLOW && speed == FAST)
			{
				speed = SLOW;
				if (direction == FORWARD)
				{
					if (GET_MOTOR_SPEED() >= minimum_pwm)
					{
						if (slow_level_pwm_ratio > 0) //IVA001: fix for div/0
							pedal_level = (GET_MOTOR_SPEED() - minimum_pwm) / slow_level_pwm_ratio;
					}
					else
					{
						pedal_level = 0;
					}
				}
			}
			else if ((data[6] & 0x0F) == FAST && speed == SLOW)
			{
				speed = FAST;
				if (direction == FORWARD)
				{
					if (GET_MOTOR_SPEED() >= minimum_pwm)
					{
						if (fast_level_pwm_ratio > 0) //IVA001: fix for div/0
							pedal_level = (GET_MOTOR_SPEED() - minimum_pwm) / fast_level_pwm_ratio;
					}
					else
					{
						pedal_level = 0;
					}
				}
			}
		}
		break;
			

		//Send drive motor settings when A1 goes into settings menu
	/*case DM_SETTINGS_CAN:
		if (data[0] == REQUEST_DM_SETTINGS)
		{
			//Send all motor settings
			i = 0;
			message[i++] = DM_SETTINGS_1;
			message[i++] = acceleration;
			message[i++] = deceleration;
			message[i++] = brake_delay;
			message[i++] = reverse_speed;
			transmit_CAN(DM_SETTINGS_CAN, 0, 5, message);
			_delay_ms(10);
			i = 0;
			message[i++] = DM_SETTINGS_2;
			message[i++] = slow_speed;
			message[i++] = fast_speed;
			message[i++] = rated_current;
			message[i++] = max_current;
			message[i++] = full_overload_time;
			transmit_CAN(DM_SETTINGS_CAN, 0, 6, message);
			_delay_ms(10);
		}
		else if (data[0] == UPDATE_DM_SETTINGS)
		{
			i = 2;
			if (data[1] == DM_SETTINGS_1)
			{
				acceleration = data[i++];
				deceleration = data[i++];
				brake_delay = data[i++];
				reverse_speed = data[i++];
			}
			else if (data[1] == DM_SETTINGS_2)
			{
				slow_speed = data[i++];
				fast_speed = data[i++];
				rated_current = data[i++];
				max_current = data[i++];
				full_overload_time = data[i++];
			}
			update_motor_settings();
		}
		break;*/

		//Send pot info when A1 asks in the settings menu
	case POT_CALIBRATION_CAN:
		emergency_stop_motor = PERMANENT;
		pedal_state = OFF;
		if (data[0] == FALSE || data[0] == POT_CALIBRATING_CANCELLED)
		{
			i = 0;
			message[i++] = (pot_low_deadband & 0xFF00) >> 8;
			message[i++] = pot_low_deadband & 0x00FF;
			message[i++] = (pot_high_deadband & 0xFF00) >> 8;
			message[i++] = pot_high_deadband & 0x00FF;
			transmit_CAN(POT_CALIBRATION_CAN, 0, 4, message);
			_delay_ms(10);
		}
		else if (data[0] == POT_CALIBRATING_LOW)
		{
			while (pedal_state == OFF)YIELD();
			total = 0;

			for (int i = 0; i < NUM_SAMPLES; i++)
			{
				//Start channel 1
				ADCA_CTRLA |= (1 << 3);
				//Wait for conversion to end 
				while (ADCA_CTRLA & (1 << 3))YIELD_ADC("11_POTCALLOW", ADCA_CTRLA); //IVA001: Channel mismatch corrected
				//Add result to total
				_delay_ms(1);
				total += ADCA_CH1RES;
			}
			//Get average of total
			total /= NUM_SAMPLES;
			if (total > ADC_NEG_ADJ)
			{
				total -= ADC_NEG_ADJ;
				temp1 = (uint16_t)((float)total * MAX_ADC_READING_COUNT / MAX_ADC_ADJ_COUNT) * ((float)V_REF / MAX_ADC_READING_COUNT);
			}
			else
			{
				temp1 = 0;
			}

			i = 0;
			message[i++] = (temp1 & 0xFF00) >> 8;
			message[i++] = temp1 & 0x00FF;
			transmit_CAN(POT_CALIBRATION_CAN, 0, 2, message);
			_delay_ms(10);
		}
		else if (data[0] == POT_CALIBRATING_HIGH)
		{
			total = 0;

			for (int i = 0; i < NUM_SAMPLES; i++)
			{
				//Start channel 1
				ADCA_CTRLA |= (1 << 3);
				//Wait for conversion to end
				while (ADCA_CTRLA & (1 << 3))YIELD_ADC("11_POTCALHI", ADCA_CTRLA); //IVA001: Channel mismatch corrected
				//Add result to total
				_delay_ms(1);
				total += ADCA_CH1RES;

			}
			//Get average of total
			total /= NUM_SAMPLES;
			if (total > ADC_NEG_ADJ)
			{
				total -= ADC_NEG_ADJ;
				temp2 = (uint16_t)((float)total * MAX_ADC_READING_COUNT / MAX_ADC_ADJ_COUNT) * ((float)V_REF / MAX_ADC_READING_COUNT);
			}
			else
			{
				temp2 = 0;
			}

			i = 0;
			message[i++] = (temp2 & 0xFF00) >> 8;
			message[i++] = temp2 & 0x00FF;
			transmit_CAN(POT_CALIBRATION_CAN, 0, 2, message);
			_delay_ms(10);
		}
		//Update the pot dead band
		else if (data[0] == POT_CALIBRATING_COMPLETE)
		{
			pot_low_deadband = temp1;
			pot_high_deadband = temp2;
			update_motor_settings();
		}
		break;

		//Measure the Pot value and send to A1 board 
	case REQUEST_POT_VALUE_CAN:
		total = 0;

		for (int i = 0; i < NUM_SAMPLES; i++)
		{
			//Start channel 1
			ADCA_CTRLA |= (1 << 3);
			//Wait for conversion to end
			while (ADCA_CTRLA & (1 << 3))YIELD_ADC("11_POTVAL", ADCA_CTRLA); //IVA001: Channel mismatch corrected
			//Add result to total
			_delay_ms(1);
			total += ADCA_CH1RES;
		}
		//Get average of total
		total /= NUM_SAMPLES;
		if (total > ADC_NEG_ADJ)
		{
			total -= ADC_NEG_ADJ;
			temp2 = (uint16_t)((float)total * MAX_ADC_READING_COUNT / MAX_ADC_ADJ_COUNT) * ((float)V_REF / MAX_ADC_READING_COUNT);
		}
		else
		{
			temp2 = 0;
		}

		i = 0;
		message[i++] = (temp2 & 0xFF00) >> 8;
		message[i++] = temp2 & 0x00FF;
		_delay_ms(10);
		transmit_CAN(POT_VALUE_CAN, 0, 2, message);
		_delay_ms(10);
		break;

		//Reset all drive motors to default values
	case RESET_DEFAULT_CAN:
		set_to_default();
		break;

		//change acceleration value
	case ACCELERATION_CAN:
		acceleration = data[0];
		update_accel_settings();
		break;

		//change deceleration value
	case DECELERATION_CAN:
		deceleration = data[0];
		update_decel_settings();
		break;
		//change break delay value
	case BREAK_DELAY_CAN:
		brake_delay = data[0];
		update_break_delay_settings();
		break;
		//change reverse speed value
	case REVERSE_SPEED_CAN:
		reverse_speed = data[0];
		update_reverse_speed();
		break;
		//change fast speed value
	case FAST_SPEED_CAN:
		fast_speed = data[0];
		update_fast_speed();
		break;
		//change slow speed value
	case SLOW_SPEED_CAN:
		slow_speed = data[0];
		update_slow_speed();
		break;
		//change current rating value
	case CURRENT_RATING_CAN:
		rated_current = data[0];
		update_current_rating();
		break;
		//change maximum current value
	case MAX_CURRENT_CAN:
		max_current = data[0];
		update_max_current();
		break;
		//change full over load time value
	case OVER_LOAD_TIME_CAN:
		full_overload_time = data[0];
		update_over_current_time();
		break;


		//React to alarms
	case ALARM_CAN:
		if (data[0] == OVERVOLTAGE)
		{
			emergency_stop_motor = PERMANENT;
			E_BRAKE_ON;
		}
		break;

		//Send an ACK if an ACK is sent from A1
	case ACK_CAN:
		can_functioning = TRUE;
		if (data[0] == A1_CAN)
		{
			//DBG("A3 ******************** CAN ACK");
			send_CAN_ACK();
			if (motor_parameter_error)
			{
				send_motor_parameter_error();
			}

		}
		break;

		//Update A3 serial number
	case A3_SERIAL_NUM_CAN:
		if (data[0] == 1)
		{
			A3_serial_num[0] = data[1];
			A3_serial_num[1] = data[2];
			A3_serial_num[2] = data[3];
			A3_serial_num[3] = data[4];
			A3_serial_num[4] = data[5];
			A3_serial_num[5] = data[6];
		}
		else if (data[0] == 2)
		{
			A3_serial_num[6] = data[1];
			A3_serial_num[7] = data[2];
			A3_serial_num[8] = data[3];
			A3_serial_num[9] = data[4];
			A3_serial_num[10] = data[5];
			A3_serial_num[11] = data[6];
		}
		else if (data[0] == 3)
		{
			A3_serial_num[12] = data[1];
			A3_serial_num[13] = data[2];
			eeprom_update_block((const void*)A3_serial_num, (void *)EEPxx_A3_SERIAL_NUM, A3_SERIAL_NUM_LEN);
		}
		else
		{
			send_serial_num();
		}
		break;

		//Reset board if another board resets
	case RESET_DEVICE_CAN_A1:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			REBOOT();
			break;
		}
		//Reset board if another board resets
	case RESET_DEVICE_CAN_A2:
		if (reset_timer >= 10)
		{
			wdt_enable(WDTO_15MS);
			REBOOT();
			break;
		}
	case ACCEL_PDL_CAN: //IVA003: Added ack for Pedal state handling
		if (data[0] == 0xAC)
		{
			if (pedal_upd_pending)
			{
				pedal_upd_pending = false;
				filter_pedal_state(false, pedal_state_q);
			}
			else
			{
				pedal_ack_pending = false;
			}
		}
	default:
		break;
	}
}


void update_pedal_state(void)
{
	//IVA003: Added filter, send new state only after previous change is handled
	filter_pedal_state(true, pedal_state);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}

void update_pedal_level(void)
{
	//Send the brush state
	uint8_t message[3];
	message[0] = PEDAL_LEVEL;
	message[1] = (curr_pedal_level >> 8) & 0xFF;
	message[2] = curr_pedal_level & 0xFF;
	transmit_CAN(ACCEL_PDL_CAN, 0, 3, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}

/*void send_motor_settings(void)
{
	//Send all motor settings
	uint8_t message[8];
	uint8_t i = 0;
	message[i++] = DM_SETTINGS_1;
	message[i++] = acceleration;
	message[i++] = deceleration;
	message[i++] = brake_delay;
	message[i++] = reverse_speed;
	transmit_CAN(DM_SETTINGS_CAN, 0, 5, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
	i = 0;
	message[i++] = DM_SETTINGS_2;
	message[i++] = slow_speed;
	message[i++] = fast_speed;
	message[i++] = rated_current;
	message[i++] = max_current;
	message[i++] = full_overload_time;
	transmit_CAN(DM_SETTINGS_CAN, 0, 6, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}*/

void send_back_safety_error(uint8_t state)
{
	//Send the back safety error
	uint8_t message[1];
	message[0] = state;
	transmit_CAN(BACK_SAFETY_CAN, 0, 1, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}

void send_top_error(void)
{
	//Send the top error
	uint8_t message[1];
	message[0] = A3_CAN;
	transmit_CAN(TOP_CAN, 0, 1, message);
	//_delay_ms(500);//IVA002: Removed unecessary delay
}

void send_pedal_error(uint8_t error)
{
	//Send the pedal error
	uint8_t message[1];
	message[0] = error;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}

void send_key_error(uint8_t error)
{
	//Send the pedal error
	uint8_t message[1];
	message[0] = error;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	//_delay_ms(10);//IVA002: Removed unecessary delay
}

void send_brake_seat_error(uint8_t error)
{
	//Send the pedal error
	uint8_t message[1];
	message[0] = error;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	//_delay_ms(10); //IVA002: Removed unecessary delay
}

//Send ACK
void send_CAN_ACK(void)
{
	uint8_t message[1];
	message[0] = A3_CAN;
	transmit_CAN(ACK_CAN, 0, 1, message);
	_delay_ms(10);
}

//Send serial number to A1
void send_serial_num(void)
{
	uint8_t i = 0;
	uint8_t message[8];
	_delay_ms(50);
	message[i++] = 1;
	message[i++] = A3_serial_num[0];
	message[i++] = A3_serial_num[1];
	message[i++] = A3_serial_num[2];
	message[i++] = A3_serial_num[3];
	message[i++] = A3_serial_num[4];
	message[i++] = A3_serial_num[5];
	_delay_ms(20);
	transmit_CAN(A3_SERIAL_NUM_CAN, 0, 7, message);
	_delay_ms(20);

	i = 0;
	message[i++] = 2;
	message[i++] = A3_serial_num[6];
	message[i++] = A3_serial_num[7];
	message[i++] = A3_serial_num[8];
	message[i++] = A3_serial_num[9];
	message[i++] = A3_serial_num[10];
	message[i++] = A3_serial_num[11];
	_delay_ms(20);
	transmit_CAN(A3_SERIAL_NUM_CAN, 0, 7, message);
	_delay_ms(20);
	i = 0;
	message[i++] = 3;
	message[i++] = A3_serial_num[12];
	message[i++] = A3_serial_num[13];
	_delay_ms(20);
	transmit_CAN(A3_SERIAL_NUM_CAN, 0, 3, message);
	_delay_ms(20);
}

//Send overcurrent error
void send_overcurrent_error(void)
{
	//Send the overcurrent error
	uint8_t message[1];
	message[0] = A3_OVERCURRENT;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(10);
}

//Send reset command on startup
void send_reset_CAN(void)
{
	uint8_t message[1];
	transmit_CAN(RESET_DEVICE_CAN_A3, 0, 1, message);
	_delay_ms(10);
}

void send_motor_parameter_error(void)
{
	//Send the overcurrent error
	uint8_t message[1];
	message[0] = MOTOR_PARAMETER;
	transmit_CAN(ALARM_CAN, 0, 1, message);
	_delay_ms(10);
}

void send_CAN_buffer_overflow(void)
{
	uint8_t message[1];
	transmit_CAN(CAN_BUFFER_OVERFLOW, 0, 1, message);
	_delay_ms(10);
}

void send_SD_buffer_overflow(void)
{
	uint8_t message[1];
	transmit_CAN(SD_BUFFER_OVERFLOW, 0, 1, message);
	_delay_ms(10);
}

void send_drive_motor_current(void)
{
	uint8_t message[1];
	message[0] = (uint8_t)moving_average_current;
	transmit_CAN(DRIVE_CURRENT_CAN, 0, 1, message);
	_delay_ms(10);
}

void send_max_drive_motor_current(void)
{
	uint8_t message[1];
	message[0] = (uint8_t)max_moving_average_current;
	transmit_CAN(MAX_DRIVE_CURRENT_CAN, 0, 1, message);
	_delay_ms(10);
}
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
	//_delay_ms(20);
	transmit_CAN(RESET_FLAG_COUNT_A3, 0, 7, message);
	_delay_ms(20);
}