/*
* Created: 4/4/2016 11:25:16 AM
*  Author: Anurag Vaddi Reddy
*/

#ifndef CAN_QUEUE_H_
#define CAN_QUEUE_H_

#include "RB_800.h"

//IVA002: Added CAN Tx Queue, standardized function names
#define RX_QUEUE_SIZE	100
#define RX_BLOCK_SIZE	8

#define TX_QUEUE_SIZE	10
#define TX_BLOCK_SIZE	(1+1+1+8) //ID,RTR,LEN,Data


void CQ_initialize(void);

uint8_t CQRx_queueIsEmpty(void);
void CQRx_dequeue(void);
void CQRx_enqueue(uint8_t* data, uint16_t settings, uint8_t data_length);
uint8_t CQTx_queueIsEmpty(void);
void CQTx_dequeue(void);
void CQTx_enqueue(uint8_t* data);

#endif /* CAN_QUEUE_H_ */