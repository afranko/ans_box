/*
 * circbuff.c - Circular Buffers (1 byte and 2 bytes)
 */

#include "circbuff.h"

extern inline BUFF_State push_cBuff(cBuff *buffer, uint16_t data);
extern inline BUFF_State push_gBuff(gBuff *buffer, uint8_t data);

/* 16 bit - ADC - Buffer */

/**
 * Inits a buffer for ADC with the given size
 * @param buffer Pointer to the buffer
 * @param size Size of the buffer
 */
cBuff* init_cBuff(uint16_t size)
{
	cBuff *buffer = (cBuff*) malloc(sizeof(cBuff));
	buffer->head = 0;
	buffer->tail = 0;
	buffer->size = size;
	return buffer;
}

/**
 * Deletes an element from the buffer
 * @param buffer Pointer to the buffer
 * @param data The data that will be deleted from the buffer
 * @return The state of buffer
 */
BUFF_State pop_cBuff(cBuff *buffer, uint16_t *data)
{
	/* Check if we have data to read */
	if(buffer->tail == buffer->head)
		return BUFF_EMPTY;

	/* Read data from buffer */
	*data = buffer->buffer[buffer->tail];
	buffer-> tail = (uint16_t)(buffer->tail+1U) % buffer->size;
	return BUFF_OK;
}

/**
 * Flushes the entire buffer
 * @param buffer Pointer to the buffer
 * @return The state of buffer
 */
BUFF_State flush_cBuff(cBuff *buffer)
{
	buffer->head = 0;
	buffer->tail = 0;

	for(int i = 0; i < buffer->size; i++)
		buffer->buffer[i] = 0;

	return BUFF_OK;
}

/**
 * Checks if the buffer is empty
 * @param buffer Pointer to the buffer
 * @return The state of buffer
 */
BUFF_State available_cBuff(cBuff *buffer)
{
	if(buffer->head == buffer->tail)
		return BUFF_EMPTY;
	else
		return BUFF_OK;
}

/**
 * Checks the last written element in the buffer
 * @param buffer Pointer to the buffer
 * @return The last written element
 */
uint16_t check_last_cBuff(cBuff *buffer)
{
	uint16_t value = (buffer->head-1) % 2048U;
	return buffer->buffer[value];
}

/* 8 bit - GSM - Buffer */

/**
 * Init a buffer for GSM with the given size
 * @param buffer Pointer to the buffer
 * @param size Size of the buffer
 */
gBuff* init_gBuff(uint16_t size)
{
	gBuff *buffer = (gBuff*) malloc(sizeof(gBuff) + size);
	buffer->head = 0;
	buffer->tail = 0;
	buffer->size = size;
	return buffer;
}

/**
 * Deletes an element from the buffer
 * @param buffer Pointer to the buffer
 * @param data The data that will be deleted from the buffer
 * @return The state of buffer
 */
BUFF_State pop_gBuff(gBuff *buffer, uint8_t *data)
{
	/* Check if we have data to read */
	if(buffer->tail == buffer->head)
		return BUFF_EMPTY;

	/* Read data from buffer */
	*data = buffer->buffer[buffer->tail];

	buffer-> tail = (uint16_t)(buffer->tail+1U) % buffer->size;
	return BUFF_OK;
}

/**
 * Flushes the entire buffer
 * @param buffer Pointer to the buffer
 * @return The state of buffer
 */
BUFF_State flush_gBuff(gBuff *buffer)
{
	buffer->head = 0;
	buffer->tail = 0;

	for(int i = 0; i < buffer->size; i++)
		buffer->buffer[i] = 0;

	return BUFF_OK;
}

/**
 * Checks if the buffer is empty
 * @param buffer Pointer to the buffer
 * @return The state of buffer
 */
BUFF_State available_gBuff(gBuff *buffer)
{
	if(buffer->head == buffer->tail)
		return BUFF_EMPTY;
	else
		return BUFF_OK;
}

/**
 * Checks the last written element in the buffer
 * @param buffer Pointer to the buffer
 * @return The last written element
 */
uint8_t check_last_gBuff(gBuff *buffer)
{
	return buffer->buffer[(buffer->head-1 % 2048U)];
}
