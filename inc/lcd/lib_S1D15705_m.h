/*
 *
 * lib_S1D15705_m.h
 * 
 */ 


#ifndef	LIB_S1D15705_H
#define LIB_S1D15705_H

	
	#include <stdint.h>
	#define CHAR_WIDTH				(6)
	#define LCD_HOR_MAX_PIXEL		(162)
	#define LCD_VER_MAX_PIXEL		(64)
	
	
	// enum  {BAD_PAGE,BAD COLUMN};

	void		LCD_Init(void);									// INICJALIZACJA LCD
	void		LCD_Update(void);								// WYSWIETLENIE BUFORA NA LCD

	void		LCD_BUFF_Clean(void);							// CZYSZCZENIE BUFORA
	uint8_t		LCD_BUFF_CleanPage(uint8_t);
	void		LCD_BUFF_Wrs(uint8_t arg_ColNr, uint8_t arg_PageNr, const uint8_t*);	// funkcja wypisuje komunikaty. W ustalonej pozycji i o ustalonej dlugosci. Nic nie sprawdza, niczego nie zwraca.
	void 		LCD_BUFF_Wrv_U16Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, uint16_t);
	void 		LCD_BUFF_Wrv_U32Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, uint32_t);
	void		LCD_BUFF_Wrv_U8Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, uint8_t);
	void		LCD_BUFF_Wrv_S8Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int8_t);
	void		LCD_BUFF_Wrv_S16Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int16_t);
	void		LCD_BUFF_Wrv_S32Dec(uint8_t arg_ColNr, uint8_t arg_PageNr, int32_t);
	void 		LCD_BUFF_PixelOn(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);			// Zapala piksel. Liczone od 0,0.
	void 		LCD_BUFF_PixelOff(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);					// Gasi piksel. Liczone od 0,0.
	void 		LCD_BUFF_PixelToggle(uint8_t arg_ColNr_u8, uint8_t arg_LineNr_u8);				// Przelacza piksel. Liczone od 0,0.
	uint8_t		LCD_BUFF_PlotLine(uint8_t col1, uint8_t lin1, uint8_t col2, uint8_t lin2);
	uint8_t		LCD_BUFF_PlotLine_NOT_MINE(uint8_t col1, uint8_t lin1, uint8_t col2, uint8_t lin2);
	uint8_t		LCD_TESTF(void);

	
#endif /* LIB_S1D15705_H */



