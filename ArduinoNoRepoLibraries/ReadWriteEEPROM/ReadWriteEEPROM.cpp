/*
    Copyright 2016 Chris Arrant
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ReadWriteEEPROM.h"

#include <stdlib.h>
#include <string.h>
#include <EEPROM.h> // must include this in the main project file otherwise compile error!


//------------------------------------------------------------------------------
//  constructor
//------------------------------------------------------------------------------
ReadWriteEEPROM::ReadWriteEEPROM()
{
  m_pu8Data = NULL;
  m_u16Size = 0;
  m_address = 0;
}

//------------------------------------------------------------------------------
//  constructor
//! 
//! @param [IN] address  EEPROM address
//! @param [IN] u16Size  Data size
//------------------------------------------------------------------------------
ReadWriteEEPROM::ReadWriteEEPROM(int address, uint16_t u16Size)
{
  m_pu8Data = NULL;
  
  init(address, u16Size);
}

//------------------------------------------------------------------------------
//  destructor
//------------------------------------------------------------------------------
ReadWriteEEPROM::~ReadWriteEEPROM()
{
  if (m_pu8Data)
    free(m_pu8Data);
   
}

//------------------------------------------------------------------------------
//! Initialize the EEPROM address and local representation
//!
//! @param [IN] address  EEPROM address
//! @param [IN] u16Size  Data size
//------------------------------------------------------------------------------
void ReadWriteEEPROM::init(int address, uint16_t u16Size)
{
  if (m_pu8Data)
    free(m_pu8Data);

  m_address = address;
  m_u16Size = u16Size;
  m_pu8Data = (uint8_t *)malloc(u16Size);

  for(uint16_t x=0; x < m_u16Size; x++)
    m_pu8Data[x]  = EEPROM.read(m_address + x);
}

//------------------------------------------------------------------------------
//! Retrieve data from EEPROM
//!
//! @param [OUT] pDest  Where to store data
//!
//! @return pDest
//------------------------------------------------------------------------------
void* ReadWriteEEPROM::getData(void *pDest)
{
  if (!m_pu8Data)
    return NULL;
    
  memcpy(pDest, m_pu8Data, m_u16Size);
  return pDest;
}

//------------------------------------------------------------------------------
//! Write data to EEPROM
//!
//! @param [IN] pDest  Data to store
//!
//! @return TRUE  Data written successfully
//! @return FALSE Error writting data
//------------------------------------------------------------------------------
bool ReadWriteEEPROM::writeValue(void *pData)
{
  uint8_t *pD = (uint8_t *)pData;
  if (memcmp(pData, m_pu8Data, m_u16Size) != 0)
  {
    for(uint16_t x=0; x < m_u16Size; x++)
    {
      EEPROM.write(m_address + x, pD[x]);
      m_pu8Data[x] = pD[x];
    }
    return true;
  }

  return false;
}

