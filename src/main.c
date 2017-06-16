


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
char msg_string[8];
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

			JSON_Value *value_array = json_value_init_array();
			JSON_Array *send_array = json_value_get_array(value_array);

			while(MSG_SENT == 0)
			{
				/* Buffer0 data */
				pop_cBuff(&gBuffer0, &msg_data);
				ch_data = (uint8_t) msg_data;
				msg_string[0] = ch_data;
				ch_data = (uint8_t) msg_data >> 8;
				msg_string[1] = ch_data;

				/* Buffer1 data */
				pop_cBuff(&gBuffer1, &msg_data);
				ch_data = (uint8_t) msg_data;
				msg_string[2] = ch_data;
				ch_data = (uint8_t) msg_data >> 8;
				msg_string[3] = ch_data;

				/* Buffer2 data */
				pop_cBuff(&gBuffer2, &msg_data);
				ch_data = (uint8_t) msg_data;
				msg_string[4] = ch_data;
				ch_data = (uint8_t) msg_data >> 8;
				msg_string[5] = ch_data;

				/* Buffer3 data */
				parse_flag = pop_cBuff(&gBuffer3, &msg_data);
				ch_data = (uint8_t) msg_data;
				msg_string[6] = ch_data;
				ch_data = (uint8_t) msg_data >> 8;
				msg_string[7] = ch_data;

				/* Send message if there is not any data */
				if(parse_flag == cBuff_EMPTY)
				{

					sendData(1234, 00000001, "2017.06.09.15:45:12",
							32, 10, 50, "2017.06.09.16:03:10", 3, 125, value_array);

					mfb.MSG_FLAG = false;
					MSG_SENT = 1;
				}

				/* Add measurement to array */
				json_array_append_string(send_array, msg_string);
			}

			/* Reset SENT FLAG */
			MSG_SENT = 0;
			json_value_free(value_array);
		}

	}

}

/*
 * ["b1b2
uint16_t meas_value;
uint8_t b1=meas_value;
uint8_t b2=meas_value>>8;
*/

