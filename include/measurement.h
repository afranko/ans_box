/*
 * measurement.h
 *
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "circ_buff.h"

typedef struct measurement_flag_block_t
{
	/* Negative offset */
	uint16_t noffset0;
	uint16_t noffset1;
	uint16_t noffset2;
	uint16_t noffset3;

	bool offMeas;
	uint16_t ocounter;

	/* Measurement */
	uint16_t measc0;
	uint16_t measc1;
	uint16_t measc2;
	uint16_t measc3;

	bool mflag;

	/* Positive Offset */

	uint16_t pofc0;
	uint16_t pofc1;
	uint16_t pofc2;
	uint16_t pofc3;


}meas_flag_block; //TODO

extern cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;
extern bool S_MEAS_FLAG, E_MEAS_FLAG, CLEAR_FLAG;

uint16_t read_last(cBuff *buff_c);
void meas_datamove(void);
void dataparse(uint16_t data, char *itoabuff);

#endif /* MEASUREMENT_H_ */
