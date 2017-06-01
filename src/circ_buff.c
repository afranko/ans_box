/*
 * circ_buff.c
 */

#include "circ_buff.h"

void init_cBuff(cBuff *buff_c)
{
	buff_c->head = 0;
	buff_c->tail = 0;
}

cBuff_State push_cBuff(cBuff *buff_c, uint16_t data)
{
	uint8_t next = (buff_c->head+1) % BUFFER_SIZE;

	/* Check if we had enough place */
	if(next == buff_c->tail)
	{
		return cBuff_FULL;
	}

	/* Write to the buffer */
	buff_c->buffer[buff_c->head] = data;
	buff_c->head = next;
	return cBuff_OK;
}

cBuff_State pop_cBuff(cBuff *buff_c, uint16_t *data)
{
	/* Check if we have data to read */
	if(buff_c->tail == buff_c->head)
	{
		return cBuff_EMPTY;
	}

	/* Read data from buffer */
	*data = buff_c->buffer[buff_c->tail];
	buff_c-> tail = (buff_c->tail+1) % BUFFER_SIZE;
	return cBuff_OK;
}

cBuff_State flush_cBuff(cBuff *buff_c)
{
	init_cBuff(buff_c);
	for(int i = 0; i < REAL_SIZE; i++)
	{
		buff_c->buffer[i] = 0;
	}
	return cBuff_OK;
}
