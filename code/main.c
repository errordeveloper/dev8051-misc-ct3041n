/** @brief Main Program
 * @author Ilya Dmitrichenko <errordeveloper@gmail.com>
 * @file main.c
 *
*/

/* Doxygen tagged comments were stripped. */

/** \mainpage Emebedded Systems Case Study
 *
 * For a final year CT3041N module at LMU -
 * a generic project for \e Intel \c 8051 microcontroller.
 *
 * Designed using <i>MCU 8051 IDE (v1.3.11)</i>
 * by <i>Martin Ošmera</i> and <i>Kara Blackowiak</i>.
 * <i>
 * - MCU 8051 IDE - Project Homepage - http://mcu8051ide.sourceforge.net/
 * - Small Device C Compile (SDCC) - Project Homepage - http://sdcc.sourceforge.net/
 * .
 * </i>
 * 
 * <b>Foreword Notes</b>
 * 
 * It had been rather difficult to find
 * an appropriate partner for group work,
 * therefore all work was done by one
 * student, Ilya Dmitrichenko.
 * 
 * In order to accomplish the log book
 * component of this assessment unit,
 * revision control software has been
 * utilised. It provides a very appropriate
 * facility for logging the programming
 * activities and keeps track of code
 * modifications. The repository had been
 * also stored on the Internet \e GitHub
 * portal (http://github.com/errordeveloper/).
 *
 * - Repository Homepage
 *   - https://github.com/errordeveloper/dev8051-misc-ct3041n/
 * - Source Code Tree
 *   - https://github.com/errordeveloper/dev8051-misc-ct3041n/tree/master/code/
 * - Commit History
 *   - https://github.com/errordeveloper/dev8051-misc-ct3041n/commits/master/
 * .
 * 
 * The best attempt was made at documenting
 * the code, however details were omitted
 * for explaining the operation of this very
 * commonly microcontroller used type of circuit.
 * 
 * As mentioned above <i>MCU 8051 IDE</i> open-source
 * package had been used to test the code in the
 * simulator. \e SDCC was used to compile the program.
 * Git revision control and Doxygen documentation
 * generator were of great help for the project.
 * \e Doxygen extracts tagged comments from the code.
 * <i>
 * - Doxygen Homepage - http://www.stack.nl/~dimitri/doxygen/
 * - Git Homepage - http://git-scm.com/
 * .
 * </i>
 *
 * <b>Introduction Notes</b>
 *
 * This report document details the implementation
 * by describing each of source code files.
 * The description is fallowed by listing of the
 * source code itself. Please note that tagged
 * comments used for description were stripped
 * from the listings. The code under pre-processor
 * condition statements relying on definition of
 * 'DOXYGEN' symbol is not to be used by the compiler.
 *
 * <i>Please also note that the code assumes C89
 * or C99 ISO standard compiler.</i>
 * 
*/

#ifdef SDCC
#include <at89x51.h>
#else
#include <reg51.h>
#endif

#include "settings.h"
#include "prototypes.h"

void main ( void )
{

  /** Initialise the DS1620 sensor
   * by writing high and low trigger
   * values and enable MCU mode
   * using sensor_setup().
   *
  */
  sensor_setup();

  /** Infinite loop is a common
   * element of an embedded
   * program, it will not exit
   * until the MCU powers-off.
   *
  */
  while(1) {

    display_number( sensor_read() );

    /* READ_DELAY(); */

  }

}
