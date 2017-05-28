#include "edge_comm/edge_comm.h"

/* This lib was written for STM32F407 based board, using other board define another macro and write functions */

GSM_MQTT MQTT;
MQTT_QoS QoSVal;
serialBuff uartBuffer;


/* Qos Settings */
void setSend(char s_QoS, char s_DUP, char s_RETAIN)
{
	QoSVal.s_QoS = s_QoS;
	QoSVal.s_DUP = s_DUP;
	QoSVal.s_RETAIN = s_RETAIN;
}

/* Init buffer, TCP connect, MQTT structure */
void CommInit(UART_HandleTypeDef *huart, unsigned long KeepAlive)
{
	initSerialBuf(&uartBuffer);
	setSend(0,0,1);
	MQTT_start(huart, KeepAlive);
	begin(&MQTT);
}

/**
 * @brief
 *
 * @param
 *
 * @retval
 *
 */

void sendData(int nodeID, int measLocID, char *timestamp, int ambientTemperature, int dustLevel, int railTemperature, char *startTime, int measSeqNum, int duration, char *meas1, char *meas2)
{
	int bigLen = 2+strlen(meas1)+3+strlen(meas2)+2+1; //terminating null

	/* Init enviroment JSON object */
	JSON_Value *env_root_value = json_value_init_object();
	JSON_Object *env_root_object = json_value_get_object(env_root_value);

	json_object_set_number(env_root_object, "nodeID", nodeID);
	json_object_set_number(env_root_object, "measlocID", measLocID);
	json_object_set_string(env_root_object, "timestamp", timestamp);
	json_object_set_number(env_root_object, "ambientTemperature", measLocID);
	json_object_set_number(env_root_object, "dustLevel", dustLevel);
	json_object_set_number(env_root_object, "railTemperature", railTemperature);

	/*Serialize JSON structure*/
	char *env_serialized_string = NULL;
	env_serialized_string = json_serialize_to_string_pretty(env_root_value);

	/*Publish JSON*/
	if(available(&MQTT))
	{
		publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT), "/environment", env_serialized_string);
	}

	/*Free memory*/
	json_free_serialized_string(env_serialized_string);
	json_value_free(env_root_value);

	/* Init movement JSON object */
	JSON_Value *mov_root_value = json_value_init_object();
	JSON_Object *mov_root_object = json_value_get_object(mov_root_value);

	/*Making string */
	char oString[bigLen];
	oString[0] = '[';
	oString[1] = '\"';
	oString[2] = '\0';
	strcat(oString, meas1);
	strcat(oString, "\",\"");
	strcat(oString, meas2);
	strcat(oString, "\"]");

	json_object_set_number(mov_root_object, "nodeID", nodeID);
	json_object_set_number(mov_root_object, "measlocID", measLocID);
	json_object_set_string(mov_root_object, "startTime", startTime);
	json_object_set_number(mov_root_object, "measSeqNum", measSeqNum);
	json_object_set_number(mov_root_object, "duration", duration);
	json_object_set_value(mov_root_object, "meas", json_parse_string(oString));

	/*Serialize JSON structure*/
	char *mov_serialized_string = NULL;
	mov_serialized_string = json_serialize_to_string_pretty(mov_root_value);

	/*Publish JSON*/
	if(available(&MQTT))
	{
		publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT), "/movement", mov_serialized_string);
	}

	/*Free memory*/
	json_free_serialized_string(mov_serialized_string);
	json_value_free(mov_root_value);

	return;
}

void MQTTProcessing(void)
{
	while(serialAvailable(&MQTT, &uartBuffer) != 0)
	{
		serialEvent();
	}
	processing(&MQTT);
}

/*
void customSend()
{
    JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_number(root_object, "name", number);
    json_object_set_string(root_object, "name", "string");
    json_object_dotset_string(root_object, "name.subname", "string");
    json_object_set_value(mov_root_object, "name", json_parse_string("[\"data1\",\"data2"]"));

	char *serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(root_value);

	if(available(&MQTT))
	{
		publish(&MQTT, QoSVal.DUP, QoSVal.QoS, QoSVal.RETAIN, _generateMessageID(&MQTT), "TOPICNAME", serialized_string);
	}

	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}
*/
