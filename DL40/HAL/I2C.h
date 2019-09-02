/****************************************************************************
 *   $Id:: i2cslave.h 3635 2010-06-02 00:31:46Z usb00423                    $
 *   Project: NXP LPC11xx I2C Slave example
 *
 *   Description:
 *     This file contains I2C slave code header definition.
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
#ifndef __I2CSLAVE_H 
#define __I2CSLAVE_H

#define FAST_MODE_PLUS      1
 
#define BUFSIZE             6
#define MAX_TIMEOUT         0x00FFFFFF

//#define PCF8594_ADDR        0xA0
#define READ_WRITE          0x01

#define RD_BIT              0x01

#define I2C_IDLE            0
#define I2C_STARTED         1
#define I2C_RESTARTED       2
#define I2C_REPEATED_START  3
#define DATA_ACK            4
#define DATA_NACK           5
#define I2C_WR_STARTED      6
#define I2C_RD_STARTED      7

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C           0x00000000  /* I2C Data Reg */
#define I2ADR_I2C           0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH         0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH      0x00000020  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL      0x00000020  /* Fast Plus I2C SCL Duty Cycle Low Reg */


void I2C_IRQHandler( void );
void I2CSlaveInit( UINT8 address );
void DrvI2C_WriteData(uint8_t u8data);
uint8_t DrvI2C_ReadData(void);
int32_t DrvI2C_SetAddress(uint8_t slaveNo, uint8_t slave_addr, uint8_t GC_Flag);
void DrvI2C_Ctrl(uint8_t start, uint8_t stop, uint8_t intFlag, uint8_t ack);

#define E_DRVI2C_ERR_ARGUMENT   127

#endif /* end __I2CSLAVE_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
