#include "HAL.h"
#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "type.h"
#include "i2c.h"
#include "gpio.h"
extern uint32_t SystemFrequency ;
//DL_I2C_CALLBACK _cb;
//void I2C_Callback_Slave(uint32_t status)
//{
//	BOOL ack = _cb(status);
//
//	DrvI2C_Ctrl(0, 0, 1, ack);
//}
//void I2C_Callback_Error(uint32_t status)
//{
//
//	DrvI2C_Ctrl( 0, 0, 1, 1);
//
//}
void HAL_I2C_Enable(UINT32 baudrate, UINT8 address, DL_I2C_CALLBACK cb)
{
	//_cb = cb;
//	DrvGPIO_InitFunction(E_FUNC_I2C);
//	DrvI2C_Open(baudrate);
//	DrvI2C_SetAddress(0, address, 0);
//	DrvI2C_Ctrl( 0, 0, 0, 1);
//	DrvI2C_InstallCallback(I2CFUNC, I2C_Callback_Slave);
//    
//    DrvI2C_InstallCallback(ARBITLOSS, I2C_Callback_Error);
//    DrvI2C_InstallCallback(BUSERROR, I2C_Callback_Error);
//    DrvI2C_InstallCallback(TIMEOUT, I2C_Callback_Error);
//
//    DrvI2C_EnableInt();

	//  for ( i = 0; i < BUFSIZE; i++ )
	//memset(I2CRegBuffer, 0,BUFSIZE);
	//GPIOInit();
  	I2CSlaveInit(address);			/* initialize I2c */
}

void HAL_I2C_Disable()
{
}

void HAL_I2C_Write(UINT8 v)
{
	DrvI2C_WriteData(v);
}

UINT8 HAL_I2C_Read()
{
	return DrvI2C_ReadData();
	//return LPC_I2C->DAT;
}

void HAL_I2C_SetAddress(UINT8 address)
{
	if (address==0) DrvI2C_SetAddress(0, (address<<1), 0);
	else 
	DrvI2C_SetAddress(0, (address<<1)+1, 0);
	//LPC_I2C->ADR0 = address;
}
volatile unsigned int ticks;
//void HAL_Init_SysTick100us()
//{
//    ticks = 0;
//    
//    SysTick->LOAD = 100 * CyclesPerUs;
//    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
//}
void SysTick_Handler(void)
{
    ticks++;
}
void HAL_Init_SysTick100us()
{
    ticks = 0;
    
    SystemInit();	// Initialize clocks
                     
    SysTick_Config (SystemFrequency/1000);    // Configure the SYSTICK
}
unsigned int HAL_GetTicks100us()
{
    return ticks;
}
void HAL_Delay100us(UINT32 d)
{
    unsigned int start = ticks;
    while( (ticks - start) <= d);
}
void HAL_ResetChip()
{
    //DrvSYS_ResetChip();
	NVIC_SystemReset();
}
void HAL_Assert()
{
	volatile int cont = 0;

	while(!cont);
		
}

