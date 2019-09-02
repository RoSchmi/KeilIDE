#ifndef _HAL_H_
#define _HAL_H_

#include "../Common/type.h"

typedef BOOL (*DL_I2C_CALLBACK)(UINT8 status);
void HAL_I2C_Enable(UINT32 baudrate, UINT8 address, DL_I2C_CALLBACK cb);
void HAL_I2C_Disable(void);
void HAL_I2C_Write(UINT8 v);
UINT8 HAL_I2C_Read(void);
void HAL_I2C_SetAddress(UINT8 address);

void HAL_ResetChip(void);

void HAL_Init_SysTick100us(void);
unsigned int HAL_GetTicks100us(void);
void HAL_Delay100us(UINT32 d);

#define DOWNSTREAM_PIN	1
#define UPSTREAM_PIN	2

void HAL_SharedPinLow(int pin);
void HAL_SharedPinInput(int pin);
UINT8 HAL_SharedPinRead(int pin);
void HAL_I2C_SetPullups(BOOL isOn);

void HAL_Assert(void);
#define ASSERT(X) if(!X) HAL_Assert()

#define HAL_DEBUG debug_printf

#endif	// _HAL_H_
