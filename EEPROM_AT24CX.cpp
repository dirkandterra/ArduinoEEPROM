/**************************************************************************//**
 * \brief EEPROM 24C32 / 24C64 library for Arduino
 * \author Copyright (C) 2012  Julien Le Sech - www.idreammicro.com
 * \version 1.0
 * \date 20120218
 *
 * This file is part of the EEPROM 24C32 / 24C64 library for Arduino.
 *
 * This library is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 ******************************************************************************/

/**************************************************************************//**
 * \file EEPROM_AT24CX.cpp
 ******************************************************************************/

/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

#include <EEPROM_AT24CX.h>

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \def EEPROM__PAGE_SIZE
 * \brief Size of a page in EEPROM memory.
 * This size is given by EEPROM memory datasheet.
 ******************************************************************************/
#define EEPROM__PAGE_SIZE         32

/**************************************************************************//**
 * \def EEPROM__RD_BUFFER_SIZE
 * \brief Size of input TWI buffer.
 * This size is equal to BUFFER_LENGTH defined in Wire library (32 bytes).
 ******************************************************************************/
#define EEPROM__RD_BUFFER_SIZE    BUFFER_LENGTH

/**************************************************************************//**
 * \def EEPROM__WR_BUFFER_SIZE
 * \brief Size of output TWI buffer.
 * This size is equal to BUFFER_LENGTH - 2 bytes reserved for address.
 ******************************************************************************/
#define EEPROM__WR_BUFFER_SIZE    (BUFFER_LENGTH - 2)

/******************************************************************************
 * Public method definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn EEPROM_AT24CX::EEPROM_AT24CX(uint8_t deviceAddress)
 *
 * \brief Constructor.
 *
 * \param   deviceAddress   EEPROM address on TWI bus.
 ******************************************************************************/
EEPROM_AT24CX::EEPROM_AT24CX(uint8_t deviceAddress){
    m_deviceAddress = deviceAddress;
}

/**************************************************************************//**
 * \fn void EEPROM_AT24CX::initialize()
 *
 * \brief Initialize library and TWI bus.
 *
 * If several devices are connected to TWI bus, this method mustn't be
 * called. TWI bus must be initialized out of this library using
 * Wire.begin() method.
 ******************************************************************************/
void EEPROM_AT24CX::initialize()
{
    Wire.begin();
}

/**************************************************************************//**
 * \fn uint8_t EEPROM_AT24CX::read_writeBytes(
 * word     address,
 * word     length,
 * uint8_t*    buff,
 * uint8_t		write)
 * 
 * \brief Write bytes in EEPROM memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes to write.
 * \param[in/out]   buff  Data.
 * \param		write	set high if write, low if read
 ********************************************************************************/
 
uint8_t EEPROM_AT24CX::read_writeBytes(word address, word length, uint8_t *buff, uint8_t write){
	uint16_t bytesRead_Wrote=0;
	uint16_t offset=0;
	while (length){
		bytesRead_Wrote=read_writePage(address+offset,length,buff,write);
		if(!bytesRead_Wrote){
			return 0;	//read_write failed
		}
		offset+=bytesRead_Wrote;
		length-=bytesRead_Wrote;
		buff+=bytesRead_Wrote;	//Increment data Pointer
	}
	return 1;
}



/******************************************************************************
 * Private method definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn void EEPROM_AT24CX::read_writePage(
 * word     address,
 * uint8_t     length,
 * uint8_t*    buff,
 * uint8_t		write)
 *
 * \brief Write page in EEPROM memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes (EEPROM__PAGE_SIZE bytes max).
 * \param[in/out]   buff  Data.
 * \param		write	set high if write, low if read
 ******************************************************************************/
uint16_t EEPROM_AT24CX::read_writePage(word address, uint8_t length, uint8_t *buff, uint8_t write){
    // Write complete buffers.
	const unsigned char retrySP=15;
	unsigned char result=0;
	unsigned char retry=retrySP;
	uint16_t bytesRemainingInPage=0;
	
	bytesRemainingInPage=EEPROM__PAGE_SIZE-(address%EEPROM__PAGE_SIZE);
	//is the requested read length greater than bytes left to read in current page?
	if(length>bytesRemainingInPage){
		length=bytesRemainingInPage;		//length is now the remaining bytes to read in this page
	}
	
	result=0;
	while (!result&&retry){		//may be busy from a previous operation, try a few times
		retry--;				
		result=read_writeBuffer(address, length, buff, write); //uint8_t writeTime, uint8_t *buffer, int length, uint8_t noRestart);
	}
 
	return result;
}

/**************************************************************************//**
 * \fn void EEPROM_AT24CX::read_writeBuffer(
 * word     address,
 * uint8_t     length,
 * uint8_t*    buff,
 * uint8_t		write)
 *
 * \brief Writes or reads bytes into/from memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes (EEPROM__WR_BUFFER_SIZE bytes max).
 * \param[in/out]   p_data  Data.
 * \param		write	set high if write, low if read
 ******************************************************************************/
uint8_t EEPROM_AT24CX::read_writeBuffer(word address, uint8_t length, uint8_t *buff, uint8_t write){
    Wire.beginTransmission(m_deviceAddress);
    Wire.write(address >> 8);
    Wire.write(address & 0xFF);
	int result=0;
	if(!write){
		result=Wire.endTransmission();
		Wire.requestFrom(m_deviceAddress, length);
	}
    for (uint8_t i = 0; i < length; i++)
    {
		//write buffer is 2 bytes smaller than read because of address, handle it!
		if(write){
			if(i==EEPROM__WR_BUFFER_SIZE){
				result=Wire.endTransmission();
				delay(10);
				address+=EEPROM__WR_BUFFER_SIZE;		//load new address
				Wire.beginTransmission(m_deviceAddress);
				Wire.write(address >> 8);
				Wire.write(address & 0xFF);
				if(result)
					return 0;
			}
			Wire.write(buff[i]);
		}
		else{
			if (Wire.available())
				buff[i] = Wire.read();
		}
    }
	
	if(write)
		result=Wire.endTransmission();
	
    // Write cycle time (tWR). See EEPROM memory datasheet for more details.
    delay(10);
	if(result){
		return 0;
	}
	return length;

}
