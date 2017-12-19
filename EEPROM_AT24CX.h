/**************************************************************************//**
 * \brief EEPROM 24C32 / 24C64 library for Arduino
 * \author Copyright (C) 2012  Julien Le Sech - www.idreammicro.com
 * \version 1.0
 * \date 20120203
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
 * \headerfile EEPROM_AT24CX.h
 ******************************************************************************/

#ifndef EEPROM_AT24CX_h
#define EEPROM_AT24CX_h

/******************************************************************************
 * Header file inclusion.
 ******************************************************************************/

#include <Arduino.h>

/**************************************************************************//**
 * \class EEPROM_AT24CX
 *
 * \brief EEPROM 24C32 / 24C64 memory driver.
 *
 * This driver is designed for 24C32 and 24C64 EEPROM memories.
 ******************************************************************************/
class EEPROM_AT24CX
{
    public:

        /******************************************************************//**
         * \fn EEPROM_AT24CX(uint8_t deviceAddress)
         *
         * \brief Constructor.
         *
         * \param   deviceAddress   EEPROM address on TWI bus.
         **********************************************************************/
        EEPROM_AT24CX(uint8_t deviceAddress);

        /******************************************************************//**
         * \fn void initialize()
         *
         * \brief Initialize library abnd TWI bus.
         * 
         * If several devices are connected to TWI bus, this method mustn't be
         * called. TWI bus must be initialized out of this library using
         * Wire.begin() method.
         **********************************************************************/        
        void initialize();

		/**************************************************************************//**
		 * \fn uint8_t read_writeBytes(
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
		 
		uint8_t read_writeBytes(word address, word length, uint8_t *buff, uint8_t write);
            
    private:

        uint8_t m_deviceAddress;

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
		uint16_t read_writePage(word address, uint8_t length, uint8_t *buff, uint8_t write);

		/**************************************************************************//**
		 * \fn 		uint8_t read_writeBuffer(
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
		 uint8_t read_writeBuffer(word address, uint8_t length, uint8_t *buff, uint8_t write);
};


#endif // EEPROM_AT24CX_h

