/*
 * circ_buff.h
 */

#ifndef CIRC_BUFF_H_
#define CIRC_BUFF_H_

#define BUFFER_SIZE		((unsigned char)0x4096U)	//	1 buffer = 4kByte	=>	4 buffer = 16kByte
#define REAL_SIZE		BUFFER_SIZE+1				//	We have one byte to detect overflow

#include "init.h"

typedef struct c_buff_t
{
	uint16_t buffer[REAL_SIZE];
	uint8_t head;
	uint8_t tail;
}cBuff;

typedef enum
{
	cBuff_FULL	= -1,
	cBuff_OK	= 0,
	cBuff_EMPTY	= 1
}cBuff_State;

void init_cBuff(cBuff *buff_c);
cBuff_State push_cBuff(cBuff *buff_c, uint16_t data);
cBuff_State pop_cBuff(cBuff *buff_c, uint16_t *data);
cBuff_State flush_cBuff(cBuff *buff_c);

#endif /* CIRC_BUFF_H_ */
