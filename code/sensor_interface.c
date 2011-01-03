#if !DOXYGEN /* Real settings. Do not edit other defines. */

/* Configure some options to compile.
 * Including optimisation and debug.
*/
#define TESTING_FUNCTIONS	1
#define STANDALONE_TEST		1

/* Configure which ports DS1620 sensor is connected to.
 * Use (P0), (P1), (P2) or (P3).
*/
#define TS_DATA		(P2_0)
#define TS_CLOCK	(P2_1)
#define TS_RESET 	(P2_2)

#endif /* End of real settings. Below is documentation. */

/**
 * @brief This file implements \c DS1620 digital temperature
 * sensor driver functions and macros.
 *
 * The information had been extracted from datasheets and 
 * code examples. The objective is to implement 3-wire
 * communication protocol that uses 8-bit commands to
 * query for measurement and configuration data, or storing
 * new configuration. The data is of 9-bit length, with
 * 9th bit to represent the sign, therefore some manipulation
 * routines are required.
 *
 * @file sensor_interface.c
 *
 * @image html sensor_function_block_diagram.gif "DS1620 Digital Thermometer and Thermostat (function block diagram)"
 *
 * Some code has been borrowed from \c DS1620 Arduino C++ Library:
 * -	https://github.com/thinkhole/Arduino-DS1620/blob/master/ds1620.cpp
 * .
 * However, after some consideration of how it would be used in the full context,
 * a single multi-purpose function had been developer.
 *
 * For datasheet information see Dallas Semiconductors, Maxim Integrated Products:
 * -	http://www.maxim-ic.com/quick_view2.cfm/qv_pk/2735
 * -	http://datasheets.maxim-ic.com/en/ds/DS1620.pdf
 * .
 *
*/

#ifdef SDCC
#include <at89x51.h>
#else
#include <reg51.h>
#endif
/** \name DS1620 Commands
 * \{
 * 
*/

/** @brief Read temperature value from the register. */
#define READ_TEMP  0xAA		
/** @brief Write to the TH (Trigger High) register. */
#define WRITE_TH   0x01
/** @brief Write to the TL (Trigger Low) register. */
#define WRITE_TL   0x02	
/** @brief Read the TH (Trigger High) register. */
#define READ_TH    0xA1	
/** @brief Read the TL (Trigger Low) register. */
#define READ_TL    0xA2	
/** @brief Read the value of the counter byte. */
#define READ_COUNT 0xA0	
/** @brief Read the value of the slope counter. */
#define READ_SLOPE 0xA9	
/** @brief Start temperature conversion. */
#define RUN_CONV 0xEE	
/** @brief Stop temperature conversion. */
#define END_CONV  0x22	
/** @brief Write configuration register. */
#define WRITE_CONF 0x0C	
/** @brief Read configuration register. */
#define READ_CONF  0xAC	
/** @brief Software testing only. */
#define TEST 0x00

/** \} */


/** \name DS1620 Constants
 * \{
 *
*/
/** @brief Time to wait (20 ms) after a \c EEPROM write
 * @details This constant is not in use, see ts_wait() .
 * 
*/
#define WRITE_DELAY 20 	
/** @brief Enables CPU-mode and disables single-shot-mode.
 * @details \see DS1620 datasheet for details */
#define TS_CONF 10
/** \} */

#if DOXYGEN

/** @name Software Configuration
 * These macros can be used to switch different parts of code.
 * \{
*/
/** @brief Include \ref display_test_loop() function. */
#define TESTING_FUNCTIONS	[( 0 | 1 )]
/** @brief Include \ref main() loop for testing. */
#define STANDALONE_TEST		[( 0 | 1 )]
/** \} */

/** \name DS1620 Interface Pins
 * 3-wire connection to the MCU Port
 *
 * \{
*/
/** @brief Data pin for DS1620 sensor chip */
#define TS_DATA	 [( P0 | P1 | P2 | P3 )]
/** @brief Clock pin for DS1620 sensor chip */
#define TS_CLOCK [( P0 | P1 | P2 | P3 )]
/** @brief Enable pin for DS1620 sensor chip */
#define TS_RESET [( P0 | P1 | P2 | P3 )]
/** \} */

#endif

/** \name Macros
 * Very simplistic handy functions are implemented as macros.
 * \{
*/
/** @brief Positive pulse on \ref TS_CLOCK. */
#define TS_TICK() TS_CLOCK = 0; TS_CLOCK = 1
/** @brief Pull \ref TS_RESET low. */
#define TS_STOP() TS_RESET = 0
/** @brief Pull \ref TS_RESET low,
 * \ref TS_CLOCK high,
 * and then \ref TS_RESET high. */
#define TS_START() TS_RESET = 0; TS_CLOCK = 1; TS_RESET = 1
/** @brief Basic DS1620 setup macro example */
#define TS_SETUP() tcq(WRITE_TL, 15); tcq(WRITE_TH, 30); \
		   tcq(WRITE_CONF, TS_CONF); tcq(RUN_CONV, 0)

/* #define TS_WAIT() */

/** \} */

short int temp = 0; /**< Gloabal temperature variable. */

/** @name Wait for 20 ms
 * @brief This function is require when writing data
 * to the \c EEPROM of the \c DS1620 sensor chip.
 *
 * @details Implemented using either nested \c for-loop
 * or inline assembly function (quote below).
 * The MCU 8051 IDE provides delay loop calculator,
 * which was used to generate the assembly code.
 *
 * The default behaviour is to use a nested for-loop.
 * This behaviour can be changed using pre-processor
 * constant USE_ASM_TS_WAIT. Skelleton code had been
 * also wirtten with different C compiler in mind.
 * 
 * \code
 * ; START: Wait loop, time: 20 us
 * ; Clock: 14745.6 kHz (12 / MC)
 * ; Used registers: R0
 * ; Rest: -889.6 ns
 * LOOP:
 *	MOV	R0, #007h
 *	NOP
 *	DJNZ	R0, $
 *	NOP
 *	NOP
 * \endcode
 *
*/
void ts_wait(void)
{
#ifdef USE_ASM_TS_WAIT

#ifdef SDCC
_asm
	MOV	R0, #007h
	NOP
	DJNZ	R0, $
	NOP
	NOP
_endasm;
#else /* !SDCC */
INLINE_ASM_BEGIN_KEYORD
	MOV	R0, #007h
	NOP
	DJNZ	R0, $
	NOP
	NOP
INLINE_ASM_END_KEYWORD
#endif /* SDCC || !SDCC */

#else /* !USE_ASM_TS_WAIT */

  unsigned char i, j;

/** Total run of nested for loops will be 50. */
  for( i = 0; i < 10; i++ ) {

    for( j = 0; j < 5; j++ ) {;}

  }

#endif /* USE_ASM_TS_WAIT */
}

/** @name Temperature Sensor Command
 * \param byte - 8-bit command to write.
 * @details Writes a command byte (bit-by-bit) to the
 * \ref TS_DATA pin, applying \ref TS_TICK after
 * each bit. Code line below performs the
 * \c "shift-right-AND-mask" operation.
 * \code
 * TS_DATA = ( bytes >> n ) & 0x01;
 * \endcode
 *
*/
void tsc( char byte )
{

/**
 * \dot
 *
 * digraph G {
 *
 * node [style=filled, fontname=Free Sans, fontsize=8];
 *
 * fontname="Free Sans", fontsize=9;
 * label = "Write 8-bit command to DS1620";
 *
 * start[label="N = 0", shape=rect]
 *
 * step1[label="Shift 8-bit data by N to the right"]
 * step2[label="Mask with \"00000001\" to get LSB"]
 * step3[label="Write result to TS_DATA", shape=rect]
 *
 * loop[label="Increment N by 1", shape=diamond]
 * cond[label="N is less then 8"]
 * fail[label="N is equal to 8"]
 *
 * start -> step1 -> step2 -> step3 -> loop;
 * loop -> cond -> step1;
 * loop -> fail -> DONE;
 *
 * }
 *
 * \enddot
 *
*/
  char n;

  for( n = 0; n < 8; n++ ) {

    TS_DATA = ( byte >> n ) & 0x01;

    TS_TICK();

  }

}

/** @name Temperature Sensor Query
 *
 * \param mode - the command to operate (sets the mode)
 * \param data - value to write, pass '0' when reading
 * 
 * @brief This function implements all-in-one DS1620
 * sensor control. This is probably the most straight-
 * forward way of communicating with the sensor.
 * Having multiple function is not considered to
 * be of any particular use. The only helper function
 * that is needed is tsc().
 * 
*/
int tsq( unsigned char mode, int data )
{

  char n; int read = 0;

#if 0
  /* There is no need to
   * assign it to zero if
   * it is already zero.
   *
  */
  if( data == 0 ) {

    if( mode == READ_TEMP ) {

    /* It should be global. */
       short int temp = 0;

    } else {

    /* As said, no need to. */
       data = 0;

    };

  int read = 0;

  }
#endif

/** <b> Switch operation modes,
 * depending on command: </b>
 *
*/
  switch( mode ) {

/** - Start or Stop conversion:
 *    - Only send the command, namely \ref RUN_CONV or \ref END_CONV
 *    - Will always return zero
 *    .
*/
    case RUN_CONV:
    case END_CONV:

      TS_START();

      tsc(mode);

      TS_STOP();

      break;

/** - Read 9-bit data for the fallowing commands:
 *    - \ref READ_SLOPE or \ref READ_COUNT
 *    - \ref READ_TH or \ref READ_TL
 *    - \ref READ_TEMP
 *    - <i> The sequence is illustrated in the flow diagram </i>
 *    .
 *
 * \dot
 *
 * digraph G {
 *
 *	node [style=filled, fontname=Free Sans, fontsize=8];
 *
 *	fontname="Free Sans", fontsize=9;
 *	label = "Read 9-bit data from DS1620";
 *
 *	start[fillcolor=white,\
 *	label="If the command is \n\
 *	\n READ_TH or READ_TL \n\
 *	or READ_SLOPE or READ_COUNT \n READ_TEMP"]
 *
 * 	write[label="Write command byte", shape=diamond]
 *
 *	ts_start[label="Pull TS_RESET low,\
 *	\n then TS_CLOCK high and TS_RESET hight"]
 *	ts_stop[label="Pull TS_RESET low"]
 *
 *	ts_data_input[label="Pull-up TS_DATA pin\n\
 *	to Hi-Z state for input"]
 *	ts_data_output[label="Pull-down TS_DATA \n\
 *	pin for output"]
 *
 *
 * 	loop[label="N = 0", shape=rect]
 * 	read[label="Pull TS_CLOCK low,\n\
 * 	read TS_DATA and\n pull TS_CLOCK high \n\
 *	\n Apply \"shift-left-OR-mask\" \n to append results"]	
 * 	next[label="Increment N by 1", shape=diamond]
 * 	cond[label="N is less then 9"]
 * 	fail[label="N is equal to 9"]
 *
 * 	data[label="Data has been read", fillcolor=white]
 *
 *	start -> ts_start -> write -> ts_data_input -> loop;
 * 	loop -> read -> next;
 * 	next -> cond -> read;
 * 	next -> fail -> ts_data_output -> ts_stop -> data;
 *
 * }
 *
 * \enddot
 *
 * As it can be see from the flow digaram, the
 * procedure underneath this function is quite linear.
 * Further bit manipulation is required for
 * handling of negative value representation:
 * 
 * \code
 *
 * temp = data & 0x00ff;
 *
 * temp = 0x0100 - temp;
 *
 * temp *= (short)-1;
 *
 * return ( temp / 2 );
 *
 * \endcode
 *
 * The idea behind writing tsq() as one long
 * function is to demonstrate the linearity
 * and general nature of operation, as well
 * optimising the size on the chip.
 *
*/
    case READ_SLOPE:
    case READ_COUNT:
    case READ_TEMP:
    case READ_TH:
    case READ_TL:


      TS_START();

      /* Send the command byte. */
      tsc(mode);

      /* Pull-up to Hi-Z state for input. */
      TS_DATA = 1;

      /* Read data with negative clock pulse. */
      for( n = 0; n < 9; n++ ) {

        TS_CLOCK = 0;

	read = TS_DATA;

	TS_CLOCK = 1;

	data = data | read << n;

      }

      /* Pull-down the pin state for writing. */
      TS_DATA = 0;

      TS_STOP();

      if( mode == READ_TL || \
      	  mode == READ_TH || \
	  mode == READ_TEMP ) {

	/* Check for negative temperature reading. */
        if(( mode == READ_TEMP ) \
	  && ( data & 0x0100 )) {
  
  	    temp = data & 0x00ff;
  
  	    temp = 0x0100 - temp;
  
  	    temp *= (short)-1;

	    return ( temp / 2 );
  
  	  } else {

	    return ( data / 2 );

	  }

      } else {

	return ( data );

      }

      /* break; */

/**  - Write 9-bit data for the \ref WRITE_TL or \ref WRITE_TH
 *	- Send command using tsc(mode)
 *	- Multiply the data value by 2
 *	- Write bits using "shift-right-AND-mask" operation
 *	.
 *
*/
    case WRITE_TH:
    case WRITE_TL:

      TS_START();

      tsc(mode);

      if( mode == WRITE_TL || mode == WRITE_TH ) {

	data *= 2;

      }

      /* Write data with negative clock pulse. */
      for( n = 0; n < 9; n++ ) {

	TS_DATA = ( data >> n ) & 0x01;

	TS_TICK();

      }

      ts_wait();

      TS_STOP();

      break;

/**  - Write 8-bit data to the configuration register
*/
    case WRITE_CONF:

      TS_START();

      tsc(mode);

      tsc(data); 

      ts_wait();

      TS_STOP();

      break;

/** - Test case: return the data */
    case TEST:

	TS_START();

	TS_STOP();

	return ( data );

	/* break; */

  }

  return 0;
}

#ifdef EXTRA_FUNCTIONS
/** @name This function read the value of configuration register.
 * \returns data stored in the configuration register of \c DS1620
 * @details It first pulls the \ref TS_RESET pin high and then
 * sends \ref READ_CONF command, reads the \ref TS_DATA pin for
 * next 8 cycles and pulls \ref TS_RESET returning read value.
 *
*/
unsigned int ts_config_read( void )
{

  unsigned char conf_data = 0;
  unsigned char conf_read = 0;
  char n;

  TS_START();

  ts_write( READ_CONF, 0 );

  /* Change pin mode to hi-z for input reading */

  TS_DATA = 1;

  for( n = 0; n < 8; n++ ) {

    TS_CLOCK = 0;

    read = TS_DATA;

    TS_CLOCK = 1;

    conf_data = conf_data | conf_read << n;

  }

  /* Change pin mode to output */

  TS_DATA = 0;

  TS_STOP();

  return ( conf_data );

}

/** @name Write data to the configuration register
 * \retval 0 Write successful
 * \retval 1 Write verification failed
 * \retval 2 Wrong input data content
 * @details It first pulls the \ref TS_RESET pin high and then
 * sends \ref WRITE_CONF command, reads the \ref TS_DATA pin for
 * next 8 cycles and pulls \ref TS_RESET returning an error code.
 * A delay of 10 ms (\ref WRITE_DELAY) is used to wait for sensor
 * to register new configuration. Basic error check is performed. 
 *
*/
char ts_config_write( unsigned char conf_data )
{

  if( conf_data > 0 ) {

    TS_START();

    ts_write( WRITE_CONF, 0 );

    ts_write( conf_data, 0 );

    /* It may take up to 10 ms */
    ts_wait();

    TS_STOP();

    if( ts_config_read() == conf_data ) {

      return 0;

    } else {

      return 1;

    }

  } else {

    return 2;

  }


}
#endif /* EXTRA_FUNCTIONS */

/** @name Sensor Test Loop
 *
 * @brief This is a loop
 * intended for hardware tests.
 * It can be disabled by setting
 * constant \ref TESTING_FUNCTIONS to 0.
 *
*/
#if TESTING_FUNCTIONS || DOXYGEN
void sensor_test_loop(unsigned char x)
{

  while(x--) {

/** Use \ref TEST command to check basic functionality.
*/
    tsq(TEST, 100);

  }

}
#endif

/** @name Standalone Testing Function
 *
 * @brief The \c main() function is only
 * compiled when \ref STANDALONE_TEST
 * is set to 1.
 *
*/
#if STANDALONE_TEST || DOXYGEN
void main (void)
{

/** Run sensor_test_loop() 2 times.
*/
  sensor_test_loop(2);

}
#endif
