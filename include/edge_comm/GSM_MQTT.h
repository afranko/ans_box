/*
  MQTT.h - Library for GSM MQTT Client.
  Created by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
  Released into the public domain.
  Modified to STM32 by Attila. E. Franko
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"

#ifndef GSM_MQTT_H_
#define GSM_MQTT_H_

#define UART_BUFFER_LENGTH 300    //Maximum length allowed for UART data
#define TOPIC_BUFFER_LENGTH 50    //Maximum length allowed Topic
#define MESSAGE_BUFFER_LENGTH 250  //Maximum length allowed data

// ######################################################################################################################
#define CONNECT     1   //Client request to connect to Server                Client          Server
#define CONNACK     2   //Connect Acknowledgment                             Server/Client   Server/Client
#define PUBLISH     3   //Publish message                                    Server/Client   Server/Client
#define PUBACK      4   //Publish Acknowledgment                             Server/Client   Server/Client
#define PUBREC      5   //Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define PUBREL      6   //Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define PUBCOMP     7   //Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define SUBSCRIBE   8   //Client Subscribe request                           Client          Server
#define SUBACK      9   //Subscribe Acknowledgment                           Server          Client
#define UNSUBSCRIBE 10  //Client Unsubscribe request                         Client          Server
#define UNSUBACK    11  //Unsubscribe Acknowledgment                         Server          Client
#define PINGREQ     12  //PING Request                                       Client          Server
#define PINGRESP    13  //PING Response                                      Server          Client
#define DISCONNECT  14  //Client is Disconnecting                            Client          Server

// QoS value bit 2 bit 1 Description
//   0       0       0   At most once    Fire and Forget         <=1
//   1       0       1   At least once   Acknowledged delivery   >=1
//   2       1       0   Exactly once    Assured delivery        =1
//   3       1       1   Reserved
#define DUP_Mask      8   // Duplicate delivery   Only for QoS>0
#define QoS_Mask      6   // Quality of Service
#define QoS_Scale     2   // (()&QoS)/QoS_Scale
#define RETAIN_Mask   1   // RETAIN flag

#define User_Name_Flag_Mask  128
#define Password_Flag_Mask   64
#define Will_Retain_Mask     32
#define Will_QoS_Mask        24
#define Will_QoS_Scale       8
#define Will_Flag_Mask       4
#define Clean_Session_Mask   2

#define DISCONNECTED          0
#define CONNECTED             1
#define NO_ACKNOWLEDGEMENT  255

uint32_t millis(void);

typedef struct serial_buff_t{
	int buffer_length;
	int buffer_head;
	int buffer_tail;
	volatile uint8_t buffer_t[UART_BUFFER_LENGTH];
	volatile uint8_t temp_val;
}serialBuff;

typedef struct gsm_mqtt_t{
	volatile bool TCP_Flag;
	volatile char GSM_ReplyFlag;
	char reply[10];
	volatile bool pingFlag;
	volatile char tcpATerrorcount;
	volatile bool MQTT_Flag;
	volatile int ConnectionAcknowledgement;
	volatile int PublishIndex;
	char Topic[TOPIC_BUFFER_LENGTH];
	volatile int TopicLength;
	char Message[MESSAGE_BUFFER_LENGTH];
	volatile int MessageLength;
	volatile int MessageFlag;
	volatile char modemStatus;
	volatile uint32_t index;
	volatile uint32_t length;
	volatile uint32_t lengthLocal;

	char inputString[UART_BUFFER_LENGTH];

	volatile unsigned int _LastMessaseID;
	volatile char _ProtocolVersion;
	volatile unsigned long _PingPrevMillis;
	volatile char _tcpStatus;
	volatile char _tcpStatusPrev;
	volatile unsigned long _KeepAliveTimeOut;

	UART_HandleTypeDef* gsm_uart;
	volatile uint8_t uartFlag;
	volatile bool respFlag;

}GSM_MQTT;

void serialPrint(GSM_MQTT *object, char value);
void serialWrite(GSM_MQTT *object, char *string);

void softLog(char *inputString);
void softLogLn(char *inputString);
void softLogSin(int value);
void softLogSinLn(int value);

int serialAvailable(GSM_MQTT *object, serialBuff *buff);
char serialRead(serialBuff *buff);
void initSerialBuf(serialBuff *buff);
void serialEvent();
void serialGet();

void _pingRespHandler(GSM_MQTT *object);

void GSM_MQTT_constructor(GSM_MQTT *object, unsigned long KeepAlive, UART_HandleTypeDef *huart);
void begin(GSM_MQTT *object);
void _connect(GSM_MQTT *object, char *ClientIdentifier, char UserNameFlag, char PasswordFlag, char *UserName, char *Password, char CleanSession, char WillFlag, char WillQoS, char WillRetain, char *WillTopic, char *WillMessage);
void publish(GSM_MQTT *object, char DUP, char Qos, char RETAIN, unsigned int MessageID, char *Topic, char *Message);
void subscribe(GSM_MQTT *object, char DUP, unsigned int MessageID, char *SubTopic, char SubQoS);
void unsubscribe(GSM_MQTT *object, char DUP, unsigned int MessageID, char *SubTopic);
void disconnect(GSM_MQTT *object);
void processing(GSM_MQTT *object);
bool available(GSM_MQTT *object);
void AutoConnect(GSM_MQTT *object);
void OnConnect(GSM_MQTT *object);
void OnMessage(char *Topic, int TopicLength, char *Message, int MessageLength);
void publishACK(GSM_MQTT *object, unsigned int MessageID);
void publishREC(GSM_MQTT *object, unsigned int MessageID);
void publishREL(GSM_MQTT *object, char DUP, unsigned int MessageID);
void publishCOMP(GSM_MQTT *object, unsigned int MessageID);
void printMessageType(uint8_t Message);
void printConnectAck(uint8_t Ack);
char sendATreply(GSM_MQTT *object, char *command, char *replystr, unsigned long waitms);

void _sendUTFString(GSM_MQTT *object, char *string);
void _sendLength(GSM_MQTT *object, int len);
void _ping(GSM_MQTT *object);
void _tcpInit(GSM_MQTT *object);
char _sendAT(GSM_MQTT *object, char *command, unsigned long waitms);
unsigned int _generateMessageID(GSM_MQTT *object);

void MQTT_start(UART_HandleTypeDef* huart, unsigned long KeepAlive);

#endif /* GSM_MQTT_H_ */
