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

#ifndef ReadWriteEEPROM_h
#define ReadWriteEEPROM_h

#include <stdint.h>

class ReadWriteEEPROM
{
public:
  ReadWriteEEPROM();
  ReadWriteEEPROM(int address, uint16_t u16Size);

  ~ReadWriteEEPROM();

public:
  void init(int address, uint16_t u16Size);

public:
  inline int startAddress()     { return m_address; }                       //!< Return the starting address
  inline int endAddress()       { return m_address + m_u16Size;  }          //!< Return the ending address
  inline uint16_t sizeAddress() { return m_u16Size;  }                      //!< Return the size of the block

public:  
  void* getData(void *pDest);
  
  inline bool writeValue(char aaa)          { return writeValue(&aaa);  }   //!< Write character
  inline bool writeValue(unsigned char aaa) { return writeValue(&aaa);  }   //!< Write character
  inline bool writeValue(int16_t aaa)       { return writeValue(&aaa);  }   //!< write 16bit int
  inline bool writeValue(uint16_t aaa)      { return writeValue(&aaa);  }   //!< Write unsigned 16bit int
  inline bool writeValue(float aaa)         { return writeValue(&aaa);  }   //!< Write float

  bool writeValue(void *pData);

public:
  inline operator char()            { return (char)m_pu8Data[0];          } //!< Cast as char
  inline operator unsigned char()   { return (unsigned char)m_pu8Data[0]; } //!< Cast as unsigned char
  inline operator int16_t()         { return *((int16_t *)m_pu8Data);     } //!< Cast as 16bit int
  inline operator uint16_t()        { return *((uint16_t *)m_pu8Data);    } //!< Cast as unsigned 16bit int
  inline operator uint32_t()        { return *(uint32_t *)m_pu8Data;      } //!< Cast as unsigned 32bit int
  inline operator float()           { return *(float *)m_pu8Data;         } //!< Cast as float  NOTE: "nan" = not a number
  inline operator long()            { return *(long *)m_pu8Data;          } //!< Cast as long
  inline operator unsigned long()   { return *(unsigned long *)m_pu8Data; } //!< Cast as unsigned long
  
private:
  uint8_t*  m_pu8Data;   //!< Local representation of EEPROM data 
  int       m_address;   //!< EEPROM address of data
  uint16_t  m_u16Size;   //!< Size of data
};

#endif // #ifndef ReadWriteEEPROM_h

