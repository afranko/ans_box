/*
 * statemachine.c
 */

#include "statemachine.h"

/* After init the machine begins to work in START state */
static machine_state m_state = S_START;
static uint32_t timer = 0U;
static uint16_t movingAvBuffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint16_t lastHead = 0;


static uint16_t movingAvarage(cBuff *buff_c);
static machine_state p_error(void);
static machine_state p_low(void);
static machine_state p_high(void);
static machine_state p_meas_up(void);
static machine_state p_meas_down(void);

/* Array for state machine */
machine_state (*func_arr[5])(void) = {p_low, p_high, p_meas_up, p_meas_down, p_error};

/* AVG counter function to avoid false threshold detection
 * It is secured to use this function only when we had enough data to calculate average value
 */
uint16_t movingAvarage(cBuff *buff_c)
{
	uint32_t avarage = 0;

	if(lastHead != buff_c->head)
	{
		for(uint8_t i = 9; i > 0; i--)
			movingAvBuffer[i] = movingAvBuffer[i-1];
		movingAvBuffer[0] = check_last_cBuff(buff_c);
	}

	lastHead = buff_c->head;

	for(uint8_t i = 0; i < 10; i++)
		avarage += movingAvBuffer[i];

	return (uint16_t)avarage/10U;
}

/* Error function */
machine_state p_error(void)
{
	uint16_t position = movingAvarage(measBuffers[0]);
	if(position > hconfig->threshold_max || position < hconfig->threshold_min)
	{
		if(position < hconfig->threshold_min)
			return S_LOW;

		if(position > hconfig->threshold_max)
			return S_HIGH;
	}
	return S_ERROR;
}

/* Low state handling function */
machine_state p_low(void)
{
	uint16_t position = movingAvarage(measBuffers[0]);

	if(position > hconfig->threshold_min)
	{
		/* Only if there isn't measurement in procces */
		if(mfb.duration == 0)
		{
			mfb.duration = 1U;
			mfb.S_MEAS_FLAG = true;
			timer = HAL_GetTick();
			return S_MEAS_UP;
		}
	}
	return S_LOW;
}

/* High state handling function */
machine_state p_high(void)
{
	 uint16_t position = movingAvarage(measBuffers[0]);

	if(position < hconfig->threshold_max)
	{
		/* Only if there isn't measurement in process */
		if(mfb.duration == 0)
		{
			mfb.S_MEAS_FLAG = true;
			mfb.duration = 1U;
			timer = HAL_GetTick();
			return S_MEAS_DOWN;
		}
	}
	return S_HIGH;
}

/* Low to high measurement session */
machine_state p_meas_up(void)
{
	uint16_t position;
	position = movingAvarage(measBuffers[0]);

	if(position > hconfig->threshold_max)
	{
		mfb.duration = (HAL_GetTick() - timer) + 2U*hconfig->meas_offset;
		mfb.E_MEAS_FLAG = true;
		timer = 0;
		return S_HIGH;
	}

	/* Go to error due to timeout */
	if((HAL_GetTick() - timer) > hconfig->meas_timeout)
	{
		mfb.duration = (HAL_GetTick() - timer) + 2U*hconfig->meas_offset;
		mfb.E_MEAS_FLAG = true;
		mfb.warningFlag = true;
		timer = 0;
		return S_ERROR;
	}

	return S_MEAS_UP;
}

/* High to Low measurement session */
machine_state p_meas_down(void)
{
	uint16_t position;
	position = movingAvarage(measBuffers[0]);

	if(position < hconfig->threshold_min)
	{
		mfb.E_MEAS_FLAG = true;
		mfb.duration = (HAL_GetTick() - timer) + 2U*hconfig->meas_offset;
		timer = 0;
		return S_LOW;
	}

	/* Go to error due to timeout */
	if((HAL_GetTick() - timer) > hconfig->meas_timeout)
	{
	/* Only if there isn't measurement in process */

		mfb.duration = (HAL_GetTick() - timer) + 2U*hconfig->meas_offset;
		mfb.E_MEAS_FLAG = true;
		mfb.warningFlag = true;
		timer = 0;
		return S_ERROR;
	}

	return S_MEAS_DOWN;
}

/*
 * @brief This function initializes FSR and starts processes
 */
void p_start(void)
{
	uint16_t position;
	uint32_t timer_s;

	timer_s = HAL_GetTick();

	/* Start function */
	while(1)
	{
		/* After 10 sec in "Start" state this function goes to CONFIG_ERROR */
		if((HAL_GetTick() - timer_s) > hconfig->meas_timeout)
			restart_system();

		for(uint8_t i = 0; i < 10; i++)
			position = movingAvarage(measBuffers[0]);

		/*Check if we can start operating*/
		if((position > hconfig->threshold_max) || (position < hconfig->threshold_min))
		{
			if(position < hconfig->threshold_min)
			{
				m_state = S_LOW;
				return;
			}

			if(position > hconfig->threshold_max)
			{
				m_state = S_HIGH;
				return;
			}
		}
	}
}

/*
 * @brief This function implements the switching between states of FSM (Final State Mahcine)
 */
void statemachine_process(void)
{
	m_state = func_arr[m_state]();
	return;
}

/*
 * @brief This function return the actual position of point blades
 * @return The actual position of points
 */
uint16_t getLastPosition(void)
{
	return movingAvarage(measBuffers[0]);
}
