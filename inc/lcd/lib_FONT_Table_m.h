/*
 * lin_FONT_table_m.h 
 *
 * Created: 2013-03-18 18:02:31
 *  Author: journal
 *
 * plik zawiera deklaracje zmiennych zapisanych w formacie ascii w pami�ci programu oraz czcionk�.
 */ 

 
 #ifndef		TABLE_ASCII_H
 #define		TABLE_ASCII_H
	#include <stdint.h>
	
	extern const uint8_t  pucFONT_Table_128x5[128][5];		// Tablica zawiera czcionk�. Kod ascii znaku jest jednocze�nie numerem wiersza, w kt�rym znajduje si� 5 bajt�w danego znaku. Dzi�ki dla Paw�a Maciejczek'a
	
#endif
