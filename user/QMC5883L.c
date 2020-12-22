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
#include "QMC5883L.h"
#include "stm32f10x_i2c.h"
#include "math.h"

uint8_t QMC5883Lmode;
int moduleReady=0;

/** @defgroup QMC5883L_Library
 * @{
 */

/** Power on and prepare for general usage.
 * This will prepare the magnetometer with default settings, ready for single-
 * use mode (very low power requirements). Default settings include 8-sample
 * averaging, 15 Hz data output rate, normal measurement bias, a,d 1090 gain (in
 * terms of LSB/Gauss). Be sure to adjust any settings you need specifically
 * after initialization, especially the gain settings if you happen to be seeing
 * a lot of -4096 values (see the datasheet for mor information).
 */

int QMC5883L_Ready(){
     return moduleReady;
}   
void QMC5883L_Initialize()
{       
    moduleReady = 0;
    QMC5883L_I2C_Init();
    // write CONFIG_A register

    uint8_t tmp = QMC5883L_SET_PERIOD_INIT;
    QMC5883L_I2C_ByteWrite(QMC5883L_DEFAULT_ADDRESS, &tmp, QMC5883L_RA_SET_PERIOD);

    //Mode : Continuous, 50Hz, RNG_2G, OSR_512
    tmp = QMC5883L_MODE_CONTINUOUS | QMC5883L_ODR_RATE_50 | QMC5883L_RNG_2G | QMC5883L_OSR_512;
    QMC5883L_I2C_ByteWrite(QMC5883L_DEFAULT_ADDRESS, &tmp, QMC5883L_RA_CONFIG_A);
    moduleReady = 1;
}
/*
void QMC5883L_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data)
{
    uint8_t tmp;
    QMC5883L_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
    *data = tmp & (1 << bitNum);
}
*/
void QMC5883L_ReadBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t* data, uint8_t length)
{
    uint8_t tmp[8] = { 0 };
    QMC5883L_I2C_BufferRead(slaveAddr, tmp, regAddr, length);
    int i;
    for(i = 0; i < length; i++)
        data[i] = tmp[i];
}

void QMC5883L_GetDATA(s16* dataX, s16* dataY, s16* dataZ)
{
    uint8_t tmp = 0x00;
    QMC5883L_I2C_ByteWrite(QMC5883L_DEFAULT_ADDRESS, &tmp, 0x00);
    uint8_t data[8];
    QMC5883L_ReadBit(QMC5883L_DEFAULT_ADDRESS, QMC5883L_RA_DATAX_L, 0, data, 8);
    //*dataX = data[0] | (data[1] << 8);
    *dataX = (s16)(data[0] + ((u16)data[1] << 8));
    *dataY = (s16)(data[2] + ((u16)data[3] << 8));
    *dataZ = (s16)(data[4] + ((u16)data[5] << 8));
}

double QMC5883L_GetHeading()
{
    short dataX, dataY;
    uint8_t tmp = 0x00;
    QMC5883L_I2C_ByteWrite(QMC5883L_DEFAULT_ADDRESS, &tmp, 0x00);
    uint8_t data[8];
    QMC5883L_ReadBit(QMC5883L_DEFAULT_ADDRESS, QMC5883L_RA_DATAX_L, 0, data, 8);
    //*dataX = data[0] | (data[1] << 8);
    dataX = (s16)(data[0] + ((u16)data[1] << 8));
    dataY = (s16)(data[2] + ((u16)data[3] << 8));
    //dataZ = (s16)(data[4] + ((u16)data[5] << 8));
    
    return (atan2(dataY, dataX) * 180.0 / 3.14) + 180.0;  
}


/*
void HMC5883L_GetHeading(s16* Mag)
{
    uint8_t tmpbuff[6] = { 0 };
    HMC5883L_I2C_BufferRead(HMC5883L_DEFAULT_ADDRESS, tmpbuff, HMC5883L_RA_DATAX_H, 6);

    uint8_t tmp = HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1);

    if (HMC5883Lmode == HMC5883L_MODE_SINGLE)
        HMC5883L_I2C_ByteWrite(HMC5883L_DEFAULT_ADDRESS, &tmp, HMC5883L_RA_MODE);
    for (int i = 0; i < 3; i++)
        Mag[i] = ((s16) ((u16) tmpbuff[2 * i] << 8) + tmpbuff[2 * i + 1]);
}
*/

void QMC5883L_WriteBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    uint8_t tmp;
    QMC5883L_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    tmp &= ~(mask); // zero all important bits in existing byte
    tmp |= data; // combine data with existing byte
    QMC5883L_I2C_ByteWrite(slaveAddr, &tmp, regAddr);
}

/** write a single bit in an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 */
void QMC5883L_WriteBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    QMC5883L_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    QMC5883L_I2C_ByteWrite(slaveAddr, &tmp, regAddr);
}

/** Read multiple bits from an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */
void QMC5883L_ReadBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    uint8_t tmp;
    QMC5883L_I2C_BufferRead(slaveAddr, &tmp, regAddr, 1);
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    tmp &= mask;
    tmp >>= (bitStart - length + 1);
    *data = tmp;
}

/** Read a single bit from an 8-bit device register.
 * @param slaveAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in readTimeout)
 */

/**
 * @brief  Initializes the I2C peripheral used to drive the QMC5883L
 * @param  None
 * @retval None
 */
void QMC5883L_I2C_Init()
{
    I2C_InitTypeDef I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable I2C and GPIO clocks */
    RCC_APB1PeriphClockCmd(QMC5883L_I2C_RCC_Periph, ENABLE);
    RCC_APB2PeriphClockCmd(QMC5883L_I2C_RCC_Port, ENABLE);

    /* Configure I2C pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin = QMC5883L_I2C_SCL_Pin | QMC5883L_I2C_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(QMC5883L_I2C_Port, &GPIO_InitStructure);

    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = QMC5883L_DEFAULT_ADDRESS; // QMC5883L 7-bit adress = 0x1E;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = QMC5883L_I2C_Speed;

    /* Apply I2C configuration after enabling it */
    I2C_Init(QMC5883L_I2C, &I2C_InitStructure);

    I2C_Cmd(QMC5883L_I2C, ENABLE);
}

/**
 * @brief  Writes one byte to the  QMC5883L.
 * @param  slaveAddr : slave address QMC5883L_DEFAULT_ADDRESS
 * @param  pBuffer : pointer to the buffer  containing the data to be written to the QMC5883L.
 * @param  WriteAddr : address of the register in which the data will be written
 * @retval None
 */
void QMC5883L_I2C_ByteWrite(u8 slaveAddr, u8* pBuffer, u8 WriteAddr)
{
    // ENTR_CRT_SECTION();

    /* Send START condition */
    I2C_GenerateSTART(QMC5883L_I2C, ENABLE);

    /* Test on EV5 and clear it */
    
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    
    /* Send HMC5883 address for write */
    I2C_Send7bitAddress(QMC5883L_I2C, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* Send the QMC5883L internal address to write to */
    I2C_SendData(QMC5883L_I2C, WriteAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    /* Send the byte to be written */
    I2C_SendData(QMC5883L_I2C, *pBuffer);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    /* Send STOP condition */
    I2C_GenerateSTOP(QMC5883L_I2C, ENABLE);
    // EXT_CRT_SECTION();
}

/**
 * @brief  Reads a block of data from the QMC5883L.
 * @param  slaveAddr  : slave address QMC5883L_DEFAULT_ADDRESS
 * @param  pBuffer : pointer to the buffer that receives the data read from the QMC5883L.
 * @param  ReadAddr : QMC5883L's internal address to read from.
 * @param  NumByteToRead : number of bytes to read from the QMC5883L ( NumByteToRead >1  only for the Magnetometer reading).
 * @retval None
 */
void QMC5883L_I2C_BufferRead(u8 slaveAddr, u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
    // ENTR_CRT_SECTION();

    /* While the bus is busy */
    while (I2C_GetFlagStatus(QMC5883L_I2C, I2C_FLAG_BUSY));
    
    /* Send START condition */
    I2C_GenerateSTART(QMC5883L_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    
    /* Send QMC5883L_Magn address for write */ // Send QMC5883L address for write
    I2C_Send7bitAddress(QMC5883L_I2C, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(QMC5883L_I2C, ENABLE);

    /* Send the QMC5883L's internal address to write to */
    I2C_SendData(QMC5883L_I2C, ReadAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(QMC5883L_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    
    /* Send QMC5883L address for read */
    I2C_Send7bitAddress(QMC5883L_I2C, slaveAddr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    /* While there is data to be read */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(QMC5883L_I2C, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(QMC5883L_I2C, ENABLE);
        }

        /* Test on EV7 and clear it */
        if (I2C_CheckEvent(QMC5883L_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the QMC5883L */
            *pBuffer = I2C_ReceiveData(QMC5883L_I2C);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(QMC5883L_I2C, ENABLE);
    // EXT_CRT_SECTION();
}

/**
 * @}
 *//* end of group QMC5883L_Library */
