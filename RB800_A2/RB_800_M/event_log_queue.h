/*
 * event_log_queue.h
 *
 * Created: 7/20/2016 2:43:41 PM
 *  Author: Anurag Vaddi Reddy
 */


#ifndef EVENT_LOG_QUEUE_H_
#define EVENT_LOG_QUEUE_H_

#include "RB_800.h"

#define MAX_LOG_QUEUE_SIZE	50
#define MAX_LOG_DATA_SIZE	10

volatile uint8_t log_queue[MAX_LOG_QUEUE_SIZE][MAX_LOG_DATA_SIZE];
volatile int8_t front_log;
volatile int8_t rear_log;
volatile uint8_t size_log;

void initialize_event_log_queue(void);
uint8_t log_queue_is_empty(void);
void log_enqueue_data(uint8_t event_code, uint8_t num_data_bytes, uint8_t* data);
void log_enqueue(uint8_t event_code);
void log_dequeue(void);

#endif /* EVENT_LOG_QUEUE_H_ */