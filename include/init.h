#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "edge_comm/edge_comm.h"

typedef enum{
	INIT_OK					= 0,
	INIT_SD_ERROR			= 1,
	HAL_RCC_ERROR			= 2,
	HAL_ADC_ERROR			= 3,
	HAL_I2C_ERROR			= 4,
	HAL_UART_ERROR			= 5,
	HAL_RTC_ERROR			= 6,
	HAL_TIM_ERROR			= 7,
	CONFIG_READ_ERROR		= 8,
	NO_CONFIG_ERROR			= 9,
	INVALID_SETTINGS_ERROR 	= 10
}InitStateCode;

/* Global variables */
extern uint16_t threshold_min;
extern uint16_t threshold_max;
extern uint16_t meas_timeout;
extern uint16_t env_meas_freq;
extern char mqtt_host[50];
extern char port[5];
extern char gsm_apn[30];
extern uint8_t ping_retry;
extern char client_name[20];

extern ADC_HandleTypeDef hadc3;

extern I2C_HandleTypeDef hi2c2;

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart3;

void init_settings();
void restart_init();
void load_config_sd();

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC3_Init(void);
static void MX_I2C2_Init(void);
static void MX_RTC_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART3_UART_Init(void);

#endif /* __INIT_H */
