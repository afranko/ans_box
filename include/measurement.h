/*
 * measurement.h
 *
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "circ_buff.h"

extern cBuff cont_0, cont_1, cont_2, cont_3;

uint16_t read_last(cBuff *buff_c);

#endif /* MEASUREMENT_H_ */
