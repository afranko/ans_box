#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "parson.h"
#include "circbuff.h"

typedef struct settings_t
{
	uint16_t threshold_min;		// [min 0] -
	uint16_t threshold_max;		// [max 4095]
	uint16_t meas_timeout; 		// [ms] - 2 byte should be enough - max. 65 sec
	uint16_t meas_offset;		// [ms]
	uint16_t env_meas_freq;		// [s]
	uint8_t ping_retry;
	char port[5];
	char gsm_apn[64];
	char client_name[24];
	char mqtt_host[];
}Settings_HandleTypeDef;

extern Settings_HandleTypeDef 	*hconfig;
extern ADC_HandleTypeDef 		hadc3;
extern I2C_HandleTypeDef 		hi2c2;
extern SPI_HandleTypeDef 		hspi1;
extern RTC_HandleTypeDef 		hrtc;
extern TIM_HandleTypeDef 		htim2;
extern TIM_HandleTypeDef 		htim4;
extern UART_HandleTypeDef 		huart2;

extern bool						isRTCSet;

void init_settings(void);
void restart_system(void);

#endif /* __INIT_H */
