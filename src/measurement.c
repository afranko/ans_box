/*
 * measurement.c
 *
 */

#include "measurement.h"

/* Counter for ADC's channels */
uint8_t bcounter = 0;

/* Counter for offset head */
uint16_t ocounter = 0;

uint16_t noffset0 = 0;
uint16_t noffset1 = 0;
uint16_t noffset2 = 0;
uint16_t noffset3 = 0;

uint16_t poffset = 0;
bool offMeas = false;
bool mFlag = false;
bool endMeas = false;

bool MSG_FLAG = false;				// Send message flag

meas_flag_block mfb;

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
		bcounter++;
	}
	meas_datamove();
}

uint16_t read_last(cBuff *buff_c)
{
	uint16_t temphead;
	if(buff_c->head == 0)
	{
		temphead = BUFFER_SIZE - 1;
	}
	else
	{
		temphead = buff_c->head - 1;
	}
	uint16_t lastData = buff_c->buffer[temphead];
	return lastData;
}

void meas_datamove(void)
{

	if(CLEAR_FLAG == true)
	{
		CLEAR_FLAG = false;
		ocounter = 0;
		noffset0 = 0;
		noffset1 = 0;
		noffset2 = 0;
		noffset3 = 0;
		poffset = 0;
		offMeas = false;
		mFlag = false;
		endMeas = false;
		flush_cBuff(&gBuffer0);
		flush_cBuff(&gBuffer1);
		flush_cBuff(&gBuffer2);
		flush_cBuff(&gBuffer3);
	}

	/* Start reading to the great data buffer */
	if(S_MEAS_FLAG == true)
	{
		S_MEAS_FLAG = false;
		noffset0 = (cont_0.head-1) % BUFFER_SIZE;
		noffset1 = (cont_1.head-1) % BUFFER_SIZE;
		noffset2 = (cont_2.head-1) % BUFFER_SIZE;
		noffset3 = (cont_3.head-1) % BUFFER_SIZE;

		/*
		 *mfb.noffest0 = (cont_0.head-1) % BUFFER_SIZE;
		 *mfb.noffest1 = (cont_1.head-1) % BUFFER_SIZE;
		 *mfb.noffest2 = (cont_2.head-1) % BUFFER_SIZE;
		 *mfb.noffest3 = (cont_3.head-1) % BUFFER_SIZE;
		 */

		cont_0.tail = cont_0.head;
		cont_1.tail = cont_1.head;
		cont_2.tail = cont_2.head;
		cont_3.tail = cont_3.head;

		/*mfb.offMeas = true; */
		offMeas = true;
	}

	/* Start reading negative offset */
	if(offMeas == true)
	{
		uint16_t temp_head = (noffset0 - ((config_s.meas_offset) / 10) + ocounter + 1) % BUFFER_SIZE;
		push_cBuff(&gBuffer0, cont_0.buffer[temp_head]);
		temp_head = (noffset1 - ((config_s.meas_offset) / 10) + ocounter + 1) % BUFFER_SIZE;
		push_cBuff(&gBuffer1, cont_1.buffer[temp_head]);
		temp_head = (noffset2 - ((config_s.meas_offset) / 10) + ocounter + 1) % BUFFER_SIZE;
		push_cBuff(&gBuffer2, cont_2.buffer[temp_head]);
		temp_head = (noffset3 - ((config_s.meas_offset) / 10) + ocounter + 1) % BUFFER_SIZE;
		push_cBuff(&gBuffer3, cont_3.buffer[temp_head]);

		ocounter++;

		if(temp_head == noffset0)
		{
			offMeas = false;
			mFlag = true;
			ocounter = 0;
		}
	}

	/* End of the measurement */
	if(mFlag == true && poffset == cont_0.tail)
	{
		mFlag = false;
		endMeas = true;
	}

	/* Start reading measurement data */
	if(mFlag == true)
	{
		uint16_t d_data;
		if(pop_cBuff(&cont_0, &d_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer0, d_data);
		}

		if(pop_cBuff(&cont_1, &d_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer1, d_data);
		}

		if(pop_cBuff(&cont_2, &d_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer2, d_data);
		}

		if(pop_cBuff(&cont_3, &d_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer3, d_data);
		}
	}

	/* Set positive offset head */
	if(E_MEAS_FLAG == true)
	{
		poffset = cont_0.head;
		E_MEAS_FLAG = false;
	}

	if(endMeas == true)			//TODO még ezt pontosítani kell, mert ide-oda elcsúszhatnak eggyel
	{
		uint16_t e_data;

		if(pop_cBuff(&cont_0, &e_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer0, e_data);
		}

		if(pop_cBuff(&cont_1, &e_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer1, e_data);
		}

		if(pop_cBuff(&cont_2, &e_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer2, e_data);
		}

		if(pop_cBuff(&cont_3, &e_data) != cBuff_EMPTY)
		{
			push_cBuff(&gBuffer3, e_data);
		}

		if(cont_0.tail == (poffset + ((config_s.meas_offset) / 10) - 1) % BUFFER_SIZE)
		{
			endMeas = false;
			ocounter = 0;
			MSG_FLAG = true;
		}
	}
}

void dataparse(uint16_t data, char *itoabuff)
{
	for(uint8_t i = 0; i < 5; i++)
	{
		itoabuff[i] = 0;
	}

	uint8_t ccounter = 5;
	char ascbuf [5];
	itoa(data, ascbuf, 10);

	if(data == 0)
	{
		data++;
	}

	while(data != 0)
	{
		data = data/10;
		ccounter--;
	}

	for(uint8_t i = 0; i < ccounter; i++)
	{
		itoabuff[i] = '0';
	}
	strcat(itoabuff, ascbuf);
	return;
}
