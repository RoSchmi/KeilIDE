// Example from Book: The definite Guide to Arm Cortex-M0 and Cortex-M0+, Chapter 14 
// (I have the E-book in Google Play Books)
// Code examples https://booksite.elsevier.com/9780128032770/example_code.php
// Or on my PC: F:\Updates_10\The definitive Guide to Cortex M0 and CortexM0+ Comanion Code examples\ExampleCodes

// Note: The files startup_LPC11xx.s and system_LPC11xx.c must be Version 1.0
// They may not be loaded from the Keil IDE which uses Version 1.1 (hangs in the initialization)

// In Options for Source Group, Tab C/C++, the folder Startup must be in the include path 


// LED is connected to pin PIO1_5
// System running at xx MHz
#include "LPC11xx.h"
void		LED_Config(void);
void 		LED_Set(void);

void 		LED_Clear(void);
void 		Delay(uint32_t nCount);

uint32_t dummy = 1;

int main(void)
{
			dummy = 2;
			//Configure LED outputs
	
			LED_Config();
#define LOOP_COUNT 0x80000
//#define LOOP_COUNT 0x00800
			while(1)
			{
				Delay(LOOP_COUNT);
				LED_Set();
				Delay(LOOP_COUNT);
				LED_Clear();
			}
}

void Delay(uint32_t nCount)
{
			while(nCount--)
			{}
}

void LED_Config(void)
{
	// Enable clocks to GPIO and IO config block
	// Bit 6: GPIO, bit 16: IO config
	LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<16) | (1<<6)); 
	__NOP();
	
	
// Short time delay to ensure the clock is on before next access 
	__NOP(); 
	__NOP(); 
	// PIO1_5 IO output config 
	// bit[10] - Open drain (0 = standard I/O, 1 = open drain) 
	// bit[5] - Hysteresis (0=disable, 1 =enable) 
	// bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater) 
	// bit[2:0] - Function (0 = IO, 1=~RTS, 2=CT32B0_CAP0) 
	LPC_IOCON->PIO1_5 = (0<<10) | (0<<5) | (0<<3) | (0x0); 
	// Optional: Turn off clock to I/O Config block to save power 
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<16); 
	// Set pin 8 as output 
	LPC_GPIO1->DIR = LPC_GPIO1->DIR | (1<<5); 
return; }

void LED_Set(void)
{
	LPC_GPIO1->MASKED_ACCESS [1<<5] = (1<<5); 
	return;
}

void LED_Clear(void) 
{ 
		// Clear bit 5 output to 1 
		LPC_GPIO1->MASKED_ACCESS [1<<5] = 0; 
	return; 
}

