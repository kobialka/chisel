/*
 * command_decoder.h
 *
 *  Created on: 22 cze 2015
 *      Author: mike
 */


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>



#ifndef COMMAND_DECODER_H_
#define COMMAND_DECODER_H_


#define MAX_KEYWORD_STRING_LTH	25											// maksymalna dlugosc komendy.
#define MAX_KEYWORD_NR 			10											// maksymalna dopuszczalna ilosc slow kluczowych.
#define MAX_TOKEN_NR 			3											// maksymalna dopuszczalna ilosc tokenów.




// =======================================================================================================
// typy zmiennych

// typedef enum		tKeywordCode{CALC, ID, LIS3DSH_GETXYZ, LIS3DSH_START, LIS3DSH_STOP, TEST} tKeywordCode;
typedef enum		tKeywordCode{MPU9250_READ_GYRO, MPU9250_READ_ACC, MPU9250_READ_MAG, MPU9250_READ_9D, MPU9250_WHO_AM_I, MPU9250_READ_START, MPU9250_READ_STOP, MPU9250_CALIB} tKeywordCode;

typedef struct		tKeyword
{
	tKeywordCode	eCode;
	char 			cString[MAX_KEYWORD_STRING_LTH + 1];
}tKeyword;



typedef enum		tTokenType{KEYWORD, NUMBER, STRING}tTokenType;
typedef union 		tTokenValue
{
	tKeywordCode	eKeyword; 						// jezeli KEYWORD
	uint32_t		u32_Number; 					// jezeli NUMBER
	char *			pcString; 						// jezeli STRING
}tTokenValue;

typedef struct	tToken
{
	tTokenType 	eType;									// KEYWORD, NUMBER, STRING
	tTokenValue uValue;									// albo enum albo unsigned int albo char*
}tToken;



// =======================================================================================================
// funkcje
void DecodeMsg(char *pcString);





#endif /* COMMAND_DECODER_H_ */
