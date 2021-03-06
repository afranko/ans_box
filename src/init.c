/*
 * init.c
 */

#include "init.h"

typedef enum{
	INIT_OK,
	INIT_SD_ERROR,
	HAL_RCC_ERROR,
	HAL_ADC_ERROR,
	HAL_I2C_ERROR,
	HAL_UART_ERROR,
	HAL_RTC_ERROR,
	HAL_TIM_ERROR,
	HAL_SPI_ERROR,
	SD_MOUNT_ERROR,
	SD_UNMOUNT_ERROR,
	SD_OPEN_ERROR,
	SD_CLOSE_ERROR,
	SD_READ_ERROR,
	SD_WRITE_ERROR,
	SD_EMPTY_ERROR,
	SD_BSD_ERROR,
	RTC_NOT_SET
}InitStateCode;

SD_HandleTypeDef 		hsd;
HAL_SD_CardInfoTypedef 	SDCardInfo;
FATFS 					FS;
FIL 					CONFIG_F;
InitStateCode			configResult;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC3_Init(void);
static void MX_I2C2_Init(void);
static void MX_RTC_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);

static void read_config(void);
static void write_default_config(void);
static void is_RTC_Set(void);

/**
 * Public Functions
 */

/**
 * @brief This function initializes the system (e.g. IO ports, GPIO pins)
 * @warning It has to be called in the first place in the MAIN function.
 */
void init_settings(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_ADC3_Init();
	MX_I2C2_Init();
	MX_RTC_Init();
	MX_SDIO_SD_Init();
	MX_SPI1_Init();
	MX_TIM2_Init();
	MX_TIM4_Init();
	MX_USART2_UART_Init();
	MX_FATFS_Init();

	is_RTC_Set();
	read_config();

	if(configResult != INIT_OK)
	{
		switch(configResult)
		{
		case RTC_NOT_SET:
			isRTCSet = false;
			break;

		default:
			restart_system();
			break;
		}
	}
}

/**
 * Private Functions
 */

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 12;	// It was 7 before
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    configResult = HAL_RCC_ERROR;
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
	  configResult = HAL_RCC_ERROR;
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
	  configResult = HAL_RCC_ERROR;
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC3 init function */
void MX_ADC3_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ENABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 4;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
	  configResult = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  configResult = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  configResult = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  configResult = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  configResult = HAL_ADC_ERROR;
  }

}

/* I2C2 init function */
void MX_I2C2_Init(void)
{
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
	  configResult = HAL_I2C_ERROR;
  }
}

/* RTC init function */
void MX_RTC_Init(void)
{
    /**Initialize RTC Only
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
	  configResult = HAL_RTC_ERROR;
  }
}

/* SDIO init function */
void MX_SDIO_SD_Init(void)
{
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 2;	// It was zero before
  if(BSP_SD_Init() != MSD_OK)
  {
	  configResult = SD_BSD_ERROR;
  }
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

	if(HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		configResult = HAL_SPI_ERROR;
	}
}

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }
  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

}

/* TIM4 init function */
void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 41999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }
  if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
  {
	  configResult = HAL_TIM_ERROR;
  }

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;		//115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; //UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
	  configResult = HAL_UART_ERROR;
  }

}

/* GPIO init function */
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Init GPIO I/O pins */
  GPIO_InitTypeDef GPIO_InitStruct;

  /* H407 USER_LED Init */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);

}

/* Read config file from SD card */
void read_config(void)
{
	FRESULT f_res;

	/* Try to mount SD */
	if(f_mount(&FS, "0:", 1) != FR_OK)
	{
		configResult = SD_MOUNT_ERROR;
		return;
	}

	f_res = f_open(&CONFIG_F, "config.json", FA_OPEN_EXISTING | FA_READ);

	/* SD physical or any other error */
	if((f_res != FR_OK ) && (f_res != FR_NO_FILE))
	{
		configResult = SD_OPEN_ERROR;
		return;
	}

	/* Mounting SD was successful, but there isn't any config file */
	if(f_res == FR_NO_FILE)
	{
		write_default_config();
		configResult = SD_EMPTY_ERROR;
	}

	/* Reading config from SD card */
	if(f_res == FR_OK)
	{
		char in_buff[512];
		UINT chpoint;
		UINT lnpoint = f_size(&CONFIG_F);

		if(f_read(&CONFIG_F, in_buff, lnpoint, &chpoint) == FR_OK)
		{
			if (lnpoint != chpoint)
			{
				configResult = SD_READ_ERROR;
				return;
			}

			const char *mqtt_host_p = NULL;
			const char *port_p = NULL;
			const char *gsm_apn_p = NULL;
			const char *client_name_p = NULL;

			/* Get config settings from JSON */
			JSON_Value *conf_setup = json_parse_string(in_buff);

			/* See below */
			//#error "There is no error handling if the user gave wrong values in the config file"
			/*
			 * Warning: There is no error handling if the user gave wrong values in the config file
			 * For example: doubles as number, or longer string then the excepted size
			 */

			client_name_p = json_object_get_string(json_object(conf_setup), "client_name");
			mqtt_host_p = json_object_get_string(json_object(conf_setup), "mqtt_host");
			port_p = json_object_get_string(json_object(conf_setup), "port");
			gsm_apn_p = json_object_get_string(json_object(conf_setup), "gsm_apn");


			hconfig = (Settings_HandleTypeDef*) malloc(sizeof(Settings_HandleTypeDef) + strlen(mqtt_host_p) +1U);

			hconfig->threshold_min = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_min");
			hconfig->threshold_max = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_max");
			hconfig->meas_timeout = (uint16_t)json_object_get_number(json_object(conf_setup), "meas_timeout");
			hconfig->meas_offset = (uint16_t)json_object_get_number(json_object(conf_setup), "meas_offset");
			hconfig->env_meas_freq = (uint16_t)json_object_get_number(json_object(conf_setup), "env_meas_freq");
			hconfig->ping_retry = (uint8_t)json_object_get_number(json_object(conf_setup), "ping_retry");

			strcpy(hconfig->mqtt_host, mqtt_host_p);
			strcpy(hconfig->port, port_p);
			strcpy(hconfig->gsm_apn, gsm_apn_p);
			strcpy(hconfig->client_name, client_name_p);

			json_value_free(conf_setup);
		}
		else
			configResult = SD_READ_ERROR;
	}

	if(f_close(&CONFIG_F) != FR_OK)
		configResult = SD_CLOSE_ERROR;

	if(f_mount(NULL, "0:", 1) != FR_OK)
		configResult = SD_UNMOUNT_ERROR;
}

/* Checks if RTC is not set */
void is_RTC_Set(void)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	if(date.Year < 18U)
		configResult = RTC_NOT_SET;
}

/* Restart function due to the errors or wrong configuration*/
void restart_system(void)
{
	for(short i = 0; i < 20; i++)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(500);
	}
	HAL_NVIC_SystemReset();
}

void write_default_config(void)
{
	JSON_Value *default_conf_value = json_value_init_object();
	JSON_Object *default_conf_object = json_value_get_object(default_conf_value);

	json_object_set_number(default_conf_object, "threshold_min", 500);
	json_object_set_number(default_conf_object, "threshold_max", 3800);
	json_object_set_number(default_conf_object, "meas_timeout", 10000);
	json_object_set_number(default_conf_object, "meas_offset", 1000);
	json_object_set_number(default_conf_object, "env_meas_freq", 1800);
	json_object_set_string(default_conf_object, "mqtt_host", "mantis1.tmit.bme.hu");
	json_object_set_string(default_conf_object, "port", "1883");
	json_object_set_string(default_conf_object, "gsm_apn", "ibox.tim.it");
	json_object_set_number(default_conf_object, "ping_retry", 3);
	json_object_set_string(default_conf_object, "client_name", "ANS_MEAS_BOX_001");

	char *serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(default_conf_value);

	UINT backp;
	UINT siz = (UINT)strlen(serialized_string);

	if(f_open(&CONFIG_F, "config.json", FA_CREATE_NEW | FA_WRITE) != FR_OK)
		configResult = SD_WRITE_ERROR;
	else
	{
		f_write(&CONFIG_F, serialized_string, siz, &backp);
		if(siz != backp)
		{
			configResult = SD_WRITE_ERROR;
			return;
		}
	}

	json_free_serialized_string(serialized_string);
	json_value_free(default_conf_value);
}
