/*
 * comm.h - Communucation Library (SIM800 GSM modem, MQTT)
 *
 */

#ifndef COMM_H_
#define COMM_H_

#include "init.h"

#define CONNECT     1U	//Client request to connect to Server                Client          Server
#define CONNACK     2U	//Connect Acknowledgment                             Server/Client   Server/Client
#define PUBLISH     3U	//Publish message                                    Server/Client   Server/Client
#define PUBACK      4U	//Publish Acknowledgment                             Server/Client   Server/Client
#define PUBREC      5U	//Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define PUBREL      6U	//Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define PUBCOMP     7U	//Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define SUBSCRIBE   8U	//Client Subscribe request                           Client          Server
#define SUBACK      9U	//Subscribe Acknowledgment                           Server          Client
#define UNSUBSCRIBE 10U	//Client Unsubscribe request                         Client          Server
#define UNSUBACK    11U	//Unsubscribe Acknowledgment                         Server          Client
#define PINGREQ     12U	//PING Request                                       Client          Server
#define PINGRESP    13U	//PING Response                                      Server          Client
#define DISCONNECT  14U	//Client is Disconnecting                            Client          Server

#define FIXEDFLAGS	0x02//Fixed FLAG bits for PUBREL, SUBSCRIBE, UNSUBSCRIBE messages

/* Structures */

#define BUFFLENGTH		255U
#define MQTTBUFFER		1024U
#define MQTT_TIMEOUT	500U

typedef enum
{
	GSM_OK,
	GSM_NO_RESP
}GSM_Resp;

typedef enum
{
	GSM_NOT_CONN,
	GSM_GPRS,
	GSM_IP_INITIAL,
	GSM_IP_START,
	GSM_IP_GPRSACT,
	GSM_IP_STATUS,
	MQTT
}GSM_State;

typedef enum
{
	NONE,
	R_CONNACK,
	R_PUBACK,
	R_PUBREC,
	R_PUBREL,
	R_SUBACK,
	R_UNSUBACK
}MQTT_Response;

typedef struct
{
	GSM_State state;
	gBuff *inBuffer;
	char responseBuffer[BUFFLENGTH];
	char *apnString;
	char connString[];
}GCF_HandleTypeDef;

typedef struct
{
	uint16_t keepAlive;
	char clientID[24];

	bool pingFlag;
	uint32_t pingSent;
	uint8_t pingRetry;

	char parseBuffer[MQTTBUFFER];

	bool connFlag;
	bool freshMessage;

	char rxBuffer[2000];
	char topicName[200];

	gBuff *inBuffer;
	GCF_HandleTypeDef *gsm;
}MQTT_HandleTypeDef;

/* Variables */
GCF_HandleTypeDef *hgcf;	// Process Handler Structure
MQTT_HandleTypeDef *hmqtt;
gBuff *inBuffer;			// Buffer for input data

/* External Variable */
extern bool welcomeFlag;

/* Functions */
void init_comm(Settings_HandleTypeDef *configx);
void comm_handler(void);
bool setRTC(void);

void publish(char topic[], char message[]);
void subscribe(char topic[]);
void unsubscribe(char topic[]);
void recMessage(char topic[], char message[]);

inline bool isConnected(void){
	return hmqtt->connFlag;
}
inline void getSerial(uint8_t byte){
	push_gBuff(inBuffer, byte);
}
inline bool isFreshMessage(void){
	return hmqtt->freshMessage;
}

#endif /* COMM_H_ */
