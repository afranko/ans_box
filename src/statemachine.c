/*
 * statemachine.c
 */

#include "statemachine.h"

/* After init the machine begins to work in START state */
machine_state m_state = S_START;

machine_state (*func_arr[6])(void);

machine_state p_error(void)
{

}

machine_state p_low(void)
{

}

machine_state p_high(void)
{

}

machine_state p_meas_up(void)
{

}

machine_state p_meas_down(void)
{

}

machine_state p_send_msg(void)
{

}

/* Init state machine and start process */
void p_start(void)
{
	/* Init Block */
	func_arr[S_LOW] = p_low;
	func_arr[S_HIGH] = p_high;
	func_arr[S_MEAS_UP] = p_meas_up;
	func_arr[S_MEAS_DOWN] = p_meas_down;
	func_arr[S_SEND_MSG] = p_send_msg;
	func_arr[S_ERROR] = p_error;




}

void statemachine_process()
{
	m_state = func_arr[m_state]();
}


