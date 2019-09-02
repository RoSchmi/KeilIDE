#include "LPC11xx.h"
// LED is connected to pin PIO1_5

void LED_Config(void)
{
  // Enable clocks to GPIO and IO config block
  // Bit 6: GPIO, bit 16: IO config
  LPC_SYSCON->SYSAHBCLKCTRL |=  ((1<<16) | (1<<6));
	
  __NOP(); // Short time delay
  __NOP();
  __NOP();
	
  // PIO1_5 IO output config
  //  bit[10]  - Open drain (0 = standard I/O, 1 = open drain)
  //  bit[5]   - Hysteresis (0=disable, 1 =enable)
  //  bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater)
  //  bit[2:0] - Function (0 = IO, 1=~RTS, 2=CT32B0_CAP0)
  LPC_IOCON->PIO1_5 = (0<<10) | (0<<5) | (0<<3) | (0x0);

  // Optional: Turn off clock to I/O Config block to save power
  LPC_SYSCON->SYSAHBCLKCTRL &=  ~(1<<16);

  // Set pin 8 as output
  LPC_GPIO1->DIR = LPC_GPIO1->DIR | (1<<5);	
  return;
}


void LED_Set(void)
{
  // Set bit 5 output to 1
  LPC_GPIO1->MASKED_ACCESS[1<<5] = (1<<5);
  return;
}


void LED_Clear(void)
{
  // Clear bit 5 output to 1
  LPC_GPIO1->MASKED_ACCESS[1<<5] = 0;
  return;	
}

