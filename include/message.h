/*
 * message.h
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "init.h"
#include "comm.h"

void initMessages(void);

bool parseEnvironmentMessage(float ambTemp, float humidity, float railTemp, char timeStamp[], char clientID[], char envBuffer[]);
bool parseMovementMessage(uint32_t duration, char meas_loc[], char timeStamp[], char message[]);
bool stampWarning(char tBuffer[], char timeStamp[], char measLoc[]);

bool sendMessage(char *message[], char messageType);
void sendLastPosition(float lastPosition, char measLoc[], char timeStamp[]);
void sendWelcome(char clientID[], char tStamp[]);

#endif /* MESSAGE_H_ */
