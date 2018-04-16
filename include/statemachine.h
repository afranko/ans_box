/*
 * statemachine.h
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "init.h"
#include "measurement.h"

typedef enum
{
	S_START		= 5,	//	Not used in function pointer array
	S_LOW		= 0,
	S_HIGH		= 1,
	S_MEAS_UP	= 2,
	S_MEAS_DOWN	= 3,
	S_ERROR		= 4
}machine_state;

extern measurement_flag_block mfb;
extern Settings_HandleTypeDef *hconfig;
extern cBuff *measBuffers[4];

void p_start(void);
void statemachine_process(void);
uint16_t getLastPosition(void);

#endif /* STATEMACHINE_H_ */
