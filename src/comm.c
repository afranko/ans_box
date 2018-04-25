/*
 * COMMUNICATION LIBRARY - SIM800, MQTT
 */

#include "comm.h"

//#define GSM_DEBUG		// DEBUG flag -> Turns echo ON

/* Private declarations */

static void onConnect(void);

static void init_config(Settings_HandleTypeDef *configx);
static GSM_Resp init_modem(GCF_HandleTypeDef *gcfx);
static GSM_Resp init_gprs(GCF_HandleTypeDef *gcfx);
static GSM_Resp init_TCPIP(GCF_HandleTypeDef *gcfx);

static bool sendString(char data[]);
static GSM_Resp sendCommandResp(GCF_HandleTypeDef *gfcx, char command[], char response[], uint16_t timeout);
static GSM_Resp sendCommandMulti(GCF_HandleTypeDef *gfcx, char command[], const char *responses[], int8_t respNumber, uint16_t timeout);
static int8_t gsmRespProcess(GCF_HandleTypeDef *gcfx, int8_t respNum, const char *responses[], uint16_t timeout);
static GSM_Resp sendConnect(GCF_HandleTypeDef *gcfx);

static void gsm_handler(GCF_HandleTypeDef *gcfx);
static void mqtt_handler(MQTT_HandleTypeDef *mqttx);

static void writeByte(uint8_t byte);
static void printMessage(char message[]);
static void sendLength(uint32_t length);
static void sendUTFLength(uint16_t length);
static void sendUTFString(char string[]);

static void connect(MQTT_HandleTypeDef *mqttx);
static void disconnect(MQTT_HandleTypeDef *mqttx);

static void pingREQ(MQTT_HandleTypeDef *mqttx);

static void receive_publish(MQTT_HandleTypeDef *mqttx);

static void ping_handler(MQTT_HandleTypeDef *mqttx);
static bool wfmap(MQTT_HandleTypeDef *mqttx, uint8_t *data);
static void mqtt_resp_process(MQTT_HandleTypeDef *mqttx);
static void drop_connection(MQTT_HandleTypeDef *mqttx);
static uint32_t getLength(MQTT_HandleTypeDef *mqttx);
static uint16_t getUTFLength(MQTT_HandleTypeDef *mqttx);

static uint32_t millis(void)
{
	return HAL_GetTick();
}

static void delay_ms(uint32_t delay)
{
	HAL_Delay(delay);
}

/* Global variables */

/* Responses */
const char *cregResps[] = {"0,1", "0,5"};

char clockAddress[] = "8.8.8.8";

/* Public Functions */

void init_comm(Settings_HandleTypeDef *configx)
{
	init_config(configx);
	if(init_modem(hgcf) != GSM_OK)
		return;
	if(init_gprs(hgcf) != GSM_OK)
		return;
	init_TCPIP(hgcf);
}

void comm_handler(void)
{
	if(hgcf->state != MQTT)
		gsm_handler(hgcf);
	else
		mqtt_handler(hmqtt);
}

bool setRTC(void)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	char trFrS[2];			// Transform String (INT -> ASCII)
	char httpRespBuff[200];
	uint8_t chPointer = 0;
	char http_head_msg[] = "HEAD /lab01/ HTTP/1.1\r\n" "Host: bme.hu\r\n";

	if(sendCommandResp(hgcf, "AT+CIPSTART=\"TCP\", \"www.bme.hu\", 80", "CONNECT", 65000U) != GSM_OK)
		return false;

	HAL_UART_Transmit(&huart2, (uint8_t*)http_head_msg, (uint16_t) strlen(http_head_msg), 30000U);	// Overall

	/* Wait For Response */
	uint32_t the_tick = HAL_GetTick();
	while((HAL_GetTick() - the_tick) < 60000 && (available_gBuff(hgcf->inBuffer) == BUFF_EMPTY));

	delay_ms(100);	// 100 ms wait

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	while(available_gBuff(hgcf->inBuffer) != BUFF_EMPTY)
		pop_gBuff(hgcf->inBuffer, (uint8_t*)&httpRespBuff[chPointer++]);

	if(chPointer == 0)
		return false;

	char *datePointer = strstr(httpRespBuff, "Date: ") + strlen("Date :");

	switch(datePointer[1])
	{
	case 'o':
		date.WeekDay = RTC_WEEKDAY_MONDAY;
		break;
	case 'u':
		if(datePointer[0] == 'T')
			date.WeekDay = RTC_WEEKDAY_TUESDAY;
		else
			date.WeekDay = RTC_WEEKDAY_SUNDAY;
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

	trFrS[0] = datePointer[5];
	trFrS[1] = datePointer[6];
	date.Date = (uint8_t)atoi(trFrS);

	switch(datePointer[8])
	{
	case 'J':
		if(datePointer[9] == 'u')
			if(datePointer[10] == 'n')
				date.Month = RTC_MONTH_JUNE;
			else
				date.Month = RTC_MONTH_JULY;
		else
			date.Month = RTC_MONTH_JANUARY;
		break;

	case 'F':
		date.Month = RTC_MONTH_FEBRUARY;
		break;

	case 'M':
		if(datePointer[10] == 'r')
			date.Month = RTC_MONTH_MARCH;
		else
			date.Month = RTC_MONTH_MAY;
		break;

	case 'A':
		if(datePointer[9] == 'p')
			date.Month = RTC_MONTH_APRIL;
		else
			date.Month = RTC_MONTH_AUGUST;
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

	trFrS[0] = datePointer[14];
	trFrS[1] = datePointer[15];
	date.Year = (uint8_t)atoi(trFrS);

	trFrS[0] = datePointer[17];
	trFrS[1] = datePointer[18];
	time.Hours = (uint8_t)atoi(trFrS);

	trFrS[0] = datePointer[20];
	trFrS[1] = datePointer[21];
	time.Minutes = (uint8_t)atoi(trFrS);

	trFrS[0] = datePointer[23];
	trFrS[1] = datePointer[24];
	time.Seconds = (uint8_t)atoi(trFrS);

	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);

	return true;
}

/**
 * @brief This function publishes a message (byte stream to a given topic.
 * @param Name of the topic that the client has to publish in
 * @param Message/ByteStream that has to be transmitted
 */
void publish(char topic[], char message[])
{
	writeByte(PUBLISH << 4 | 0x01);	// QOS, DUP = 0, RETAIN = 1
	sendLength(2U + strlen(topic) + strlen(message));
	sendUTFString(topic);
	printMessage(message);
}

/**
 * @brief This function unsubscribes the client from a chosen topic.
 * @param Name of the topic that the client has to unsubscribe from.
 */
void unsubscribe(char topic[])
{
	writeByte(UNSUBSCRIBE << 4 | FIXEDFLAGS);	// 0XA2 - [UNSUB|0010];
	sendLength(2U + 2U + strlen(topic));		// packID + UTFLen + Topic
	writeByte(0);		// RANDOM PACKET ID
	writeByte(4);
	sendUTFString(topic);						// send TopicName
}

/*
 * @brief This function subscribe the client to a given topic
 * @param Name of the topic that the client has to subscribe to.
 */
void subscribe(char topic[])
{
	writeByte(SUBSCRIBE << 4 | FIXEDFLAGS);
	sendLength(2U+2U+strlen(topic)+1U);		//PackID, UTFLen + topic + QoS
	writeByte(0);		// RANDOM PACKET ID
	writeByte(4);
	sendUTFString(topic);
	writeByte(0);
}

/*
 * @brief This function is called when the client receives a message
 * @param topic Name of the topic where the message came from
 * @param message The whole message that has been received
 * @note If you aren't interested in the name of topic or the message use NULL as parameter.
 * @note This function sets freshMessage flag to false!
 * @warning This function doesn't check that if the parameters don't have the appropriate size! Max/min. sizes: topic[200], message[10000]
 */
void recMessage(char topic[], char message[])
{
	if(topic != NULL)
		strcpy(topic, hmqtt->topicName);
	if(message != NULL)
		strcpy(message,hmqtt->rxBuffer);
	hmqtt->freshMessage = false;
}

extern inline bool isConnected(void);

extern inline void getSerial(uint8_t byte);

extern inline bool isFreshMessage(void);


/* Private Functions */

/* User Functions */

/* This function will be called after connecting MQTT broker */
void onConnect(void)
{
	//publish("/testout", "TEST!");	// DEBUG
	char topicname[25] = "/";
	strcat(topicname, hmqtt->clientID);
	subscribe(topicname);
	welcomeFlag = true;
}


/* Copy GSM and MQTT config handle structures and inBuffer*/
void init_config(Settings_HandleTypeDef *configx)
{
	/* GSM_HANDLER˙*/

	hgcf = (GCF_HandleTypeDef*) malloc(sizeof(GCF_HandleTypeDef) + strlen(configx->mqtt_host) +
			strlen("AT+CIPSTART=\"TCP\",\"\",\"\"") + strlen(configx->port) + 1U);

	hgcf->state = GSM_NOT_CONN;

	inBuffer = init_gBuff(2048U);
	hgcf->inBuffer = inBuffer;

	/* Making apnString */
	hgcf->apnString = (char*) malloc(strlen(configx->gsm_apn) + strlen("AT+CSTT=\"\"") + 1U);
	strcpy(hgcf->apnString, "AT+CSTT=\"");
	strcat(hgcf->apnString, configx->gsm_apn);
	strcat(hgcf->apnString, "\"");

	/* Making connString */
	strcpy(hgcf->connString, "AT+CIPSTART=\"TCP\",\"");
	strcat(hgcf->connString, configx->mqtt_host);
	strcat(hgcf->connString, "\",\"");
	strcat(hgcf->connString, configx->port);
	strcat(hgcf->connString, "\"");

	/* MQTT_HANDLER */

	hmqtt = (MQTT_HandleTypeDef*) malloc(sizeof(MQTT_HandleTypeDef));
	hmqtt->gsm = hgcf;
	hmqtt->connFlag = false;
	hmqtt->freshMessage = false;
	hmqtt->inBuffer = inBuffer;

	hmqtt->pingFlag = false;
	hmqtt->pingSent = 0;

	strcpy(hmqtt->clientID, configx->client_name);
	hmqtt->keepAlive = 60U;
	hmqtt->pingRetry = configx->ping_retry;
}

/* Init modem and discover GSM network */
GSM_Resp init_modem(GCF_HandleTypeDef *gcfx)
{
	sendString("AT");	// Init modem and set autobaud
	//sendCommandResp(gcfx, "AT+CPIN=2012", "ZNEME", 30000);
	if(sendCommandResp(gcfx, "AT", "OK", 10000) != GSM_OK)
		return GSM_NO_RESP;

#ifdef GSM_DEBUG
	if(sendCommandResp(gcfx, "ATE1", "OK", 3000) != GSM_OK)
		return GSM_NO_RESP;
#endif

#ifndef GSM_DEBUG
	if(sendCommandResp(gcfx, "ATE0", "OK", 3000) != GSM_OK)
		return GSM_NO_RESP;
#endif

	return GSM_OK;
}

/* Attach modem to GPRS network */
GSM_Resp init_gprs(GCF_HandleTypeDef *gcfx)
{
	if(gcfx->state == GSM_NOT_CONN)
	{
		if(sendCommandMulti(gcfx, "AT+CREG?", cregResps, 2, 60000) != GSM_OK)
			return GSM_NO_RESP;
		gcfx->state = GSM_GPRS;
	}

	if(sendCommandResp(gcfx, "AT+CIPMUX?", "CIPMUX: 0", 3000) != GSM_OK)
	{
		if(sendCommandResp(gcfx, "AT+CIPMUX=0", "OK", 3000) != GSM_OK)
			return GSM_NO_RESP;
	}

	if(sendCommandResp(gcfx, "AT+CIPMODE?", "CIPMODE: 1", 3000) != GSM_OK)
	{
		if(sendCommandResp(gcfx, "AT+CIPMODE=1", "OK", 3000) != GSM_OK)
			return GSM_NO_RESP;
	}

	if(sendCommandResp(gcfx, "AT+CGATT?", "CGATT: 1", 10000) != GSM_OK)
	{
		if(sendCommandResp(gcfx, "AT+CGATT=1", "OK", 10000) != GSM_OK)
			return GSM_NO_RESP;
	}
	else
	{
		if(sendCommandMulti(gcfx, "AT+CIPSTATUS", (const char*[]){"CLOS", "IP STATUS"}, 2, 5000) == GSM_OK)
		{
			gcfx->state = GSM_IP_STATUS;
			return GSM_OK;
		}
	}

	gcfx->state = GSM_IP_INITIAL;
	return GSM_OK;
}

/* Connect to TCP/IP network */
GSM_Resp init_TCPIP(GCF_HandleTypeDef *gcfx)
{
	if(gcfx->state == GSM_IP_INITIAL)
	{
		/*sendCommandResp(gcfx, "AT+CGDCONT=?", "OK", 5000);
		sendCommandResp(gcfx, "AT+CGDCONT?", "OK", 5000);
		sendCommandResp(gcfx, "AT+CGDCONT=1,\"IP\",\"internet.telekom\"", "OK", 5000);
		*/
		if(sendCommandResp(gcfx, gcfx->apnString, "OK", 5000) != GSM_OK)
			return GSM_NO_RESP;
		gcfx->state = GSM_IP_START;
	}

	if(gcfx->state == GSM_IP_START)	//TODO the bug caused by this command is fixed in init_gprs()
	{
		if(sendCommandResp(gcfx, "AT+CIICR", "OK", 30000))
			return GSM_NO_RESP;
		gcfx->state = GSM_IP_GPRSACT;
	}

	if(gcfx->state == GSM_IP_GPRSACT)
	{
		if(sendCommandMulti(gcfx, "AT+CIFSR", (const char*[]){"OK", "."}, 2, 10000) != GSM_OK)
			return GSM_NO_RESP;
		gcfx->state = GSM_IP_STATUS;
	}

	return GSM_OK;
}

/* Sending commands to the server */
bool sendString(char data[])
{
	if(HAL_UART_Transmit(&huart2, (uint8_t*)data, (uint16_t)strlen(data), 3000U) != HAL_OK)
			return false;
	if(HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 3000U) != HAL_OK)
			return false;
	return true;
}

GSM_Resp sendCommandResp(GCF_HandleTypeDef *gfcx, char command[], char response[], uint16_t timeout)
{
	sendString(command);
	int8_t respNum = gsmRespProcess(gfcx, 1, (const char**)&response, timeout);
	delay_ms(2000);
	if(respNum < 0)
		return GSM_NO_RESP;
	else
		return GSM_OK;
}

static GSM_Resp sendCommandMulti(GCF_HandleTypeDef *gfcx, char command[], const char *responses[], int8_t respNumber, uint16_t timeout)
{
	sendString(command);
	int8_t respNum = gsmRespProcess(gfcx, respNumber, responses, timeout);
	delay_ms(2000);
	if(respNum < 0)
		return GSM_NO_RESP;
	else
		return GSM_OK;
}

GSM_Resp sendConnect(GCF_HandleTypeDef *gcfx)
{
 	if(sendCommandResp(gcfx, gcfx->connString, "CONNECT", 25000) != GSM_OK)
		return GSM_NO_RESP;

	gcfx->state = MQTT;

	return GSM_OK;
}

/*
 * Return the number of the response (0-255) and -1 if there isn't any known answer
 */
int8_t gsmRespProcess(GCF_HandleTypeDef *gcfx, int8_t respNum, const char *responses[], uint16_t timeout)
{
	uint8_t i = 0;	// Iterator
	char lastRead;

	uint32_t timer = millis();
	while(timeout > (millis() - timer))
	{
		while(available_gBuff(gcfx->inBuffer) != BUFF_EMPTY)
		{
			pop_gBuff(gcfx->inBuffer, (uint8_t*)&lastRead);

			if(lastRead != '\n')
			{
				gcfx->responseBuffer[i++] = lastRead;
				if(i > BUFFLENGTH-2U)	// No additional place for \n and \0
					i = 0;				// Overflow detected -> Restart parsing
			}
			else
			{
				gcfx->responseBuffer[i++] = lastRead;
				gcfx->responseBuffer[i] = '\0';
				for(int8_t responseNumber = 0; responseNumber < respNum; responseNumber++)
				{
					if(strstr(gcfx->responseBuffer, responses[responseNumber]) != NULL)
						return responseNumber;
				}
				i = 0;			// Restart reading
			}
		}
	}
	return -1;					// Indicates error -> Timeout
}

void gsm_handler(GCF_HandleTypeDef *gcfx)
{
	/*if(gcfx->state == GSM_GPRS)
		if(sendCommandResp(gcfx, "AT+CIPSTATUS", "CLOS", 3000) == GSM_OK)	// To catch "CLOSING" as well as "CLOSED"
			gcfx->state = GSM_IP_GPRSACT;*/
	switch(gcfx->state)
	{
	case GSM_NOT_CONN:
		init_modem(gcfx);
		/* no break */
	case GSM_GPRS:
		init_gprs(gcfx);
		break;
	case GSM_IP_STATUS:
		sendConnect(gcfx);
		break;
	default:
		init_TCPIP(gcfx);
	}
}

/* MQTT functions */

void writeByte(uint8_t byte)
{
	HAL_UART_Transmit(&huart2, &byte, 1U, 3000U);
}

void printMessage(char message[])
{
	HAL_UART_Transmit(&huart2, (uint8_t*)message, (uint16_t)strlen(message), 10000U);
	/*
	uint16_t len = (uint16_t) strlen(message);
	if(len > 65000)
	{
		HAL_UART_Transmit(&huart2, (uint8_t*)message, 65000U, 10000U);
		HAL_UART_Transmit(&huart2, (uint8_t*)message+65000U, len % 65000U, 10000U);
	}
	else
		HAL_UART_Transmit(&huart2, (uint8_t*)message, len, 10000U);*/
}

/* Calculates the remaining length of the message */
void sendLength(uint32_t length)
{
	uint8_t sendByte;				// See algorithm in official MQTT documentation
	do
	{
		sendByte = length % 128;
		length /= 128;
		if(length > 0)
			sendByte |= 0x80;
		writeByte(sendByte);
	}
	while(length > 0);
}

/* Sending the length of an UTF-8 formated string */
void sendUTFLength(uint16_t length)
{
	uint8_t byte = (uint8_t)length >> 8 & 0xFF;		// MSB
	HAL_UART_Transmit(&huart2, &byte, 1U, 3000);
	byte = (uint8_t)length & 0xFF;					// LSB
	HAL_UART_Transmit(&huart2, &byte, 1U, 3000);
}

/* Sending an UTF-8 formated string with its length*/
void sendUTFString(char string[])
{
	uint16_t length = (uint16_t) strlen(string);
	sendUTFLength(length);
	HAL_UART_Transmit(&huart2, (uint8_t*) string, length, 3000U);
}

void mqtt_handler(MQTT_HandleTypeDef *mqttx)
{
	if(!mqttx->connFlag)
		connect(mqttx);
	else
	{
		mqtt_resp_process(mqttx);
		ping_handler(mqttx);
	}
}

/* Wait For Message And Pop into the buffer */
bool wfmap(MQTT_HandleTypeDef *mqttx, uint8_t *data)
{
	uint32_t time = millis();
	while(pop_gBuff(mqttx->inBuffer, data) != BUFF_OK)
	{
		if(millis() - time > 2000U)
			return false;
	}
	return true;
}

void mqtt_resp_process(MQTT_HandleTypeDef *mqttx)
{
	char lastChar = 0;
	uint8_t byte = 0;
	uint8_t packetType;

	if(available_gBuff(mqttx->inBuffer) != BUFF_EMPTY)
	{
		pop_gBuff(mqttx->inBuffer, (uint8_t*)&lastChar);

		if(lastChar == 'C')
		{
			drop_connection(mqttx);
			return;
		}

		packetType = (uint8_t)lastChar >> 4 & 0x0F;

		switch(packetType)
		{
		case CONNACK:
			if(!wfmap(mqttx, &byte))
				return;				// Absence of ping (AoP) will handle communication error

			if(byte != 2)			// Remaining Length
				return;

			if(!wfmap(mqttx, &byte))
				return;				// AoP

			if(byte != 0)			// Session Present Flag
				return;				// AoP

			if(!wfmap(mqttx, &byte))
				return;				// AoP

			if(byte != 0)			// Connection Accepted
				return;				// AoP

			onConnect();

			mqttx->gsm->state = MQTT;
			mqttx->connFlag = true;
			mqttx->pingSent = millis();
			mqttx->pingFlag = false;
			break;

		case PUBLISH:
			receive_publish(mqttx);
			break;

		case PINGRESP:
			if(!wfmap(mqttx, &byte))// Absence of ping (AoP) will handle communication error
				return;				// AoP

			if(byte != 0)			// Valid PingResponse Message
				return;				// AoP

			mqttx->pingFlag = false;
		}
	}
}

/* Sends a CONNECT message to the broker */
void connect(MQTT_HandleTypeDef *mqttx)
{
	writeByte(CONNECT << 4);	// 0x10 - [CONNECT|0000]
	sendLength(10U + 2U + strlen(mqttx->clientID));	// Remaining Length (variable header(10) + length of payload)

	sendUTFString("MQTT");
	writeByte(0x04);				// Protocol Version (4)
	writeByte(0x02);				// FLAGS	(0x02) - CLEAN SESSION
	writeByte((uint8_t) (mqttx->keepAlive >> 8 & 0xFF));	// KeepAlive MSB
	writeByte((uint8_t) (mqttx->keepAlive & 0xFF));		// KeepAlive LSB
	sendUTFString(mqttx->clientID);

	uint32_t ticker = millis();
	while((available_gBuff(mqttx->inBuffer) != BUFF_OK) && (millis() - ticker < 30000U)) {}
	mqttx->gsm->state = GSM_IP_STATUS;	// TCP cannot be reused
	mqtt_resp_process(mqttx);
	if(mqttx->gsm->state != MQTT)
	{
		delay_ms(500);
		HAL_UART_Transmit(&huart2, "+++", 3, 3000U);
		delay_ms(500);
		sendCommandResp(mqttx->gsm, "AT+CIPCLOSE", "OK", 5000U);
	}
}

/* Disconnects the client from the broker */
void disconnect(MQTT_HandleTypeDef *mqttx)
{
	writeByte(DISCONNECT << 4);	// 0xE0 - [CONNECT|0000]
	sendLength(0U);
	mqttx->connFlag = false;
	mqttx->pingFlag = false;
	mqttx->gsm->state = GSM_IP_STATUS;
	delay_ms(500);
	HAL_UART_Transmit(&huart2, "+++", 3, 3000U);
	delay_ms(500);
	sendCommandResp(mqttx->gsm, "AT+CIPCLOSE", "OK", 5000U);
}

/* Pings the broker */
void pingREQ(MQTT_HandleTypeDef *mqttx)
{
	writeByte(PINGREQ << 4);	// PINGREQ HEADER + 0 remaining length
	writeByte(0);
	if(!mqttx->pingFlag)
		mqttx->pingSent = millis();
	mqttx->pingFlag = true;
}

/* Handles ping timeout and KeepAlive */
void ping_handler(MQTT_HandleTypeDef *mqttx)
{
	if(millis() - mqttx->pingSent > mqttx->keepAlive * 1000)
		pingREQ(mqttx);
	if(mqttx->pingFlag)
	{
		//if(millis() - mqttx->pingSent > mqttx->keepAlive * 1000U * mqttx->pingRetry)
		if(millis() - mqttx->pingSent > mqttx->keepAlive * 1000U * 2U)
			disconnect(mqttx);
	}/*
	else
		if(millis() - mqttx->pingSent > mqttx->keepAlive * 1000)
			pingREQ(mqttx);*/
}

uint32_t getLength(MQTT_HandleTypeDef *mqttx)
{
	uint32_t length = 0;			// See algorithm in official MQTT documentation
	uint8_t data;
	for(char i = 0; i < 4; i++)
	{
		if(!wfmap(mqttx, &data))
			return 0;
		length |= (uint32_t) (data & 0x7F)  << (i*8);
		if(data < 0x80)
			break;
	}
	return length;
}

/* Calculate the length of an UTF-8 string from input data */
uint16_t getUTFLength(MQTT_HandleTypeDef *mqttx)
{
	uint16_t length = 0;
	uint8_t data;

	if(!wfmap(mqttx, &data))
		return 0;
	length = (uint16_t)(data << 8) & 0xFF00;

	if(!wfmap(mqttx, &data))
		return 0;
	length |= data;

	return length;
}

/* Publish message receive handler */
void receive_publish(MQTT_HandleTypeDef *mqttx)
{
	uint32_t remLength = getLength(mqttx);
	uint16_t nextLength;
	uint8_t rData;

	if(remLength == 0)
		return;
	nextLength = getUTFLength(mqttx);
	if(nextLength > 200)
		return;					// Security Issue -> Topic name should be shorter than 200 chars
	for(uint16_t i = 0, j = 0; i < nextLength && j < nextLength*2; j++)
	{
		if(wfmap(mqttx, &rData))
			mqttx->topicName[i++] = rData;
	}
	mqttx->topicName[nextLength] = '\0';

	remLength -= (nextLength+2);

	if(remLength > 2000)
		return;					// Security Issue -> Message should be shorter than 2000 chars

	uint32_t i, j;
	for(i = 0, j = 0; i < remLength && j < remLength*2; j++)
	{
		if(wfmap(mqttx, &rData))
			mqttx->rxBuffer[i++] = rData;
	}
	mqttx->rxBuffer[i] = '\0';
	if(j < remLength*2)
		mqttx->freshMessage = true;
	return;
}

void drop_connection(MQTT_HandleTypeDef *mqttx)
{
	char clString[50];
	for(unsigned char i = 0; i < 50; i++)
	{
		if(!wfmap(mqttx, (uint8_t*) &clString[i]))
			return;			// Absence of ping will handle communication error
		if(clString[i] == '\n')
			break;
	}
	if(strstr(clString, "CLOS") != NULL)
	{
		mqttx->pingFlag = false;
		mqttx->connFlag = false;
		mqttx->gsm->state = GSM_IP_STATUS;
		delay_ms(500);
		HAL_UART_Transmit(&huart2, "+++", 3, 3000U);
		delay_ms(500);
	}
	return;
}
