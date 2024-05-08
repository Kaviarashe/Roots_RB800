/*
 * can_queue.c
 *
 * Created: 4/4/2016 11:25:06 AM
 *  Author: Anurag Vaddi Reddy
 */


#include "can_queue.h"

#ifndef __SIM__
 //IVA001: CAN Queue lock for avoiding data corruption
#define CQ_MX_LOCK()	CAN_MX_LOCK()
#define CQ_MX_UNLOCK()	CAN_MX_UNLOCK()
#endif

volatile int8_t CQRx_front;
volatile int8_t CQRx_rear;
volatile uint8_t CQRx_size;
uint8_t CQRx_data_queue[RX_QUEUE_SIZE][RX_BLOCK_SIZE];
uint16_t CQRx_settings_queue[RX_QUEUE_SIZE];

volatile int8_t CQTx_front;
volatile int8_t CQTx_rear;
volatile uint8_t CQTx_size;
uint8_t CQTx_data_queue[TX_QUEUE_SIZE][TX_BLOCK_SIZE];
void CQ_initialize(void)
{
	//DBG2("InitQ %d, 0x%X", CQ_Size, &CQ_Size);
	CQRx_size = 0;
	CQRx_front = 0;
	CQRx_rear = -1;
	CQTx_size = 0;
	CQTx_front = 0;
	CQTx_rear = -1;
}

//Checks if there any unprocessed communication left in the queue
uint8_t CQRx_queueIsEmpty(void)
{
	//DBG2("ChQ %d, 0x%X", CQ_Size, &CQ_Size);
	CQ_MX_LOCK();
	uint16_t tmp = CQRx_size;
	CQ_MX_UNLOCK();

	if (tmp == 0)
		return TRUE;
	return FALSE;
}

//Add unprocessed communication to queue
void CQRx_enqueue(uint8_t* data, uint16_t setting, uint8_t data_length)
{
	DBG1("A1: %d", CQRx_size);
	CQ_MX_LOCK();
	//Get position of new data
	CQRx_rear = (CQRx_rear + 1) % RX_QUEUE_SIZE;
	CQRx_size++;
	CQ_MX_UNLOCK();
	//DBG2("EnQ %d, 0x%X", CQ_Size, &CQ_Size);
	//Print error message to screen if CAn buffer overflow
	//Buffer overflow should not occur in the field, increase size if buffer ever overflows
	if (CQRx_size >= RX_QUEUE_SIZE)
	{
		send_CAN_buffer_overflow();
	}

	uint8_t i;
	//Copy over data
	for (i = 0; i < RX_BLOCK_SIZE; i++)
	{
		if (i < data_length)
		{
			CQRx_data_queue[CQRx_rear][i] = data[i];
		}
		else
		{
			CQRx_data_queue[CQRx_rear][i] = 0;
		}
	}

	CQRx_settings_queue[CQRx_rear] = setting;
}

//Get the communication data at the front of the queue
void CQRx_dequeue(void)
{
	if (CQRx_size == 0)
		return;

	uint16_t  id;
	uint8_t*  data;

	//DBG2("DeQ %d, 0x%X", CQ_Size, &CQ_Size);
	CQ_MX_LOCK();
	id = CQRx_settings_queue[CQRx_front];
	data = CQRx_data_queue[CQRx_front];
	//Update new front
	CQRx_size--;
	CQRx_front = (CQRx_front + 1) % RX_QUEUE_SIZE;
	CQ_MX_UNLOCK();
	processCAN(id & 0xff, data);
}

//IVA002: Added CAN Tx Queue
//CAN TX Queue
uint8_t CQTx_queueIsEmpty(void)
{
	//DBG2("ChQ %d, 0x%X", CQ_Size, &CQ_Size);
	CQ_MX_LOCK();
	uint16_t tmp = CQTx_size;
	CQ_MX_UNLOCK();

	if (tmp == 0)
		return TRUE;
	return FALSE;
}

//Add unprocessed communication to queue
void CQTx_enqueue(uint8_t* data)
{
	DBG1("A2:CQTx: %d", CQTx_size);

	CQ_MX_LOCK();
	//Get position of new data
	CQTx_rear = (CQTx_rear + 1) % TX_QUEUE_SIZE;
	CQTx_size++;
	CQ_MX_UNLOCK();

	//DBG2("EnQ %d, 0x%X", CQ_Size, &CQ_Size);
	//Print error message to screen if CAn buffer overflow
	//Buffer overflow should not occur in the field, increase size if buffer ever overflows
	if (CQTx_size >= TX_QUEUE_SIZE)
	{
	}

	uint8_t *p = CQTx_data_queue[CQTx_rear];
	//Copy over data
	memcpy(&p[0], &data[0], TX_BLOCK_SIZE);
}

//Get the communication data at the front of the queue
void CQTx_dequeue(void)
{
	if (CQTx_size == 0)
		return;

	uint8_t*  data;

	data = CQTx_data_queue[CQTx_front];
	//Update new front
	CQTx_size--;
	CQTx_front = (CQTx_front + 1) % TX_QUEUE_SIZE;
	transmit_CAN_((uint16_t)data[0], data[1], data[2], &data[3]);
}
