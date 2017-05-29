/*
 * init.c
 */

#include "init.h"

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

FATFS FS;
FIL CONFIG_F;

InitStateCode config_status;

void init_settings()
{
	  HAL_Init();
	  SystemClock_Config();
	  MX_GPIO_Init();
	  MX_ADC3_Init();
	  MX_I2C2_Init();
	  MX_RTC_Init();
	  MX_SDIO_SD_Init();
	  MX_TIM2_Init();
	  MX_TIM4_Init();
	  MX_USART3_UART_Init();
	  MX_FATFS_Init();

	  /* Load default settings from SD */
	  load_config_sd();

	  if(config_status != INIT_OK)
	  {
		  restart_init();
	  }
}

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
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    config_status = HAL_RCC_ERROR;
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
	  config_status = HAL_RCC_ERROR;
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
	  config_status = HAL_RCC_ERROR;
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
static void MX_ADC3_Init(void)
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
	  config_status = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  config_status = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  config_status = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  config_status = HAL_ADC_ERROR;
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
	  config_status = HAL_ADC_ERROR;
  }

}

/* I2C2 init function */
static void MX_I2C2_Init(void)
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
	  config_status = HAL_I2C_ERROR;
  }
}

/* RTC init function */
static void MX_RTC_Init(void)
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
	  config_status = HAL_RTC_ERROR;
  }
}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 42000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 42000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 2000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
	  config_status = HAL_UART_ERROR;
  }

}

static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* H407 USER_LED Init */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);
}

void restart_init()
{
	for(int i = 0; i < 15; i++)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(500);
	}

	/* Waiting for SD change and manual reset */
	if(config_status == NO_CONFIG_ERROR) //TODO
	{ //nagyon TODO,nem csak no_config, hanem amire értelmes
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
		while(1);
	}

	HAL_NVIC_SystemReset();
}

void write_default_cfgfile()
{
	JSON_Value *default_conf_value = json_value_init_object();
	JSON_Object *default_conf_object = json_value_get_object(default_conf_value);

	json_object_set_number(default_conf_object, "threshold_min", 0);
	json_object_set_number(default_conf_object, "threshold_max", 4095);
	json_object_set_number(default_conf_object, "meas_timeout", 1000);
	json_object_set_number(default_conf_object, "env_meas_freq", 100);
	json_object_set_string(default_conf_object, "mqtt_host", "mqtthost.name");
	json_object_set_string(default_conf_object, "port", "1883");
	json_object_set_string(default_conf_object, "gsm_apn", "provider.apn");
	json_object_set_number(default_conf_object, "ping_retry", 2);
	json_object_set_string(default_conf_object, "client_name", "unique_identifier");

	char *serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(default_conf_value);

	UINT backp;
	UINT siz = (UINT)strlen(serialized_string);
	f_open(&CONFIG_F, "config.json", FA_CREATE_NEW | FA_WRITE);
	f_write(&CONFIG_F, serialized_string, siz, &backp);

	json_free_serialized_string(serialized_string);
	json_value_free(default_conf_value);
}

void load_config_sd()
{
	FRESULT f_res;

	/* Mount SD */
	if(f_mount(&FS, "SD1", 1) != FR_OK) //TODO default drive-e?
	{
		config_status = INIT_SD_ERROR;
		return;
	}

	f_res = f_open(&CONFIG_F, "config.json", FA_OPEN_EXISTING | FA_READ);

	/* SD physical or any other error */
	if((f_res != FR_OK ) && (f_res != FR_NO_FILE))
	{
		config_status = CONFIG_READ_ERROR;
	}

	/* SD is OK, but there isn't any config file */
	else if(f_res == FR_NO_FILE)
		{
			write_default_cfgfile();
			config_status = NO_CONFIG_ERROR;
		}
		else
		{
			char in_buff[500];
			UINT chpoint;
			UINT lnpoint = f_size(&CONFIG_F);

			if(f_read(&CONFIG_F, in_buff, lnpoint, &chpoint) == FR_OK)
			{
				const char *mqtt_host_p = NULL;
				const char *port_p = NULL;
				const char *gsm_apn_p = NULL;
				const char *client_name_p = NULL;

				if (lnpoint != chpoint)
				{
					//TODO na ide kell valamit kitalálni
				}

				JSON_Value *conf_setup = json_parse_string(in_buff);

				/* Get config settings from JSON */

				threshold_min = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_min");
				threshold_max = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_max");
				meas_timeout = (uint16_t)json_object_get_number(json_object(conf_setup), "meas_timeout");
				env_meas_freq = (uint16_t)json_object_get_number(json_object(conf_setup), "env_meas_freq");
				mqtt_host_p = json_object_get_string(json_object(conf_setup), "mqtt_host");
				port_p = json_object_get_string(json_object(conf_setup), "port");
				gsm_apn_p = json_object_get_string(json_object(conf_setup), "gsm_apn");
				ping_retry = (uint8_t)json_object_get_number(json_object(conf_setup), "ping_retry");
				client_name_p = json_object_get_string(json_object(conf_setup), "client_name");

				strcpy(mqtt_host, mqtt_host_p);
				strcpy(port, port_p);
				strcpy(gsm_apn, gsm_apn_p);
				strcpy(client_name, client_name_p);

				json_value_free(conf_setup);

				//checkup();	//TODO
			}
			else
			{
				config_status = CONFIG_READ_ERROR;
			}
		}

	f_close(&CONFIG_F);
	f_mount(NULL, "", 1);
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
