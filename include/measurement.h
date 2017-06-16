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

	bool mFlag;
	uint16_t measLength;

	/* Positive Offset */
	uint16_t pofc0;
	uint16_t pofc1;
	uint16_t pofc2;
	uint16_t pofc3;

	bool endMeas;

	bool S_MEAS_FLAG,		// Start of measurement flag
	E_MEAS_FLAG,			// End of measurement flag
	CLEAR_FLAG,				// Clear measurement flag
	MSG_FLAG;				// Send message flag

}meas_flag_block; //TODO

extern cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;

uint16_t read_last(cBuff *buff_c);
void meas_datamove(void);
void dataparse(uint16_t data, char *itoabuff);
void init_meas_flag_block(meas_flag_block *flagBlock);

#endif /* MEASUREMENT_H_ */
