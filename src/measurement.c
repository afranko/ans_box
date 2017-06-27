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

	if(mfb.CLEAR_FLAG == true) //TODO timeout esetén van üzenet? - WARNING MESSAGE
	{
		init_meas_flag_block(&mfb);
		flush_cBuff(&gBuffer0);
		flush_cBuff(&gBuffer1);
		flush_cBuff(&gBuffer2);
		flush_cBuff(&gBuffer3);
		mfb.CLEAR_FLAG = false;
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

		/* Set timer */
		mfb.duration = HAL_GetTick();

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

			/* Reset End of Measurement flag */
			mfb.endMeas = false;

			/* Set Send Message Flag */
			mfb.MSG_FLAG = true;

			/* Set Timer */
			mfb.duration = (HAL_GetTick() - mfb.duration);
		}
	}
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
	flagBlock->duration = 0;
}

void intoa_conv(uint16_t data, char *ibuffer)
{
	for(uint8_t j = 0; j < 5; j++)
	{
		ibuffer[j] = 0;
	}

	uint8_t len_0 = 4;
	char s_buf[5];

	itoa(data, s_buf, 10);

	if(data == 0)
	{
		len_0--;
	}

	while(data != 0)
	{
		data = data / 10;
		len_0--;
	}

	for(uint8_t i = 0; i < len_0; i++)
	{
		ibuffer[i] = '0';
	}

	strcat(ibuffer, s_buf);
	ibuffer[4] = 0;
	return;
}

void getTimeStamp(RTC_HandleTypeDef *hrtc, char *TimeString)
{
	char TiSt[20], uncon[3];
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	HAL_RTC_GetTime(hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &date, RTC_FORMAT_BIN);

	strcpy(TiSt, "20");

	itoa(date.Year, uncon, 10);
	strcat(TiSt, uncon);

	strcat(TiSt, ".");

	itoa(date.Month, uncon, 10);
	if((date.Month / 10) == 0)
	{
		uncon[1] = uncon[0];
		uncon[0] = '0';
	}
	strcat(TiSt, uncon);

	strcat(TiSt, ".");

	itoa(date.Date, uncon, 10);
	if((date.Date / 10) == 0)
	{
		uncon[1] = uncon[0];
		uncon[0] = '0';
	}
	strcat(TiSt, uncon);

	strcat(TiSt, ".");

	itoa(time.Hours, uncon, 10);
	if((time.Hours / 10) == 0)
	{
		uncon[1] = uncon[0];
		uncon[0] = '0';
	}
	strcat(TiSt, uncon);

	strcat(TiSt, ":");

	itoa(time.Minutes, uncon, 10);
	if((time.Minutes / 10) == 0)
	{
		uncon[1] = uncon[0];
		uncon[0] = '0';
	}
	strcat(TiSt, uncon);

	strcat(TiSt, ":");

	itoa(time.Seconds, uncon, 10);
	if((time.Seconds / 10) == 0)
	{
		uncon[1] = uncon[0];
		uncon[0] = '0';
	}
	strcat(TiSt, uncon);
	strcpy(TimeString, TiSt);
	return;
}
