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

void sendEnvironmentMessage(char *meas_loc, char* timeStamp)
{
	/* Init JSON Object */
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	/* Init Big Array */
	JSON_Value *arr_val = json_value_init_array();
	JSON_Array *arr_obj = json_value_get_array(arr_val);

	/* Add array to object */
	json_object_set_value(root_object, "hasAttributes", arr_val);

		/* Array block */
		JSON_Value *arrval = json_value_init_object();
		JSON_Object *arro = json_value_get_object(arrval);

		json_object_set_string(arro, "hasAttributeClazzType", "DA");
		json_object_set_string(arro, "hasName", "env_meas");

		/* Sub Array Block */
		JSON_Value *arrsub_arr = json_value_init_array();
		JSON_Array *arrsub_obj = json_value_get_array(arrsub_arr);

		json_object_set_value(arro, "hasValueContainers", arrsub_arr);

		    /* Sub Object no.1 */
		    JSON_Value *arrsub_val_1 = json_value_init_object();
		    JSON_Object *arrsub_o_1 = json_value_get_object(arrsub_val_1);

		    json_object_dotset_number(arrsub_o_1, "hasValue.value", 20);
		    json_object_dotset_string(arrsub_o_1, "uuid.value", "ambientTemp");

		    json_array_append_value(arrsub_obj, arrsub_val_1);

		    /* Sub Object no.2 */
		    JSON_Value *arrsub_val_2 = json_value_init_object();
		    JSON_Object *arrsub_o_2 = json_value_get_object(arrsub_val_2);

		    json_object_dotset_number(arrsub_o_2, "hasValue.value", 125);
		    json_object_dotset_string(arrsub_o_2, "uuid.value", "ambientHumidity");
		    json_array_append_value(arrsub_obj, arrsub_val_2);

		    /* Sub Object no.3 */
		    JSON_Value *arrsub_val_3 = json_value_init_object();
		    JSON_Object *arrsub_o_3 = json_value_get_object(arrsub_val_3);

		    json_object_dotset_number(arrsub_o_3, "hasValue.value", 20);
		    json_object_dotset_string(arrsub_o_3, "uuid.value", "railTemp");
		    json_array_append_value(arrsub_obj, arrsub_val_3);

		    /* Append the big array */
		    json_array_append_value(arr_obj, arrval);

		    /* Remaining pairs */
		    json_object_dotset_string(root_object, "hasEventDescriptionUuid.value", "6511");

		    JSON_Value *loc2val = json_value_init_object();
		    JSON_Object *loc2 = json_value_get_object(loc2val);
		    json_object_set_string(loc2, "value", meas_loc);
		    json_object_dotset_value(root_object, "hasLocation.measLocId", loc2val);

		    JSON_Value *times2val = json_value_init_object();
		    JSON_Object *times2 = json_value_get_object(times2val);

		    json_object_set_string(times2, "format", "DATETIME");
		    json_object_set_string(times2, "value", timeStamp);

		    json_object_dotset_value(root_object, "hasTimestamp.hasUTCDateTime", times2val);

		serialized_string = json_serialize_to_string_pretty(root_value);

		/* Publish JSON */
		if(available(&MQTT))
		{
			publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT), "/environment", serialized_string);
		}

		json_free_serialized_string(serialized_string);
		json_value_free(root_value);

		return;

}

void sendMovementMessage(uint32_t duration, char *meas_loc, char* timeStamp, JSON_Value *measurement_array)
{
	/* Init JSON Object */
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;

	/* Init Big Array */
	JSON_Value *arr_val = json_value_init_array();
	JSON_Array *arr_obj = json_value_get_array(arr_val);

	/* Add array to object */
	json_object_set_value(root_object, "hasAttributes", arr_val);

		/* Array block */
		JSON_Value *arrval = json_value_init_object();
	    JSON_Object *arro = json_value_get_object(arrval);

	    json_object_set_string(arro, "hasAttributeClazzType", "DA");
	    json_object_set_string(arro, "hasName", "mov_meas");

	    	/* Sub Array Block */
	    	JSON_Value *arrsub_arr = json_value_init_array();
	    	JSON_Array *arrsub_obj = json_value_get_array(arrsub_arr);

	    	json_object_set_value(arro, "hasValueContainers", arrsub_arr);

	    		/* Sub Object no.1 */
	    		JSON_Value *arrsub_val_1 = json_value_init_object();
	    		JSON_Object *arrsub_o_1 = json_value_get_object(arrsub_val_1);

	    		json_object_dotset_value(arrsub_o_1, "hasValue.values", measurement_array);
	    		json_object_dotset_string(arrsub_o_1, "uuid.value", "move");

	    		json_array_append_value(arrsub_obj, arrsub_val_1);

	    		/* Sub Object no.2 */
	    		JSON_Value *arrsub_val_2 = json_value_init_object();
	    		JSON_Object *arrsub_o_2 = json_value_get_object(arrsub_val_2);

	    		json_object_dotset_number(arrsub_o_2, "hasValue.value", duration);
	    		json_object_dotset_string(arrsub_o_2, "uuid.value", "duration");
	    		json_array_append_value(arrsub_obj, arrsub_val_2);

	    /* Append the big array */
	    json_array_append_value(arr_obj, arrval);

	    /* Remaining pairs */
	    json_object_dotset_string(root_object, "hasEventDescriptionUuid.value", "5392"); //TODO - frimware const str

	    JSON_Value *loc2val = json_value_init_object();
	    JSON_Object *loc2 = json_value_get_object(loc2val);
	    json_object_set_string(loc2, "value", meas_loc);
	    json_object_dotset_value(root_object, "hasLocation.measLocId", loc2val);

	    JSON_Value *times2val = json_value_init_object();
	    JSON_Object *times2 = json_value_get_object(times2val);

	    json_object_set_string(times2, "format", "DATETIME");
	    json_object_set_string(times2, "value", timeStamp);

	    json_object_dotset_value(root_object, "hasTimestamp.hasUTCDateTime", times2val);

	serialized_string = json_serialize_to_string_pretty(root_value);

	/* Publish JSON */
	if(available(&MQTT))
	{
		publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT), "/movement", serialized_string);
	}

	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

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
