/*
 * lin_FONT_table_m.h 
 *
 * Created: 2013-03-18 18:02:31
 *  Author: journal
 *
 * plik zawiera deklaracje zmiennych zapisanych w formacie ascii w pamiêci programu oraz czcionkê.
 */ 

 
 #ifndef		TABLE_ASCII_H
 #define		TABLE_ASCII_H
	#include <stdint.h>
	
	extern const uint8_t  pucFONT_Table_128x5[128][5];		// Tablica zawiera czcionkê. Kod ascii znaku jest jednoczeœnie numerem wiersza, w którym znajduje siê 5 bajtów danego znaku. Dziêki dla Paw³a Maciejczek'a
	
#endif
