/**
 * @brief This file implements LED digital display driver function.
 * @file segment_display.c
 *
 * @details The code has been tested with 4-digit 7-segment
 * multiplexed LED display using GUI emulator.
 *
 * @image html segment_display.png "MCU 8051 IDE Virtual Multiplexed LED Display"
 * @image katex segment_display.png "MCU 8051 IDE Virtual Multiplexed LED Display"
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
 * @author Ilya Dmitrichenko <errordeveloper@gmail.com>
 *
*/

#ifdef SDCC
#include <at89x51.h>
#else
#include <reg51.h>
#endif

#include "settings.h"

#if DOXYGEN /* These definitions are only for documentation. */

/** @name Software Configuration
 * These macros can be used to switch different parts of code.
 * \{
*/
/** @brief Include \ref display_test_loop() function. */
#define TESTING_FUNCTIONS	[( 0 | 1 )]
/** @brief Include \ref main() loop for testing. */
#define STANDALONE_TEST		[( 0 | 1 )]

/** There two ways of dealing with the way first digit is connected.
 * One is faster and can be chosen by setting \ref BASIC_METHOD to 1.
 * The other method involves an extra bit-shift operation and is more
 * universal. With the second method \ref SHIFTDIR needs to be defined
 * only if pins are connected in unusual order.
 */
#define BASIC_METHOD		[( 0 | 1 )] /**< Apply faster code for digit selection. */

/** \} */

/** @name LED Interface Pins
 * These constants define which ports are in use for LED display.
 * \{
*/
#define DIGIT [( P0 | P1 | P2 | P3 )] /**< Set the port for digit selection. */
#define SEGMENT [( P0 | P1 | P2 | P3 )] /**< Set the port for segment selection. */
/** \} */

/** LED segment displays can be connected
 * with common cathodes or anodes.
 * \{
*/
#define COMMON_PIN [( ANODE | CATHODE )]

#endif /* DOXYGEN */

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
   #define SHIFTDIR >> 
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
 * configuration of \ref FIRST_DIGIT.
 *
 * \arg d digit position (0 to 3)
 * \arg v value of digit (0 to 9)
 *
 * @details Presuming that the digit 0
 * is connected to pin 7 of \ref DIGIT,
 * a clear pattern can be identified:
 *
 * \code
 *	 switch(d)
 *	 {
 * 		case 0:
 * 			DIGIT = 0x80; // 10000000 = 128
 * 		case 1:
 * 			DIGIT = 0x40; // 01000000 = 64
 * 		case 2:
 * 			DIGIT = 0x20; // 00100000 = 32
 * 		case 3:
 * 			DIGIT = 0x16; // 00010000 = 16
 * 		default:
 * 			DIGIT = 0xff; // Off
 * 	}
 * \endcode
 *
 * It can be expressed as:
 *
 * \code
 * DIGIT = 128/(2**d)
 * \endcode
 *
 * However, the '**' operator is not valid in C,
 * and using 'math.h' is not considered
 * appropriate for this small design.

 * It is in fact most appropriate to use '>>'
 * bit-shift operator:
 *
 * \code
 * DIGIT = (128 >> d);
 * \endcode
 *
 * Though, it turns out that the above values
 * (128, 64, 32, 16) are wrong.
 * We should invert the bits: 
 *
 * \code
 * DIGIT = ~(128 >> d);
 * \endcode
 *
 * By experiment, two techniques were found.
 * One uses hard-coded assignments for each
 * particular predefined pin configuration.
 * Setting \ref BASIC_METHOD to 1 enables
 * this technique.
 *
 * More universal but computation-intensive
 * method would be:
 * 
 * \code
 * switch(FIRST_DIGIT)
 * {
 *  	case LOWER:
 *	~( (1<<FIRST_DIGIT) >> d ); break;
 *  	case UPPER:
 *	~( (1<<FIRST_DIGIT) << d ); break;
 * }
 * \endcode
 *
 * The code, in fact, takes advantage of
 * pre-processor conditional definitions
 * and uses \ref SHIFTDIR set to '<<' or
 * '>>' depending what the value of
 * \ref FIRST_DIGIT is set to. In an
 * unusual case \ref SHIFTDIR has to
 * be set manually.
 *
*/
void display_digit(unsigned char d, unsigned char v)
{

  #if BASIC_METHOD

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
#if TESTING_FUNCTIONS || DOXYGEN
void display_test_loop(unsigned char x)
{

  short unsigned int i, j;

/** Scroll full count x times.
*/
  while(x--) {

    for( i=0; i<10; i++ ) {

      for( j=0; j<4; j++ ) {

	display_digit( j, i );

      }

    }

  }

}
#endif


#ifndef MAIN_PROGRAM
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

/** Run display_test_loop() 2 times.
*/
  display_test_loop(2);
  DIGIT = 0xff;

}
#endif
#endif

/** \} */
/** EOF */
