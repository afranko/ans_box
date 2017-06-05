


/* Includes ------------------------------------------------------------------*/
#include "statemachine.h"

ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart3;

Settings_HandleTypeDef config_s;

/*----------------------*/
/* 		   Table 		*/
/*----------------------*/
/*	 cont_0		PF7 	*/
/*	 cont_1		PF8 	*/
/*	 cont_2		PF9 	*/
/*	 cont_3		PF10 	*/
/*----------------------*/
//TODO

cBuff cont_0, cont_1, cont_2, cont_3, gBuffer;

extern uint32_t bcounter;

int main(void)
{
	/* Initialization */
	init_settings();

	/* Init circular buffers */
	init_cBuff(&cont_0);
	init_cBuff(&cont_1);
	init_cBuff(&cont_2);
	init_cBuff(&cont_3);
	init_cBuff(&gBuffer);

	//comm_init();

	HAL_ADC_Start_IT(&hadc3);//TODO it? HAL_ADC_Start_IT

	/* Waiting for the first conversation */
	while(bcounter == 0);

	/* Go to "Start" state */
	p_start();

	while(1)
	{

		//mqtt_process();
		statemachine_process();

	}

}

