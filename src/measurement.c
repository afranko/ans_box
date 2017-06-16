/*
 * measurement.c
 *
 */

#include "measurement.h"

/* Counter for ADC's channels */
uint8_t bcounter = 0;

/* Measurement Flag Block */
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

	if(mfb.CLEAR_FLAG == true) //TODO timeout esetén van üzenet?
	{
		init_meas_flag_block(&mfb);
		flush_cBuff(&gBuffer0);
		flush_cBuff(&gBuffer1);
		flush_cBuff(&gBuffer2);
		flush_cBuff(&gBuffer3);
	}

	/* Start reading to the great data buffer */
	if(mfb.S_MEAS_FLAG == true)
	{

		/* Set offset to read data before
		 * the starting of measurement */
		mfb.noffset0 = (cont_0.head-1) % BUFFER_SIZE;
		mfb.noffset1 = (cont_1.head-1) % BUFFER_SIZE;
		mfb.noffset2 = (cont_2.head-1) % BUFFER_SIZE;
		mfb.noffset3 = (cont_3.head-1) % BUFFER_SIZE;

		/* Set read pointers */
		cont_0.tail = cont_0.head;
		cont_1.tail = cont_1.head;
		cont_2.tail = cont_2.head;
		cont_3.tail = cont_3.head;

		/* Enable offset data reading */
		mfb.offMeas = true;

		/* Clear START FLAG */
		mfb.S_MEAS_FLAG = false;

	}

	/* Start reading negative offset */
	if(mfb.offMeas == true)
	{
		/* Load 0. Buffer */
		uint16_t temp_head = mfb.noffset0
				-(config_s.meas_offset/10)
					+mfb.ocounter;

		push_cBuff(&gBuffer0, cont_0.buffer[(temp_head % BUFFER_SIZE)]);

		/* Load 1. Buffer */
		temp_head = mfb.noffset1
				-(config_s.meas_offset/10)
					+mfb.ocounter;

		push_cBuff(&gBuffer1, cont_1.buffer[(temp_head % BUFFER_SIZE)]);

		/* Load 2. Buffer */
		temp_head = mfb.noffset2
				-(config_s.meas_offset/10)
					+mfb.ocounter;

		push_cBuff(&gBuffer2, cont_2.buffer[(temp_head % BUFFER_SIZE)]);

		/* Load 3. Buffer */
		temp_head = mfb.noffset3
				-(config_s.meas_offset/10)
					+mfb.ocounter;

		push_cBuff(&gBuffer3, cont_3.buffer[(temp_head % BUFFER_SIZE)]);

		if((config_s.meas_offset/10) <= mfb.ocounter)
		{
			mfb.offMeas = false;
			mfb.mFlag = true;
		}

		mfb.ocounter++;
	}

	/* End of the measurement */
	if((mfb.measc0 == mfb.measLength) &&
			(mfb.measc1 == mfb.measLength) &&
				(mfb.measc2 == mfb.measLength) &&
					(mfb.measc3 == mfb.measLength) && (mfb.mFlag == true))
	{
		mfb.mFlag = false;
		mfb.endMeas = true;
	}

	/* Set positive offset head */
	if(mfb.E_MEAS_FLAG == true)
	{
		/*Set measurement length */
		mfb.measLength = (cont_0.head) - (mfb.noffset0+1);

		mfb.E_MEAS_FLAG = false;
	}

	/* Start reading measurement data */
	if(mfb.mFlag == true)
	{
		uint16_t d_data;

		if((pop_cBuff(&cont_0, &d_data) != cBuff_EMPTY) && (mfb.measc0 < mfb.measLength))
		{
			push_cBuff(&gBuffer0, d_data);
			mfb.measc0++;
		}

		if((pop_cBuff(&cont_1, &d_data) != cBuff_EMPTY) && (mfb.measc1 < mfb.measLength))
		{
			push_cBuff(&gBuffer1, d_data);
			mfb.measc1++;
		}

		if((pop_cBuff(&cont_2, &d_data) != cBuff_EMPTY) && (mfb.measc2 < mfb.measLength))
		{
			push_cBuff(&gBuffer2, d_data);
			mfb.measc2++;
		}

		if((pop_cBuff(&cont_3, &d_data) != cBuff_EMPTY) && (mfb.measc3 < mfb.measLength))
		{
			push_cBuff(&gBuffer3, d_data);
			mfb.measc3++;
		}
	}

	/* Start reading positive Measurement */
	if(mfb.endMeas == true)
	{
		uint16_t e_data;

		if((pop_cBuff(&cont_0, &e_data) != cBuff_EMPTY) && (mfb.pofc0 < (config_s.meas_offset/10)))
		{
			push_cBuff(&gBuffer0, e_data);
			mfb.pofc0++;
		}

		if((pop_cBuff(&cont_1, &e_data) != cBuff_EMPTY) && (mfb.pofc1 < (config_s.meas_offset/10)))
		{
			push_cBuff(&gBuffer1, e_data);
			mfb.pofc1++;
		}

		if((pop_cBuff(&cont_2, &e_data) != cBuff_EMPTY) && (mfb.pofc2 < (config_s.meas_offset/10)))
		{
			push_cBuff(&gBuffer2, e_data);
			mfb.pofc2++;
		}

		if((pop_cBuff(&cont_3, &e_data) != cBuff_EMPTY) && (mfb.pofc3 < (config_s.meas_offset/10)))
		{
			push_cBuff(&gBuffer3, e_data);
			mfb.pofc3++;
		}

		/* End offset measurement and set Message FLAG */
		if((mfb.pofc0 == (config_s.meas_offset/10)) &&
					(mfb.pofc1 == (config_s.meas_offset/10)) &&
						(mfb.pofc2 == (config_s.meas_offset/10)) &&
							(mfb.pofc3 == (config_s.meas_offset/10)))
		{
			mfb.endMeas = false;
			mfb.MSG_FLAG = true;
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

void init_meas_flag_block(meas_flag_block *flagBlock)
{
	flagBlock->S_MEAS_FLAG = false;
	flagBlock->E_MEAS_FLAG = false;
	flagBlock->CLEAR_FLAG = false;
	flagBlock->MSG_FLAG = false;

	flagBlock->noffset0 = 0;
	flagBlock->noffset1 = 0;
	flagBlock->noffset2 = 0;
	flagBlock->noffset3 = 0;

	flagBlock->ocounter = 0;

	flagBlock->offMeas = false;

	flagBlock->mFlag = false;
	flagBlock->measLength = BUFFER_SIZE;

	flagBlock->measc0 = 0;
	flagBlock->measc1 = 0;
	flagBlock->measc2 = 0;
	flagBlock->measc3 = 0;

	flagBlock->pofc0 = 0;
	flagBlock->pofc1 = 0;
	flagBlock->pofc2 = 0;
	flagBlock->pofc3 = 0;

	flagBlock->endMeas = false;
}
