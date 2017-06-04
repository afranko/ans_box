/*
 * measurement.c
 *
 */

#include "measurement.h"

/* Counter for ADC's channels */
uint32_t bcounter = 0;

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
}

uint16_t read_last(cBuff *buff_c)
{
	uint16_t lastData = buff_c->buffer[(buff_c->head)-1];
	return lastData;
}
