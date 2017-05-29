/*
 * statemachine.h
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

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

void statemachine_process();

#endif /* STATEMACHINE_H_ */
