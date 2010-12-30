#include <at89x51.h>

/**
 *
 * @file segment_display.c
 */

#define TESTING_FUNCTIONS
#define STANDALONE_TEST

//#define BASIC_METHOD

/** This code has been tested with 4-digit 7-segment multiplexed
 * LED display using GUI emulator.
 *
 * It is inteded to run on MCU with different pin configurations
 * for the connection of LED segments. Using two 8-bit ports
 * up to 8 digits can be driven. There a few precompiler macros
 * to configure the topology of connection. Common cathode or
 * anode options are include, but reverse segment configuration
 * has not been implemented; though, pins for digit selection
 * may be connected in variouse ways.
 */

#define DIGIT (P0) /**< Set the port for digit selection. */
#define SEGMENT (P1) /**< Set the port for segment selection. */

/** LED segment displays can be connected
 * with common cathodes or anodes. */
#define ANODE 1
#define CATHODE 0

/** This code can operate in two pin configuration modes,
 * modifications needed for different combination of these. */

#define FIRST_DIGIT 0 /**< This can be set to 0 - 7. */

#ifndef BASIC_METHOD
#warning "Computantionaly intensive universal method will be used!"
  #ifndef SHIFTDIR
   #if ( (FIRST_DIGIT == 4) || (FIRST_DIGIT == 0) )
   #define SHIFTDIR <<
   #elif ( (FIRST_DIGIT == 7) || (FIRST_DIGIT == 3) )
   #define SHIFTDIT >>
   #else
   #warning "In unversal method values of first digit other then 7, 4, 3 or 0 require specific direction!"
   #endif
  #endif
#endif

/* Configure the macros. */
#define COMMON_PIN ANODE


/** Store a digit code look-up table (array) in ROM,
 * perhaps otherwise an enumeration could be used. */
static const char figure[10] = {

#ifdef REVERSE_SEGMENTS
#error "Reverse connection of segments not implemented!"
#endif

#if COMMON_PIN == ANODE

  0xc0, // = 0
  0xf9, // = 1
  0xa4, // = 2
  0xb0, // = 3
  0x99, // = 4
  0x92, // = 5
  0x82, // = 6
  0xF8, // = 7
  0x80, // = 8
  0x90  // = 9

#else /* COMMON_PIN == CATHODE */

  0x3F, // = 0
  0x06, // = 1
  0x5B, // = 2
  0x47, // = 3
  0x66, // = 4
  0x6d, // = 5
  0x75, // = 6
  0x07, // = 7
  0x7f, // = 8
  0x6f  // = 9

// 0x39 // = C
// 0x71 // = F

#endif /* COMMON_PIN */

};

/* Firstly let's presume that
the digit 0 is connected to pin 7.
There is a clear patern here:
switch(d)
case 0: P0 = 0x80; // 128
case 1: P0 = 0x40; // 64
case 2: P0 = 0x20; // 32
case 3: P0 = 0x16; // 16

* It can be expressed as:
P0 = 128/(2**j)
'**' operator is not valid in C,
and using 'math.h' is not cosidered
appropriate for this small design.

* It is in fact most appropriate
to use '>>' bitshift operator:
P0 = (128 >> j);

* However, it turns out that
the above valuse (128,64,32,16)
are wrong. We should invert the
bits.  */

void display_digit(unsigned char d, unsigned char v)
{

  #ifdef BASIC_METHOD

   #if ( FIRST_DIGIT == 7 )
   DIGIT = ~( 128 >> d );

   #elif ( FIRST_DIGIT == 3 )
   DIGIT = ~( 8 >> d);

   #elif ( FIRST_DIGIT == 4 )
   DIGIT = ~( 16 << d);

   #elif ( FIRST_DIGIT == 0 )
   DIGIT = ~( 1 << d );

   #else
   #warning "In basic method each values of first digit can be 7, 4, 3 or 0 only!"
   #endif

  #else /* GOOD_METHOD_IN_THEORY */
  /* More universal but computation-intensive
   * would be: ~( (1<<FIRST_DIGIT) >> d )
   * and ~( (1<<FIRST_DIGIT) << d ) */

  DIGIT = ~( (1<<FIRST_DIGIT) SHIFTDIR d );

  #endif

  SEGMENT = figure[v];

}

#ifdef TESTING_FUNCTIONS
void display_test_loop(unsigned char x)
{

  int i, j;

  while(x--) {

    for( i=0; i<10; i++ ) {

      for( j=0; j<4; j++ ) {

	display_digit( j, i );
      }

    }

  }

}
#endif

#ifdef STANDALONE_TEST
void main (void)
{

  display_test_loop(2);
  DIGIT = 0xff;

}
#endif
