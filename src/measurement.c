/*
 * measurement.c
 *
 */

#include "measurement.h"

/* Counter for ADC's channels */
uint32_t bcounter = 0;

/* Counter for offset head */
uint16_t ocounter = 0;

uint16_t noffset = 0;
uint16_t poffset = 0;
bool offMeas = false;
bool mFlag = false;
bool endMeas = false;

bool MSG_FLAG = false;				// Send message flag

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC3)
	{
		/* Read data from ADC's DR */
		uint16_t adc_data = (uint16_t)HAL_ADC_GetValue(hadc);

		/* Writing to buffers */
		uint8_t chrank = bcounter % 4;

		switch(chrank)
		{
		case 0:
			push_cBuff(&cont_0, adc_data);
			break;
		case 1:
			push_cBuff(&cont_1, adc_data);
			break;
		case 2:
			push_cBuff(&cont_2, adc_data);
			break;
		case 3:
			push_cBuff(&cont_3, adc_data);
			break;
		}

		/* Increment counter*/
		bcounter++; //TODO overflow?
	}
	meas_datamove();
}

uint16_t read_last(cBuff *buff_c)
{
	uint16_t lastData = buff_c->buffer[(buff_c->head)-1];
	return lastData;
}

void meas_datamove(void)
{

	if(CLEAR_FLAG == true)
	{
		CLEAR_FLAG = false;
		ocounter = 0;
		noffset = 0;
		poffset = 0;
		offMeas = false;
		mFlag = false;
		endMeas = false;
		flush_cBuff(&gBuffer);
	}

	/* Start reading to the great data buffer */
	if(S_MEAS_FLAG == true)
	{
		S_MEAS_FLAG = false;
		noffset = cont_0.head;
		cont_0.tail = cont_0.head+1;
		offMeas = true;
	}

	/* Start reading negative offset */
	if(offMeas == true)
	{
		uint16_t temp_head = (noffset - ocounter) % BUFFER_SIZE;
		push_cBuff(&gBuffer, cont_0.buffer[temp_head]);
		ocounter++;
		if(temp_head == (noffset - ((config_s.meas_offset) / 10)) % BUFFER_SIZE)
		{
			offMeas = false;
			mFlag = true;
			ocounter = 0;
		}
	}

	/* End of the measurement */
	if(poffset == cont_0.tail)
	{
		mFlag = false;
		endMeas = true;
	}

	/* Start reding measurement data */
	if(mFlag == true)
	{
		uint16_t data;
		pop_cBuff(&cont_0, &data);
		push_cBuff(&gBuffer, data);
	}

	/* Set positive offset head */
	if(E_MEAS_FLAG == true)
	{
		poffset = cont_0.head+1;
		E_MEAS_FLAG = false;
	}

	if(endMeas == true)
	{
		uint16_t temp_head = (poffset + ocounter) % BUFFER_SIZE;
		push_cBuff(&gBuffer, cont_0.buffer[temp_head]);
		ocounter++;
		if(temp_head == (noffset + ((config_s.meas_offset) / 10)) % BUFFER_SIZE)
		{
			endMeas = false;
			ocounter = 0;
			MSG_FLAG = true;
		}
	}
}
