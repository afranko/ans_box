/*
 * measurement.h
 *
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "circ_buff.h"

#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7

typedef struct measurement_flag_block_t
{
	/* Negative offset */
	uint16_t noffset0;
	uint16_t noffset1;
	uint16_t noffset2;
	uint16_t noffset3;

	bool offMeas;
	uint16_t ocounter;

	/* Measurement */
	uint16_t measc0;
	uint16_t measc1;
	uint16_t measc2;
	uint16_t measc3;

	bool mFlag;
	uint16_t measLength;

	/* Positive Offset */
	uint16_t pofc0;
	uint16_t pofc1;
	uint16_t pofc2;
	uint16_t pofc3;

	bool end_set;
	bool endMeas;

	uint32_t duration;		// Measurement Duration

	bool S_MEAS_FLAG,		// Start of measurement flag
	E_MEAS_FLAG,			// End of measurement flag
	CLEAR_FLAG,				// Clear measurement flag
	MSG_FLAG;				// Send message flag

	bool warning_flag;		// Measurement Timeout flag
	bool MSG_SENT;			// Message Sent flag

}meas_flag_block;

/* Buffers */
extern cBuff cont_0, cont_1, cont_2, cont_3, gBuffer0, gBuffer1, gBuffer2, gBuffer3;

/* Flag for forced measurement */
extern bool HOLDFlag;

uint16_t read_last(cBuff *buff_c);
void meas_datamove(void);
void init_meas_flag_block(meas_flag_block *flagBlock);
void intoa_conv(uint16_t data, char *ibuffer);
void getTimeStamp(RTC_HandleTypeDef *hrtc, char *TimeString);
void envMeas(RTC_HandleTypeDef *hrtc, I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi);
void readTelaire(I2C_HandleTypeDef *hi2c, float *humidity, float *temperature);
void readPT(SPI_HandleTypeDef *hspi, float *rtemp);
void movMeas(RTC_HandleTypeDef *hrtc);

#endif /* MEASUREMENT_H_ */
