#include "DaisyLink.h"

UINT8 reg = DL_I2C_INVALID_REG;
UINT32 offset = -1;

BOOL DL_I2C_Callback(UINT8 status)
{
	UINT8 ack = TRUE;
	UINT8 c;

	switch(status)
	{
		// SLA+W has been received and ACK has been returned
		case 0x60:
		case 0x68:

			reg = DL_I2C_INVALID_REG;

		break;

		// DATA has been received and ACK has been returned
		case 0x80:

			c = HAL_I2C_Read();

			if(reg == DL_I2C_INVALID_REG)	// selecting a register
			{
				reg = c;
				offset = 0;
			}
			else
			{
				DL_Platform_WriteRegister(reg, offset, c);
				offset++;
			}
			

		break;

		// DATA has been received and NACK has been returned
		case 0x88:

			c = HAL_I2C_Read();

			if(reg != DL_I2C_INVALID_REG)
			{
				DL_Platform_WriteRegister(reg, offset, c);

				offset++;
			}

		break;

		// SLA+R has been received and ACK has been returned
		case 0xB0: 
		case 0xA8:

			if(reg != DL_I2C_INVALID_REG)
			{
				HAL_I2C_Write(DL_Platform_ReadRegister(reg, offset));
				offset++;
			}

		break;

		// DATA has been transmitted and ACK has been received 
		case 0xB8:

			if(reg != DL_I2C_INVALID_REG)
			{
				HAL_I2C_Write(DL_Platform_ReadRegister(reg, offset));
				offset++;
			}

		break;

		// DATA has been transmitted and NACK has been received 
		case 0xC0:

		break;

		// STOP or Repeat START has been received
		case 0xA0:
			
		break;

		default:
			;
	}

	return ack;
}

// DAISY LINK

// DaisyLink

//#include <m8c.h>        // part specific constants and macros
//#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
//#include "delay.h"
//#include <string.h>
//#include "DaisyLink.h"

// The legacy neighbor bus has pull-down resistors rather than pull-up resistors
// so the polarity of the neighbor bus is reversed
//#define LegacyNeighborBus 1

// Default values for PIN_UPSTREAM and PIN_DOWNSTREAM pins are:
// P1[3] = PIN_UPSTREAM
// P1[5] = PIN_DOWNSTREAM
// Upstream is defined as "up" toward the mainboard; Downstrea is defined as
// "down" and away from the mainboard toward the end of the DaisyLink chain.
//
// IMPORTANT: It is possible to change these, but if alternative
// pins are not in P1, it will be necessary to edit Setup_As_Input, Setup_As_Output,
// and Read() to target the correct registers.

#define PIN_UPSTREAM   UPSTREAM_PIN
#define PIN_DOWNSTREAM DOWNSTREAM_PIN

#define DEFAULT_I2C_ID 127			/* Default I2C address responded to only during the SETUP state */

// Possible internal states
#define STATE_RESET          0		/* Module has just reset either due to power-on or signal from upstream */
#define STATE_SETUP          1		/* Module is waiting for its I2C address to be set.  Responds to address 127 (0x7F) */
#define STATE_STANDBY        2		/* Module is waiting to be enabled by having its pullup resistor state set.  Responds to its final I2C address */
#define STATE_ACTIVE         3		/* Module's main function is active */

// Internal node state
char State;
char InterruptPulse;
char DrivingNeighborBus;

//void Delay_ms(char delay);

#define Setup_As_Input(pin)  HAL_SharedPinInput(pin)
#define Setup_As_Output(pin) HAL_SharedPinLow(pin)
//#define Output_Low(pin) (PRT1DR &= ~pin)
//#define Output_High(pin) (PRT1DR |= pin)

#ifdef LegacyNeighborBus
#define IsActive(pin) (PRT1DR & pin)
#define IsNotActive(pin) (0 == (PRT1DR & pin))
#else
#define IsActive(pin) (0 == HAL_SharedPinRead(pin))
#define IsNotActive(pin) HAL_SharedPinRead(pin)
#endif

#define Enable_I2C_Pullups HAL_I2C_SetPullups(TRUE)
#define Disable_I2C_Pullups HAL_I2C_SetPullups(FALSE)

#define Refresh(I2C_ram) memcpy(I2C_ram, &DL_Reg, sizeof(struct DL_Regs))


//==================================================================================
//unsigned long ticks;	// counts in 100us up to 2^32 ticks ~= 4 billion ~= 400000s ~= 119 hours
WORD lastHigh;			// Value of ticks the last time PIN_UPSTREAM was high
WORD intStart;			// Value of ticks when an interrupt condition was started


// IMPORTANT: Manually added _VC3_ISR entry to boot.tpl file
// This interrupt gets called every 100uS if SysClk is 24Mhz
// and VC3 Source is VC1 and VC1 Divider is 16 and VC3 Divider is 150
// It is only accurate to +/- 2.5% when using internal oscillator
// 
//    org   18h                      ;VC3 Interrupt Vector
//    ljmp _VC3_ISR
//    reti
//#pragma interrupt_handler VC3_ISR
//void VC3_ISR(void)
//{	
//	ticks++;
//}

// The tick count is used by both DaisyLink and the standard function
// so it should never be written to except on RESET.
unsigned long GetTicks(void)
{
	return HAL_GetTicks100us();
}

// This structure is a mirror of the starting 8 bytes of the I2C mapped RAM.
// We only use 6 bytes for now but we reserve the rest of the space.
// Although the I2C mapped memory can be changed via I2C, this information
// remains unchanged and is used to keep the I2C memory mapped info intact.
struct DL_Regs DL_Reg;
//{
//	BYTE ID;           // 0 This node's DaisyLink ID
//	BYTE Config;       // 1 Configuration and status bits
//	BYTE DLVersion;    // 2 Version number of DaisyLink protocol
//	BYTE ModuleType;   // 3 Type of DaisyLink node    
//	BYTE ModuleVersion;// 4 Version number of the module
//	BYTE Manufacturer; // 5 Manufacturer of the module
//	//BYTE Reserved4;  // 6 Reserved for future use		
//	//BYTE Reserved5;  // 7 Reserved for future use		
//} DL_Reg;

// These are the bits currently defined in the Config register of the 
// DL_Regs structure above
#define DL_REGS_CONFIG_PULLUPS	    0x01	/* Set high if I2C pullups are active for this module */
#define DL_REGS_CONFIG_DISABLE      0x02	/* Must be cleared in the STANDBY state for normal module operation */
#define DL_REGS_CONFIG_INTERRUPT	0x80	/* Bit is high if this module is in an interrupt state - must be cleared by writing 0 to Config register */

#define EzI2Cs_SetAddr HAL_I2C_SetAddress

void ResetToInitState(void)
{
	State              = STATE_RESET;
	InterruptPulse     = 0;
	DrivingNeighborBus = 0;
	//ticks              = 0;

	Setup_As_Input(PIN_UPSTREAM);			
	Setup_As_Output(PIN_DOWNSTREAM);

// Legacy Neighbor bus floats low with pull-downs and must be driven high when enabled
// Current Neighbor bus floats high with pull-ups and must be driven low when enabled
//#ifdef LegacyNeighborBus
//	PRT1DR = (PIN_UPSTREAM | PIN_DOWNSTREAM);		// Legacy Neighbor bus drives high
//#else
//	PRT1DR = 0;										// Current Neighbor bus drives low
//#endif

	Disable_I2C_Pullups;
	
	DL_Reg.ID     = 0;
	DL_Reg.Config = DL_REGS_CONFIG_DISABLE;
    
    // KEEP IT HERE TO RE_initialize on every problem......etc
    HAL_I2C_Enable(DL_I2C_BAUDRATE, 0, DL_I2C_Callback);
    
	EzI2Cs_SetAddr(0);
	Enable_I2C_Pullups; //TQD Enable pull up
	

	
}

BYTE*I2CRAM;

void DL_Initialize(struct DL_Regs* reg, BYTE manufacturer, BYTE type, BYTE moduleVersion)
{
    I2CRAM = (BYTE*)reg;

	// DaisyLink
	DL_Reg.ModuleType    = type;
	DL_Reg.ModuleVersion = moduleVersion;
	DL_Reg.DLVersion     = 4;
	DL_Reg.Manufacturer  = manufacturer;
		
	// Initialize I2C module
	//EzI2Cs_SetRamBuffer(ramsize, ramsize,(BYTE *) I2CRAM);
	//EzI2Cs_Start();
	
    
	// enable interrupts
	//M8C_EnableGInt;
	
	ResetToInitState();
}


// Returns 1 if DaisyLink initialization is complete and Module
// function should be active.  Returns 0 if Module function should
// ignore I2C activity.  For some reason this compiler does not
// allow a function to return a bool.

int DL_Process()
{
	struct DL_Regs *vDL_Regs = (struct DL_Regs *)I2CRAM;		// The copy of DL_Reg "seen" by the I2C
	char active = 0;

	switch( State )
	{
	// In this state, a reset condition exists.  The I2C should be unresponsive
	// and the DaisyLink state should wait until the upstream line goes high
	// before proceeding to the SETUP state.
	// PIN_UPSTREAM is active => low
	// PIN_UPSTREAM is not active => high
	case STATE_RESET:
		if( IsNotActive(PIN_UPSTREAM) )		// If the mainboard has released the RESET condition
		{
			State = STATE_SETUP;
			DL_Reg.ID = DEFAULT_I2C_ID;
			Refresh(I2CRAM);
			Enable_I2C_Pullups;
			EzI2Cs_SetAddr(DEFAULT_I2C_ID);
			Setup_As_Output(PIN_DOWNSTREAM); // Reset the next	- TQD - should be low
		}
		
		break;
	// In this state, the Module will respond to DaisyLink registers at the I2C default address.
	// This state is maintained until either the upstream line goes low again, or the I2C
	// address is changed.
	case STATE_SETUP:
		
		if( IsActive(PIN_UPSTREAM) )				// If the module is RESET again
		{
			ResetToInitState();
			Refresh(I2CRAM);
		}
		else if( DEFAULT_I2C_ID != vDL_Regs->ID )	// If the I2C ID has been set, move to the next state
		{
			DL_Reg.ID = vDL_Regs->ID;
			State = STATE_STANDBY;
			EzI2Cs_SetAddr(DL_Reg.ID);
			Setup_As_Input(PIN_DOWNSTREAM);			// Allow the downstream module to leave the RESET state - TQD should be high
		}
		break;
	// In this state, the module will respond to DaisyLink registers at the assigned I2C address.
	// This state is maintained until either the upstream line goes low again, or the DL_REGS_CONFIG_DISABLE
	// bit is set low by the mainboard (and with it, the state of the DL_REGS_CONFIG_PULLUPS bit).
	case STATE_STANDBY:
		if( IsActive(PIN_UPSTREAM) )				// If the module is RESET again
		{
			ResetToInitState();
			Refresh(I2CRAM);
		}
		else if( 0 == (vDL_Regs->Config & DL_REGS_CONFIG_DISABLE))			// If the mainboard has finished setting up this module
		{
			DL_Reg.Config = (vDL_Regs->Config & DL_REGS_CONFIG_PULLUPS);	// Keep the new resistor setting
			if( 0 == (DL_Reg.Config & DL_REGS_CONFIG_PULLUPS) )			// If pullups are disabled (this is not the last module in the chain)
			{
				Disable_I2C_Pullups; 
			}
			Refresh(I2CRAM);
			lastHigh = (WORD)GetTicks();
			active = 1;
			State = STATE_ACTIVE;

		}
		break;
	// In this state, the module will respond to DaisyLink Registers and all module function registers
	// at the assigned I2C address.  It will remain in this state until interrupted by the module's
	// main function, a module upstream, or the mainboard (a reset).
	case STATE_ACTIVE:
		// If we are in the middle of an interrupt pulse and shouldn't be
		if( InterruptPulse && (((WORD)GetTicks() - intStart) > 10) )
		{
			InterruptPulse = 0;
		}
		// If upstream neighbor bus is being driven and it isn't by us
		if( !DrivingNeighborBus && IsActive(PIN_UPSTREAM) )
		{
			// Reset the DaisyLink
			State = STATE_RESET;
			ResetToInitState();
			Refresh(I2CRAM);

			break;
		}
		// If we should be driving the neighbor bus and aren't
		if( InterruptPulse || IsActive(PIN_DOWNSTREAM) )
		{
			if( !DrivingNeighborBus )
			{
				DrivingNeighborBus = 1;
				Setup_As_Output(PIN_UPSTREAM);
			}
		}
		// If shouldn't be driving the neighbor bus, but are
		else if( DrivingNeighborBus )
		{
			DrivingNeighborBus = 0;
			Setup_As_Input(PIN_UPSTREAM);
		}
		// If we are in an interrupt state
		if( DL_Reg.Config & DL_REGS_CONFIG_INTERRUPT )
		{
			// If the config register has been cleared
			if( !(vDL_Regs->Config & DL_REGS_CONFIG_INTERRUPT) )
			{
				DL_Reg.Config &= ~DL_REGS_CONFIG_INTERRUPT;		// Clear the interrupt
				vDL_Regs->Config = DL_Reg.Config;
			}
			vDL_Regs->ID = DL_Reg.ID;		// Refresh the ID register
			// Refresh all DaisyLink registers except for the ID and Config registers
			memcpy(&(vDL_Regs->DLVersion), &(DL_Reg.DLVersion), sizeof(struct DL_Regs)-2);
		}
		else
		{
			Refresh(I2CRAM);				// Refresh the DaisyLink I2C registers
		}
		active = 1;
		break;
	default:
		ResetToInitState();
		Refresh(I2CRAM);
		break;
	}

	return active;
}

void DL_Interrupt()
{
	struct DL_Regs *vDL_Regs = (struct DL_Regs *)I2CRAM;		// The copy of DL_Reg "seen" by the I2C

	DL_Reg.Config |= DL_REGS_CONFIG_INTERRUPT;		// Set the interrupt bit for this module
	Refresh(vDL_Regs);
	Setup_As_Output(PIN_UPSTREAM);			// Alert the mainboard to the interrupt
	DrivingNeighborBus = 1;					// Remember that we are driving the neighbor bus now
	intStart = (WORD)GetTicks();					// Remember when the interrupt was started
	InterruptPulse = 1;						// Remember that the interrupt pulse was started
}

