/*
 * circ_buff.h
 */

#ifndef CIRC_BUFF_H_
#define CIRC_BUFF_H_

#include "init.h"

/*
 * GOVERFLOW = 0 => Standard Circular Buffer Mode, can't be overwritten (losing data not allowed)
 * GOVERFLOW = 1 => OverFlow mode, the buffer can't be full, we overwrite the oldest data (losing data allowed)
 */

#define	GOVERFLOW	1

#define BUFFER_SIZE		2048

/* 1 buffer = 4kByte (1 place = 2 Byte = 1 Word) =>
 * 4 buffer = 16kByte 	=>
 * But we had only 16000 Byte so we have 4*24 	=>
 * We need one word to detect overflow 	=> 	Still Safe
 */

typedef struct c_buff_t
{
	uint16_t buffer[BUFFER_SIZE];
	uint16_t head;
	uint16_t tail;
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
