/*  string.c  */

#define NULL 0
#define TetraMask_bm 0x0000000f


#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "string.h"




void CopyString(char pcSource[], char pcDestination[])
{	
	unsigned char ucCharacterCounter;

	for(ucCharacterCounter=0; pcSource[ucCharacterCounter] != NULL; ucCharacterCounter++)
	{
		pcDestination[ucCharacterCounter] = pcSource[ucCharacterCounter];
	}
	pcDestination[ucCharacterCounter] = NULL;
}


//---------------------------------------------------------------------------------------
tCompResult eCompareString(char pcStr1[], char pcStr2[])
{
	unsigned char ucCharacterCounter;
	
	for(ucCharacterCounter=0; (pcStr1[ucCharacterCounter] == pcStr2[ucCharacterCounter]); ucCharacterCounter++)
	{
		if(pcStr1[ucCharacterCounter] == NULL)
		{
			return EQUAL;
		}
	}
	return DIFFERENT;
}


//---------------------------------------------------------------------------------------
void AppendString (char pcSourceStr[], char pcDestinationStr[])
{
	unsigned char ucCharacterCounter;
	
	for(ucCharacterCounter=0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++){};
	
	CopyString(pcSourceStr, pcDestinationStr+ucCharacterCounter);
}


//---------------------------------------------------------------------------------------
void ReplaceCharactersInString(char pcString[], char cOldChar, char cNewChar)
{
	unsigned char ucCharacterCounter;
	
	for(ucCharacterCounter=0; pcString[ucCharacterCounter] != NULL; ucCharacterCounter++)
	{
		if(pcString[ucCharacterCounter] == cOldChar)
		{
			pcString[ucCharacterCounter] = cNewChar;
		}
	}
}



//---------------------------------------------------------------------------------------
void IntToHexStr (int16_t Value_i16, char pcStr[])
{
	uint8_t TetraCounter_u8;
	uint8_t ValueBuff_u8;

	pcStr[1] = 'x';
	pcStr[0] = '0';
	pcStr[6] = NULL;

	for (TetraCounter_u8 = 0; TetraCounter_u8 < 4; TetraCounter_u8++)
	{
		ValueBuff_u8 = ( (Value_i16 >> (TetraCounter_u8*4)) & TetraMask_bm );
		if (ValueBuff_u8 > 9)
		{
			pcStr[5 - TetraCounter_u8] = ValueBuff_u8 + ('A' - 10);
		}
		else
		{
			pcStr[5 - TetraCounter_u8] = ValueBuff_u8 + '0';
		}
	}

}



//---------------------------------------------------------------------------------------
void UIntToHexStr (unsigned int uiValue, char pcStr[])
{
	unsigned char uiTetraCounter;
	unsigned char uiValueBuff;
	
	pcStr[1] = 'x';
	pcStr[0] = '0';
	pcStr[6] = NULL;

	for (uiTetraCounter = 0; uiTetraCounter < 4; uiTetraCounter++)
	{
		uiValueBuff = ( (uiValue >> (uiTetraCounter*4)) & TetraMask_bm );
		if (uiValueBuff > 9)
		{
			pcStr[5 - uiTetraCounter] = uiValueBuff + ('A' - 10);
		}
		else
		{
			pcStr[5 - uiTetraCounter] = uiValueBuff + '0';
		}
	}


}

//---------------------------------------------------------------------------------------
void UCharToHexStr (unsigned char ucValue, char pcStr[])
{
	unsigned char ucTetraCounter;
	unsigned char ucValueBuff;
	
	for (ucTetraCounter = 0; ucTetraCounter < 2; ucTetraCounter++)
	{
		ucValueBuff = ( (ucValue >> (ucTetraCounter*4)) & TetraMask_bm );
		if (ucValueBuff > 9)
		{
			pcStr[3 - ucTetraCounter] = ucValueBuff + ('A' - 10);
		}
		else
		{
			pcStr[3 - ucTetraCounter] = ucValueBuff + '0';
		}
	}
	pcStr[1] = 'x';
	pcStr[0] = '0';
	pcStr[4] = NULL;
}



//---------------------------------------------------------------------------------------
tResult eHexStringToUInt(char pcStr[], unsigned int *puiValue)
{
	unsigned char ucCharacterCounter; 
	unsigned char ucCharacterBuff;
	unsigned int  uiValueBuff = 0;
	
	
	if ((pcStr[0] != '0') || (pcStr[1] != 'x') || (pcStr[2] == NULL))
	{
		return STR_ERROR;
	}
		
	for (ucCharacterCounter = 2; pcStr[ucCharacterCounter] != NULL; ucCharacterCounter++)
	{
		if (ucCharacterCounter > 5)
		{
			return STR_ERROR;
		}
		
		uiValueBuff = uiValueBuff << 4;	
		if( (ucCharacterBuff = pcStr[ucCharacterCounter]) > ('a'-1) )  // 0x60 => '''
		{
			ucCharacterBuff = ucCharacterBuff -'a'+10;
		}
		else if( ucCharacterBuff > '@')
		{
			ucCharacterBuff = ucCharacterBuff -'A'+10;
		}
		else
		{
			ucCharacterBuff = ucCharacterBuff -'0';
		}
		uiValueBuff = uiValueBuff + ucCharacterBuff;
	}		
	*puiValue = uiValueBuff;
	return OK;
}




//---------------------------------------------------------------------------------------
void AppendHexIntToString(unsigned int uiValue, char pcDestinationStr[])
{
	unsigned char ucCharacterCounter;

	for (ucCharacterCounter = 0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++){};
	IntToHexStr((uiValue), pcDestinationStr + ucCharacterCounter);
}


//---------------------------------------------------------------------------------------
void AppendIntToString(unsigned int uiValue, char pcDestinationStr[])
{
	unsigned char ucCharacterCounter;

	for (ucCharacterCounter = 0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++){};

	//IntToStr((uiValue), pcDestinationStr + ucCharacterCounter);
}

//---------------------------------------------------------------------------------------
void AppendUIntToString(unsigned int uiValue, char pcDestinationStr[])
{
	unsigned char ucCharacterCounter;
	
	for (ucCharacterCounter = 0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++){};
	UIntToHexStr((uiValue), pcDestinationStr + ucCharacterCounter);
}
