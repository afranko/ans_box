/*
 * edge_comm.h
 *
 *  Created on: 2017. febr. 23.
 *      Author: Attila E. Franko
 *
 *      Communication library for Mantis Edge devices
 */

#ifndef EDGEDIV_COMM_H_
#define EDGEDIV_COMM_H_

#include "edge_comm/parson.h"
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "GSM_MQTT.h"

typedef struct mqtt_qos_v
{
	char s_QoS;
	char s_DUP;
	char s_RETAIN;
}MQTT_QoS;

void CommInit(UART_HandleTypeDef *huart, unsigned long KeepAlive);
char* parseMovementMessage(uint32_t duration, char *meas_loc, char* timeStamp);
void sendMovementMessage(char* meas_string, bool wFlag);
void sendEnvironmentMessage(char *meas_loc, char* timeStamp);

void MQTTProcessing();
void setSend(char QoS, char DUP, char RETAIN);

#endif /* EDGEDIV_COMM_H_ */
