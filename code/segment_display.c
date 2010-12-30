#if !DOXYGEN /* Real settings. Do not edit other defines. */

/* Configure some options to compile.
 * Including optimisation and debug.
*/
#define TESTING_FUNCTIONS	1
#define STANDALONE_TEST		1
#define BASIC_METHOD		0


/* Configure which ports LED display is connected to.
 * Use (P0), (P1), (P2) or (P3).
*/
#define DIGIT		(P0)
#define SEGMENT 	(P1)

/* Set the pin number to which the first
 * digit (least significant) is connected.
*/
#define FIRST_DIGIT		(0)

/* Change the common pin setting.
 * Use (ANODE) or (CATHODE).
*/
#define COMMON_PIN		ANODE

#endif /* End of real settings. Below is documentation. */

/**
 * @brief This file implements LED digital display driver function.
 * @file segment_display.c
 *
 * @details The code has been tested with 4-digit 7-segment
 * multiplexed LED display using GUI emulator.
 *
 * @image html segment_display.png "MCU 8051 IDE Virtual Multiplexed LED Display"
 *
 * The software is intended to run on MCU with different pin
 * configurations for the connection of LED segments. Using
 * two 8-bit ports up to 8 digits can be driven. There are
 * a few pre-processor macros to configure the topology of pin
 * connection. Common cathode or anode options are included,
 * but reverse segment configuration has not been implemented;
 * though, pins for digit selection
 * may be connected in various ways.
 *
 *
*/

#include <at89x51.h> 

#if DOXYGEN /* These definitions are only for documentation. */

/** \group These macros can be used to switch different parts of code.
 * \{
*/
#define TESTING_FUNCTIONS	[( 0 | 1 )] /**< Include \ref display_test_loop() function. */
#define STANDALONE_TEST		[( 0 | 1 )] /**< Include \ref main() loop for testing. */

/** There two ways of dealing with the way first digit is connected.
 * One is faster and can be chosen by setting \ref BASIC_METHOD to 1.
 * The other method involves an extra bit-shift operation and is more
 * universal. With the second method \ref SHIFTDIR needs to be defined
 * only if pins are connected in unusual order.
 */
#define BASIC_METHOD		[( 0 | 1 )] /**< Apply faster code for digit selection. */

/** \} */

/** These constants define which ports are in use for LED display.
 * \{
*/
#define DIGIT [( P0 | P1 | P2 | P3 )] /**< Set the port for digit selection. */
#define SEGMENT [( P0 | P1 | P2 | P3 )] /**< Set the port for segment selection. */
/** \} */

#endif

/** LED segment displays can be connected
 * with common cathodes or anodes.
 * \{
*/
#define COMMON_PIN [( ANODE | CATHODE )]

#define ANODE 1 
#define CATHODE 0
/** \} */

#if DOXYGEN
/** This code can operate in four pin configuration modes,
 * modifications needed for unusual combination of these.
 *
 * \{
*/
#define SHIFTDIR [( << | >> )] /**< When universal method is used this is set to '<<' or '>>'. */

#define FIRST_DIGIT [( 0 | 3 | 4 | 7 )] /**< This can be set to 0, 3, 4 or 7. Other values require more changes. */

/** \} */
#endif

#if !BASIC_METHOD
#warning "Computationally intensive universal method will be used!"
  #ifndef SHIFTDIR
   #if ( (FIRST_DIGIT == 4) || (FIRST_DIGIT == 0) )
   #define SHIFTDIR <<
   #elif ( (FIRST_DIGIT == 7) || (FIRST_DIGIT == 3) )
   #define SHIFTDIT >> 
   #else
   #warning "In universal method values of first digit other then 7, 4, 3 or 0 require specific direction!"
   #endif
  #endif
#endif

/** Store a digit code look-up table (array) in ROM,
 * perhaps otherwise an enumeration could be used.
 *
 * There two option for pre-processor to chose from
 * depending of \ref COMMON_PIN.
 *
 *\{
*/
#if DOXYGEN
char figure[10] = {
#else
static const char figure[10] = {
#endif

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

#if 0
#define SYM_MINUS
#define SYM_F
#define SYM_C
#endif

}; /** \} */

/** @name The Function to Display a Digit
 *
 * @brief This function takes the array
 * \ref figure and looks-up the code by
 * index to write to pins of \ref SEGMENT
 * port. It also selects the right value
 * to pull \ref DIGIT pins depending on
 * configuration of \ref FIRST_DIGIT. */
/* Firstly let's presume that
the digit 0 is connected to pin 7.
There is a clear pattern here:
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

/* More universal but computation-intensive
 * would be: ~( (1<<FIRST_DIGIT) >> d )
 * and ~( (1<<FIRST_DIGIT) << d ) */

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

  DIGIT = ~( (1<<FIRST_DIGIT) SHIFTDIR d );

  #endif

  SEGMENT = figure[v];

}

/** @name Display Test Loop
 *
 * @brief This is a scrolling loop
 * intended for display hardware
 * tests and basic demo. It can be
 * disabled by setting constant
 * \ref TESTING_FUNCTIONS to 0.
 *
*/
#if TESTING_FUNCTIONS
void display_test_loop(unsigned char x)
{

  short unsigned int i, j;

  while(x--) {

    for( i=0; i<10; i++ ) {

      for( j=0; j<4; j++ ) {

	display_digit( j, i );
      }

    }

  }

}
#endif


/** @name Standalone Testing Function
 *
 * @brief The main() function is only
 * compiled when \ref STANDALONE_TEST
 * is set to 1.
 *
*/
#if STANDALONE_TEST || DOXYGEN
void main (void)
{

  display_test_loop(2);
  DIGIT = 0xff;

}
#endif

/** \} */
/** EOF */
