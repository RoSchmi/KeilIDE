#ifndef _DAISYLINK_H_
#define _DAISYLINK_H_

#include "../Common/type.h"
#include "../HAL/HAL.h"

#define DL_I2C_BAUDRATE		100000
#define DL_I2C_INVALID_REG	0xFF

// return codes for DL_Process
//#define DAISYLINK_READY 0
//#define DAISYLINK_NEWADDRESS 1
//#define DAISYLINK_INTERRUPT_UP 2
//#define DAISYLINK_INTERRUPT_DOWN 3

#define DL_REGS_SIZE		8
struct  DL_Regs{
    BYTE ID;           // 0 This node's DaisyLink ID
	BYTE Config;       // 1 Configuration and status bits
	BYTE DLVersion;    // 2 Version number of DaisyLink protocol
	BYTE ModuleType;   // 3 Type of DaisyLink node    
	BYTE ModuleVersion;// 4 Version number of the module
	BYTE Manufacturer; // 5 Manufacturer of the module
	BYTE Reserved4;  // 6 Reserved for future use		
	BYTE Reserved5;  // 7 Reserved for future use		
} __attribute__((__packed__));

void DL_Initialize(struct DL_Regs *reg, BYTE manufacturer, BYTE type, BYTE moduleVersion);
int DL_Process(void);
void DL_Interrupt(void);

// platform functions
void DL_Platform_WriteRegister(UINT8 reg, UINT32 offset, UINT8 value);
UINT8 DL_Platform_ReadRegister(UINT8 reg, UINT32 offset);
unsigned long GetTicks(void);

#endif	// _DAISYLINK_H_
