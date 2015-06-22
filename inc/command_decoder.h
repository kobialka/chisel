/*
 * command_decoder.h
 *
 *  Created on: 22 cze 2015
 *      Author: mike
 */

#ifndef COMMAND_DECODER_H_
#define COMMAND_DECODER_H_


#define MAX_KEYWORD_STRING_LTH 10																// maksymalna dlugosc komendy.
#define MAX_KEYWORD_NR 7																				// maksymalna dopuszczalna ilosc slow kluczowych.
#define MAX_TOKEN_NR 3																				// maksymalna dopuszczalna ilosc tokenów.




// =======================================================================================================
// typy zmiennych

typedef enum		tKeywordCode{CALC, ID} tKeywordCode;
typedef struct		tKeyword
{
	tKeywordCode	eCode;
	char 			cString[MAX_KEYWORD_STRING_LTH + 1];
}tKeyword;



typedef enum		tTokenType{KEYWORD, NUMBER, STRING}tTokenType;
typedef union 		tTokenValue
{
	tKeywordCode	eKeyword; 						// jezeli KEYWORD
	unsigned int	uiNumber; 						// jezeli NUMBER
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
