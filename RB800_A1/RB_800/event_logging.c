/*
 * event_logging.c
 *
 * Created: 7/13/2016 11:09:44 AM
 *  Author: Anurag Vaddi Reddy
 */


#include "event_logging.h"

 //Send event that needs to be logged to A2 over CAN
void log_event(uint8_t event_code)
{
	uint8_t message[1];
	message[0] = event_code;
	transmit_CAN(EVENT_LOG_CAN, 0, 1, message);
	_delay_ms(10);
}
//IVA003: Removed stale code
//Send event plus its data that need to be logged to A2 over CAN
//void log_event_data(uint8_t event_code, uint8_t data[], uint8_t num_bytes)
//{
//#ifndef __SIM__
//	uint8_t message[1 + num_bytes];
//#else
//	uint8_t message[1000];
//#endif
//
//	message[0] = event_code;
//	for (int i = 0; i < num_bytes; i++)
//	{
//		message[i + 1] = data[i];
//	}
//	transmit_CAN(EVENT_LOG_CAN, 0, 1 + num_bytes, message);
//	_delay_ms(10);
//}