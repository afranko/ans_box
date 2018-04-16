/*
 * measurement.h
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "message.h"

typedef struct
{
	/* GLOBAL FLAGS */
	bool S_MEAS_FLAG;
	bool E_MEAS_FLAG;
	bool MSG_FLAG;
	bool MSG_PARSED;
	bool warningFlag;

	uint32_t duration;

	/* INNER FLAGS */
	bool offMeas;
	bool mFlag;
	bool endSet;
	bool endMeas;

	/* OFFSETS */
	uint16_t noffset;
	uint16_t measLength;

	uint16_t measc0;
	uint16_t measc1;
	uint16_t measc2;
	uint16_t measc3;

	uint16_t pofc0;
	uint16_t pofc1;
	uint16_t pofc2;
	uint16_t pofc3;

}measurement_flag_block;

extern cBuff *measBuffers[4];
extern cBuff *sendBuffers[4];
extern Settings_HandleTypeDef *hconfig;

measurement_flag_block mfb;

char clientIdentifier[24];
RTC_HandleTypeDef *rtcClock;

void initFlags(void);
void doMovMeas(bool *msgFlag, bool *warningFlag);
bool movMeasToMessage(char movBuffer[], char warTimeStamp[]);
bool initPT(SPI_HandleTypeDef *hspi);
bool doEnvMeas(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi, char envBuffer[]);
void sendWelcomeMessage(void);
bool getTimeStamp(char timeStamp[]);

inline void initMeasurementParams(char clientName[], RTC_HandleTypeDef *hrtcm)
{
	strcpy(clientIdentifier, clientName);
	rtcClock = hrtcm;
}

#endif /* MEASUREMENT_H_ */
