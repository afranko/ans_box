/*
 * measurement.c
 *
 */

#include "measurement.h"
#include <math.h>

/* Counter for ADC's channels */
uint8_t bcounter = 0;

/* Measurement Flag Block */
meas_flag_block mfb;

/* Output buffer */
char outputString[70000];

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
}

uint16_t read_last(cBuff *buff_c)
{
	return buff_c->buffer[(buff_c->head-1) % BUFFER_SIZE];
}

void meas_datamove(void)
{

	if(mfb.CLEAR_FLAG == true)
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
					(mfb.measc3 == mfb.measLength) && (mfb.mFlag) && (mfb.end_set))
	{
		mfb.mFlag = false;
		mfb.endMeas = true;
	}

	/* Set positive offset head */
	if(mfb.E_MEAS_FLAG)
	{
		/*Set measurement length */
		if(cont_0.head > mfb.noffset0)
			mfb.measLength = ((cont_0.head) - (mfb.noffset0+1));
		else
			mfb.measLength = (BUFFER_SIZE - mfb.noffset0+1) + cont_0.head;

		mfb.end_set = true;
		mfb.E_MEAS_FLAG = false;
	}

	/* Start reading measurement data */
	if(mfb.mFlag)
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
	if(mfb.endMeas)
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
			mfb.duration = (HAL_GetTick() - mfb.duration + config_s.meas_offset);
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
	flagBlock->measLength = 1000;

	flagBlock->measc0 = 0;
	flagBlock->measc1 = 0;
	flagBlock->measc2 = 0;
	flagBlock->measc3 = 0;

	flagBlock->pofc0 = 0;
	flagBlock->pofc1 = 0;
	flagBlock->pofc2 = 0;
	flagBlock->pofc3 = 0;

	flagBlock->end_set = false;
	flagBlock->endMeas = false;
	flagBlock->duration = 0;

	flagBlock->warning_flag = false;
	flagBlock->MSG_SENT = false;
}

void intoa_conv(uint16_t data, char *ibuffer)
{
	uint8_t len_0 = 4;
	char s_buf[5];

	for(uint8_t j = 0; j < 5; j++)
	{
		ibuffer[j] = 0;
	}

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

void envMeas(RTC_HandleTypeDef *hrtc, I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi)
{
	float humidity, amb_temperature, rail_temp;
	readTelaire(hi2c, &humidity, &amb_temperature);
	readPT(hspi, &rail_temp);

	char tStamp[20];
	getTimeStamp(hrtc, tStamp);
	sendEnvironmentMessage(config_s.client_name, tStamp, rail_temp, amb_temperature, humidity);

	return;
}

void readTelaire(I2C_HandleTypeDef *hi2c, float *humidity, float *temperature)
{
	uint8_t received_data[4];
	HAL_I2C_Master_Receive(hi2c, 0x28 << 1, received_data, 4, HAL_MAX_DELAY);

	if((received_data[0] & 0xC0U)!= 0)
		return;

	*humidity = (float) (((received_data[0] & 0x3FU) << 8) | received_data[1]) * 100 / 16384;
	*temperature = (float) ((received_data[2] << 6) | ((received_data[3] & 0xFCU) >> 2)) * 165 / 16384 - 40;

	return;
}

void readPT(SPI_HandleTypeDef *hspi, float *rtemp)
{

	uint8_t w_add = 0x80U, r_add = 0x00U, bias_on = 0x81U, conv_start = 0xA1U, bias_off = 0x03U;
	uint8_t received_data[8];
	uint16_t raw_adc_code;
	float rtd;

	/* Turn BIAS ON */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME
	HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, &bias_on, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	/* START CONV */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME
	HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, &conv_start, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(63); //MAX CONV TIME

	/* Read Data */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME
	HAL_SPI_Transmit(hspi, &r_add, 1, HAL_MAX_DELAY);

	for(uint8_t i = 0; i < 8; i++)
		HAL_SPI_Receive(hspi, &received_data[i], 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);

	/* Turn BIAS OFF */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME
	HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, &bias_off, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	/* Conversation */
	raw_adc_code = (uint16_t) (received_data[1] << 7) | ((received_data[2] & 0xFE) >> 1);
	rtd = (float) raw_adc_code / 32768 * 430;

	*rtemp = rtd*2.5707-257.07;
	return;

}

void movMeas(RTC_HandleTypeDef *hrtc)
{
	cBuff_State parse_flag;
	uint16_t msg_data;
	char msg_string[20];
	char itoa_subbuf[5];
	uint8_t first_iteration = 0;
	char *json_s;
	uint32_t ostr_len;

	/* Timestamp */
	char tStamp[20];
	getTimeStamp(hrtc, tStamp);

	/* Copy JSON */
	json_s = parseMovementMessage(mfb.duration, config_s.client_name, tStamp);
	uint16_t json_slen = strstr(json_s, "xxxx")-json_s;
	strncpy(outputString, json_s, json_slen);

	/* Defending ourselves */
	ostr_len = json_slen;

	while(mfb.MSG_SENT == 0)
	{
		/* Buffer0 data */
		pop_cBuff(&gBuffer0, &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcpy(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer1 data */
		pop_cBuff(&gBuffer1, &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer2 data */
		pop_cBuff(&gBuffer2, &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer3 data */
		parse_flag = pop_cBuff(&gBuffer3, &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);

		/* Send message if there is not any data */
		if(parse_flag == cBuff_EMPTY)
		{
			sprintf(outputString + ostr_len, "\n");
			ostr_len++;
			for(uint8_t it1 = 0; it1< 24; it1++)
			{
				sprintf(outputString + ostr_len, " ");
				ostr_len++;
			}

			/* sprintf terminate the string with '\0' char */
			sprintf(outputString + ostr_len, strstr(json_s, "]"));

			/* Send Message */
			sendMovementMessage(outputString, mfb.warning_flag);

			/* Reset Flags */
			mfb.warning_flag = false;
			mfb.MSG_FLAG = false;
			mfb.MSG_SENT = 1;
			mfb.CLEAR_FLAG = true;

			/* Back to run */
			HOLDFlag = false;
		}

		else
		{
			/* Add measurement to array */
			if(first_iteration == 0)
			{
				sprintf(outputString + ostr_len, msg_string);
				ostr_len = ostr_len + 19;
				sprintf(outputString + ostr_len, "\"");
				ostr_len++;
				first_iteration = 1;
			}
			else
			{
				sprintf(outputString + ostr_len, ",\n");
				ostr_len = ostr_len+2;
				for(uint8_t it2 = 0; it2< 28; it2++)
				{
					sprintf(outputString + ostr_len, " ");
					ostr_len++;
				}
				sprintf(outputString + ostr_len, "\"");
				ostr_len++;
				sprintf(outputString + ostr_len, msg_string);
				ostr_len = ostr_len + 19;
				sprintf(outputString + ostr_len, "\"");
				ostr_len++;
			}
		}
	}

	/* Reset SENT FLAG & Free JSON string */
	json_free_serialized_string(json_s);

	mfb.MSG_SENT = 0;
	return;
}
