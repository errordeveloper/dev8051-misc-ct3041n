#if !DOXYGEN /* Real settings. Do not edit other defines. */

/* Configure some options to compile.
 * Including optimisation and debug.
*/
#define TESTING_FUNCTIONS	1
#define STANDALONE_TEST		1

/* Configure which ports DS1620 sensor is connected to.
 * Use (P0), (P1), (P2) or (P3).
*/
#define DIGIT		(P0)
#define SEGMENT 	(P1)

#endif /* End of real settings. Below is documentation. */

/**
 * @brief This file implements DS1620 digital temperature
 * sensor driver functions and macros.
 * @file sensor_interface.c
 *
 * @image html sensor_function_block_diagram.gif "DS1620 Digital Thermometer and Thermostat (function block diagram)"
 *
 * Some code has been borrowed from DS1620 Arduino Library:
 *	https://github.com/thinkhole/Arduino-DS1620
*/

/** DS1620 Commands
 * \{
*/

#define READ_TEMP  0xAA		/**< Read temperature value from the register. */
#define WRITE_TH   0x01		/**< Write to the TH (Trigger High) register. */
#define WRITE_TL   0x02		/**< Write to the TL (Trigger Low) register. */
#define READ_TH    0xA1		/**< Read the TH (Trigger High) register. */
#define READ_TL    0xA2		/**< Read the TL (Trigger Low) register. */
#define READ_COUNT 0xA0		/**< Read the value of the counter byte. */
#define READ_SLOPE 0xA9		/**< Read the value of the slope counter. */
#define RUN_CONV 0xEE		/**< Start temperature conversion. */
#define END_CONV  0x22		/**< Stop temperature conversion. */
#define WRITE_CONF 0x0C		/**< Write configuration register. */
#define READ_CONF  0xAC		/**< Read configuration register. */

/** \} */


/** DS1620 Constants
 * \{
*/
#define WRITE_DELAY 20 		/**< Time to wait (ms) after a EEPROM write */
/** \} */


int tsc ( char c, int d )

#define ts_command(X) ts_write(X, 0)

unsigned char ts_config_read( void );
char ts_config_write( unsigned char conf_data )

#if DOXYGEN
/** DS1620 Interface pins to the MCU port
 * \{
*/
#define TS_DATA	 [( P0 | P1 | P2 | P3 )] /**< Data pin for DS1620 sensor chip */
#define TS_CLOCK [( P0 | P1 | P2 | P3 )] /**< Clock pin for DS1620 sensor chip */
#define TS_RESET [( P0 | P1 | P2 | P3 )] /**< Enable pin for DS1620 sensor chip */
/** \} */
#endif

/** Very simplistic handy functions are implemented as macros.
 * \{
*/
#define TS_TICK() ( TS_CLOCK = 0; TS_CLOCK = 1 ) /**< Positive pulse on \ref TS_CLOCK. */

#define TS_STOP() ( TS_RESET = 0 ) /**< Pull \ref TS_RESET low. */

#define TS_START() \
	( TS_RESET = 0; \
	  TS_CLOCK = 1; \
	  TS_RESET = 1 ) /**< Pull \ref TS_RESET low, \ref TS_CLOCK high, and then \ref TS_RESET high. */

/** \} */

int tsq( char mode, int data )
{


  if( data == NULL ) {

    if ( mode == READ_TEMP ) {

       short int data = 0;

    else {

       int data = 0;

    }

    int read = 0;

  }

  char n;

  switch( mode ) {

/* = Start or Stop conversion = */
    case RUN_CONV:
    case END_CONV:

      TS_START();
      
      tsc(mode);

      TS_STOP();

      break;

/* = Read 9-bit data = */
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

      if( mode == READ_TL || mode == READ_TH ) {

	/* Check for negative temperature reading. */
        if(( mode == READ_TEMP ) \
	  && ( data & 0x0100 )) {
  
  	    data &= 0x00ff;
  
  	    data = 0x0100 - data;
  
  	    data *= (short)-1;
  
  	  }

	return ( data / 2 );


      } else {

	return ( data );

      }

      /* break; */

/* = Write 9-bit data = */
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

      TS_WAIT();

      TS_STOP();

      break;

  }
  
  return 0;
}

void tsc( char byte )
{

  for( n = 0; n < 8; n++ ) {
  
    TS_DATA = ( byte >> n ) & 0x01;
  
    TS_TICK();
  
  }

}

/** @name This function read the value of configuration register.
 * \returns data stored in the configuration register of DS1620
 * @details It first pulls the \ref TS_RESET pin high and then
 * sends \ref READ_CONF command, reads the \ref TS_DATA pin for
 * next 8 cycles and pulls \ref TS_RESET returning read value.
 *
*/
unsigned int ts_config_read( void )
{

  unsigned char conf_data = 0;
  unsigned char conf_read = 0;

  TS_START();

  ts_write( READ_CONF, 0 );

  /* Change pin mode to hi-z for input reading */

  TS_DATA = 1;

  for( char n = 0; n < 8; n++ ) {

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
    TS_WAIT();

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

/** @name Sensor Test Loop
 *
 * @brief This is a scrolling loop
 * intended for hardware tests.
 * It can be disabled by setting
 * constant \ref TESTING_FUNCTIONS to 0.
 *
*/
#if TESTING_FUNCTIONS || DOXYGEN
void sensor_test_loop(unsigned char x)
{

  short unsigned int i, j;

  while(x--) {

    for( i=0; i<10; i++ ) {

      for( j=0; j<4; j++ ) {

	sensor_read( j, i );

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

  sensor_test_loop(2);

}
#endif
