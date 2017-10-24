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

#define BACKSPACE				(0x08)
#define TERMINATOR				(0x0D)		// Carriage return
//#define TERMINATOR			(0x0A)		// Line feed
#define UART_RECIEVER_SIZE		25
#define UART_TRANSMITER_SIZE 	84



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
	char 					cData[UART_TRANSMITER_SIZE]; 				// Lancuch, ktory ma byc wyslany
	enum eTransmiterStatus 	eStatus;									// Status bufora
	unsigned char 			fLastCharacter;								// Czy pobrano ostatni znak, czyli NULL'a. Flaga.
	unsigned char 			cCharCtr;									// Do iteracji po lancuchu znakowym.
	uint8_t					u8FrameID;									// ID ramki, gdy wysyłane są surowe dane
	uint8_t					u8FrameDataLen;								// Ilość bajtóœ w ramce.
} tsTransmiterBuffer;


// =======================================================================================================
// funkcje
void 					UART_InitWithInt(uint32_t u32BaudRate);
void 					Reciever_PutCharacterToBuffer(char cCharacter);
teRecieverStatus 		eReciever_GetStatus(void);
void 					Reciever_GetStringCopy(char * ucDestination);
uint8_t					Transmiter_GetFrameID(void);
uint8_t 				Transmiter_GetFrameDataLen(void);
uint8_t 				Transmiter_CheckIfDataLeft(void);
char 					Transmiter_GetCharacterFromBuffer(void);
uint8_t		 			Transmiter_GetRawByteFromBuffer(void);
void 					Transmiter_SendString( char cString[]);
void 					Transmiter_SendFrame(char * pu8Frame);
enum eTransmiterStatus eTransmiter_GetStatus(void);





#endif /* UART_H_ */
