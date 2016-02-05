/*
 * uart.h
 *
 *  Created on: 22 cze 2015
 *      Author: mike
 */

#ifndef UART_H_
#define UART_H_


// =======================================================================================================
// Deklaracje zmiennych globalnych

extern UART_HandleTypeDef huart4;

#define TERMINATOR				13
//#define TERMINATOR				10
#define UART_RECIEVER_SIZE		20
#define UART_TRANSMITER_SIZE 	40



// =======================================================================================================
// typy zmiennych
typedef enum teRecieverStatus { EMPTY, READY, OVERFLOW} teRecieverStatus;
enum eTransmiterStatus {FREE, BUSY};

typedef struct tsRecieverBuffer
{
	char				cData[UART_RECIEVER_SIZE];
	unsigned char		ucCharCtr;
	teRecieverStatus	eStatus;
} tsRecieverBuffer;

typedef struct TransmiterBuffer{
	char cData[UART_TRANSMITER_SIZE]; 								// Lancuch, ktory ma byc wyslany
	enum eTransmiterStatus eStatus;									// Status bufora
	unsigned char fLastCharacter;									// Czy pobrano ostatni znak, czyli NULL'a. Flaga.
	unsigned char cCharCtr;											// Do iteracji po lancuchu znakowym.
} tsTransmiterBuffer;


// =======================================================================================================
// funkcje
void UART_InitWithInt(uint32_t u32BaudRate);
void Reciever_PutCharacterToBuffer(char cCharacter);
teRecieverStatus eReciever_GetStatus(void);
void Reciever_GetStringCopy(char * ucDestination);

char Transmiter_GetCharacterFromBuffer(void);
void Transmiter_SendString( char cString[]);
enum eTransmiterStatus eTransmiter_GetStatus(void);





#endif /* UART_H_ */
