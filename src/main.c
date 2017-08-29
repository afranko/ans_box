/* Includes ------------------------------------------------------------------ */
#include "statemachine.h"

ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

Settings_HandleTypeDef config_s;

/*----------------------------------*/
/* 		   		Table	 			*/
/*----------------------------------*/
/*	 cont_0		gBuffer0	PF7 	*/
/*	 cont_1		gBuffer1	PF8		*/
/*	 cont_2		gBuffer2	PF9		*/
/*	 cont_3		gBuffer3	PF10	*/
/*----------------------------------*/

cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;

uint16_t env_counter = 0;
bool timeFlag = false;
bool envMeasFlag = false;
bool HOLDFlag = false;

extern serialBuff uartBuffer;
extern GSM_MQTT MQTT;
extern uint8_t bcounter;
extern meas_flag_block mfb;
extern miniBuff serial_time_buff;
extern uint8_t serial_time_value;
extern bool commandFlag;

int main(void)
{

	/* Initialization */
	init_settings();
	init_meas_flag_block(&mfb);
	timeFlag = true;

	/* Init circular buffers */
	init_cBuff(&cont_0);
	init_cBuff(&cont_1);
	init_cBuff(&cont_2);
	init_cBuff(&cont_3);
	init_cBuff(&gBuffer0);
	init_cBuff(&gBuffer1);
	init_cBuff(&gBuffer2);
	init_cBuff(&gBuffer3);

	/* Set external sensor*/
	setMAX(&hspi1);

 	CommInit(&huart2, 60);

 	HAL_ADC_Start_IT(&hadc3);

 	while(MQTT.MQTT_Flag != true) //TODO - led égjen/vagy ne
 	{
 		MQTTProcessing();
 	}

	/* Waiting for the first conversation */
	while(bcounter == 0);

	/* Go to "Start" state */
	p_start();

	/* Main Loop */
	while(1)
	{
		if(commandFlag == true)
		{
			if((mfb.duration == 0) && (mfb.S_MEAS_FLAG == false))
			{
				HOLDFlag = true;
				mfb.S_MEAS_FLAG = true;
				mfb.E_MEAS_FLAG = true;
			}
			commandFlag = false;
		}

		/* Aggregate Measurement Data */
		meas_datamove();

		/* StateMachine */
		if(HOLDFlag != true)
		{
			statemachine_process();
		}

		/* MQTT Communication */
		MQTTProcessing();

		/* Environment Measurement */
		if(envMeasFlag == true)
		{
			envMeas(&hrtc, &hi2c2, &hspi1);
			envMeasFlag = false;
		}

		/* Send Movement message */
		if(mfb.MSG_FLAG == true)
		{
			movMeas(&hrtc);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(timeFlag == true)
		{
			serialGet();
		}
		else
		{
			push_miniBuff(&serial_time_buff, serial_time_value);
			HAL_UART_Receive_IT(&huart2, &serial_time_value, 1);
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(env_counter == config_s.env_meas_freq)
		{
			envMeasFlag = true;
			env_counter = 0;
		}
		env_counter++;
	}
}

/*
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == MQTT.gsm_uart->Instance)
	{
		if(txString.buffer != NULL)
		{
			uint8_t txr_data = tx_read(&txString);
			HAL_UART_Transmit_IT(huart, &txr_data, 1);
			tx_go(&txString);
		}
	}
}
*/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode)
	{
		if(timeFlag)
			HAL_UART_Receive_IT(huart, &uartBuffer.temp_val, sizeof(char));
		else
			HAL_UART_Receive_IT(&huart2, &serial_time_value, 1);
	}
}
