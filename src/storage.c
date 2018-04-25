/*
 * storage.c
 */

#include "storage.h"

FIL 	CONFIG_S;

static uint16_t storedNum = 0;

static uint16_t checkStored(void);
static bool seekTimeout(DWORD pState);

/*
 * @brief This functions stores a message on the SD card
 * @param The message as a string that is wanted to be stored
 * @return This function return true if everything works fine (mount, open, write, unmount) otherwise it return false.
 */
bool writeToStorage(char message[], char messageType)
{
	FRESULT f_res;
	DWORD actualPointer;
	UINT backp;
	uint32_t siz;
	uint8_t timeout = 0;

	if(storedNum >= 100U)	// If we store more than 100 messages, that's indicates a huge problem in communication
		return false;

	/* Mount SD - Mounting Error*/
	do
	{
		if(timeout > 1)
			f_mount(NULL, "0:", 1);
		f_res = f_mount(&FS, "0:", 1);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	timeout = 0;

	/* Open file */
	do
	{
		if(timeout > 0)
			f_close(&CONFIG_S);
		f_res = f_open(&CONFIG_S, "storage.mnts", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		timeout++;
	}
	while(f_res != FR_OK && f_res != FR_NO_FILE && timeout != UINT8_MAX);

	/* SD physical or any other error */
	if(timeout == UINT8_MAX)
		return false;

	/* SD is OK, but there isn't any stored file -> Let's create a new one*/
	else if(f_res == FR_NO_FILE)
	{
		timeout = 0;
		do
		{
			f_res = f_open(&CONFIG_S, "storage.mnts", FA_CREATE_NEW | FA_WRITE | FA_READ);
			timeout++;
		}
		while(f_res != FR_OK && timeout != UINT8_MAX);
		if(timeout == UINT8_MAX)
			return false;

		timeout = 0;
		do
		{
			if(timeout > 0)
				if(!seekTimeout(0))
					return false;
			f_write(&CONFIG_S, 0x0000, 2, &backp); // TWO BYTES - 0x00 (MSB), 0x00 (LSB)
			timeout++;
		}
		while(backp != 2 && timeout == UINT8_MAX);
		if(timeout == UINT8_MAX)
			return false;
	}

	/* Write message */
	actualPointer = f_size(&CONFIG_S);
	if(!seekTimeout(actualPointer))
		return false;

	siz = (uint16_t)strlen(message);

	/* Place Head Sentinels */
	timeout = 0;
	do
	{
		if(timeout > 0)
			if(!seekTimeout(actualPointer))
				return false;
		f_write(&CONFIG_S, (uint8_t[]){0xFF, 0xAA}, 2, &backp);
		timeout++;
	}
	while(backp != 2 && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	actualPointer = f_tell(&CONFIG_S);

	/* Print type of the message */
	timeout = 0;
	do
	{
		if(timeout > 0)
			if(!seekTimeout(actualPointer))
				return false;
		f_write(&CONFIG_S, (char*)&messageType, 1, &backp);
		timeout++;
	}
	while(backp != 1 && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	actualPointer = f_tell(&CONFIG_S);

	/* Print the message itself */
	timeout = 0;
	do
	{
		if(timeout > 0)
			if(!seekTimeout(actualPointer))
				return false;
		f_write(&CONFIG_S, message, siz, &backp);
		timeout++;
	}
	while(backp !=  siz && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	actualPointer = f_tell(&CONFIG_S);

	/* Place Tail Sentinels */
	timeout = 0;
	do
	{
		if(timeout > 0)
			if(!seekTimeout(actualPointer))
				return false;
		f_write(&CONFIG_S, (uint8_t[]){0xAA,0xFF}, 2, &backp);
		timeout++;
	}
	while(backp != 2 && timeout != UINT8_MAX);

	if(timeout == UINT8_MAX)
		return false;

	actualPointer = f_tell(&CONFIG_S);

	/* Set number of stored messages */
	if(!seekTimeout(0))
		return false;

	++storedNum;	// Increasing number of stored messages
	timeout = 0;

	do
	{
		if(timeout > 0)
			if(!seekTimeout(0))
				return false;
		f_write(&CONFIG_S, (uint8_t*)&storedNum, 2, &backp);
		timeout++;
	}
	while(backp != 2 && timeout != UINT8_MAX);

	if(timeout == UINT8_MAX)
		return false;

	/* Close and unmount File */
	f_close(&CONFIG_S);
	f_mount(NULL, "0:", 1);

	/* Everything is OK */
	return true;
}

/*
 * @brief This function read a stored message from SD card
 * @param The stored messages as a String [OUT]
 * @return This function returns true if everything (mount, open, read, delete, unmount) was successful, otherwise it returns false.
 */
bool readFromStorage(char *message[], char *messageType)
{
	FRESULT f_res;
	DWORD actualPointer, readFirstPointer;
	UINT backp;
	uint8_t lastRead[2] = {0x00, 0x00}, timeout = 0;

	/* Mount SD - Mounting Error*/
	do
	{
		if(timeout > 1)
			f_mount(NULL, "0:", 1);
		f_res = f_mount(&FS, "0:", 1);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	timeout = 0;

	do
	{
		if(timeout > 0)
			f_close(&CONFIG_S);
		f_res = f_open(&CONFIG_S, "storage.mnts", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);

	timeout = 0;

	/* SD physical or any other error */
	if(f_res != FR_OK)
		return false;

	/* Find the first stored message on SD card*/
	while((lastRead[0] != 0xFF) || (lastRead[1] != 0xAA))
	{
		lastRead[0] = lastRead[1];
		actualPointer = f_tell(&CONFIG_S);
		do
		{
			if(timeout > 0)
				if(!seekTimeout(actualPointer))
					return false;
			f_read(&CONFIG_S, &lastRead[1], 1, &backp);
			timeout++;
		}
		while(backp != 1 && timeout != UINT8_MAX);
		if(backp != 1)
			return false;
	}

	/* Read and delete Message from Storage */

	lastRead[0] = 0;
	lastRead[1] = 0;

	readFirstPointer = f_tell(&CONFIG_S) - 2U;

	uint32_t sizeOfString = 0;

	while(true)
	{
		if(sizeOfString == 2)
			*messageType = (char) lastRead[0];	// Set MessageType
		if(sizeOfString > 2)
		{
			if(*messageType == 'E')
				message[0][sizeOfString-3] = (char) lastRead[0];	// Read Environment Message
			else if(*messageType == 'M')
				message[1][sizeOfString-3] = (char) lastRead[0];	// Read Movement Message
			else if(*messageType == 'W')
				message[2][sizeOfString-3] = (char) lastRead[0];	// Read Warning Message
			else
				return false;
		}
		lastRead[0] = lastRead[1];
		actualPointer = f_tell(&CONFIG_S);
		timeout = 0;
		do
		{
			if(timeout > 0)
				if(!seekTimeout(actualPointer))
					return false;
			f_read(&CONFIG_S, &lastRead[1], 1, &backp);
			timeout++;
		}
		while(backp != 1 && timeout != UINT8_MAX);
		if(backp != 1)
			return false;
		if((lastRead[0] == 0xAA) && (lastRead[1] == 0xFF))
		{
			switch(*messageType)	// Terminating NULL
			{
			case 'E': message[0][sizeOfString-2] = '\0'; break;
			case 'M': message[1][sizeOfString-2] = '\0'; break;
			case 'W': message[2][sizeOfString-2] = '\0'; break;
			}
			break;
		}
		if(sizeOfString++ > 66000U)
			return false;
	}

	sizeOfString -= 3U;

	/* Delete read bytes */

	if(!seekTimeout(readFirstPointer))
		return false;

	for(uint16_t actByte = 0; actByte < sizeOfString + 4U + 1U; actByte++)
	{
		actualPointer = f_tell(&CONFIG_S);
		timeout = 0;
		do
		{
			if(timeout > 0)
				if(!seekTimeout(actualPointer))
					return false;
			f_write(&CONFIG_S, 0U, 1, &backp);
			timeout++;
		}
		while(backp != 1 && timeout != UINT8_MAX);
		if(backp != 1)
			return false;
	}

	/* Decrease number of stored bytes - Pointer set to Zero */

	storedNum--;
	if(!seekTimeout(0))
		return false;
	timeout = 0;

	do
	{
		if(timeout > 0)
			if(!seekTimeout(actualPointer))
				return false;
		f_write(&CONFIG_S, (uint8_t*) &storedNum, 2, &backp);
		timeout++;
	}
	while(backp != 2 && timeout != UINT8_MAX);
	if(backp != 2)
		return false;

	/* Check if the file can be reduced */

	timeout = 0;
	if(storedNum == 0)
	{
		do
		{
			f_res = f_truncate(&CONFIG_S);
			timeout++;
		}
		while(f_res != FR_OK && timeout != UINT8_MAX);
	}

	/* Close and unmount File */
	f_close(&CONFIG_S);

	f_mount(NULL, "0:", 1);

	/* Everything is OK */
	return true;
}

/*
 * @brief This function return how many message are stored in the SD card
 * @return The function returns an integer that tells how many message are stored in the SD (it can be zero) or zero if an error appeared.
 */
uint16_t checkStorage(void) { return storedNum;}

/*
 * @brief This function return how many message are stored in the SD card
 * @return The function returns an integer that tells how many message are stored in the SD (it can be zero) or zero if an error appeared.
 */
uint16_t checkStored(void)
{
	FRESULT f_res;
	UINT backp;
	uint16_t storedMessage;
	uint8_t timeout = 0;

	/* Mount SD - Mounting Error*/
	do
	{
		if(timeout > 1)
			f_mount(NULL, "0:", 1);
		f_res = f_mount(&FS, "0:", 1);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	timeout = 0;

	do
	{
		if(timeout > 0)
			f_close(&CONFIG_S);
		f_res = f_open(&CONFIG_S, "storage.mnts", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);

	timeout = 0;

	/* SD physical or any other error e.g. file doesn't exist (if there wasn't any stored message) */
	if(f_res != FR_OK)
		return 0;

	/* Read Number of stored messages */
	timeout = 0;
	do
	{
		if(timeout > 0)
			if(!seekTimeout(0))
				return false;
		f_read(&CONFIG_S, (uint8_t*)&storedMessage, 2, &backp);
		timeout++;
	}
	while(backp != 2 && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;

	/* Close and unmount File */
	f_close(&CONFIG_S);

	f_mount(NULL, "0:", 1);

	/* Everything is OK */
	return storedMessage;
}

/* f_lseek() function with timeout */
bool seekTimeout(DWORD pState)
{
	uint8_t timeout = 0;
	FRESULT	f_res;
	do
	{
		f_res = f_lseek(&CONFIG_S, pState);
		timeout++;
	}
	while(f_res != FR_OK && timeout != UINT8_MAX);
	if(timeout == UINT8_MAX)
		return false;
	else
		return true;
}

/*
 * @brief Synchronize the MCU storage value with the stored one
 */
void syncStorage(void)
{
	storedNum = checkStored();
}
