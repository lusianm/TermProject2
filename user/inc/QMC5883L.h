//QMC5883L I2C library for ARM STM32F103xx Microcontrollers - Main header file 
//Has bit, byte and buffer I2C R/W functions
// 24/05/2012 by Harinadha Reddy Chintalapalli <harinath.ec@gmail.com>
// Changelog:
//     2012-05-24 - initial release. Thanks to Jeff Rowberg <jeff@rowberg.net> for his AVR/Arduino
//                  based development which inspired me & taken as reference to develop this.
/* ============================================================================================
 QMC5883L device I2C library code for ARM STM32F103xx is placed under the MIT license
 Copyright (c) 2012 Harinadha Reddy Chintalapalli

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ================================================================================================
 */

#ifndef _QMC5883L_H_
#define _QMC5883L_H_

#ifdef __cplusplus
extern "C" {
#endif 

/* Includes */
#include "HAL_QMC5883L.h" 

#define QMC5883L_ADDRESS            0x0D // this device only has one address
#define QMC5883L_DEFAULT_ADDRESS    (QMC5883L_ADDRESS<<1)

#define QMC5883L_RA_DATAX_L         0x00
#define QMC5883L_RA_DATAX_H         0x01
#define QMC5883L_RA_DATAY_L         0x02
#define QMC5883L_RA_DATAY_H         0x03
#define QMC5883L_RA_DATAZ_L         0x04
#define QMC5883L_RA_DATAZ_H         0x05
#define QMC5883L_RA_STATUS          0x06
#define QMC5883L_RA_Temper_L        0x07
#define QMC5883L_RA_Temper_H        0x08
#define QMC5883L_RA_CONFIG_A        0x09
#define QMC5883L_RA_CONFIG_B        0x0A
#define QMC5883L_RA_SET_PERIOD      0x0B

//Status
#define QMC5883L_STATUS_DOR			0x04
#define QMC5883L_STATUS_OVL			0X02
#define QMC5883L_STATUS_DRDY		0X01


//Control1
#define QMC5883L_MODE_STANDBY       0x00  // POWER ON RESET
#define QMC5883L_MODE_CONTINUOUS    0x01

#define QMC5883L_ODR_RATE_10		0x00  //Clock Rate
#define QMC5883L_ODR_RATE_50		0x04
#define QMC5883L_ODR_RATE_100		0x08
#define QMC5883L_ODR_RATE_200		0x0C

#define QMC5883L_RNG_2G				0x00  //자기장 범위
#define QMC5883L_RNG_8G				0x10

#define QMC5883L_OSR_512			0x00 // Over Sample Ration
#define QMC5883L_OSR_256			0x40
#define QMC5883L_OSR_128			0x80
#define QMC5883L_OSR_64				0xC0

//Control2
#define QMC5883L_SOFT_RST			0x80 // Over Sample Ration
#define QMC5883L_ROL_PNT			0x40
#define QMC5883L_INT_ENB			0x01

//SET_PERIOD
#define QMC5883L_SET_PERIOD_INIT	0x01


void QMC5883L_Initialize();

// CONFIG_A register
uint8_t QMC5883L_GetSampleAveraging();
void QMC5883L_SetSampleAveraging(uint8_t averaging);
uint8_t QMC5883L_GetDataRate();
void QMC5883L_SetDataRate(uint8_t rate);
uint8_t QMC5883L_GetMeasurementBias();
void QMC5883L_SetMeasurementBias(uint8_t bias);

// CONFIG_B register
uint8_t QMC5883L_GetGain();
void QMC5883L_SetGain(uint8_t gain);

// MODE register
uint8_t QMC5883L_GetMode();
void QMC5883L_SetMode(uint8_t mode);

// DATA* registers
double QMC5883L_GetHeading();
// STATUS register

void QMC5883L_WriteBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
void QMC5883L_WriteBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
void QMC5883L_ReadBits(uint8_t slaveAddr, uint8_t regAddr , uint8_t bitStart, uint8_t length, uint8_t *data);
//void QMC5883L_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
void QMC5883L_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t* data, uint8_t length);

void QMC5883L_I2C_Init();
void QMC5883L_I2C_ByteWrite(u8 slaveAddr, u8* pBuffer, u8 WriteAddr);

void QMC5883L_I2C_BufferRead(u8 slaveAddr,u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
int QMC5883L_Ready();
#ifdef __cplusplus
}
#endif

#endif /* _QMC5883L_H_ */

/******************* (C) COPYRIGHT 2012 Harinadha Reddy Chintalapalli *****END OF FILE****/
