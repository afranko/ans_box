/*
 * AnsBox - MANTIS sensor acquisition box for railroad switches
 *
 */

#include "statemachine.h"
#include "storage.h"

ADC_HandleTypeDef 	hadc3;
I2C_HandleTypeDef 	hi2c2;
SPI_HandleTypeDef 	hspi1;
RTC_HandleTypeDef 	hrtc;
TIM_HandleTypeDef 	htim2;
TIM_HandleTypeDef	htim4;
UART_HandleTypeDef	huart2;

/* Buffers and buffer holder arrays (to avoid copying them in functions) */
cBuff *measBuffers[4], *sendBuffers[4];

/* In this application buffer pointer underflow isn't a problem, but at
 * different buffer sizes it has to be solved with an underflow function if(tail<0)...
 * Offset timeframe has to be > 9 ms!
 */

/* Input */
uint8_t receivedByte;
char topicName[50];
char inMessage[100];
char messageType;

/* Settings - CONFIG & RTC */
Settings_HandleTypeDef *hconfig;
bool isRTCSet = true;

/* Environment */
char envString[1200];
uint32_t env_counter;
bool envMeasFlag = false;

/* Movement */
char movString[66000];
bool msgFlag = false;
bool warningFlag = false;

/* Warning */
char timeStamp[20];
char warningString[500];

/* Last Position */
char *lpTs = NULL;
uint16_t actData = 0;
float actResponse = 0.0;

/* Flags */
bool lastMessage = false;
bool lastPositionTrigger = false;
bool welcomeFlag = false;

/* Storage pointer */
char *storedStrings[] = {envString, movString, warningString};

int main(void)
{
	init_settings();
	initPT(&hspi1);
	initMeasurementParams(hconfig->client_name, &hrtc);
	initMessages();

	/* Init Buffers */
	for(uint8_t bufferIndicator = 0; bufferIndicator < 4; bufferIndicator++)
	{
		measBuffers[bufferIndicator] = init_cBuff(2048);
		sendBuffers[bufferIndicator] = init_cBuff(2048);
	}

	HAL_UART_Receive_IT(&huart2, &receivedByte, 1); // Start UART
	init_comm(hconfig);

	if(!isRTCSet)
		if(!setRTC())
			restart_system();	// Restart if we don't have the clock

	/* Connection */
	while(!isConnected()) comm_handler();

	/* Waiting for the first conversation */
	HAL_ADC_Start_IT(&hadc3);
	while(measBuffers[0]->head < (hconfig->meas_offset / 10)) {};

	//syncStorage();
	initFlags();

	/* Start FSR */
	p_start();

	while(true)
	{
		comm_handler();
		if(welcomeFlag)
		{
			sendWelcomeMessage();
			getTimeStamp(timeStamp);
			actData = getLastPosition();
			if(actData < hconfig->threshold_min)
				actResponse = 0.0;
			else if(actData > hconfig->threshold_max)
				actResponse = 1.0;
			else
				actResponse = 0.5;
			sendLastPosition(actResponse, hconfig->client_name, timeStamp);
			welcomeFlag = false;
		}
		if(isFreshMessage())
		{
			recMessage(topicName, inMessage);
			lpTs = strstr(inMessage, "\"timestamp\": \"") + 14U;
			lpTs[19] = '\0';
			lastPositionTrigger = true;
		}

		if(lastPositionTrigger)
		{
			actData = getLastPosition();
			if(actData < hconfig->threshold_min)
				actResponse = 0.0;
			else if(actData > hconfig->threshold_max)
				actResponse = 1.0;
			else
				actResponse = 0.5;
			if(lpTs != NULL)
				sendLastPosition(actResponse, hconfig->client_name, lpTs);
			lastPositionTrigger = false;
		}

		statemachine_process();

		doMovMeas(&msgFlag, &warningFlag);

		if(msgFlag)
		{
			if(movMeasToMessage(movString, timeStamp))
				sendMessage(storedStrings, 'M');
			/*	if(!sendMessage(storedStrings, 'M'))
					writeToStorage(movString, 'M'); */
			if(warningFlag)
				if(stampWarning(warningString, timeStamp, hconfig->client_name))
					sendMessage(storedStrings, 'W');
				/*	if(!sendMessage(storedStrings, 'W'))
						writeToStorage(warningString, 'W'); */
			actData = getLastPosition();
			if(actData < hconfig->threshold_min)
				actResponse = 0.0;
			else if(actData > hconfig->threshold_max)
				actResponse = 1.0;
			else
				actResponse = 0.5;
			sendLastPosition(actResponse, hconfig->client_name, timeStamp);
			msgFlag = false;
		}

		if(envMeasFlag)
		{
			doEnvMeas(&hi2c2, &hspi1, envString);
			sendMessage(storedStrings, 'E');
		/*	if(!sendMessage(storedStrings, 'E'))
				writeToStorage(envString, 'E');	*/
			envMeasFlag = false;
		}
		/*
		if(isConnected())
		{
			if(checkStorage() > 0)
			{
				if(readFromStorage(storedStrings, &messageType))
					sendMessage(storedStrings, messageType);
			}
		}
		*/
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		getSerial(receivedByte);
		HAL_UART_Receive_IT(&huart2, &receivedByte, 1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		if(env_counter == hconfig->env_meas_freq)
		{
			envMeasFlag = true;
			env_counter = 0;
		}
		env_counter++;
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode)
	{
		HAL_UART_Receive_IT(&huart2, &receivedByte, 1);
	}
}

