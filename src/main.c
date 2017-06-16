


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

char sendString[21], asciibuff[5];

cBuff_State ParseFlag;
uint16_t data_0, data_1 ,data_2, data_3;
uint8_t MSG_SENT = 0;

/* Notice that cont_0 and gBuffer0 are lead buffers
 * It's supposed that other buffers acts like their lead buffers
 * For example: if cont_0.head = 231 => cont_1.head = 231 (changes should end in 40 ms)
 */

extern uint32_t bcounter; /*TODO Ha tudunk másra triggerelni, akkor is oké.
pl.: RTC MQTT-n keresztül, vagy mégjobb: offset_100HZ hogy mindig legyen elég adat */

extern meas_flag_block mfb;

int main(void)
{
	/* Initialization */
	init_settings();
	init_meas_flag_block(&mfb);

	/* Init circular buffers */
	init_cBuff(&cont_0);
	init_cBuff(&cont_1);
	init_cBuff(&cont_2);
	init_cBuff(&cont_3);
	init_cBuff(&gBuffer0);
	init_cBuff(&gBuffer1);
	init_cBuff(&gBuffer2);
	init_cBuff(&gBuffer3);

 	//CommInit(&huart3, 60);

	HAL_ADC_Start_IT(&hadc3);//TODO it? HAL_ADC_Start_IT

	/* Waiting for the first conversation */
	while(bcounter == 0);

	/* Go to "Start" state */
	p_start();

	//TODO még mindig vannak bajok
	while(1)
	{

		statemachine_process();
		//MQTTProcessing();

		/* Send message */
		if(mfb.MSG_FLAG == true)
		{
			int zota = 0;
			zota++;
			/*
			JSON_Value *value_array = json_value_init_array();
			JSON_Array *send_array = json_value_get_array(value_array);


			while(MSG_SENT == 0)
			{
				pop_cBuff(&gBuffer0, &data_0);
				pop_cBuff(&gBuffer1, &data_1);
				pop_cBuff(&gBuffer2, &data_2);
				ParseFlag = pop_cBuff(&gBuffer3, &data_3);

				if(ParseFlag == cBuff_EMPTY)
				{

					ParseFlag = cBuff_OK;
					sendData(1234, 00000001, "2017.06.09.15:45:12",
							32, 10, 50, "2017.06.09.16:03:10", 3, 125, value_array);

					MSG_FLAG = false;
					MSG_SENT = 1;
				}

				dataparse(data_0, asciibuff);
				strcpy(sendString, asciibuff);

				dataparse(data_1, asciibuff);
				strcat(sendString, asciibuff);

				dataparse(data_2, asciibuff);
				strcat(sendString, asciibuff);

				dataparse(data_3, asciibuff);
				strcat(sendString, asciibuff);

				json_array_append_string(send_array, sendString);


			}
			//elvileg nem kell

			//json_array_clear(send_array);
			json_value_free(value_array); //TODO, ez se kell
			*/
		}

	}

}

/*
 * ["b1b2
uint16_t meas_value;
uint8_t b1=meas_value;
uint8_t b2=meas_value>>8;
*/
//TODO Csabát megbaszni, szárazon!!!!444!!!

