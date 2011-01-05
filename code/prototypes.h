/** @brief All function prototypes are in this header.
 * @file prototypes.h
 *
*/

#if !DOXYGEN

/**@name Sensor Interface Functions
 * \{
*/

void	ts_wait( void );

void	tsc( char byte );

int	tsq( unsigned char mode, int data );

void	sensor_setup( void );

int	sensor_read( void );

void	sensor_test_loop( unsigned char x );

/** \} */

/**@name Display Driver Functions
 * \{
*/

void display_digit( unsigned char d, unsigned char v );

void display_number( int x, unsigned int t );

void display_test_loop( unsigned char t );

/** \} */

#endif
