/* Communication Library for Stm32F4 via SIM800 & MQTT */

#include "edge_comm/edge_comm.h"

GSM_MQTT MQTT;
MQTT_QoS QoSVal;
serialBuff uartBuffer;

/* UUIDs */
char mevd_uuid[] = "5392";		//Movement Event Description UUID
char eevd_uuid[] = "3143";		//Environment Event Description UUID

char rt_eevd_uuid[] = "366";	//RailTemperature UUID
char h_eevd_uuid[] = "365";		//AmbientHumidity UUID
char at_eevd_uuid[] = "364";	//AmbientTemperature UUID

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

void sendEnvironmentMessage(char *meas_loc, char* timeStamp, float railTemp, float ambTemp, float hum)
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

		json_object_set_string(arro, "hasAttributeClassType", "DA");
		json_object_set_string(arro, "hasName", "env_meas");

		/* Sub Array Block */
		JSON_Value *arrsub_arr = json_value_init_array();
		JSON_Array *arrsub_obj = json_value_get_array(arrsub_arr);

		json_object_set_value(arro, "hasValueContainers", arrsub_arr);

		    /* Sub Object no.1 */
		    JSON_Value *arrsub_val_1 = json_value_init_object();
		    JSON_Object *arrsub_o_1 = json_value_get_object(arrsub_val_1);

		    json_object_dotset_number(arrsub_o_1, "hasValue.value", ambTemp);
		    json_object_dotset_string(arrsub_o_1, "uuid.value", at_eevd_uuid);

		    json_array_append_value(arrsub_obj, arrsub_val_1);

		    /* Sub Object no.2 */
		    JSON_Value *arrsub_val_2 = json_value_init_object();
		    JSON_Object *arrsub_o_2 = json_value_get_object(arrsub_val_2);

		    json_object_dotset_number(arrsub_o_2, "hasValue.value", hum);
		    json_object_dotset_string(arrsub_o_2, "uuid.value", h_eevd_uuid);
		    json_array_append_value(arrsub_obj, arrsub_val_2);

		    /* Sub Object no.3 */
		    JSON_Value *arrsub_val_3 = json_value_init_object();
		    JSON_Object *arrsub_o_3 = json_value_get_object(arrsub_val_3);

		    json_object_dotset_number(arrsub_o_3, "hasValue.value", railTemp);
		    json_object_dotset_string(arrsub_o_3, "uuid.value", rt_eevd_uuid);
		    json_array_append_value(arrsub_obj, arrsub_val_3);

		    /* Append the big array */
		    json_array_append_value(arr_obj, arrval);

		    /* Remaining pairs */
		    json_object_dotset_string(root_object, "hasEventDescriptionUuid.value", eevd_uuid);

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

char* parseMovementMessage(uint32_t duration, char *meas_loc, char* timeStamp)
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

	    json_object_set_string(arro, "hasAttributeClassType", "DA");
	    json_object_set_string(arro, "hasName", "mov_meas");

	    	/* Sub Array Block */
	    	JSON_Value *arrsub_arr = json_value_init_array();
	    	JSON_Array *arrsub_obj = json_value_get_array(arrsub_arr);

	    	json_object_set_value(arro, "hasValueContainers", arrsub_arr);

	    		/* Sub Object no.1 */
	    		JSON_Value *arrsub_val_1 = json_value_init_object();
	    		JSON_Object *arrsub_o_1 = json_value_get_object(arrsub_val_1);

	    		/* Mini example Array */
	    		JSON_Value *miniarr_val = json_value_init_array();
	    		JSON_Array *miniarr = json_value_get_array(miniarr_val);
	    		json_array_append_string(miniarr, "xxxx");
	    		json_array_append_string(miniarr, "yyyy");
	    		json_object_dotset_value(arrsub_o_1, "hasValue.values", miniarr_val);

	    		/* Sub Object no.1 */

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
	    json_object_dotset_string(root_object, "hasEventDescriptionUuid.value", mevd_uuid);

	    JSON_Value *loc2val = json_value_init_object();
	    JSON_Object *loc2 = json_value_get_object(loc2val);
	    json_object_set_string(loc2, "value", meas_loc);
	    json_object_dotset_value(root_object, "hasLocation.measLocId", loc2val);

	    JSON_Value *times2val = json_value_init_object();
	    JSON_Object *times2 = json_value_get_object(times2val);

	    json_object_set_string(times2, "format", "DATETIME");
	    json_object_set_string(times2, "value", timeStamp);

	    json_object_dotset_value(root_object, "hasTimestamp.hasUTCDateTime", times2val);

	/* Create String */
	serialized_string = json_serialize_to_string_pretty(root_value);

	/* Free JSON */
	json_value_free(root_value);

	/* Return string */
	return serialized_string;
}

void sendMovementMessage(char* meas_string, bool wFlag)
{
	/* Publish JSON */
	if(available(&MQTT))
	{
		if(wFlag == true)
		{
			publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT),
					"/warning", "Measurement Timeout!");
		}
			publish(&MQTT, QoSVal.s_DUP, QoSVal.s_QoS, QoSVal.s_RETAIN, _generateMessageID(&MQTT),
					"/movement", meas_string);
		}
	return;
}

void MQTTProcessing(void)
{
	while(serialAvailable(&MQTT, &uartBuffer) != 0)
	{
		serialEvent();
	}
	processing(&MQTT);
	return;
}
