/*
 * ModbusRTU_RingBuffer.h
 *
 *  Created on: 8 sie 2022
 *      Author: midec
 */

#ifndef MODBUS_MODBUSRTU_RINGBUFFER_H_
#define MODBUS_MODBUSRTU_RINGBUFFER_H_

#include "ModbusRTU_Defines.h"
volatile ring_buffer_t data_ring_buffer;

static uint8_t modbusRTURingBuffer_GetChar(ring_buffer_t *ring_buffer, char *c)
{
	if ((ring_buffer) && (c))
	{
		if (ring_buffer->count > 0)
		{
			*c = ring_buffer->data[ring_buffer->tail++];
			ring_buffer->count--;
			if (ring_buffer->tail >= ring_buffer->size)
				ring_buffer->tail = 0;
			return 0;
		}
	}
	return 1;
}

static uint8_t modbusRTURingBuffer_PutChar(ring_buffer_t *ring_buffer, char c)
{
	if (ring_buffer)
	{
		if (ring_buffer->count < ring_buffer->size)
		{
			ring_buffer->data[ring_buffer->head++] = c;
			ring_buffer->count++;
			if (ring_buffer->head >= ring_buffer->size)
				ring_buffer->head = 0;
			return 0;
		}
	}
	return 1;
}

static uint8_t modbusRTURingBuffer_Clear(ring_buffer_t *ring_buffer)
{
	if (ring_buffer)
	{
		ring_buffer->head = 0;
		ring_buffer->tail = 0;
		ring_buffer->count = 0;

		return 0;
	}
	return 1;
}

static void modbusRTURingBuffer_GoBack(ring_buffer_t *ring_buffer, int whereToGo)
{
	if (ring_buffer && whereToGo >= 0)
	{
		int wentBackHowMany = (ring_buffer->tail - whereToGo + ring_buffer->size) % ring_buffer->size;
		ring_buffer->tail = whereToGo % ring_buffer->size;
		ring_buffer->count += wentBackHowMany;
	}
}

#endif /* MODBUS_MODBUSRTU_RINGBUFFER_H_ */
