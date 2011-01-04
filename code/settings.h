/** @brief This header is to be included
 * in all source code C files.
 * @file settings.h
 *
 * @details All documentation is
 * included in the driver source
 * code C files. This header is
 * for software configuration
 * at the compile time.
 *
 * @author Ilya Dmitrichenko <errordeveloper@gmail.com>
*/

#if !DOXYGEN
//#define MAIN_PROGRAM

/* Configure some options to compile.
 * Including optimisation and debug.
*/
#define TESTING_FUNCTIONS	1
#define STANDALONE_TEST		1


/* Cofiguration of segment_display.c
*/

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


/* Cofiguration of sensor_interface.c
*/

/* Configure which ports DS1620 sensor is connected to.
 * Use (P0), (P1), (P2) or (P3).
*/
#define TS_DATA		(P2_0)
#define TS_CLOCK	(P2_1)
#define TS_RESET 	(P2_2)

#endif
