/*
 * Blackbird.h
 *
 * Created: 29-Sep-2025
 * Updated: 9-Nov-2025
 * Author: Steve Reames
 * Released to Creative Commons
 *
 * Based on ATtiny 406 processor (SOIC-20 package)
 */ 


#ifndef BLACKBIRD_INCLUDE
#define BLACKBIRD_INCLUDE

/*****************************************************************************/
/*                          AVR Include Files                                */
/*****************************************************************************/
#include <avr/io.h>
#define F_CPU 32768UL				// needed for delay.h
//#include <util/delay.h>			// contains the function _delay_ms()
#include <avr/interrupt.h>			// need this for the sei() call
//#include <wdt.h>					// watchdog timer - disabled right now
//#include <avr/sleep.h>
//#include <power.h>
//#include <util/atomic.h>
//#include <avr/eeprom.h>
//#include "nvmstrl_basic.h"
#include <avr/cpufunc.h>			// required for writing to Configuration Change Protection (CCP) Registers
#include <avr/fuse.h>


/*****************************************************************************/
/*                         Function Prototypes                               */
/*****************************************************************************/
// Initialize.c
void InitializeHardware(void);

// main.c


/*****************************************************************************/
/*                C Preprocessor Defines & Type Definitions                  */
/*****************************************************************************/

#define TRUE	1
#define FALSE	0


#endif /* BLACKBIRD_INCLUDE */













/*********************************************************************************/


