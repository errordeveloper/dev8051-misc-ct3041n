/**
 * @brief This file implements LED digital display driver.
 * @file segment_display.c
 *
 * @details The code has been tested with 4-digit 7-segment
 * multiplexed LED display using GUI emulator.
 *
 * @image html segment_display.png "MCU 8051 IDE Virtual Multiplexed LED Display"
 * @image latex segment_display.png "MCU 8051 IDE Virtual Multiplexed LED Display"
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
 * @note DISPLAY_HOLD() is not implemented. It requires
 * testing with real LED display in order to determin
 * the hold time period, which depends on fade-out
 * time of real LED display.
 
 * @note Therefore the display_digit() function turns
 * the segments off in order to prevent effect of moving
 * digit. The function display_number() uses second
 * argument to sepcify number of cycles.
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
#include "prototypes.h"

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
 * Index 10, 11 and 12 are used for '-', 'C' and 'F'.
 *
 *\{
*/
#if DOXYGEN
char figure[13] = {
#else
static const char figure[13] = {
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
  0x90, // = 9
  0xbf, // = -
  0xc6, // = C
  0x8e  // = F

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
  0x6f, // = 9
  0x40, // = -
  0x39, // = C
  0x71  // = F

#endif /* COMMON_PIN */

#if !DOXYGEN
#define SYM_MINUS 10
#define SYM_C 11
#define SYM_F 12
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
 * @param d digit position (0 to 3)
 * @param v value of digit (0 to 9)
 *
*/
void display_digit( unsigned char d, unsigned char v )
{

/** @details Presuming that the digit 0
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

  SEGMENT = figure[v];

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
   #warning "In basic method values of first digit can be 7, 4, 3 or 0 only!"
   #endif

  #else /* GOOD_METHOD_IN_THEORY */

  DIGIT = ~( (1<<FIRST_DIGIT) SHIFTDIR d );

  #endif

}

/** @name The Function to Display a Number
 * @brief This function calls display_digit()
 *
 * @param x number to display (-999 to 9999)
 * @param t number of refresh cycles
 *
 * @details It uses an extra array for
 * marking which digit should be OFF
 * in case if it is zero and there no
 * other digit in front of it.
 * It could probably use a 4-bit mask.
 * Bit arrays are not allowed by SDCC.
 *
 * @note There is no boundary checking,
 * therefore the value of the argument
 * should be between -999 and 9999.
 * It was considered not appropriate
 * to implement fixed decimal point.
*/
void display_number( int x, unsigned int t )
{

#ifndef DOXYGEN
#define REM(X, Y) X/Y; X %= Y
#endif

  char n[4], m[4] = {1, 1, 1, 1}, i;
  /** Determin the value of each
   * decimal place by using int
   * division and remainder.
   *
  */
  if( x >= 0 ) {

  n[3] = REM(x, 1000);
  if( n[3] == 0 )
   m[3] = 0;

  n[2] = REM(x, 100);
  if( n[2] == 0 && m[3] == 0 ) m[2] = 0;

  n[1] = REM(x, 10);
  if( n[1] == 0 && m[2] == 0 ) m[1] = 0;

  n[0] = x;

  } else {

  x *= -1;

  n[3] = SYM_MINUS;

  n[2] = REM(x, 100);
  if( n[2] == 0 ) m[2] = 0;

  n[1] = REM(x, 10);
  if( n[1] == 0 && m[2] == 0 ) m[1] = 0;

  n[0] = x;

  }
  /** Repeat for t times. */
  while(t--) {

    for( i = 0; i < 4; i++ ) {
      /** If a digit is zero the it
       * it is not displayed, unless
       * there is a non-zero digit
       * in from of it.
       *
      */
      if( m[i] ) display_digit( i, n[i] );
      /* DISPLAY_HOLD(); */
      DIGIT = 0xff;

    }

  }

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
void display_test_loop( unsigned char t )
{

  short unsigned int i, j;

/** Scroll full count t times.
*/
  while(t--) {

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
  //display_test_loop(2);
  //DIGIT = 0xff;

  display_number(-768, 20);
  display_number(1769, 20);
  display_number(2761, 20);

}
#endif
#endif

/** \} */
/** EOF */
