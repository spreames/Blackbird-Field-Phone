/*
 * Interrupts.c
 *
 * Created: 29-Sep-2025
 * Updated: 10-Oct-2025
 * Author: Steve Reames
 * Released:
 */ 

#include "Blackbird.h"

extern volatile uint8_t TimerTickFlag;
extern uint8_t T20HzCounter;

/*****************************************************************************/
/*                          ISR - System Tick                                */
/*****************************************************************************/
// The Timer/Counter Type A generated an interrupt every 8.33 ms. This is
//   used for the main loop timing and also for generating a 20 Hz clock.
ISR(TCA0_CMP0_vect)
{
	TimerTickFlag = TRUE;							// set timer tick for main software loop
	TCA0.SINGLE.INTFLAGS = 0x10;					// clear interrupt
	if (T20HzCounter != 0)
	{
		T20HzCounter--;
	}
	else
	{
		T20HzCounter = 2;							// reset the counter
		PORTA.OUTTGL = PIN7_bm;						// Toggle the 20 Hz clock pin
	}	
	return;
}




