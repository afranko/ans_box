#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "edge_comm/edge_comm.h"

typedef struct settings_t
{
	uint16_t threshold_min;		// [min 0] -
	uint16_t threshold_max;		// [max 4095]
	uint16_t meas_timeout; 		// [ms] - 2 byte should be enough - max. 65 sec
	uint16_t meas_offset;		// [ms]
	uint16_t env_meas_freq;		// [s]
	char mqtt_host[50];
	char port[5];
	char gsm_apn[30];
	uint8_t ping_retry;
	char client_name[17];
}Settings_HandleTypeDef;

typedef enum{
	INIT_OK					= 0,
	INIT_SD_ERROR			= 1,
	HAL_RCC_ERROR			= 2,
	HAL_ADC_ERROR			= 3,
	HAL_I2C_ERROR			= 4,
	HAL_UART_ERROR			= 5,
	HAL_RTC_ERROR			= 6,
	HAL_TIM_ERROR			= 7,
	HAL_SPI_ERROR			= 8,
	CONFIG_READ_ERROR		= 9,
	NO_CONFIG_ERROR			= 10,
	INVALID_SETTINGS_ERROR 	= 11,
	RTC_NOT_SET				= 12,
	CONFIG_WRITE_ERROR		= 13,
	CONFIG_CLOSE_ERROR		= 14
}InitStateCode;

extern Settings_HandleTypeDef config_s;

extern ADC_HandleTypeDef hadc3;

extern I2C_HandleTypeDef hi2c2;

extern SPI_HandleTypeDef hspi1;

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart2;

void init_settings(void);
void restart_init(void);
void load_config_sd(void);
void config_error(void);

void setRTC(void);
void checkRTC(void);
void getSerialTime(void);
bool check_string(char *reply);
void setMAX(SPI_HandleTypeDef *hspi);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC3_Init(void);
void MX_I2C2_Init(void);
void MX_RTC_Init(void);
void MX_SDIO_SD_Init(void);
void MX_SPI1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM4_Init(void);
void MX_USART2_UART_Init(void);

#endif /* __INIT_H */
