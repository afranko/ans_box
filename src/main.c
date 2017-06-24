


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

cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;

cBuff_State parse_flag;
uint16_t msg_data;
uint8_t ch_data;
char msg_string[20];
char itoa_subbuf[5];
uint8_t MSG_SENT = 0;
extern GSM_MQTT MQTT;

extern uint8_t bcounter; /*TODO Ha tudunk másra triggerelni, akkor is oké.
pl.: RTC MQTT-n keresztül, vagy mégjobb: offset_100HZ hogy mindig legyen elég adat */

extern meas_flag_block mfb;

extern miniBuff serial_time_buff;
extern uint8_t serial_time_value;
bool timeFlag = false;

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

 	CommInit(&huart3, 60);

 	while(MQTT.MQTT_Flag != true)
 	{
 		MQTTProcessing();
 	}

	HAL_ADC_Start_IT(&hadc3);//TODO it? HAL_ADC_Start_IT

	/* Waiting for the first conversation */
	while(bcounter == 0)
	{
		MSG_SENT = 0;
	}

	/* Go to "Start" state */
	p_start();

	//TODO még mindig vannak bajok
	while(1)
	{

		statemachine_process();
		MQTTProcessing();


		/* Send message */
		if(mfb.MSG_FLAG == true)
		{

			JSON_Value *value_array = json_value_init_array();
			JSON_Array *send_array = json_value_get_array(value_array);

			while(MSG_SENT == 0)
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

					sendMovementMessage(mfb.duration ,value_array);
					//sendEnvironmentMessage();

					mfb.MSG_FLAG = false;
					MSG_SENT = 1;
					mfb.CLEAR_FLAG = true;
				}
				else
				{
					/* Add measurement to array */
					json_array_append_string(send_array, msg_string);
					/* Free array -> in sendMovementMessage() function */
				}
			}

			/* Reset SENT FLAG */
			MSG_SENT = 0;
		}

	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		if(timeFlag == true)
		{
			serialGet();
		}
		else
		{
			push_miniBuff(&serial_time_buff, serial_time_value);
			HAL_UART_Receive_IT(&huart3, &serial_time_value, 1);
		}
	}
}

