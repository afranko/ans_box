/*
 * statemachine.c
 */

#include "statemachine.h"

/* After init the machine begins to work in START state */
machine_state m_state = S_START;

machine_state (*func_arr[6])(void);

machine_state p_error(void) //WHAT ABOUT INF CYCLE? TODO
{
	uint16_t position;
	position = read_last(&cont_0);

	while(1)
	{
		if(position > config_s.threshold_max || position < config_s.threshold_min)
		{

			if(position < config_s.threshold_min)
			{
				return S_LOW;
			}

			if(position > config_s.threshold_max)
			{
				return S_HIGH;
			}
		}
	}
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
	uint16_t pos;
	uint32_t timer;

	/* Init Block */
	func_arr[S_LOW] = p_low;
	func_arr[S_HIGH] = p_high;
	func_arr[S_MEAS_UP] = p_meas_up;
	func_arr[S_MEAS_DOWN] = p_meas_down;
	func_arr[S_SEND_MSG] = p_send_msg;
	func_arr[S_ERROR] = p_error;

	timer = HAL_GetTick();

	/* Start function */
	while(1)
	{
		/* After 10 sec in "Start" state the function goes to CONFIG_ERROR */
		if((HAL_GetTick() - timer) > 10000)
		{
			config_error();
			restart_init();
		}

		pos = read_last(&cont_0);

		/*Check if we can start operating*/
		if(pos > config_s.threshold_max || pos < config_s.threshold_min)
		{

			if(pos < config_s.threshold_min)
			{
				m_state = S_LOW;
				return;
			}

			if(pos > config_s.threshold_max)
			{
				m_state = S_HIGH;
				return;
			}
		}
	}
}

void statemachine_process()
{
	m_state = func_arr[m_state]();
}


