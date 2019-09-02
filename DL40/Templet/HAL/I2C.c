/****************************************************************************
 *   $Id:: i2cslave.c 3635 2010-06-02 00:31:46Z usb00423                    $
 *   Project: NXP LPC11xx I2C Slave example
 *
 *   Description:
 *     This file contains I2C slave code example which include I2C slave 
 *     initialization, I2C slave interrupt handler, and APIs for I2C slave
 *     access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "type.h"
#include "i2c.h"
#include "DaisyLink.h"
#include "HAL.h"

//volatile uint32_t I2CMasterState = I2C_IDLE;
//volatile uint32_t I2CSlaveState = I2C_IDLE;
//
//volatile uint32_t I2CMode;

//volatile uint8_t I2CWrBuffer[BUFSIZE];
//volatile uint8_t I2CRegBuffer[BUFSIZE];
//volatile uint32_t I2CReadLength;
//volatile uint32_t I2CWriteLength;
//
//volatile uint32_t RdIndex = 0;
//volatile uint32_t WrIndex = 0;
//uint8_t rx_data =0;
//uint8_t tx_data =0;


/* 
From device to device, the I2C communication protocol may vary, 
in the example below, the protocol uses repeated start to read data from or 
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could 
be READ or WRITE depending on the I2C command.
*/   

/*****************************************************************************
** Function name:		I2C_IRQHandler
**
** Descriptions:		I2C interrupt handler, deal with master mode only.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
extern UINT8 reg ;
extern UINT32 offset ;
UINT8 rx_data;

void I2C_IRQHandler(void) 
{
  uint8_t StatValue;
  UINT8 c;

  
  /* this handler deals with master read and master write only */
  StatValue = LPC_I2C->STAT;
  //DL_I2C_Callback (StatValue);
  		   
  switch ( StatValue )
  {
	// For Receive
	case 0x60:
		reg = DL_I2C_INVALID_REG;					
		LPC_I2C->CONSET = 0x04;
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0x68:
		reg = DL_I2C_INVALID_REG;
		LPC_I2C->CONSET = 0x24;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0x70:
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0x78:

		LPC_I2C->CONSET = 0x24;	
		LPC_I2C->CONCLR = 0x08;
		break;
	
	case 0x80:					//Data has been received and ACK has been returned.
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
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		//LPC_I2C->CONCLR = 0x0C;
		break;

	case 0x88:	//Data has been received and NOT ACK has been returned.
		c = HAL_I2C_Read();

		if(reg != DL_I2C_INVALID_REG)
		{
			DL_Platform_WriteRegister(reg, offset, c);
			offset++;
		}
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0x90:	 // It also never go here
		LPC_I2C->CONCLR = 0x0C;
		break;

	case 0x98:
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	// For Transmit	
	case 0xA8: 					// SLA+R has been received and ACK has been returned					
		//LPC_I2C->DAT = 1;
		if(reg != DL_I2C_INVALID_REG)
		{
			HAL_I2C_Write(DL_Platform_ReadRegister(reg, offset));
			offset++;
		}
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0xB0:			// It never go this state
		//LPC_I2C->DAT = 2;
		if(reg != DL_I2C_INVALID_REG)
		{
			HAL_I2C_Write(DL_Platform_ReadRegister(reg, offset));
			offset++;
		}
		LPC_I2C->CONSET = 0x24;	
		LPC_I2C->CONCLR = 0x08;
		break;
	
	case 0xB8:       // DATA has been transmitted and ACK has been received				
		//LPC_I2C->DAT = 3;
		if(reg != DL_I2C_INVALID_REG)
		{
			HAL_I2C_Write(DL_Platform_ReadRegister(reg, offset));
			offset++;
		}
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0xC0:	//Data has been transmitted, NOT ACK has been received				
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0xC8:
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;

	case 0xA0:	//A STOP condition or Repeated START has been received				
		LPC_I2C->CONSET = 0x04;	
		LPC_I2C->CONCLR = 0x08;
		break;
	// Non Specific state
	case 0x00:
		LPC_I2C->CONSET = 0x14;
		LPC_I2C->CONCLR = 0x08;
		break;

	default:
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;	
		break;
  }

  return;
}

/*****************************************************************************
** Function name:		I2CSlaveInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
** 
*****************************************************************************/
void I2CSlaveInit( UINT8 address ) 
{
  /* SSP and I2C reset are overlapped, a known bug, 
  for now, both SSP and I2C use bit 0 for reset enable. 
  Once the problem is fixed, change to "#if 1". */
#if 1
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);
#else
  LPC_SYSCON->PRESETCTRL |= (0x1<<0);
#endif
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;	/*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;		/* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;	
  LPC_IOCON->PIO0_5 |= 0x01;		/* I2C SDA */

  /*--- Clear flags ---*/
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

  /*--- Reset registers ---*/
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x1<<9);
  LPC_IOCON->PIO0_5 |= (0x1<<9);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
#endif
 
 if (address==0)
 {
 	LPC_I2C->ADR0 = ((address<<1)); 
 }
 else
   {
  LPC_I2C->ADR0 = ((address<<1)+1);    
  }
//  I2CSlaveState = I2C_IDLE;
  
  /* Enable the I2C Interrupt */
  NVIC_EnableIRQ(I2C_IRQn);

  //LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;
  LPC_I2C->CONSET = 0x44;
  return;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_WriteData				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8data 	- [in]		Byte Data	       		                           				   */
/* Returns:                                                                                                */
/*               None											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Write Data into I2C Data register										           		   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvI2C_WriteData(uint8_t u8data)
{
	//tx_data = u8data;
	LPC_I2C->DAT = u8data;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_ReadData				                                                   			   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None	       		                               										   */
/* Returns:                                                                                                */
/*               Data from I2C Data register											       		   	   */
/*                                                                                      			       */
/* Description:                                                                                            */
/*               Read Data from I2C Data register										               	   */		
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DrvI2C_ReadData(void)
{
	return LPC_I2C->DAT ;
	//return rx_data;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvI2C_SetAddress				                                                   	   		   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               slaveNo 	   - [in]		There are 4 slave addrss. The slaveNo is 0 ~ 3.        		   */
/*               slave_addr    - [in]		7-bit data	       		                           			   */
/*               GC_Flag 	   - [in]		1:Enable / 0:Disable	       		                           */
/* Returns:                                                                                                */
/*               0 : Success 											                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               Set 4 7-bit slave addresses and enable General Call function		        			   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvI2C_SetAddress(uint8_t slaveNo, uint8_t slave_addr, uint8_t GC_Flag)
{
	switch (slaveNo)
	{
		case 0:
			LPC_I2C->ADR0 =  slave_addr;
			
			break;
		default:
			return E_DRVI2C_ERR_ARGUMENT;
	}	
	return 0;
}
void DrvI2C_Ctrl(uint8_t start, uint8_t stop, uint8_t intFlag, uint8_t ack)
{
//	if (start)
//		Reg |= I2C_STA;
//	if (stop)
//	    Reg |= I2C_STO;
	if (intFlag)
	{
		LPC_I2C->CONSET &= ~I2CONSET_SI ;  // Clear
		LPC_I2C->CONSET |=I2CONSET_SI;	  // Set

	}
	if (ack)
	{
		LPC_I2C->CONSET &= ~I2CONSET_AA ;   // Clear
		LPC_I2C->CONSET |=I2CONSET_AA;	  // Set
	}
}
/******************************************************************************
**                            End Of File
******************************************************************************/

