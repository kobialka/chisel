// _________________________________________________
//|													|
//||
//|_________________________________________________|
// _________________________________________________
/*
 * lib_S1D15705.c
 * 
 * biblioteka napisana na CMSIS4.2
 * 
 */ 
// _________________________________________________

// _________________________________________________
//|													|
//| 			PLIKI BILBIOTECZNE					|
//|_________________________________________________|
// _________________________________________________
	#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
	#include "lcd/lib_S1D15705_m.h"
	#include "lcd/lib_FONT_Table_m.h"
	

// _________________________________________________



// _________________________________________________
//|													|
//| 		PODLACZENIE LCD DO UKLADU				|
//|_________________________________________________|
// _________________________________________________
	#define 	LCD_CONTROL_PORT					(GPIOB)
	#define		LCD_DATA_PORT						(GPIOD)
	#define		__LCD_CONTROL_PORT_CLK_ENABLE()		(__GPIOB_CLK_ENABLE())
	#define		__LCD_DATA_PORT_CLK_ENABLE()		(__GPIOD_CLK_ENABLE())
	#define 	LCD_WR_pin							(GPIO_PIN_5)
	#define		LCD_A0_pin							(GPIO_PIN_4)
	#define		LCD_RESET_pin 						(GPIO_PIN_7)
	#define		LCD_CS_pin 							(GPIO_PIN_6)
	#define   	LCD_DATA_PINS_OFFSET				(0)						// Przesuniecie pinow od pinu 0.

	

// _________________________________________________



// _________________________________________________
//|													|
//|					STRUKTURY						|
//|_________________________________________________|
// _________________________________________________
typedef struct sLcdBuffor_Typedef{
		uint8_t pFrame_u8 [8][162];
		uint8_t pCursorSign_u8 [5];
	} sLcdBuffor_Typedef;

	sLcdBuffor_Typedef sLcdBuffor;
	GPIO_InitTypeDef GPIO_InitStruct;
// _________________________________________________
	
	
	
// _________________________________________________
//|													|
//| 	DEKLARACJE WSZYSTKICH FUNKCJI W MODULE		|
//|_________________________________________________|
// _________________________________________________	
/*	Uzytkownik ma do dyspozycji jedynie funkcje operujace na buforze lcd.	*/
	void		LCD_BUFF_Clean(void);													// CZYSZCZENIE BUFORA
	void 		LCD_BUFF_Wrd(uint8_t Column, uint8_t Page, uint8_t Data);				// Bajt do BUFORA. Inkrementuje numer ColNr_u8.
	void		LCD_BUFF_Wrs(uint8_t Column, uint8_t Page, const uint8_t* String);		// Wypisz string.
	void		LCD_BUFF_Wrv_U16Dec(uint8_t Column, uint8_t Page, uint16_t Data);		// Wypisz zmienna 16 bitowa w postaci dziesietnej.
	void		LCD_BUFF_Wrv_U32Dec(uint8_t Column, uint8_t Page, uint32_t Data);		// Wypisz zmienna 32 bitowa w postaci dziesietnej.
	void		LCD_BUFF_Wrv_U8Dec(uint8_t Column, uint8_t Page, uint8_t);				// Wypisz zmienna 8 bitowa w postaci dziesietnej.
	void		LCD_BUFF_Wrv_S8Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int8_t);
	void		LCD_BUFF_Wrv_S16Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int16_t);
	void		LCD_BUFF_Wrv_S32Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int32_t);
	uint8_t		LCD_BUFF_Rdd(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8);
	void 		LCD_BUFF_PixelOn(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);			// Zapala piksel. Liczone od 0,0.
	void 		LCD_BUFF_PixelOff(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);			// Gasi piksel. Liczone od 0,0.
	void 		LCD_BUFF_PixelToggle(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);		// Przelacza piksel. Liczone od 0,0.
	uint8_t		LCD_BUFF_PlotLine(uint8_t HStart_u8, uint8_t VStart_u8, uint8_t HEnd_u8, uint8_t VEnd_u8);
	
	void 		LCD_Clean(void);									// Czysci LCD.
	void 		LCD_Init(void);										// Inicjalizacja LCD. Konfiguruje GPIO, wlacza LCD.
	void 		LCD_PortSetup(void);								// Konfiguracja GPIO dla LCD.
	uint8_t 	LCD_SetColumn(uint8_t);								// Ustawia zadana kolumne w LCD. 
	uint8_t		LCD_SetCursor( uint8_t, uint8_t);					// Ustawia kolumne i strone w LCD.
	uint8_t		LCD_SetPage(uint8_t);								// Ustawia zadana strone w LCD. 
	void		LCD_Update(void);									// Przepisuje BUFOR do LCD
//	void		LCD_UpdateCursor(void);
	void 		LCD_Wrc(uint8_t);									// Rozkaz do LCD. Po Wykonaniu rozkazu jego wartosc zostaje na szynie.
	void 		LCD_Wrd(uint8_t);									// Bajt do LCD. NIE inkrementuje numer ColNr_u8.
// _________________________________________________



// _________________________________________________
//|													|
//|		DEFINICJE FUNKCJI DZIALAJACYCH NA LCD		|
//|_________________________________________________|
//__________________________________________________
void LCD_PortSetup(void){							// KONFIGURACJA PIN�W
	__LCD_CONTROL_PORT_CLK_ENABLE();
	__LCD_DATA_PORT_CLK_ENABLE();
	
	// CONTROL PORT		
	GPIO_InitStruct.Pin		= LCD_A0_pin | LCD_CS_pin | LCD_WR_pin | LCD_RESET_pin;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_PULLDOWN;
	GPIO_InitStruct.Speed	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(LCD_CONTROL_PORT,&GPIO_InitStruct);
	
	// DATA PORT
	GPIO_InitStruct.Pin		= ((uint16_t)0xff<<LCD_DATA_PINS_OFFSET);				// Magistrala 8bit: Piny 0-7
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_PULLDOWN;
	GPIO_InitStruct.Speed	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(LCD_DATA_PORT,&GPIO_InitStruct);
	
	// RESET=1
	HAL_GPIO_WritePin(LCD_CONTROL_PORT, LCD_RESET_pin, GPIO_PIN_SET);
	// A0=0: przesylane rozkazy, A0=1: przesylane dane.
	HAL_GPIO_WritePin(LCD_CONTROL_PORT, LCD_A0_pin, GPIO_PIN_RESET);
	// RD=1	- na stale zwarte przewodem do Vcc. Blokuje piny LCD w stanie wejsc.
	// WR=0 
	HAL_GPIO_WritePin(LCD_CONTROL_PORT, LCD_WR_pin, GPIO_PIN_RESET);
}
// _________________________________________________

// _________________________________________________
void LCD_Wrc(uint8_t Rozkaz_u8){					// WYSLANIE ROZKAZU DO LCD. Po Wykonaniu rozkazu jego wartosc zostaje na szynie.
	HAL_GPIO_WritePin(LCD_DATA_PORT,((uint16_t)0x00FF<<LCD_DATA_PINS_OFFSET), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DATA_PORT,((uint16_t)Rozkaz_u8<<LCD_DATA_PINS_OFFSET), GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_A0_pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_CS_pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_WR_pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_WR_pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_CS_pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_A0_pin,GPIO_PIN_SET);
}
// _________________________________________________


// _________________________________________________
uint8_t LCD_SetColumn(uint8_t arg_ColumnNr_u8){		// USTAWIENIE KOLUMNY W LCD
	if (0 != (arg_ColumnNr_u8 < 163) ){
		LCD_Wrc(0x10 | ((arg_ColumnNr_u8 +3) >> 4));
		LCD_Wrc(0x0f & (arg_ColumnNr_u8 +3));		
	}
	else{
		return 2; // kod bledu: zly numer kolumny.
	}
	return 0;
}
// _________________________________________________


// _________________________________________________
uint8_t LCD_SetPage(uint8_t arg_PageNr_u8){			// USTAWIENIE STRONY W LCD
	if (arg_PageNr_u8 < 8){
		LCD_Wrc(0xb0 | arg_PageNr_u8); // nr strony jest ok i wlasnie go ustawiono.
	}
	else{
		return 1; // kod bledu: zly numer strony.
	}
	return 0;
}
// _________________________________________________


// _________________________________________________
uint8_t LCD_SetCursor(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8){		// USTAWIENIE POZYCJI KURSORA W LCD
	if (arg_PageNr_u8 < 8){
		if (arg_ColNr_u8 < 162){
			LCD_Wrc(0xb0 | arg_PageNr_u8);  			// nr strony jest ok i wlasnie go ustawiono.
			LCD_Wrc(0x10 | ((arg_ColNr_u8 +3) >> 4));
			LCD_Wrc(0x0f & (arg_ColNr_u8 +3));		// nr kolumny jest ok i wlasnie goo ustawiono.
		}
		else{
			return 2; // kod bledu: zly numer kolumny i dobry numer strony.
		}
	}
	else{
		return 1; // kod bledu: zly numer strony i niewiadomo czy dobry numer kolumny.
	}
	return 0;
}
// _________________________________________________


// _________________________________________________
void LCD_Wrd(uint8_t Dana_u8){						// ZAPIS DANEJ DO LCD. Inkrementuje numer kolumny.
	HAL_GPIO_WritePin(LCD_DATA_PORT,((uint16_t)0x00FF<<LCD_DATA_PINS_OFFSET), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DATA_PORT,((uint16_t)Dana_u8<<LCD_DATA_PINS_OFFSET), GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_CS_pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_WR_pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_WR_pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CONTROL_PORT,LCD_CS_pin,GPIO_PIN_SET);
}
// _________________________________________________


// _________________________________________________
void LCD_Init(void){								// INICJALIZACJA LCD
	LCD_PortSetup();
	LCD_Wrc(0xab);		// oscylator ON
	LCD_Wrc(0xa3);		// LCD bias - jasnosc ?!
	LCD_Wrc(0xa6);		// inwersja obrazu a6-OFF/a7-ON
	LCD_Wrc(0xc8);		// lustrzane odbicie
	LCD_Wrc(0+64);		// linia startowana 0
	LCD_Wrc(0x2b);		// power control ?!?!
	LCD_Wrc(0x26);		// Voltage adjusting ?!?!
	LCD_Wrc(0x81);		// electric control - byte 1
	LCD_Wrc(0x10);		// electric control - byte 2
	LCD_Wrc(0xaf);		// display on(0xaf)/off(0xae)
	LCD_Clean();
	LCD_BUFF_Clean();
}
// _________________________________________________


// _________________________________________________
void LCD_Clean(void){								// WYCZYSZCZENIE LCD
	uint8_t PageCounter_u8;
	uint8_t ColumnCounter_u8;
	
	for (PageCounter_u8=0; PageCounter_u8 < 9; PageCounter_u8++){			// Petla stron.
		LCD_Wrc(0xb0 | PageCounter_u8 );  			
		LCD_Wrc(0x10 | ((0 + 3) >> 4));
		LCD_Wrc(0x0f & (0 + 3));		
		for (ColumnCounter_u8=0; ColumnCounter_u8<162; ColumnCounter_u8++){	// Petla kolumn.
			LCD_Wrd(0);
		}
	}
	LCD_SetCursor(0,0);														// Powrot na pozycje zero.
}
// _________________________________________________




// _________________________________________________
//|													|
//|		DEFINICJE FUNKCJI DZIALAJACYCH NA BUFORZE	|
//|_________________________________________________|
// _________________________________________________
void LCD_BUFF_Wrd(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, uint8_t arg_DanaBuff_u8){			// ZAPIS BAJTU ZNAKU DO BUFORA
	sLcdBuffor.pFrame_u8[arg_PageNr_u8][arg_ColNr_u8] = arg_DanaBuff_u8;	
}
// _________________________________________________



// _________________________________________________
uint8_t LCD_BUFF_Rdd(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8){
	return sLcdBuffor.pFrame_u8[arg_PageNr_u8][arg_ColNr_u8];
}
// _________________________________________________



// _________________________________________________
void LCD_BUFF_Clean(void){							// CZYSZCZENIE BUFORA
	uint8_t PageCounter_u8;
	uint8_t	ColumnCounter_u8;
	
	for (PageCounter_u8=0; PageCounter_u8 < 9; PageCounter_u8++){				// petla stron.
		for (ColumnCounter_u8=0; ColumnCounter_u8 < 162; ColumnCounter_u8++){	// petla kolumn.
			LCD_BUFF_Wrd(ColumnCounter_u8, PageCounter_u8,0);
		}
	}
}
// _________________________________________________



// _________________________________________________
uint8_t LCD_BUFF_CleanPage(uint8_t arg_PageToClear_u8){
	uint8_t	ColumnCounter_u8;
	
	if(arg_PageToClear_u8 <8){
		for (ColumnCounter_u8=0; ColumnCounter_u8 < 162; ColumnCounter_u8++){	// petla kolumn.
			LCD_BUFF_Wrd(ColumnCounter_u8, arg_PageToClear_u8, 0);
		}
		return 0;
	}
	else{
		return 1;
	}
}
// _________________________________________________



// _________________________________________________
void LCD_Update(void){								// WYSWIETLENIE BUFORA NA LCD
	uint8_t PageCounter_u8;
	uint8_t	ColumnCounter_u8;
	
	for(PageCounter_u8 = 0; PageCounter_u8 < 8; PageCounter_u8++){				// Petla stron.
		LCD_Wrc(0xb0 | PageCounter_u8 );  			
		LCD_Wrc(0x10 | ((0 + 3) >> 4));
		LCD_Wrc(0x0f & (0 + 3));												// Powrot do poczatku aktualnej stronu w LCD.
		for(ColumnCounter_u8 = 0; ColumnCounter_u8 < 162; ColumnCounter_u8++){	// Petla kolumn.
			LCD_Wrd(sLcdBuffor.pFrame_u8[PageCounter_u8][ColumnCounter_u8]);	// Zapis danej do LCD.
		}
	}
}
// _________________________________________________



// _________________________________________________
void LCD_BUFF_Wrv_U16Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, uint16_t Zmienna_u16){
	uint8_t pString_u8[]={"00000"};
	
	pString_u8[0] += (Zmienna_u16/10000);
	pString_u8[1] += (Zmienna_u16%10000)	/1000;
	pString_u8[2] += (Zmienna_u16%1000)		/100;
	pString_u8[3] += (Zmienna_u16%100)		/10;
	pString_u8[4] += Zmienna_u16%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________



// _________________________________________________
void LCD_BUFF_Wrv_U32Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, uint32_t Zmienna_u32){
	uint8_t pString_u8[]={"0000000000"};
	
	pString_u8[0] += (Zmienna_u32/1000000000);
	pString_u8[1] += (Zmienna_u32%1000000000)	/100000000;
	pString_u8[2] += (Zmienna_u32%100000000)	/10000000;
	pString_u8[3] += (Zmienna_u32%10000000)		/1000000;
	pString_u8[4] += (Zmienna_u32%1000000)		/100000;
	pString_u8[5] += (Zmienna_u32%100000)		/10000;
	pString_u8[6] += (Zmienna_u32%10000)		/1000;
	pString_u8[7] += (Zmienna_u32%1000)			/100;
	pString_u8[8] += (Zmienna_u32%100)			/10;
	pString_u8[9] += Zmienna_u32%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________



// _________________________________________________
// WYPISANIE ZMIENNEJ DO BUFORA
void LCD_BUFF_Wrv_U8Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, uint8_t Zmienna_u8){
	uint8_t pString_u8[]={"000"};
	
	pString_u8[0] += (Zmienna_u8/100);
	pString_u8[1] += (Zmienna_u8%100)	/10;
	pString_u8[2] += Zmienna_u8%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________


// _________________________________________________
void LCD_BUFF_Wrv_S8Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, int8_t arg_Dana){
	uint8_t pString_u8[]={" 000"};
	if(0 > arg_Dana){
		pString_u8[0] = '-';
		arg_Dana = (~arg_Dana) + 1;
	}
	pString_u8[1] += (arg_Dana/100);
	pString_u8[2] += (arg_Dana%100)	/10;
	pString_u8[3] += arg_Dana%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________


// _________________________________________________
void		LCD_BUFF_Wrv_S16Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, int16_t arg_Dana_s16){
	uint8_t pString_u8[]={" 00000"};
	if(0 > arg_Dana_s16){
		pString_u8[0] = '-';
		arg_Dana_s16 = (~arg_Dana_s16) + 1;
	}

	pString_u8[0] += (arg_Dana_s16/10000);
	pString_u8[1] += (arg_Dana_s16%10000)	/1000;
	pString_u8[2] += (arg_Dana_s16%1000)		/100;
	pString_u8[3] += (arg_Dana_s16%100)		/10;
	pString_u8[4] += arg_Dana_s16%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________


// _________________________________________________
void		LCD_BUFF_Wrv_S32Dec(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, int32_t arg_Dana_s32){
	uint8_t pString_u8[]={" 0000000000"};
	if(0 > arg_Dana_s32){
		pString_u8[0] = '-';
		arg_Dana_s32 = (~arg_Dana_s32) + 1;
	}

	pString_u8[1] += (arg_Dana_s32/1000000000);
	pString_u8[2] += (arg_Dana_s32%1000000000)	/100000000;
	pString_u8[3] += (arg_Dana_s32%100000000)	/10000000;
	pString_u8[4] += (arg_Dana_s32%10000000)	/1000000;
	pString_u8[5] += (arg_Dana_s32%1000000)		/100000;
	pString_u8[6] += (arg_Dana_s32%100000)		/10000;
	pString_u8[7] += (arg_Dana_s32%10000)		/1000;
	pString_u8[8] += (arg_Dana_s32%1000)		/100;
	pString_u8[9] += (arg_Dana_s32%100)			/10;
	pString_u8[10] += arg_Dana_s32%10;
	LCD_BUFF_Wrs(arg_ColNr_u8, arg_PageNr_u8, pString_u8);
}
// _________________________________________________


// _________________________________________________
// ZAPIS STRINGA DO BUFORA
void LCD_BUFF_Wrs(uint8_t arg_ColNr_u8, uint8_t arg_PageNr_u8, const uint8_t* arg_strString_u8){					// funkcja wypisuje komunikaty. W ustalonej pozycji i o ustalonej dlugosci. Nic nie sprawdza, niczego nie zwraca.
	uint8_t ByteCounter_u8;
	uint8_t* pLetter_u8 = (uint8_t*)arg_strString_u8;						// wsk przechowuje adres pierwszego elementu argmentu str (tablicy).

	while (*pLetter_u8){													// r�b jezeli wskaznik nie wskazuje na zero.
		for (ByteCounter_u8 = 0; ByteCounter_u8 < 5; ByteCounter_u8++ ){	// drukuje znak, czyli 5 bajtow.
			LCD_BUFF_Wrd(arg_ColNr_u8++, arg_PageNr_u8, pucFONT_Table_128x5[*pLetter_u8][ByteCounter_u8]);
		}
		LCD_BUFF_Wrd(arg_ColNr_u8++, arg_PageNr_u8, 0);					// Dodaje odstep pomiedzy literami.
		pLetter_u8++;
	}
}
// _________________________________________________



// _________________________________________________
void LCD_BUFF_PixelOn(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8){		// Zapala piksel. Liczone od 0,0.
	LCD_BUFF_Wrd(arg_ColNr_u8, arg_LineNr_u8/8, (LCD_BUFF_Rdd(arg_ColNr_u8, arg_LineNr_u8/8) | (1<<(arg_LineNr_u8%8))  )  );
}
// _________________________________________________



// _________________________________________________
void LCD_BUFF_PixelOff(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8){						// Gasi piksel. Liczone od 0,0.
	LCD_BUFF_Wrd(arg_ColNr_u8, arg_LineNr_u8/8, (LCD_BUFF_Rdd(arg_ColNr_u8, arg_LineNr_u8/8) & ~(1<<(arg_LineNr_u8%8))  )  );
}


void LCD_BUFF_PixelToggle(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8){						// Przelacza piksel. Liczone od 0,0.
	LCD_BUFF_Wrd(arg_ColNr_u8, arg_LineNr_u8/8, (LCD_BUFF_Rdd(arg_ColNr_u8, arg_LineNr_u8/8) ^ (1<<(arg_LineNr_u8%8))  )  );	
}



uint8_t	LCD_BUFF_PlotLine(uint8_t HStart_u8, uint8_t VStart_u8, uint8_t HEnd_u8, uint8_t VEnd_u8){
	uint8_t HCurrent_u8, VCurrent_u8;						// Aktualna pozycja.
	int8_t 	HStep_i8, VStep_i8;								// Przyrosty.
	int16_t HDist_i16, VDist_i16;							// Wymiary wektora.
	int16_t	HDoubleDist_i16, VDoubleDist_i16;				// Operujemy na wartosciach podwojonych zeby uniknac dzielenia, tym samym liczb ulamkowych.
	int16_t HAccDist_i16, VAccDist_i16;						// Akumulator przyrostu.
	
	if(0 != (HStart_u8>161) ){								// Czy wprowadzono prawidlowe dane
		return 1;											
	}
	else if(0 != (HEnd_u8>161) ){
		return 2;
	}
	else if(0 != (VStart_u8>63) ){
		return 3;
	}
	else if(0 != (VEnd_u8>63) ){
		return 4;
	}
	else{
		HDist_i16 = (HEnd_u8-HStart_u8); 					// Skladowa pozioma
		VDist_i16 = (VEnd_u8-VStart_u8); 					// Skladowa pionowa
		HCurrent_u8 = HStart_u8; 						
		VCurrent_u8 = VStart_u8; 						
		HStep_i8 = 1; 									
		VStep_i8 = 1; 									

		if(HDist_i16 < 0){								// jesli skladowa pozioma jest ujemna, to bedziemy sie "cofac" (krok ujemny).
			HStep_i8 = -1; 								
			HDist_i16 = -HDist_i16;  					// zmieniamy znak skladowej na dodatni
		}
		if(VDist_i16 < 0){								// jesli skladowa pionowa jest ujemna, to bedziemy sie "cofac" (krok ujemny).
			VStep_i8 = -1; 								
			VDist_i16 = -VDist_i16;		 				// zmieniamy znak skladowej na dodatki
		}		
		VDoubleDist_i16 = 2*VDist_i16; 					// podwojona skladowa pionowa
		HDoubleDist_i16 = 2*HDist_i16; 				
		LCD_BUFF_PixelToggle(HStart_u8,VStart_u8);	
		
		if (VDist_i16 <= HDist_i16){						// Kat w zakresach -45:45 albo 135:225 stopni.
			VAccDist_i16 = 0;
			while(HCurrent_u8 != HEnd_u8 ){		// usunac drugi warunek (z zerem) i sprawdzic czas rysowania lini.
				HCurrent_u8 = HCurrent_u8 + HStep_i8;
				VAccDist_i16 = VAccDist_i16 + VDoubleDist_i16; 	
				if(VAccDist_i16 > HDist_i16){
					VCurrent_u8 = VCurrent_u8 + VStep_i8;
					VAccDist_i16 = VAccDist_i16 - HDoubleDist_i16;
				}
				LCD_BUFF_PixelToggle(HCurrent_u8,VCurrent_u8);
			}
		}
		else{
			HAccDist_i16 = 0;
			while(VCurrent_u8 != VEnd_u8){				// usunac drugi warunek (z zerem) i sprawdzic czas rysowania lini.
				VCurrent_u8 = VCurrent_u8 + VStep_i8;
				HAccDist_i16 = HAccDist_i16 + HDoubleDist_i16;
				if(HAccDist_i16 > VDist_i16){
					HCurrent_u8 = HCurrent_u8 + HStep_i8;
					HAccDist_i16 = HAccDist_i16 - VDoubleDist_i16;
				}
				LCD_BUFF_PixelToggle(HCurrent_u8,VCurrent_u8);
			} 
		}	
		return 0;
	}
}
		
uint8_t	LCD_BUFF_PlotLine_NOT_MINE(uint8_t col1, uint8_t lin1, uint8_t col2, uint8_t lin2)
{
	// Nie ja napisalem ta funkcje.
	int CurrentX, CurrentY, Xinc, Yinc, Dx, Dy, TwoDx, TwoDy, TwoDxAccumulatedError, TwoDyAccumulatedError;
	
			
	if(0 != (col1>161) )
	{
		return 1;
	}
	else if(0 != (col2>161) )				// Rysujemy od lewa do prawa, od x1 do x2.
	{
		return 2;
	}
	else if(0 != (lin1>63) )
	{
		return 3;
	}
	else if(0 != (lin2>63) )
	{
		return 4;
	}
	else
	{
		
		Dx = (col2-col1); 				// obliczenie skladowej poziomej
		Dy = (lin2-lin1); 				// obliczenie skladowej pionowej

		TwoDx = Dx + Dx; 			// podwojona skladowa pozioma
		TwoDy = Dy + Dy; 			// podwojona skladowa pionowa

		CurrentX = col1; 				// zaczynamy od X1
		CurrentY = lin1; 				// oraz Y1

		Xinc = 1; 				// ustalamy krok zwiekszania pozycji w poziomie
		Yinc = 1; 				// ustalamy krok zwiekszania pozycji w pionie

		if(Dx < 0) 				// jesli skladowa pozioma jest ujemna
		{
			Xinc = -1; 				// to bedziemy sie "cofac" (krok ujemny)
			Dx = -Dx;  				// zmieniamy znak skladowej na dodatni
			TwoDx = -TwoDx; 			// jak r�wniez podwojonej skladowej
		}

		if (Dy < 0) 				// jesli skladowa pionowa jest ujemna
		{
			Yinc = -1; 				// to bedziemy sie "cofac" (krok ujemny)
			Dy = -Dy; 				// zmieniamy znak skladowej na dodatki
			TwoDy = -TwoDy; 			// jak r�wniez podwojonej skladowej
		}
		
		
		LCD_BUFF_PixelToggle(col1,lin1);
		
		if ((Dx != 0) || (Dy != 0)) 		
		{
			if (Dy <= Dx)
			{
				TwoDxAccumulatedError = 0;
				do
				{
					CurrentX += Xinc;
					TwoDxAccumulatedError += TwoDy; 	
					if(TwoDxAccumulatedError > Dx)
					{
						CurrentY += Yinc;
						TwoDxAccumulatedError -= TwoDx;
					}
					LCD_BUFF_PixelToggle(CurrentX,CurrentY);
				} while (CurrentX != col2);
			}
			else
			{
				TwoDyAccumulatedError = 0;
				do
				{
					CurrentY += Yinc;
					TwoDyAccumulatedError += TwoDx;
					if(TwoDyAccumulatedError>Dy)
					{
						CurrentX += Xinc;
						TwoDyAccumulatedError -= TwoDy;
					}
					LCD_BUFF_PixelToggle(CurrentX,CurrentY);
				} while (CurrentY != lin2);
			}
		}
		return 0;
	}
}

/*
		do zrobienia:
			void	lcd_wr_ascii(uint8_t);				// Pisanie pojedynczego kodu ASCII.
			uint8_t lcd_util_digits8(uint8_t);			// Funkcja zwraca liczbe cyfr zmiennej 8 bitowej, na potrzeby wyswietlenia na LCD.
			LCD_BUFF_CleanPage(uint8_t)
			
*/



uint8_t		LCD_TESTF(){
	LCD_BUFF_PlotLine(1,1,162,162);
	LCD_Update();
	LCD_BUFF_PlotLine_NOT_MINE(1,1,162,162);
	LCD_Update();
	return 0;
}

