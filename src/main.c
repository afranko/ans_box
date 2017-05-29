


/* Includes ------------------------------------------------------------------*/
#include "init.h"
#include "statemachine.h"

ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart3;

/* Global variables */
uint16_t threshold_min;		// [min 0] -
uint16_t threshold_max;		// [max 4095]
uint16_t meas_timeout; 		// [ms]
uint16_t env_meas_freq;		// [s]
char mqtt_host[50];
char port[5];
char gsm_apn[30];
uint8_t ping_retry;
char client_name[20];

int main(void)
{
	/* Initialization */
	init_settings();
	//comm_init();

	p_start();

	while(1)
	{

		//mqtt_process();
		statemachine_process();
	}

}
