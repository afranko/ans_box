/*
 * init.c
 */

#include "init.h"

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

FATFS FS;
FIL CONFIG_F;

InitStateCode config_status;

miniBuff serial_time_buff;
uint8_t serial_time_value;

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
	  MX_USART2_UART_Init();
	  MX_FATFS_Init();

	  /* Load default settings from SD */
	  load_config_sd();

	  /* Check if RTC is set */
	  checkRTC();

	  if(config_status == RTC_NOT_SET)
	  {
	  	  setRTC();
	  }

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
	  config_status = HAL_I2C_ERROR;
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
	  config_status = HAL_RTC_ERROR;
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
  hsd.Init.ClockDiv = 0;
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
	  config_status = HAL_TIM_ERROR;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }
  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
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
  if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
  {
	  config_status = HAL_TIM_ERROR;
  }

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
	  config_status = HAL_UART_ERROR;
  }

}

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

  /* Modem EN\ pin init */

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);

}

void config_error()
{
	config_status = NO_CONFIG_ERROR;
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

	//TODO villogáskódok??

	HAL_NVIC_SystemReset();
}

void write_default_cfgfile()
{
	JSON_Value *default_conf_value = json_value_init_object();
	JSON_Object *default_conf_object = json_value_get_object(default_conf_value);

	json_object_set_number(default_conf_object, "threshold_min", 500);
	json_object_set_number(default_conf_object, "threshold_max", 3800);
	json_object_set_number(default_conf_object, "meas_timeout", 10000);
	json_object_set_number(default_conf_object, "meas_offset", 1000);
	json_object_set_number(default_conf_object, "env_meas_freq", 1800);
	json_object_set_string(default_conf_object, "mqtt_host", "mantis1.tmit.bme.hu");
	json_object_set_string(default_conf_object, "port", "80");
	json_object_set_string(default_conf_object, "gsm_apn", "internet.telekom");
	json_object_set_number(default_conf_object, "ping_retry", 3);
	json_object_set_string(default_conf_object, "client_name", "ANS_MEAS_BOX_001");

	char *serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(default_conf_value);

	UINT backp;
	UINT siz = (UINT)strlen(serialized_string);

	if(f_open(&CONFIG_F, "config.json", FA_CREATE_NEW | FA_WRITE) != FR_OK)
	{
		config_status = CONFIG_WRITE_ERROR;
	}
	else
	{
		f_write(&CONFIG_F, serialized_string, siz, &backp);
		if(siz != backp)
		{
			config_status = CONFIG_WRITE_ERROR;
			return;
		}
	}

	json_free_serialized_string(serialized_string);
	json_value_free(default_conf_value);
}

void load_config_sd()
{
	FRESULT f_res;

	/* Mount SD */
	if(f_mount(&FS, "SD1", 1) != FR_OK)
	{
		config_status = INIT_SD_ERROR;
		return;
	}

	f_res = f_open(&CONFIG_F, "config.json", FA_OPEN_EXISTING | FA_READ);

	/* SD physical or any other error */
	if((f_res != FR_OK ) && (f_res != FR_NO_FILE))
	{
		config_status = CONFIG_READ_ERROR;
		return;
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
					config_status = CONFIG_READ_ERROR;
					return;
				}

				/* Get config settings from JSON */
				JSON_Value *conf_setup = json_parse_string(in_buff);

				config_s.threshold_min = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_min");
				config_s.threshold_max = (uint16_t)json_object_get_number(json_object(conf_setup), "threshold_max");
				config_s.meas_timeout = (uint16_t)json_object_get_number(json_object(conf_setup), "meas_timeout");
				config_s.meas_offset = (uint16_t)json_object_get_number(json_object(conf_setup), "meas_offset");
				config_s.env_meas_freq = (uint16_t)json_object_get_number(json_object(conf_setup), "env_meas_freq");
				mqtt_host_p = json_object_get_string(json_object(conf_setup), "mqtt_host");
				port_p = json_object_get_string(json_object(conf_setup), "port");
				gsm_apn_p = json_object_get_string(json_object(conf_setup), "gsm_apn");
				config_s.ping_retry = (uint8_t)json_object_get_number(json_object(conf_setup), "ping_retry");
				client_name_p = json_object_get_string(json_object(conf_setup), "client_name");

				strcpy(config_s.mqtt_host, mqtt_host_p);
				strcpy(config_s.port, port_p);
				strcpy(config_s.gsm_apn, gsm_apn_p);
				strcpy(config_s.client_name, client_name_p);

				json_value_free(conf_setup);
			}
			else
			{
				config_status = CONFIG_READ_ERROR;
			}
		}

	if(f_close(&CONFIG_F) != FR_OK)
	{
		config_status = CONFIG_CLOSE_ERROR;
	}

	if(f_mount(NULL, "SD1", 1) != FR_OK)
	{
		config_status = CONFIG_CLOSE_ERROR;
	}
}

void checkRTC(void)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	if(date.Year < 17)
	{
		config_status = RTC_NOT_SET;
	}
	return;
}

void setRTC(void)
{
	char  s_string[] = "AT+CIPSTART=\"TCP\",\"www.bme.hu\",\"80\"\r\n";
	char http_head_msg[] = "HEAD /lab01/ HTTP/1.1\r\n" "Host: bme.hu\r\n";
	char s_prov[100];
	char buff_str[256];
	uint8_t p_buffs  = 0, p_butt = 0;
	char setter_str[26];
	uint32_t the_tick = 0;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	char univ_str[3];

	strcpy(s_prov, "AT+CSTT=\"");
	strcat(s_prov, config_s.gsm_apn);
	strcat(s_prov, "\"\r\n");

	init_miniBuff(&serial_time_buff);
	HAL_UART_Receive_IT(&huart2, &serial_time_value, 1);

	HAL_UART_Transmit(&huart2, "AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "ATE1\r\n", strlen("ATE0\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}


	HAL_UART_Transmit(&huart2, "AT+CREG?\r\n", strlen("AT+CREG?\r\n"), HAL_MAX_DELAY);

	HAL_UART_Transmit(&huart2, "AT+CIPMUX=0\r\n", strlen("AT+CIPMUX=0\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT+CIPMODE=1\r\n", strlen("AT+CIPMODE=1\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT+CGATT=1\r\n", strlen("AT+CGATT=1\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT+CIPSTATUS\r\n", strlen("AT+CIPSTATUS\r\n"), HAL_MAX_DELAY);


	HAL_UART_Transmit(&huart2, s_prov, strlen(s_prov), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT+CIICR\r\n", strlen("AT+CIICR\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, "AT+CIFSR\r\n", strlen("AT+CIFSR\r\n"), HAL_MAX_DELAY);
	if(check_string("") != true)
	{
		return;
	}


	HAL_UART_Transmit(&huart2, s_string, strlen(s_string), HAL_MAX_DELAY);
	if(check_string("CONNECT") != true)
	{
		return;
	}

	HAL_UART_Transmit(&huart2, http_head_msg, strlen(http_head_msg), HAL_MAX_DELAY);
	the_tick = HAL_GetTick();
	while((HAL_GetTick() - the_tick) < 30000);

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	while(pop_miniBuff(&serial_time_buff,  &(buff_str[p_buffs])) != miniBuff_EMPTY)
	{
		p_buffs++;
	}
	char *point_s = strstr(buff_str, "Date: ");
	for(p_buffs = 0, p_butt = 6; p_buffs < 26; p_buffs++, p_butt++)
	{
		setter_str[p_buffs] = point_s[p_butt];
	}

	switch(setter_str[1])
	{
	case 'o':
		date.WeekDay = RTC_WEEKDAY_MONDAY;
		break;
	case 'u':
		if(setter_str[0] == 'T')
		{
			date.WeekDay = RTC_WEEKDAY_TUESDAY;
		}
		else
		{
			date.WeekDay = RTC_WEEKDAY_SUNDAY;
		}
		break;
	case 'e':
		date.WeekDay = RTC_WEEKDAY_WEDNESDAY;
		break;
	case 'h':
		date.WeekDay = RTC_WEEKDAY_THURSDAY;
		break;
	case 'r':
		date.WeekDay = RTC_WEEKDAY_FRIDAY;
		break;
	case 'a':
		date.WeekDay = RTC_WEEKDAY_SATURDAY;
		break;
	}

	univ_str[0] = setter_str[5];
	univ_str[1] = setter_str[6];
	date.Date = atoi(univ_str);

	switch(setter_str[8])
	{
	case 'J':
		if(setter_str[9] == 'u')
		{
			if(setter_str[10] == 'n')
			{
				date.Month = RTC_MONTH_JUNE;
			}
			else
			{
				date.Month = RTC_MONTH_JULY;
			}
		}
		else
		{
			date.Month = RTC_MONTH_JANUARY;
		}
		break;
	case 'F':
		date.Month = RTC_MONTH_FEBRUARY;
		break;
	case 'M':
		if(setter_str[10] == 'r')
		{
			date.Month = RTC_MONTH_MARCH;
		}
		else
		{
			date.Month = RTC_MONTH_MAY;
		}
		break;
	case 'A':
		if(setter_str[9] == 'p')
		{
			date.Month = RTC_MONTH_APRIL;
		}
		else
		{
			date.Month = RTC_MONTH_AUGUST;
		}
		break;
	case 'S':
		date.Month = RTC_MONTH_SEPTEMBER;
		break;
	case 'O':
		date.Month = RTC_MONTH_OCTOBER;
		break;
	case 'N':
		date.Month = RTC_MONTH_NOVEMBER;
		break;
	case 'D':
		date.Month = RTC_MONTH_DECEMBER;
		break;
	}

	univ_str[0] = setter_str[14];
	univ_str[1] = setter_str[15];
	date.Year = atoi(univ_str);

	univ_str[0] = setter_str[17];
	univ_str[1] = setter_str[18];
	time.Hours = atoi(univ_str);

	univ_str[0] = setter_str[20];
	univ_str[1] = setter_str[21];
	time.Minutes = atoi(univ_str);

	univ_str[0] = setter_str[23];
	univ_str[1] = setter_str[24];
	time.Seconds = atoi(univ_str);

	if(time.Hours > 21)
	{
		time.Hours = (time.Hours+2)%24;
		if(date.Month == 1 || date.Month == 3 || date.Month == 5 || date.Month == 7 ||
				date.Month == 8 || date.Month == 10 || date.Month == 12)
		{
			date.Date = (date.Date+1) % 32;
		}
		if(date.Month == 4 || date.Month == 6 || date.Month == 9 || date.Month == 11)
		{
			date.Date = (date.Date+1) % 31;
		}
		if(date.Month == 2)
		{
			if((date.Year%4) == 0)
			{
				date.Date = (date.Date+1) % 30;
			}
			else
			{
				date.Date = (date.Date+1) % 29;
			}
		}

		if(date.Date == 0)
		{
			date.Date++;
		}

		if(date.Date == 1)
		{
			date.Month = (date.Month+1)%13;
		}

		if(date.Month == 0)
		{
			date.Month++;
		}

		date.WeekDay = (date.WeekDay+1)%8;

		if(date.WeekDay == 0)
		{
			date.WeekDay++;
		}

		if((date.Month == 1) && (date.Date == 1))
		{
			date.Year++;
		}
	}
	else
	{
		time.Hours = time.Hours+2;
	}

	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);

	config_status = INIT_OK;
	return;
}

bool check_string(char *reply)
{
	char tim_ser_str[256];
	uint8_t serial_head = 0;
	uint32_t tick_t = 0;
	tick_t = HAL_GetTick();

	while((HAL_GetTick() - tick_t) < 5000)
	{
		uint8_t c_data;
		if(pop_miniBuff(&serial_time_buff, &c_data) != miniBuff_EMPTY)
		{
			if(c_data != 0)
			{
				tim_ser_str[serial_head] = c_data;
				serial_head++;
			}
		}
	}

	if(strstr(tim_ser_str, reply) != NULL)
	{
		return true;
	}
	return false;
}
