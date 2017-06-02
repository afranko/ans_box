/*
 * statemachine.h
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "measurement.h"

extern cBuff cont_0, cont_1, cont_2, cont_3;

typedef enum
{
	S_START		= 6,
	S_LOW		= 0,
	S_HIGH		= 1,
	S_MEAS_UP	= 2,
	S_MEAS_DOWN	= 3,
	S_SEND_MSG	= 4,
	S_ERROR		= 5
}machine_state;


void p_start(void);
machine_state p_error(void);

void statemachine_process();

#endif /* STATEMACHINE_H_ */
