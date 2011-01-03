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
 * - MCU 8051 IDE - Project Homepage - http://mcu8051ide.sourceforge.net/
 * - Small Device C Compile (SDCC) - Project Homepage - http://sdcc.sourceforge.net/
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
 * also stored on the Internet Git Hub
 * portal (http://github.com/errordeveloper/).
 *
 * - Repository Homepage - https://github.com/errordeveloper/dev8051-misc-ct3041n
 * - Source Code Tree - 
 * 
 * The best attempt was made at documenting
 * the code, however details were omitted
 * for explaining the operation of this very
 * commonly used type of circuit.
 * 
 * As mentioned above \emph{MCU 8051 IDE} open-source
 * package had been used. \emph{Small Device C Compiler
 * (SDCC)} \cite{sw:sdcc} was used to compile the program.
 * Git \cite{sw:git} revision control and Doxygen \cite{sw:doxygen}
 * documentation generator were of great help for the project.
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

}
