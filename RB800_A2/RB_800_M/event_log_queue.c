/*
 * event_log_queue.c
 *
 * Created: 7/20/2016 2:43:15 PM
 *  Author: Anurag Vaddi Reddy
 */


#include "event_log_queue.h"

void initialize_event_log_queue(void)
{
	size_log = 0;
	front_log = 0;
	rear_log = -1;
}

//Check if there are any more events to log in the queue
uint8_t log_queue_is_empty(void)
{
	return size_log == 0;
}

//Add event with data into queue
void log_enqueue_data(uint8_t event_code, uint8_t num_data_bytes, uint8_t* data)
{
	///*Testing without log data*/
   //return;

	if (!sd_card_passed)
		return;

	//Update event position in the queue
	rear_log = (rear_log + 1) % MAX_LOG_QUEUE_SIZE;
	size_log++;

	//If the log queue overflow inform user through A1
	if (size_log >= MAX_LOG_QUEUE_SIZE)
	{
		send_SD_buffer_overflow();
	}

	//Copy over data to queue
	log_queue[rear_log][0] = event_code;
	log_queue[rear_log][1] = num_data_bytes;
	for (uint8_t j = 0; j < num_data_bytes; j++)
	{
		log_queue[rear_log][j + 2] = data[j];
	}
}

//Add event to queue
void log_enqueue(uint8_t event_code)
{
	///*Testing without log data*/
	//return;

	if (!sd_card_passed)
		return;

	rear_log = (rear_log + 1) % MAX_LOG_QUEUE_SIZE;
	size_log++;

	if (size_log >= MAX_LOG_QUEUE_SIZE)
	{
		send_SD_buffer_overflow();
	}

	log_queue[rear_log][0] = event_code;
	log_queue[rear_log][1] = 0;
}

//Remove front event from queue and write to SD card
void log_dequeue(void)
{
	if (size_log == 0 || event_logging == OFF)
		return;

	log_event((uint8_t*)log_queue[front_log]);

	//Update front event of queue
	size_log--;
	front_log = (front_log + 1) % MAX_LOG_QUEUE_SIZE;
}