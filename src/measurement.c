/*
 * measurement.c
 *      Author: A. Franko
 */

#include "measurement.h"

static void initMovMeas(void);
static void readNoffset(void);
static void readActValues(void);
static void readOoffset(void);
static void readMeasEnd(void);
static void intoa_conv(uint16_t data, char subBuffer[]);

static bool readTelAire(I2C_HandleTypeDef *hi2c, float *humidity, float *ambTemp);
static bool readPT(SPI_HandleTypeDef *hspi, float *railTemp);

extern inline void initMeasurementParams(char clientName[], RTC_HandleTypeDef *hrtcm);

uint8_t bcounter = 0;
char parseString[2000];

/* Movement Functions */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC3)
	{
		/* Read data from DR of ADC */
		uint16_t adc_data = (uint16_t)HAL_ADC_GetValue(hadc);

		/* Writing to buffers and Increment Counter */
		uint8_t chrank = bcounter++ % 4;

		/* Pushing data into the appropriate buffer */
		push_cBuff(measBuffers[chrank], adc_data);
	}
}

void initMovMeas(void)
{
	mfb.S_MEAS_FLAG = false;

	for(uint8_t inC = 0; inC < 4; inC++)
		flush_cBuff(sendBuffers[inC]);

	/* Set offset and synchronize buffers */
	mfb.noffset = measBuffers[0]->head;

	measBuffers[0]->tail = (uint16_t)(mfb.noffset - (hconfig->meas_offset / 10));
	measBuffers[0]->tail %= measBuffers[0]->size;

	measBuffers[1]->tail = (uint16_t)(mfb.noffset - (hconfig->meas_offset / 10));
	measBuffers[1]->tail %= measBuffers[1]->size;

	measBuffers[2]->tail = (uint16_t)(mfb.noffset - (hconfig->meas_offset / 10));
	measBuffers[2]->tail %= measBuffers[2]->size;

	measBuffers[3]->tail = (uint16_t)(mfb.noffset - (hconfig->meas_offset / 10));
	measBuffers[3]->tail %= measBuffers[3]->size;


	mfb.offMeas = true;
}

void readNoffset(void)
{
	uint16_t nData;

	if((measBuffers[0]->tail < mfb.noffset) && (pop_cBuff(measBuffers[0], &nData) == BUFF_OK))
		push_cBuff(sendBuffers[0], nData);

	if((measBuffers[1]->tail < mfb.noffset) && (pop_cBuff(measBuffers[1], &nData) == BUFF_OK))
		push_cBuff(sendBuffers[1], nData);

	if((measBuffers[2]->tail < mfb.noffset) && (pop_cBuff(measBuffers[2], &nData) == BUFF_OK))
		push_cBuff(sendBuffers[2], nData);

	if((measBuffers[3]->tail < mfb.noffset) && (pop_cBuff(measBuffers[3], &nData) == BUFF_OK))
		push_cBuff(sendBuffers[3], nData);

	if((measBuffers[0]->tail == mfb.noffset) && (measBuffers[1]->tail == mfb.noffset)
				&& (measBuffers[2]->tail == mfb.noffset) && (measBuffers[3]->tail == mfb.noffset))
	{
		mfb.offMeas = false;
		mfb.mFlag = true;
	}
}

void readOoffset(void)
{
	mfb.E_MEAS_FLAG = false;

	/* Set measurement length */
	if(measBuffers[0]->head > mfb.noffset)
		mfb.measLength = (measBuffers[0]->head) - (mfb.noffset);
	else
		mfb.measLength = (measBuffers[0]->size - mfb.noffset) + measBuffers[0]->head;
	mfb.endSet = true;
}

void readActValues(void)
{
	uint16_t actValue;

	if((mfb.measc0 < mfb.measLength) && (pop_cBuff(measBuffers[0], &actValue) == BUFF_OK))
	{
		push_cBuff(sendBuffers[0], actValue);
		mfb.measc0++;
	}

	if((mfb.measc1 < mfb.measLength) && (pop_cBuff(measBuffers[1], &actValue) == BUFF_OK))
	{
		push_cBuff(sendBuffers[1], actValue);
		mfb.measc1++;
	}

	if((mfb.measc2 < mfb.measLength) && (pop_cBuff(measBuffers[2], &actValue) == BUFF_OK))
	{
		push_cBuff(sendBuffers[2], actValue);
		mfb.measc2++;
	}

	if((mfb.measc3 < mfb.measLength) && (pop_cBuff(measBuffers[3], &actValue) == BUFF_OK))
	{
		push_cBuff(sendBuffers[3], actValue);
		mfb.measc3++;
	}

	if((mfb.measc0 == mfb.measLength) && (mfb.measc1 == mfb.measLength) && (mfb.measc2 == mfb.measLength)
		&& (mfb.measc3 == mfb.measLength) && (mfb.endSet))
	{
		mfb.mFlag = false;
		mfb.endMeas = true;
		mfb.endSet = false;
	}
}

void readMeasEnd(void)
{
	uint16_t oData;

	if((mfb.pofc0 < hconfig->meas_offset/10) && (pop_cBuff(measBuffers[0], &oData) == BUFF_OK))
	{
		push_cBuff(sendBuffers[0], oData);
		mfb.pofc0++;
	}

	if((mfb.pofc1 < hconfig->meas_offset/10) && (pop_cBuff(measBuffers[1], &oData) == BUFF_OK))
	{
		push_cBuff(sendBuffers[1], oData);
		mfb.pofc1++;
	}

	if((mfb.pofc2 < hconfig->meas_offset/10) && (pop_cBuff(measBuffers[2], &oData) == BUFF_OK))
	{
		push_cBuff(sendBuffers[2], oData);
		mfb.pofc2++;
	}

	if((mfb.pofc3 < hconfig->meas_offset/10) && (pop_cBuff(measBuffers[3], &oData) == BUFF_OK))
	{
		push_cBuff(sendBuffers[3], oData);
		mfb.pofc3++;
	}

	if((mfb.pofc0 == hconfig->meas_offset/10) && (mfb.pofc1 == hconfig->meas_offset/10) &&
			(mfb.pofc2 == hconfig->meas_offset/10)&& (mfb.pofc3 == hconfig->meas_offset/10))
	{
		mfb.MSG_FLAG = true;
		mfb.measLength = 0;
		mfb.endMeas = false;
	}
}

/*
 * @brief This function implements the movement measurement cycles
 */
void doMovMeas(bool *msgFlag, bool *warningFlag)
{
	if(mfb.S_MEAS_FLAG)
		initMovMeas();
	if(mfb.offMeas)
		readNoffset();
	if(mfb.E_MEAS_FLAG)
		readOoffset();
	if(mfb.mFlag)
		readActValues();
	if(mfb.endMeas)
		readMeasEnd();

	*msgFlag = mfb.MSG_FLAG;
	*warningFlag = mfb.warningFlag;
}

void intoa_conv(uint16_t data, char subBuffer[])
{
	uint8_t len_0 = 4;
	char s_buf[5];

	for(uint8_t j = 0; j < 5; j++)
	{
		subBuffer[j] = 0;
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
		subBuffer[i] = '0';
	}

	strcat(subBuffer, s_buf);
	return;
}

/*
 * @brief This function initializes measurement flag block
 */
void initFlags(void)
{
	mfb.duration = 0;
	mfb.S_MEAS_FLAG = false;
	mfb.E_MEAS_FLAG = false;
	mfb.MSG_FLAG = false;
	mfb.MSG_PARSED = false;
	mfb.endMeas = false;
	mfb.endSet = false;
	mfb.mFlag = false;
	mfb.measLength = 0;
	mfb.offMeas = false;
	mfb.warningFlag = false;

	mfb.noffset = 0;

	mfb.measc0 = 0;
	mfb.measc1 = 0;
	mfb.measc2 = 0;
	mfb.measc3 = 0;

	mfb.pofc0 = 0;
	mfb.pofc1 = 0;
	mfb.pofc2 = 0;
	mfb.pofc3 = 0;
}

/*
 * @brief This function parses data into JSON and serializes it
 * @param A buffer that will contain the serialized message
 * @param Timestamp to synchronize movement message with warning message
 */
bool movMeasToMessage(char movBuffer[], char warTimeStamp[])
{
	BUFF_State parse_flag;
	uint16_t msg_data;
	char msg_string[20];
	char itoa_subbuf[5];
	bool first_iteration = false;
	uint32_t ostr_len;

	/* Timestamp */
	char tStamp[20];
	getTimeStamp(tStamp);
	strcpy(warTimeStamp, tStamp);	// For Warning Message

	/* Copy JSON */
	if(!parseMovementMessage(mfb.duration, hconfig->client_name, tStamp, parseString))
		return false;	// JSON serializing error
	uint16_t json_slen = (strstr(parseString, "xxxx") - parseString);		// Pointer arithmetic
	strncpy(movBuffer, parseString, json_slen);

	/* Defending ourselves */
	ostr_len = json_slen;

	while(!mfb.MSG_PARSED)
	{
		/* Buffer0 data */
		pop_cBuff(sendBuffers[0], &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcpy(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer1 data */
		pop_cBuff(sendBuffers[1], &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer2 data */
		pop_cBuff(sendBuffers[2], &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);
		strcat(msg_string, "-");

		/* Buffer3 data */
		parse_flag = pop_cBuff(sendBuffers[3], &msg_data);
		intoa_conv(msg_data, itoa_subbuf);
		strcat(msg_string, itoa_subbuf);

		/* Send message if there is not any data */
		if(parse_flag == BUFF_EMPTY)
		{
			sprintf(movBuffer + ostr_len, "\n");
			ostr_len++;
			for(uint8_t i = 0; i < 24; i++)
			{
				sprintf(movBuffer + ostr_len, " ");
				ostr_len++;
			}

			/* sprintf terminate the string with '\0' char */
			sprintf(movBuffer + ostr_len, strstr(parseString, "]"));
			mfb.MSG_PARSED = true;
		}
		else
		{
			/* Add measurement to array */
			if(!first_iteration)
			{
				sprintf(movBuffer + ostr_len, msg_string);
				ostr_len = ostr_len + 19;
				sprintf(movBuffer + ostr_len, "\"");
				ostr_len++;
				first_iteration = true;
			}
			else
			{
				sprintf(movBuffer + ostr_len, ",\n");
				ostr_len = ostr_len+2;
				for(uint8_t i = 0; i < 28; i++)
				{
					sprintf(movBuffer + ostr_len, " ");
					ostr_len++;
				}
				sprintf(movBuffer + ostr_len, "\"");
				ostr_len++;
				sprintf(movBuffer + ostr_len, msg_string);
				ostr_len = ostr_len + 19;
				sprintf(movBuffer + ostr_len, "\"");
				ostr_len++;
			}
		}
	}
	mfb.warningFlag = false;
	mfb.duration = 0;
	mfb.MSG_PARSED = false;
	mfb.MSG_FLAG = false;

	mfb.noffset = 0;

	mfb.measc0 = 0;
	mfb.measc1 = 0;
	mfb.measc2 = 0;
	mfb.measc3 = 0;

	mfb.pofc0 = 0;
	mfb.pofc1 = 0;
	mfb.pofc2 = 0;
	mfb.pofc3 = 0;

	return true;
}

/* Environment Functions */

/*
 * @brief This function initializes the PT-100 module (via SPI)
 * @param Pointer to the SPI handling structure
 * @return This function returns true if everything was OK otherwise it returns false
 */
bool initPT(SPI_HandleTypeDef *hspi)
{
	uint8_t config_reg = 0x80U;
	uint8_t config_command = 0x03U;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10);

	if(HAL_SPI_Transmit(hspi, &config_reg, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;
	if(HAL_SPI_Transmit(hspi, &config_command, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	return true;
}

/*
 * @brief This function do a complete environment measurement session (temperatures and humidity included)
 * @param Pointer to I2C handling structure
 * @param Pointer to SPI handling structure
 * @param A Buffer consisting of chars to store the formatted JSON message as a string
 * @note Before using this function, PT-100 sensor must be initialized by initPT function
 * @return This functions returns true if the measurement session is completed otherwise it returns false
 */
bool doEnvMeas(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi, char envBuffer[])
{
	float humidity, amb_temp, rail_temp;

	if(!readTelAire(hi2c, &humidity, &amb_temp))
		return false;

	if(!readPT(hspi, &rail_temp))
		return false;

	char timeStamp[20];
	if(!getTimeStamp(timeStamp))
		return false;

	if(!parseEnvironmentMessage(amb_temp, humidity, rail_temp, timeStamp, clientIdentifier, envBuffer))
		return false;

	return true;
}

/* Reading Ambient Temperature and Humidity from TelAire sensor */
bool readTelAire(I2C_HandleTypeDef *hi2c, float *humidity, float *ambTemp)
{
	uint8_t receivedData[4];
	if(HAL_I2C_Master_Receive(hi2c, 0x28 << 1, receivedData, 4U, HAL_MAX_DELAY) != HAL_OK)
		return false;

	if((receivedData[0] & 0xC0U)!= 0)	// Checking ERROR bit
		return false;

	*humidity = (float) (((receivedData[0] & 0x3FU) << 8) | receivedData[1]) * 100 / (float) 16384;
	*ambTemp = (float) ((receivedData[2] << 6) | ((receivedData[3] & 0xFC) >> 2)) * 165 / (float) 16384 - 40;

	return true;
}

/* Reading Rail temperature from PT100 sensor */
bool readPT(SPI_HandleTypeDef *hspi, float *railTemp)
{
	uint8_t w_add = 0x80U, r_add = 0x00U, bias_on = 0x81U, conv_start = 0xA1U, bias_off = 0x03U;
	uint8_t received_data[8];
	uint16_t raw_adc_code;
	float rtd;

	/* Turn BIAS ON */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	if(HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;
	if(HAL_SPI_Transmit(hspi, &bias_on, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	/* START CONV */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	if(HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;
	if(HAL_SPI_Transmit(hspi, &conv_start, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(63); //MAX CONV TIME

	/* Read Data */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	if(HAL_SPI_Transmit(hspi, &r_add, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;

	for(uint8_t i = 0; i < 8; i++)
		if(HAL_SPI_Receive(hspi, &received_data[i], 1, HAL_MAX_DELAY) != HAL_OK)
			return false;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);

	/* Turn BIAS OFF */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(10); //MAX STARTUP TIME TIME

	if(HAL_SPI_Transmit(hspi, &w_add, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;
	if(HAL_SPI_Transmit(hspi, &bias_off, 1, HAL_MAX_DELAY) != HAL_OK)
		return false;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	/* Conversation */
	raw_adc_code = (uint16_t) ((received_data[1] << 7) | ((received_data[2] & 0xFE) >> 1));
	rtd = (float) raw_adc_code / 32768 * 430;

	*railTemp = rtd*2.5707-257.07;
	return true;
}

/* Get the actual time stamp */
bool getTimeStamp(char timeStamp[])
{
	char rawStamp[20], conVal[3];	//Converted value (integer -> ASCII)
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	if(HAL_RTC_GetTime(rtcClock, &time, RTC_FORMAT_BIN) != HAL_OK)
		return false;
	if(HAL_RTC_GetDate(rtcClock, &date, RTC_FORMAT_BIN) != HAL_OK)
		return false;

	strcpy(rawStamp, "20");		// 20

	itoa(date.Year, conVal, 10);
	strcat(rawStamp, conVal);	// 20xx

	strcat(rawStamp, ".");		// 20xx.

	itoa(date.Month, conVal, 10);
	if((date.Month / 10) == 0)
	{
		conVal[1] = conVal[0];	// If month > 10 -> We have to use padding (e.g. 09)
		conVal[0] = '0';
	}
	strcat(rawStamp, conVal);	// 20yy.mm

	strcat(rawStamp, ".");		// 20yy.mm.

	itoa(date.Date, conVal, 10);
	if((date.Date / 10) == 0)
	{
		conVal[1] = conVal[0];	// If month > 10 -> We have to use padding (e.g. 07)
		conVal[0] = '0';
	}
	strcat(rawStamp, conVal);	// 20yy.mm.dd

	strcat(rawStamp, ".");		// 20yy.mm.dd.

	itoa(time.Hours, conVal, 10);
	if((time.Hours / 10) == 0)
	{
		conVal[1] = conVal[0];	// Padding
		conVal[0] = '0';
	}
	strcat(rawStamp, conVal);	// 20yy.mm.dd.hh

	strcat(rawStamp, ":");		// 20yy.mm.dd.hh:

	itoa(time.Minutes, conVal, 10);
	if((time.Minutes / 10) == 0)
	{
		conVal[1] = conVal[0];	// Padding
		conVal[0] = '0';
	}
	strcat(rawStamp, conVal);	// 20yy.mm.dd.hh:mm

	strcat(rawStamp, ":");		// 20yy.mm.dd.hh:mm:

	itoa(time.Seconds, conVal, 10);
	if((time.Seconds / 10) == 0)
	{
		conVal[1] = conVal[0];
		conVal[0] = '0';
	}
	strcat(rawStamp, conVal);
	strcpy(timeStamp, rawStamp);

	return true;
}

/*
 * @brief This function send a Welcome message
 */
void sendWelcomeMessage(void)
{
	char timeStamp[20];
	getTimeStamp(timeStamp);
	sendWelcome(clientIdentifier, timeStamp);
}
