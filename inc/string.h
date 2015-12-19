/*  string.h  */


// =======================================================================================================
// typy zmiennych
typedef enum tCompResult{DIFFERENT, EQUAL} tCompResult;
typedef enum tResult{OK, STR_ERROR} tResult;

#include <stdint.h>

// =======================================================================================================
// funkcje
void CopyString(char pcSource[], char pcDestination[]);
tCompResult eCompareString(char pcStr1[], char pcStr2[]);
void AppendString (char pcSourceStr[], char pcDestinationStr[]);
void ReplaceCharactersInString(char pcString[], char cOldChar, char cNewChar);
void IntToHexStr (int16_t uiValue, char pcStr[]);
void UIntToHexStr (unsigned int uiValue, char pcStr[]);
void UCharToHexStr (unsigned char uValue, char pcStr[]);
tResult eHexStringToUInt(char pcStr[], unsigned int *puiValue);
void AppendIntToString(unsigned int uiValue, char pcDestinationStr[]);
void AppendUIntToString(unsigned int uiValue, char pcDestinationStr[]);















