/**
 * Demonstration code for <b>MCU 8051 IDE</b>
 *
 * Virtual HW and C language
 * Requires MCU AT89C51 or similar ( [Project] -> [Edit project] -> [Select MCU] )
 * @file demo_c_0.c
 */

// Create virtual LED matrix and load configuration file "ledmatrix.vhc"
// [Virtual HW] -> [LED Matrix]

// To compile the code press F11 (This code is precompiled)
// To start simulator press F2
// To simulate the program press F6 (animate) or F7 (step) or F8 (step over) or F9 (run)

// To save some time you can use program hibernation function
// [Simulator] -> [Resume hibernated program] and select "ledmatrix.m5hib"


#include <at89x51.h>

#ifdef ENABLE_MATRIX_DISPLAY

static const char image[7][8] = {
// NUMBERS:
// 0 =
	{ 0xff, 0xe3, 0xdd, 0xdd,
	  0xdd, 0xdd, 0xe3, 0xff },

// 1 =
	{ 0xff, 0xe7, 0xef, 0xef,
	  0xef, 0xef, 0xc7, 0xff },

// 2 =
	{ 0xff, 0xe7, 0xdb, 0xdf,
	  0xe7, 0xfb, 0xc3, 0xff },

// 3 =
	{ 0xff, 0xe7, 0xdb, 0xdf,
	  0xe7, 0xdf, 0xdb, 0xe7 },

// 11 =
	{ 0xff, 0x99, 0xbb, 0xbb,
	  0xbb, 0xbb, 0x11, 0xff },

// 88 =
	{ 0x11, 0x55, 0x55, 0x11,
	  0x55, 0x55, 0x11, 0xff },

// 99 =
	{ 0xff, 0x11, 0x55, 0x11,
	  0x77, 0x77, 0x11, 0xff }

// Number 51 was used in the original example code:
// 51 = { 0xb1, 0x9d, 0xbd, 0xb1, 0xb7, 0xb7, 0x11, 0xff }

// LETTERS:
// O =	{ 0xc3, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xc3 }
};

void matrix_display(char x);






/*  Basic usage example:
int main(void)
{

  while(1) matrix_display(5);

} */

void matrix_display(char x) {

	int i;

	//while(t--) {
		for(i=0; i<8; i++) {
			P1 = 0xff;
			P0 = image[x][i];
			P1 = (1 << i) ^ 255;
		}
	//}
}

#endif