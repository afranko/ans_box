/*
 * measurement.h
 *
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "circ_buff.h"

extern cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;
extern bool S_MEAS_FLAG, E_MEAS_FLAG, CLEAR_FLAG;

uint16_t read_last(cBuff *buff_c);
void meas_datamove(void);
void dataparse(uint16_t data, char *itoabuff);

#endif /* MEASUREMENT_H_ */
