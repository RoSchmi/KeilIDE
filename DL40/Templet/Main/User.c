/// Config Pins////
// I2C_SCL_PU  -> TP4 Connect to TP30 -> PIO2_9
// I2C_SDA_PU  -> TP3 Connect to TP8  -> PIO0_7
// UP_STREAM   -> TP2 Connect to TP27 -> PIO2_6
// DOWN_STREAM -> TP1 Connect to TP17 -> PIO1_8
///


#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "type.h"
#include "i2c.h"
#include "DaisyLink.h"
#include "gpio.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>


////////////////////////////
#define MANUFACTURER_GHI 0x10
#define MODULE_TYPE		 0x01 
#define MODULE_VERSION	1
/////////////////////////
//extern volatile uint8_t I2CWrBuffer[BUFSIZE];
//extern volatile uint8_t I2CRegBuffer[BUFSIZE];
//extern volatile uint32_t I2CSlaveState;
//extern volatile uint32_t I2CReadLength, I2CWriteLength;

/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
#define REGS_SIZE (DL_REGS_SIZE + 4)		// CHANGE MEH
struct Reg_t
{
	struct DL_Regs dl;

	// Application-specific I2C regs	
	
	// Should not be written by master (but we're safe if it's written)
	//RGB_T CurrentColor;        // 8-10
	
	// Read/write by master
	
	BYTE led0;                      // 12, LSB
	BYTE led1;                          // 11
	BYTE led2;
	BYTE led3;

} __attribute__((__packed__));
struct Reg_t Reg;
unsigned long lastTicks, elapsed;
void DisableUARTPin(void);
void EnablePWM(void);
//unsigned long colorTime;
//long tempElapsed, tempColorTime;
//long rStart, gStart, bStart;
//long rVector, gVector, bVector;
//long r, g, b;
//int shift = 0;
#define LED_PORT 1
#define LED_PIN 11

extern unsigned int ticks;

int main (void)
{
  //uint32_t i;
  HAL_Init_SysTick100us();
  GPIOInit();
  DrvGPIO_Open(LED_PORT, LED_PIN,  E_IO_OUTPUT);
  DrvGPIO_ClrBit(LED_PORT,LED_PIN);
	//DisableUARTPin();
 
  //EnablePWM();

  // Clear out our daisylink registers and initialize daisylink
  memset(&Reg, 0, REGS_SIZE);
  DL_Initialize((struct DL_Regs*)&Reg, MANUFACTURER_GHI, MODULE_TYPE, MODULE_VERSION);
  //i =0;
  while(TRUE)
	{
		// Wait until we are ready to begin the state machine
		if( 0 == DL_Process() /*|| Reg.Activate == 0x00???*/ )
		{
			// Everything should be in an "off" state
			lastTicks = GetTicks();

			continue;
		}
		if (Reg.led0)
		{
			 DrvGPIO_SetBit(LED_PORT,LED_PIN);
		}
		else
		{
			 DrvGPIO_ClrBit(LED_PORT,LED_PIN);
		}


		// Start state machine here

	}// End state machine loop

//  for ( i = 0; i < BUFSIZE; i++ )
//  {
//	I2CRegBuffer[i] = 0x00;
//  }
//  
//  I2CSlaveInit();			/* initialize I2c */
 
  /* When the NACK occurs, the master has stopped the 
  communication. Just check the content of I2CRd/WrBuffer. */
  //while ( I2CSlaveState != DATA_NACK );
  //return 0;
}
 
void DL_Platform_WriteRegister(UINT8 reg, UINT32 offset, UINT8 value)
{
	if((reg+offset) < REGS_SIZE)
	{
		*( ((BYTE*)&Reg) + reg + offset) = value;
	}
}

UINT8 DL_Platform_ReadRegister(UINT8 reg, UINT32 offset)
{
	UINT8 c = 0xFF;

	if((reg+offset) < REGS_SIZE)
	{
		c = *( ((BYTE*)&Reg) + reg + offset );
	}

	return c;
}
// Downstream PIO01_8 (TP1 - TP17)
#define DS_PORT 1
#define DS_PIN 8
// Upstream PIO02_6 (TP2 - TP27)
#define US_PORT 2
#define US_PIN 6
void HAL_SharedPinLow(int pin)
{
	switch(pin)
	{
		case DOWNSTREAM_PIN:
            DrvGPIO_ClrBit(DS_PORT, DS_PIN);
			DrvGPIO_Open(DS_PORT, DS_PIN,  E_IO_OUTPUT);

			break;

		case UPSTREAM_PIN:
			DrvGPIO_ClrBit(US_PORT, US_PIN);
			DrvGPIO_Open(US_PORT, US_PIN,  E_IO_OUTPUT);

			break;
	}	
}

void HAL_SharedPinInput(int pin)
{
   switch(pin)
	{
		case DOWNSTREAM_PIN:
			DrvGPIO_Open(DS_PORT, DS_PIN,  E_IO_INPUT);
	
			break;

		case UPSTREAM_PIN:
			DrvGPIO_Open(US_PORT, US_PIN,  E_IO_INPUT);
			
			break;
	}
}

UINT8 HAL_SharedPinRead(int pin)
{
//  test1 =   HAL_SharedPinRead(DOWNSTREAM_PIN);
//	 test2 =   HAL_SharedPinRead(UPSTREAM_PIN);
 switch(pin)
	{
		case DOWNSTREAM_PIN:
			return (((DrvGPIO_GetBit(DS_PORT, DS_PIN)) != 0)?1:0);
		
			//break;

		case UPSTREAM_PIN:
			return (((DrvGPIO_GetBit(US_PORT, US_PIN)) != 0)?1:0);
		
			//break;
	}	
	return 0;
}  

#define I2C_SCL_PU_PORT 2
#define I2C_SCL_PU_PIN 9

#define I2C_SDA_PU_PORT	0
#define I2C_SDA_PU_PIN	7
void HAL_I2C_SetPullups(BOOL isOn)
{
	if(isOn)
	{


		DrvGPIO_SetBit(I2C_SCL_PU_PORT, I2C_SCL_PU_PIN);
		DrvGPIO_Open(I2C_SCL_PU_PORT, I2C_SCL_PU_PIN,  E_IO_OUTPUT);
		
        DrvGPIO_SetBit(I2C_SDA_PU_PORT, I2C_SDA_PU_PIN);
        DrvGPIO_Open(I2C_SDA_PU_PORT, I2C_SDA_PU_PIN,  E_IO_OUTPUT);


	}
	else
	{
		DrvGPIO_Open(I2C_SCL_PU_PORT, I2C_SCL_PU_PIN,  E_IO_INPUT);
		DrvGPIO_Open(I2C_SDA_PU_PORT, I2C_SDA_PU_PIN,  E_IO_INPUT);
      
	}
}
void DisableUARTPin()
{
// 	LPC_IOCON->PIO1_6 &= ~(3<<3); // No pull
// 	LPC_IOCON->PIO1_7 &= ~(3<<3); // No pull
	DrvGPIO_Open(1,6,E_IO_INPUT);
	DrvGPIO_Open(1,7,E_IO_INPUT);

}
void EnablePWM()
{
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);    // Enable TIMER16_1 clock
    LPC_IOCON->PIO1_10 = 0x2; //0b10;        // PIO1_10 IS MAT1 output
    LPC_TMR16B1->MR3 = 1000;          // MR3 = Period
    LPC_TMR16B1->MR1 = 500;            // MR1 = 50% duty cycle
    LPC_TMR16B1->MCR = 0x400;        // MR3 resets timer
    LPC_TMR16B1->PWMC = 0xA;//0b1010;        // Enable PWM1 and PWM3
    LPC_TMR16B1->TCR = 0x1;//0b1;            // Enable Timer1
}
/******************************************************************************
**                            End Of File
******************************************************************************/
