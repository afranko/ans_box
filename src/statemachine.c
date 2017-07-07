/*
 * statemachine.c
 */

#include "statemachine.h"

/* After init the machine begins to work in START state */
machine_state m_state = S_START;
uint32_t timer = 0;

/* Array for state machine */
machine_state (*func_arr[5])(void) = {p_low, p_high, p_meas_up, p_meas_down, p_error};

machine_state p_error(void)
{
	uint16_t position;

	position = read_last(&cont_0);
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
	return S_ERROR;
}

machine_state p_low(void)
{
	uint16_t position = read_last(&cont_0);
	if(position > config_s.threshold_min)
	{
		/* Only if there isn't measurement in procces */
		if(mfb.duration == 0)
			mfb.S_MEAS_FLAG = true;
		return S_MEAS_UP;
	}
	return S_LOW;
}

machine_state p_high(void)
{
	uint16_t position = read_last(&cont_0);
	if(position < config_s.threshold_max)
	{
		/* Only if there isn't measurement in procces */
		if(mfb.duration == 0)
			mfb.S_MEAS_FLAG = true;
		return S_MEAS_DOWN;
	}
	return S_HIGH;
}

machine_state p_meas_up(void)
{
	uint16_t position;

	if(timer == 0)
			timer = HAL_GetTick();
	while(1)
	{
		position = read_last(&cont_0);

		/* Go to error due to timeout */
		if((HAL_GetTick() - timer) > config_s.meas_timeout)
		{
			/* Only if there isn't measurement in procces */
			if(!mfb.end_set)
			{
				mfb.E_MEAS_FLAG = true;
				mfb.warning_flag = true;
			}
			timer = 0;
			return S_ERROR;
		}

		if(position > config_s.threshold_max)
		{
			/* Only if there isn't measurement in procces */
			if(!mfb.end_set)
				mfb.E_MEAS_FLAG = true;
			timer = 0;
			return S_HIGH;
		}
		return S_MEAS_UP;
	}
}

machine_state p_meas_down(void)
{
	uint16_t position;

	if(timer == 0)
		timer = HAL_GetTick();
	while(1)
	{
		position = read_last(&cont_0);

		/* Go to error due to timeout */
		if((HAL_GetTick() - timer) > config_s.meas_timeout)
		{
			/* Only if there isn't measurement in procces */
			if(!mfb.end_set)
			{
				mfb.E_MEAS_FLAG = true;
				mfb.warning_flag = true;
			}
			timer = 0;
			return S_ERROR;
		}

		if(position < config_s.threshold_min)
		{
			/* Only if there isn't measurement in procces */
			if(!mfb.end_set)
				mfb.E_MEAS_FLAG = true;
			timer = 0;
			return S_LOW;
		}
		return S_MEAS_DOWN;
	}
}

/* Init state machine and start process */
void p_start(void)
{
	uint16_t pos;
	uint32_t timer_s;

	timer_s = HAL_GetTick();

	/* Start function */
	while(1)
	{
		/* After 10 sec in "Start" state the function goes to CONFIG_ERROR */
		if((HAL_GetTick() - timer_s) > config_s.meas_timeout)
		{
			config_error();
			restart_init();
		}

		pos = read_last(&cont_0);

		/*Check if we can start operating*/
		if((pos > config_s.threshold_max) || (pos < config_s.threshold_min))
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

void statemachine_process(void)
{
	m_state = func_arr[m_state]();
	return;
}
