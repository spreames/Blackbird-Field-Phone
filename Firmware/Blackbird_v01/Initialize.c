/*
 * Initialize.c
 *
 * Created: 29-Sep-2025
 * Updated: 10-Oct-2025
 * Author: Steve Reames
 * Released ...
 */ 

#include "Blackbird.h"

/*****************************************************************************/
/*                         Initialize Hardware                               */
/*****************************************************************************/
// The chapter references in these comments are from the Atmel ATtiny406 (tinyAVR 0-series) Datasheet.
// The fuse bits will select the 16/20 MHz clock at power-up. That will be changed
//   later in this code.
// This code assumes a 32 KHz clock.
void InitializeHardware(void)
{
	// *** Interrupts configuration - Chapter 13 ***
	// The Timer Tick will be the highest priority interrupt (Level 1).
	//   All other interrupts will be Level 0, but also a round-robin schedule.
	// Set the Round-robin Scheduling Enable bit:
	CPUINT.CTRLA |= CPUINT_LVL0RR_bm;
	// Set the level 1 interrupt vector:
	CPUINT.LVL1VEC = TCA0_CMP0_vect_num;				//The Timer/Counter creates the Timer Tick
	
	
	// *** I/O Ports Configuration - Chapter 16 ***
	// Port Control
	// PINOUT (ATtiny406, SOIC-20 package)
	// * pin 16 = PA0, UDPI programming pin
	// * pin 17 = PA1, LUT0-IN1 (input)
	// * pin 18 = PA2, LUT0-IN2 (input)
	// * pin 19 = PA3, OneShot25 (output)
	// * pin 2 = PA4, LUT0-OUT (output)
	// * pin 3 = PA5, TCB0, Buzz_clk (output)
	// * pin 4 = PA6, <unused>
	// * pin 5 = PA7, 20Hz_clk (output)

	// Output pins:
	PORTA.DIRSET = PIN3_bm | PIN4_bm | PIN5_bm | PIN7_bm;						// set PA3, PA4, PA5 and PA7 to output
	
	//PORTA.PIN0CTRL = 0;											// UPDI programming pin (in/out)
	//PORTA.PIN1CTRL = 0;
	//PORTA.PIN2CTRL = 0;
	//PORTA.PIN3CTRL = 0;											// output
	//PORTA.PIN4CTRL = 0;											// output
	//PORTA.PIN5CTRL = 0;											// output
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm | PORT_ISC_INPUT_DISABLE_gc;	// pullup (unused)
	//PORTA.PIN7CTRL = 0;											// output
	
	
	// * pin 11 = PB0, TCA0, WO0, 20Hz_clk (output)
	// * pin 10 = PB1, PushToRing (input)
	// * pin 9 = PB2, EnHVpositive (output)
	// * pin 8 = PB3, EnHVnegative (output)
	// * pin 7 = PB4, 20Hz_clk_input (input)
	// * pin 6 = PB5, CLKOUT (output)
	
	// Output pins:
	PORTB.DIRSET = PIN5_bm | PIN3_bm | PIN2_bm | PIN0_bm;	// set PB5, PB3, PB2, PB0 to output
	
	//PORTB.PIN0CTRL = 0;
	//PORTB.PIN1CTRL = 0;
	//PORTB.PIN2CTRL = 0;
	//PORTB.PIN3CTRL = 0;
	//PORTB.PIN4CTRL = 0;
	//PORTB.PIN5CTRL = 0;


	// * pin 12 = PC0, BuzzClkIn (input)
	// * pin 13 = PC1, EnBuzzerAmp (output)
	// * pin 14 = PC2, (unused)
	// * pin 15 = PC3, IncomingBuzz (input)
	
	// Output pins:
	PORTC.DIRSET = PIN1_bm;											// set PC1 to output
	
	//PORTC.PIN0CTRL = 0;
	//PORTC.PIN1CTRL = 0;
	PORTC.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_INPUT_DISABLE_gc;	// pullup (unused)
	//PORTC.PIN3CTRL = 0;


	// *** Clock Control configuration - Chapter 10 ***	// Clock is set for 32 KHz internal
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLB), (0x00));				// Set main clock prescale to 1
	ccp_write_io((void *) & (CLKCTRL.OSC32KCTRLA), (0x02));				// Run the 32 KHz all of the time.
	while ((CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm) == 0) { ; }		// Wait while the 32 KHz clock stabilizes:
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLA), (0x81));				// Switch to the internal low-power 32 KHz clock & enable output on pin	
	while ((CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm) == 0) { ; }		// Wait while the 32 KHz clock stabilizes:
	ccp_write_io((void *) & (CLKCTRL.OSC20MCTRLA), (0x00));				// Do not run the 16/20 MHz clock
	// Now, the pins have been set and the clock system has been configured.
	//   For peripherals, the clock will be 32KHz divided by 1.
	
	// *** Timer/Counter type A (TCA) configuration - Chapter 20 ***
	// This timer has two main functions. One, to create a 8.33 ms (120 Hz) interrupt that
	//   will becomes the timer tick for all software loops. The ISR will create
	//   the 20 Hz clock based on this timer.
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;				// No divider
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm					// Compare 0 enable, output pin enabled
						| TCA_SINGLE_WGMODE_SINGLESLOPE_gc;		// Single Slope PWM mode
	TCA0.SINGLE.PER = 272;										// TOP value, freq of PWM = 32768/(N+1) where N is TCA0.SINGLE.PER (8.33 ms)
	TCA0.SINGLE.CNT = 0x0000;									// clear the counter
	TCA0.SINGLE.CMP0 = 136;										// set output pin at 50% duty cycle
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0EN_bm;					// enable interrupts
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;					// Enable the peripheral

	// *** Timer/Counter type B (TCB) configuration - Chapter 21 ***
	// Create PWM signals to control the frequency of the Buzz Clock.
	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc;							// Divide clock by 1 (no divider)
	TCB0.CTRLB = TCB_CCMPEN_bm									// Enable compare and output pin
				| TCB_CNTMODE_PWM8_gc;							// 8-bit PWM mode
	TCB0.CCMPL = 18;											// TOP value, 8-bit value, the period of the waveform
	TCB0.CCMPH = 6;												// COMPARE value, 8-bit value, controls the duty cycle, set output pin at about 1/3 of the period for high harmonic content
	TCB0.CNT = 0;												// Counter should be initialized to BOTTOM value
	TCB0.CTRLA |= TCB_ENABLE_bm;								// Enable the peripheral


	// *** Custom Configurable Logic - Chapter 27 ***
	// We are implementing a 2-input AND gate.
	CCL.LUT0CTRLB = CCL_INSEL0_MASK_gc | CCL_INSEL1_IO_gc;		// IN0 is masked, IN1 is I/O pin
	CCL.LUT0CTRLC = CCL_INSEL2_IO_gc;							// IN2 is I/O pin
	
	CCL.TRUTH0 = 0x40;											// Truth table for AND gate
	
	CCL.LUT0CTRLA = CCL_OUTEN_bm;								// Enable output pin for LUT0-OUT
	
	CCL.LUT0CTRLA |= CCL_ENABLE_bm;								// Enable LUT0
	CCL.CTRLA = CCL_ENABLE_bm;									// Enable the CCL module

	// *** Port Multiplexer configuration - Chapter 15 ***
	// Only used when the output pin for a module is connected to the
	//   default port. For this case no alternate pinout is used.
	

	// *** Sleep Controller configuration - Chapter 11 ***
	// Sleep mode is not used.

	sei();					// enable all interrupts

}
