/*
 * storage.h
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include "init.h"

/* Structure of Storage:
 * The (readable) bytes of the storage (file):
 * 1. COUNTER [LSB] - The number of the stored messages
 * 2. COUNTER [MSB]	- The number of the stored messages
 * 3. Head Sentinel - 0xFF 1.byte
 * 4. Head Sentinel - 0xAA 2.byte
 * 5. Indicator Byte - 'E' stands for Environment message and 'M' stands for Movement message
 * 6. First byte of the first stored message
 * . Second byte of the stored message...
 * ...
 * N. Tail Sentinel - 0xAA 1.byte
 * N+1. Tail Sentinel - 0xFF 2.byte
 * N+2. Head Sentinel - 0xFF ...
 * ...
 */

extern FATFS	FS;

bool writeToStorage(char message[], char messageType);
bool readFromStorage(char *message[], char *messageType);
void syncStorage(void);
uint16_t checkStorage(void);

#endif /* STORAGE_H_ */
