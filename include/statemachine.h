/*
 * statemachine.h
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "measurement.h"

extern cBuff cont_0, cont_1, cont_2, cont_3;

typedef enum
{
	S_START		= 5,	//	Not used in function pointer array
	S_LOW		= 0,
	S_HIGH		= 1,
	S_MEAS_UP	= 2,
	S_MEAS_DOWN	= 3,
	S_ERROR		= 4
}machine_state;

extern meas_flag_block mfb;
extern Settings_HandleTypeDef config_s;

machine_state p_error(void);
machine_state p_low(void);
machine_state p_high(void);
machine_state p_meas_up(void);
machine_state p_meas_down(void);
void p_start(void);
void statemachine_process(void);

#endif /* STATEMACHINE_H_ */
