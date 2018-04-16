/*
 * message.c
 */

#include "message.h"

static void initEnvironmentMessage(void);
static void initMovementMessage(void);
static void initWarningMessage(void);
static void initLastPosMessage(void);
static void initWelcomeMessage(void);

bool json_serialize_mod(const JSON_Value *value, char tBuffer[], unsigned int sizeOfBuffer);

/* UUIDs */
char mevd_uuid[] = "5392";		// Movement Event Description UUID
char eevd_uuid[] = "3143";		// Environment Event Description UUID
char levd_uuid[] = "6356";		// Last Position Event Description UUID

char lp_uuid[] = "367";			// LastPoistion UUID
char rt_eevd_uuid[] = "366";	// RailTemperature UUID
char h_eevd_uuid[] = "365";		// AmbientHumidity UUID
char at_eevd_uuid[] = "364";	// AmbientTemperature UUID

// JSON variables to safe space

char lpMessage[1200];	// Last Position Array
char wMessage[300];		// Welcome message

/* ENVIRONMENT */
JSON_Value *root_value_env = NULL;
JSON_Object *root_object_env = NULL;

JSON_Value *arr_val_env = NULL;
JSON_Array *arr_obj_env = NULL;

JSON_Value *arrval_env = NULL;
JSON_Object *arro_env = NULL;

JSON_Value *arrsub_arr_env = NULL;
JSON_Array *arrsub_obj_env = NULL;

JSON_Value *arrsub_val_1_env = NULL;
JSON_Object *arrsub_o_1_env = NULL;

JSON_Value *arrsub_val_3_env = NULL;
JSON_Object *arrsub_o_3_env = NULL;

JSON_Value *arrsub_val_2_env = NULL;
JSON_Object *arrsub_o_2_env = NULL;

JSON_Value *loc2val_env = NULL;
JSON_Object *loc2_env = NULL;

JSON_Value *times2val_env = NULL;
JSON_Object *times2_env = NULL;

/* MOVEMENT */

JSON_Value *root_value_mov = NULL;
JSON_Object *root_object_mov = NULL;

JSON_Value *arr_val_mov = NULL;
JSON_Array *arr_obj_mov = NULL;

JSON_Value *arrval_mov = NULL;
JSON_Object *arro_mov = NULL;

JSON_Value *arrsub_arr_mov = NULL;
JSON_Array *arrsub_obj_mov = NULL;

JSON_Value *arrsub_val_1_mov = NULL;
JSON_Object *arrsub_o_1_mov = NULL;

JSON_Value *miniarr_val_mov = NULL;
JSON_Array *miniarr_mov = NULL;

JSON_Value *arrsub_val_2_mov = NULL;
JSON_Object *arrsub_o_2_mov = NULL;

JSON_Value *loc2val_mov = NULL;
JSON_Object *loc2_mov = NULL;

JSON_Value *times2val_mov = NULL;
JSON_Object *times2_mov = NULL;

/* WARNING */

JSON_Value *root_value_war = NULL;
JSON_Object *root_object_war = NULL;

/* LAST POSITION */

JSON_Value *root_value_las = NULL;
JSON_Object *root_object_las = NULL;

JSON_Value *arr_val_las = NULL;
JSON_Array *arr_obj_las = NULL;

JSON_Value *arrval_las = NULL;
JSON_Object *arro_las = NULL;

JSON_Value *arrsub_arr_las = NULL;
JSON_Array *arrsub_obj_las = NULL;

JSON_Value *arrsub_val_1_las = NULL;
JSON_Object *arrsub_o_1_las = NULL;

JSON_Value *arrsub_val_3_las = NULL;
JSON_Object *arrsub_o_3_las = NULL;

JSON_Value *arrsub_val_2_las = NULL;
JSON_Object *arrsub_o_2_las = NULL;

JSON_Value *loc2val_las = NULL;
JSON_Object *loc2_las = NULL;

JSON_Value *times2val_las = NULL;
JSON_Object *times2_las = NULL;

/* WELCOME */

JSON_Value *root_value_wel = NULL;
JSON_Object *root_object_wel = NULL;

/*
 * @brief This function insert the measured (ENV) data into the JSON structure and transforms it to string
 * @param Actual value of ambient temperature
 * @param Actual value of humidity
 * @param Actual value of rail temperature
 * @param Actual value of time stamp
 * @param Buffer that holds the message
 * @return This function returns true if parsing was successful otherwise it returns false
 */
bool parseEnvironmentMessage(float ambTemp, float humidity, float railTemp, char timeStamp[], char clientID[], char envBuffer[])
{
	json_object_dotset_number(arrsub_o_1_env, "hasValue.value", ambTemp);
	json_object_dotset_number(arrsub_o_2_env, "hasValue.value", humidity);
	json_object_dotset_number(arrsub_o_3_env, "hasValue.value", railTemp);
	json_object_set_string(loc2_env, "value", clientID);
	json_object_set_string(times2_env, "value", timeStamp);

	if(!json_serialize_mod(root_value_env, envBuffer, 1200))
		return false;
	else
		return true;
}

/*
 * @brief This function initializes Environment Message
 */
void initEnvironmentMessage(void)
{
	/* Init JSON Object */
	root_value_env = json_value_init_object();
	root_object_env = json_value_get_object(root_value_env);

	/* Init Big Array */
	arr_val_env = json_value_init_array();
	arr_obj_env = json_value_get_array(arr_val_env);

	/* Add array to object */
	json_object_set_value(root_object_env, "hasAttributes", arr_val_env);

	/* Array block */
	arrval_env = json_value_init_object();
	arro_env = json_value_get_object(arrval_env);

	json_object_set_string(arro_env, "hasAttributeClassType", "DA");
	json_object_set_string(arro_env, "hasName", "env_meas");

	/* Sub Array Block */
	arrsub_arr_env = json_value_init_array();
	arrsub_obj_env = json_value_get_array(arrsub_arr_env);

	json_object_set_value(arro_env, "hasValueContainers", arrsub_arr_env);

	/* Sub Object no.1 */
	arrsub_val_1_env = json_value_init_object();
	arrsub_o_1_env = json_value_get_object(arrsub_val_1_env);

	json_object_dotset_string(arrsub_o_1_env, "uuid.value", at_eevd_uuid);

	json_array_append_value(arrsub_obj_env, arrsub_val_1_env);

	/* Sub Object no.2 */
	arrsub_val_2_env = json_value_init_object();
	arrsub_o_2_env = json_value_get_object(arrsub_val_2_env);

	json_object_dotset_string(arrsub_o_2_env, "uuid.value", h_eevd_uuid);
	json_array_append_value(arrsub_obj_env, arrsub_val_2_env);

	/* Sub Object no.3 */
	arrsub_val_3_env = json_value_init_object();
	arrsub_o_3_env = json_value_get_object(arrsub_val_3_env);

	json_object_dotset_string(arrsub_o_3_env, "uuid.value", rt_eevd_uuid);
	json_array_append_value(arrsub_obj_env, arrsub_val_3_env);

	/* Append the big array */
	json_array_append_value(arr_obj_env, arrval_env);

	/* Remaining pairs */
	json_object_dotset_string(root_object_env, "hasEventDescriptionUuid.value", eevd_uuid);

	loc2val_env = json_value_init_object();
	loc2_env = json_value_get_object(loc2val_env);
	json_object_dotset_value(root_object_env, "hasLocation.measLocId", loc2val_env);

	times2val_env = json_value_init_object();
	times2_env = json_value_get_object(times2val_env);

	json_object_set_string(times2_env, "format", "DATETIME");

	json_object_dotset_value(root_object_env, "hasTimestamp.hasUTCDateTime", times2val_env);

	return;
}

/*
 * @brief This function initializes Movement Message
 */
void initMovementMessage(void)
{
	/* Init JSON Object */
	root_value_mov = json_value_init_object();
	root_object_mov = json_value_get_object(root_value_mov);

	/* Init Big Array */
	arr_val_mov = json_value_init_array();
	arr_obj_mov = json_value_get_array(arr_val_mov);

	/* Add array to object */
	json_object_set_value(root_object_mov, "hasAttributes", arr_val_mov);

	/* Array block */
	arrval_mov = json_value_init_object();
	arro_mov = json_value_get_object(arrval_mov);

	json_object_set_string(arro_mov, "hasAttributeClassType", "DA");
	json_object_set_string(arro_mov, "hasName", "mov_meas");

	/* Sub Array Block */
	arrsub_arr_mov = json_value_init_array();
	arrsub_obj_mov = json_value_get_array(arrsub_arr_mov);

	json_object_set_value(arro_mov, "hasValueContainers", arrsub_arr_mov);

	/* Sub Object no.1 */
	arrsub_val_1_mov = json_value_init_object();
	arrsub_o_1_mov = json_value_get_object(arrsub_val_1_mov);

	/* Mini example Array */
	miniarr_val_mov = json_value_init_array();
	miniarr_mov = json_value_get_array(miniarr_val_mov);
	json_array_append_string(miniarr_mov, "xxxx");
	json_array_append_string(miniarr_mov, "yyyy");
	json_object_dotset_value(arrsub_o_1_mov, "hasValue.values", miniarr_val_mov);

	/* Sub Object no.1 */

	json_object_dotset_string(arrsub_o_1_mov, "uuid.value", "move");
	json_array_append_value(arrsub_obj_mov, arrsub_val_1_mov);

	/* Sub Object no.2 */
	arrsub_val_2_mov = json_value_init_object();
	arrsub_o_2_mov = json_value_get_object(arrsub_val_2_mov);

	json_object_dotset_string(arrsub_o_2_mov, "uuid.value", "duration");
	json_array_append_value(arrsub_obj_mov, arrsub_val_2_mov);

	/* Append the big array */
	json_array_append_value(arr_obj_mov, arrval_mov);

	/* Remaining pairs */
	json_object_dotset_string(root_object_mov, "hasEventDescriptionUuid.value", mevd_uuid);

	loc2val_mov = json_value_init_object();
	loc2_mov = json_value_get_object(loc2val_mov);

	json_object_dotset_value(root_object_mov, "hasLocation.measLocId", loc2val_mov);

	times2val_mov = json_value_init_object();
	times2_mov = json_value_get_object(times2val_mov);

	json_object_set_string(times2_mov, "format", "DATETIME");

	json_object_dotset_value(root_object_mov, "hasTimestamp.hasUTCDateTime", times2val_mov);

	return;
}

/*
 * @brief This function initializes Warning Message
 */
void initWarningMessage(void)
{
	/* Init JSON Object */
	root_value_war = json_value_init_object();
	root_object_war = json_value_get_object(root_value_war);
}

/*
 * @brief Initialization Last Position Message
 */
void initLastPosMessage(void)
{
	/* Init JSON Object */
	root_value_las = json_value_init_object();
	root_object_las = json_value_get_object(root_value_las);

	/* Init Big Array */
	arr_val_las = json_value_init_array();
	arr_obj_las = json_value_get_array(arr_val_las);

	/* Add array to object */
	json_object_set_value(root_object_las, "hasAttributes", arr_val_las);

	/* Array block */
	arrval_las = json_value_init_object();
	arro_las = json_value_get_object(arrval_las);

	json_object_set_string(arro_las, "hasAttributeClassType", "DA");
	json_object_set_string(arro_las, "hasName", "last_position");

	/* Sub Array Block */
	arrsub_arr_las = json_value_init_array();
	arrsub_obj_las = json_value_get_array(arrsub_arr_las);

	json_object_set_value(arro_las, "hasValueContainers", arrsub_arr_las);

	/* Sub Object no.1 */
	arrsub_val_1_las = json_value_init_object();
	arrsub_o_1_las = json_value_get_object(arrsub_val_1_las);

	json_object_dotset_string(arrsub_o_1_las, "uuid.value", lp_uuid);

	json_array_append_value(arrsub_obj_las, arrsub_val_1_las);

	/* Append the big array */
	json_array_append_value(arr_obj_las, arrval_las);

	/* Remaining stuffs */

	json_object_dotset_string(root_object_las, "hasEventDescriptionUuid.value", levd_uuid);

	loc2val_las = json_value_init_object();
	loc2_las = json_value_get_object(loc2val_las);
	json_object_dotset_value(root_object_las, "hasLocation.measLocId", loc2val_las);

	times2val_las = json_value_init_object();
	times2_las = json_value_get_object(times2val_las);

	json_object_set_string(times2_las, "format", "DATETIME");

	json_object_dotset_value(root_object_las, "hasTimestamp.hasUTCDateTime", times2val_las);
}

/*
 * @brief Initialization Welcome Message
 */
void initWelcomeMessage(void)
{
	root_value_wel = json_value_init_object();
	root_object_wel = json_value_get_object(root_value_wel);
}

/*
 * @brief This function initializes each type of message
 */
void initMessages(void)
{
	initEnvironmentMessage();
	initMovementMessage();
	initWarningMessage();
	initLastPosMessage();
	initWelcomeMessage();
}

/*
 * @brief This function insert the measured (MOV) data into the JSON structure and transforms it to string
 * @param Actual value of duration
 * @param The identifier of the location (MQTT client) i.e. the BOX ID
 * @param Actual value of time stamp
 * @param Buffer that holds the message
 * @return This function returns true if parsing was successful otherwise it returns false
 */
bool parseMovementMessage(uint32_t duration, char meas_loc[], char timeStamp[], char message[])
{
	json_object_dotset_number(arrsub_o_2_mov, "hasValue.value", duration);
	json_object_set_string(loc2_mov, "value", meas_loc);
	json_object_set_string(times2_mov, "value", timeStamp);
	if(!json_serialize_mod(root_value_mov, message, 2000))
		return false;
	else
		return true;
}

/* Serializing JSON structure */
bool json_serialize_mod(const JSON_Value *value, char tBuffer[], unsigned int sizeOfBuffer) {
    size_t byteToWrite = json_serialization_size_pretty(value);
    if (byteToWrite == 0) {
        return false;
    }
    if(byteToWrite > sizeOfBuffer)
    	return false;
    if(json_serialize_to_buffer_pretty(value, tBuffer, byteToWrite) != JSONSuccess)
    	return false;
    else
    	return true;
}

/*
 * @brief This function sends messages to the broker
 * @param Pointer that holds the buffers of messages
 * @param The type of the actual message that is wanted to be sent
 * @return This function returns true if MQTT connection is alive, otherwise it returns false
 */
bool sendMessage(char *message[], char messageType)
{
	if(!isConnected())
		return false;
	switch(messageType)
	{
	case 'E':
		publish("/environment", message[0]);
		break;
	case 'M':
		publish("/movement", message[1]);
		break;
	case 'W':
		publish("/warning", message[2]);
		break;
	}
	return true;
}

/*
 * @brief This function creates a JSON warning message and serialize it
 * @param Buffer for the serialized message
 * @param Buffer that contains TimeStamp
 * @param The ID of the actual measurement box
 */
bool stampWarning(char tBuffer[], char timeStamp[], char measLoc[])
{
	json_object_set_string(root_object_war, "boxId", measLoc);
	json_object_set_string(root_object_war, "message", "WARNING! The switching"
			" operation failed during the last measurement!");
	json_object_set_string(root_object_war, "value", timeStamp);
	json_serialize_mod(root_value_war, tBuffer, 2000);
	return true;
}

/*
 * @brief This function sends a last position message to the broker
 * @param The value of the actual position of point blades
 */
void sendLastPosition(float lastPosition, char measLoc[], char timeStamp[])
{
	if(!isConnected())
		return;

	json_object_dotset_number(arrsub_o_1_las, "hasValue.value", lastPosition);
	json_object_set_string(loc2_las, "value", measLoc);
	json_object_set_string(times2_las, "timestamp", timeStamp);
	json_serialize_mod(root_value_las, lpMessage, 1200);

	publish("/last", lpMessage);
}

void sendWelcome(char clientID[], char tStamp[])
{
	strcpy(wMessage, "Client: ");
	strcat(wMessage, clientID);
	strcat(wMessage, " has connected!");

	json_object_set_string(root_object_wel, "boxId", clientID);
	json_object_set_string(root_object_wel, "message", wMessage);
	json_object_set_string(root_object_wel, "value", tStamp);
	if(!isConnected())
		return;
	json_serialize_mod(root_value_wel, wMessage, 2000);
	publish("/welcome", wMessage);
}
