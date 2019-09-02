/// Config Pins////
// I2C_SCL_PU  -> TP4 Connect to TP30 -> PIO2_9
// I2C_SDA_PU  -> TP3 Connect to TP8  -> PIO0_7
// UP_STREAM   -> TP2 Connect to TP27 -> PIO2_6
// DOWN_STREAM -> TP1 Connect to TP17 -> PIO1_8
///


#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "../Common/type.h"
#include "../HAL/i2c.h"
#include "../DaisyLink/DaisyLink.h"
#include "../GPIO/gpio.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "DL40.h"

/*******************************************************************************
**   Main Function  main()
*******************************************************************************/

struct Reg_t
{
	struct DL_Regs dl;
	BYTE Activate;
	BYTE Port;
	BYTE Pin;
	BYTE Dir;
	BYTE Value;

} __attribute__((__packed__));
struct Reg_t Reg;
void DL40_UpdatePinState()
{
			if (Reg.Dir ==  E_IO_INPUT)
			{
				DrvGPIO_ClrBit(Reg.Port,Reg.Pin);
				DrvGPIO_Open(Reg.Port, Reg.Pin,  E_IO_INPUT);
			}
			else 
			{
				DrvGPIO_ClrBit(Reg.Port,Reg.Pin);
				DrvGPIO_Open(Reg.Port, Reg.Pin,  E_IO_OUTPUT);
				if (Reg.Value)
					DrvGPIO_SetBit(Reg.Port,Reg.Pin);
				else 
					DrvGPIO_ClrBit(Reg.Port,Reg.Pin);
			}
}

	 
int main (void)
{
  BYTE i=0;
  HAL_Init_SysTick100us();
  GPIOInit();
  //BlinkLED(500);
	
  memset(&Reg, 0, REGS_SIZE);
  DL_Initialize((struct DL_Regs*)&Reg, MANUFACTURER_GHI, MODULE_TYPE, MODULE_VERSION);
  i =0;
	//Test GPIO
  //	for (i=0; i<29; i++)
  //	{
  //	DrvGPIO_ClrBit(ports[i],pins[i]);
	//	DrvGPIO_Open(ports[i],pins[i],  E_IO_OUTPUT);
	//	DrvGPIO_SetBit(ports[i],pins[i]);
 	//}
	// End test GPIO
  while(TRUE)
	{
		// Wait until we are ready to begin the state machine
		if( 0 == DL_Process() || Reg.Activate == 0x00 )
		{
			continue;
		}
		 Reg.Activate = 0;
		DL40_UpdatePinState();

		// Start state machine here

	}// End state machine loop


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
/******************************************************************************
**                            End Of File
******************************************************************************/
