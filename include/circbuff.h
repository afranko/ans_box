/*
 * circbuff.h - Circular buffers with the size of 1 byte and 2 bytes
 */

#ifndef CIRCBUFF_H_
#define CIRCBUFF_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Structs and enums */

typedef enum
{
	BUFF_OK,
	BUFF_EMPTY
}BUFF_State;

typedef struct circ_buff_adc_t
{
	volatile uint16_t head;
	volatile uint16_t tail;
	uint16_t size;
	uint16_t buffer[2048];
}cBuff;

typedef struct circ_buff_gsm_t
{
	volatile uint16_t head;
	volatile uint16_t tail;
	uint16_t size;
	uint8_t buffer[];
}gBuff;

/* Functions */

cBuff* init_cBuff(uint16_t size);
BUFF_State pop_cBuff(cBuff *buffer, uint16_t *data);
BUFF_State flush_cBuff(cBuff *buffer);
BUFF_State available_cBuff(cBuff *buffer);
uint16_t check_last_cBuff(cBuff *buffer);

gBuff* init_gBuff(uint16_t size);
BUFF_State pop_gBuff(gBuff *buffer, uint8_t *data);
BUFF_State flush_gBuff(gBuff *buffer);
BUFF_State available_gBuff(gBuff *buffer);
uint8_t check_last_gBuff(gBuff *buffer);

/**
 * Adds an element to the buffer
 * @param buffer Pointer to the buffer
 * @param data The data that will be added into the buffer
 * @return The state of buffer
 */
inline BUFF_State push_cBuff(cBuff *buffer, uint16_t data)
{
	/* Write to the buffer */
	buffer->buffer[buffer->head] = data;
	buffer->head = (uint16_t)(buffer->head+1U) % buffer->size;

	/* Check if we had enough place */
	if(buffer->head == buffer->tail)
		buffer->tail = (buffer->tail + 1U) % buffer->size;

	return BUFF_OK;
}

/**
 * Adds an element to the buffer
 * @param buffer Pointer to the buffer
 * @param data The data that will be added into the buffer
 * @return The state of buffer
 */
inline BUFF_State push_gBuff(gBuff *buffer, uint8_t data)
{
	/* Write to the buffer */
	buffer->buffer[buffer->head] = data;
	buffer->head = (uint16_t)(buffer->head+1U) % buffer->size;

	/* Check if we had enough place */
	if(buffer->head == buffer->tail)
		buffer->tail = (buffer->tail + 1U) % buffer->size;

	return BUFF_OK;
}

#endif /* CIRCBUFF_H_ */
