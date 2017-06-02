/*
 * measurement.c
 *
 */

#include "measurement.h"


extern uint32_t mes;

uint8_t hello = 0;
uint8_t helloka = 0;

int tomb[2000]; //TODO
int co = 0;

uint8_t bcounter = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC3)
	{
		/* Read data drom ADC's DR */
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

	bcounter++;
	}

	/*FOR TEST ONLY TODO
	if(helloka == 0)
	{
		helloka = 1;
		HAL_TIM_Base_Start_IT(&htim4);
	}
	uint32_t mes = HAL_ADC_GetValue(hadc); //MEKKORA? TODO

	tomb[co] = mes;
	co++; */

	/*if(co == 1999)
	{
		HAL_ADC_Stop_IT(hadc);
	}*/
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* FOR TEST ONLY TODO
	if(htim->Instance == TIM4)
	{
		hello++;
	}
	if(hello == 6)
	{
		helloka++;
	}*/
}

uint16_t read_last(cBuff *buff_c)
{
	uint16_t lastData = buff_c->buffer[buff_c->head-1];
	return lastData;
}
