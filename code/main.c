/** @brief Main Program
 * @author Ilya Dmitrichenko <errordeveloper@gmail.com>
 * @file main.c
 *
*/

/** \mainpage Emebedded Systems Case Study
 *
 * For a final year CT3041N module at LMU
 * 
 * A generic project for \e Intel \c 8051 microcontroller.
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
 * commonly used type of circuit.
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
*/

#ifdef SDCC
#include <at89x51.h>
#else
#include <reg51.h>
#endif

#include "settings.h"

void main ( void )
{

  /** Initialise the DS1620 sensor
   * by writing high and low trigger
   * values and enable MCU mode
  */
}
