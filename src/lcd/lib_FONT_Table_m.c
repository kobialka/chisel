/*
 * lib_FONT_Table_m.c 
 * 
 * plik zawiera tablice znakow czyli czcionke.
 */


// ****  pliki naglowkowe  ****
	#include "lcd/lib_FONT_Table_m.h"

	
	
	const uint8_t pucFONT_Table_128x5[128][5]	=
		{
			{0x00,0x00,0x00,0x00,0x00}, // [0] null
			{0x00,0x00,0x00,0x00,0x00}, // [1]
			{0x00,0x00,0x00,0x00,0x00}, // [2]
			{0x00,0x00,0x00,0x00,0x00}, // [3]
			{0x00,0x00,0x00,0x00,0x00}, // [4]
			{0x00,0x00,0x00,0x00,0x00}, // [5]
			{0x00,0x00,0x00,0x00,0x00}, // [6]
			{0x00,0x00,0x00,0x00,0x00}, // [7]
			{0x00,0x00,0x00,0x00,0x00}, // [8]
			{0x00,0x00,0x00,0x00,0x00}, // [9]
			{0x00,0x00,0x00,0x00,0x00}, // [10]
			{0x00,0x00,0x00,0x00,0x00}, // [11]
			{0x00,0x00,0x00,0x00,0x00}, // [12]
			{0x00,0x00,0x00,0x00,0x00}, // [13]
			{0x00,0x00,0x00,0x00,0x00}, // [14]
			{0x00,0x00,0x00,0x00,0x00}, // [15]
			{0x00,0x00,0x00,0x00,0x00}, // [16]
			{0x00,0x00,0x00,0x00,0x00}, // [17]
			{0x00,0x00,0x00,0x00,0x00}, // [18]
			{0x00,0x00,0x00,0x00,0x00}, // [19]
			{0x00,0x00,0x00,0x00,0x00}, // [20]
			{0x00,0x00,0x00,0x00,0x00}, // [21]
			{0x00,0x00,0x00,0x00,0x00}, // [22]
			{0x00,0x00,0x00,0x00,0x00}, // [23]
			{0x00,0x00,0x00,0x00,0x00}, // [24] Cancel
			{0x00,0x00,0x00,0x00,0x00}, // [25]
			{0x00,0x00,0x00,0x00,0x00}, // [26]
			{0x00,0x00,0x00,0x00,0x00}, // [27] Escape
			{0x00,0x00,0x00,0x00,0x00}, // [28]
			{0x00,0x00,0x00,0x00,0x00}, // [29]
			{0x00,0x00,0x00,0x00,0x00}, // [30]
			{0x00,0x00,0x00,0x00,0x00}, // [31]
			{0x00,0x00,0x00,0x00,0x00}, // [32] Space
			{0x00,0x06,0x5f,0x06,0x00}, // [33] !		
			{0x07,0x03,0x00,0x07,0x03}, // [34] "		
			{0x24,0x7e,0x24,0x7e,0x24}, // [35] #		
			{0x00,0x24,0x2b,0x6a,0x12}, // [36] $
			{0x63,0x13,0x08,0x64,0x63}, // [37] %
			{0x36,0x49,0x56,0x20,0x50}, // [38] &
			{0x00,0x00,0x07,0x03,0x00}, // [39] '
			{0x00,0x00,0x3e,0x41,0x00}, // [40] (		
			{0x00,0x00,0x41,0x3e,0x00}, // [41] )		
			{0x08,0x3e,0x1c,0x3e,0x08}, // [42] *		
			{0x08,0x08,0x3e,0x08,0x08}, // [43] +		
			{0x00,0x00,0xe0,0x60,0x00}, // [44] ,		
			{0x08,0x08,0x08,0x08,0x08}, // [45] -		
			{0x00,0x00,0x60,0x60,0x00}, // [46] .		
			{0x20,0x10,0x08,0x04,0x02}, // [47] /		
			{0x3e,0x61,0x5d,0x43,0x3e}, // [48] 0		
			{0x00,0x04,0x02,0x7f,0x00}, // [49] 1		
			{0x62,0x51,0x49,0x49,0x46}, // [50] 2		
			{0x22,0x41,0x49,0x49,0x36}, // [51] 3		
			{0x18,0x14,0x12,0x7f,0x10}, // [52] 4		
			{0x27,0x45,0x45,0x45,0x39}, // [53] 5		
			{0x3e,0x49,0x49,0x49,0x32}, // [54] 6		
			{0x03,0x61,0x11,0x09,0x07}, // [55] 7		
			{0x36,0x49,0x49,0x49,0x36}, // [56] 8		
			{0x26,0x49,0x49,0x49,0x3e}, // [57] 9		
			{0x00,0x00,0x6c,0x6c,0x00}, // [58] :		
			{0x00,0x00,0x6c,0xec,0x00}, // [59] ;		
			{0x00,0x08,0x14,0x22,0x41}, // [60] <		
			{0x24,0x24,0x24,0x24,0x24}, // [61] =		
			{0x00,0x41,0x22,0x14,0x08}, // [62] >
			{0x02,0x01,0xb1,0x09,0x06}, // [63] ?
			{0x3e,0x41,0x5d,0x55,0x1e}, // [64] @
			{0x7c,0x12,0x11,0x12,0x7c}, // [65] A		
			{0x7F,0x49,0x49,0x49,0x36}, // [66] B		
			{0x3E,0x41,0x41,0x41,0x22}, // [67] C		
			{0x7F,0X41,0x41,0x41,0x3E}, // [68] D		
			{0x7F,0x49,0x49,0x41,0x63}, // [69] E		
			{0x7F,0x09,0x09,0x01,0x03}, // [70] F		
			{0x3E,0x41,0x49,0x49,0x3A}, // [71] G		
			{0x7F,0x08,0x08,0x08,0x7F}, // [72] H		
			{0x00,0x41,0x7F,0x41,0x00}, // [73] I		
			{0x23,0x41,0x41,0x41,0x3F}, // [74] J		
			{0x7F,0x08,0x14,0x22,0x41}, // [75] K		
			{0x7F,0x40,0x40,0x40,0x40}, // [76] L		
			{0x7F,0x02,0x0c,0x02,0x7f}, // [77] M		
			{0x7f,0x02,0x04,0x08,0x7f}, // [78] N		
			{0x3e,0x41,0x41,0x41,0x3e}, // [79] O		
			{0x7f,0x09,0x09,0x09,0x06}, // [80] P		
			{0x3e,0x41,0x61,0x41,0xbe}, // [81] Q		
			{0x7f,0x09,0x09,0x09,0x76}, // [82] R		
			{0x26,0x49,0x49,0x49,0x32}, // [83] S		
			{0x03,0x01,0x7f,0x01,0x03}, // [84] T		
			{0x3f,0x40,0x40,0x40,0x3f}, // [85] U		
			{0x07,0x38,0x40,0x38,0x07}, // [86] V		
			{0x7f,0x20,0x18,0x20,0x7f}, // [87] W		
			{0x63,0x14,0x08,0x14,0x63}, // [88] X		
			{0x03,0x04,0x78,0x04,0x03}, // [89] Y		
			{0x73,0x49,0x45,0x45,0x63}, // [90] Z		
			{0x00,0x7f,0x41,0x41,0x00}, // [91] [
			{0x02,0x04,0x08,0x10,0x20}, // [92] "\" tak sie zaczynaja znaki specjalne. nie moze byc znakiem w stringu.
			{0x00,0x41,0x41,0x7f,0x00}, // [93] ]
			{0x04,0x02,0x01,0x02,0x04}, // [94] ^
			{0x40,0x40,0x40,0x40,0x40}, // [95] _
			{0x00,0x00,0x02,0x03,0x00}, // [96] '
			{0x20,0x54,0x54,0x54,0x78}, // [97] a		
			{0x7f,0x48,0x44,0x44,0x38}, // [98] b		
			{0x00,0x38,0x44,0x44,0x44}, // [99] c		
			{0x38,0x44,0x44,0x48,0x7f}, // [100] d		
			{0x38,0x54,0x54,0x54,0x08}, // [101] e
			{0x00,0x08,0x7e,0x09,0x09}, // [102] f
			{0x18,0xa4,0xa4,0xa4,0x7c}, // [103] g
			{0x00,0x7f,0x04,0x04,0x78}, // [104] h
			{0x00,0x00,0x7d,0x40,0x00}, // [105] i
			{0x00,0x40,0x80,0x84,0x7d}, // [106] j
			{0x00,0x7f,0x10,0x28,0x44}, // [107] k
			{0x00,0x00,0x7f,0x40,0x00}, // [108] l
			{0x7c,0x04,0x18,0x04,0x78}, // [109] m
			{0x00,0x7c,0x04,0x04,0x78}, // [110] n
			{0x38,0x44,0x44,0x44,0x38}, // [111] o
			{0xfc,0x44,0x44,0x44,0x38}, // [112] p
			{0x38,0x44,0x44,0x44,0xfc}, // [113] q
			{0x44,0x78,0x44,0x44,0x08}, // [114] r
			{0x08,0x54,0x54,0x54,0x20}, // [115] s
			{0x00,0x04,0x3e,0x44,0x24}, // [116] t
			{0x00,0x3c,0x40,0x20,0x7c}, // [117] u
			{0x1c,0x20,0x40,0x20,0x1c}, // [118] v
			{0x3c,0x60,0x30,0x60,0x3c}, // [119] w
			{0x00,0x6c,0x10,0x10,0x6c}, // [120] x
			{0x00,0x9c,0xa0,0x60,0x3c}, // [121] y
			{0x00,0x64,0x54,0x54,0x4c}, // [122] z
			{0x00,0x08,0x3e,0x41,0x41}, // [123] {
			{0x00,0x00,0x7f,0x00,0x00}, // [124] |
			{0x00,0x41,0x41,0x3e,0x08}, // [125] }
			{0x00,0x00,0x00,0x00,0x00}, // [126] ~
			{0x00,0x08,0x04,0x08,0x04}, // [127]
		};
		
		

		
