/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireControlSlave.

    FireControlSlave is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireControlSlave is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireControlSlave.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SerialCommand.h"
#include "utils.h"


#define USE_BLUETOOTH



#if defined(USE_BLUETOOTH)
  #include <RBL_nRF8001.h>
#endif


SerialCommand::SerialCommand(HardwareSerial& s)
  : m_serial(s)
{
  m_u16BufferPosition   = 0;
  m_u8RadioIndex        = 0;
  m_bReading            = 0;
}

SerialCommand::~SerialCommand()
{
}


void SerialCommand::init()
{
}

// input format:
// #0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,
// ^-start                                       ^
//                                               |-end
uint8_t *SerialCommand::loop()
{
  char ch = 0;

#if defined(USE_BLUETOOTH)
  while( ble_available() )
#else
  while( m_serial.available() > 0 )
#endif  
  {
#if defined(USE_BLUETOOTH)
    ch = ble_read();
#else
    ch = m_serial.read();
#endif

    if (!m_bReading)
    {
      if (ch == '\r')
        continue;
  
      if (ch == '\n')
        continue;
  
      if (ch == '#')
      {
        m_u8RadioIndex = 0;
        m_u16BufferPosition = 0;
        m_bReading = true;
        Serial.println("");
        Serial.println("");
        Serial.println("NEW DATA");
      }
  
      continue;
    }      

Serial.println(ch,HEX);
    m_arrRadioBuffer[m_u8RadioIndex++] = ch;

    if ( m_u8RadioIndex >= RADIO_SIZE)
    {
      m_bReading = false;
      m_u8RadioIndex = 0;
      return m_arrRadioBuffer;
    }

  }

  return NULL;
}

// input format:
// #0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,
// ^-start                                       ^
//                                               |-end
void SerialCommand::sendData(uint8_t *pData, uint16_t u16Size)
{
#if defined(USE_BLUETOOTH)
  ble_write('#');
  delay(5);

  for(int x=0; x < u16Size; x++)
    ble_write(pData[x]);
    
  #if 0    
    int iRemaining = u16Size;
    uint8_t *pCur = pData;
    while(iRemaining > 0)
    {
      LogDebug("Remaining %d", iRemaining);
      int iEnd = (iRemaining > 16) ? 16 : iRemaining;
      for(int x=0; x < iEnd; x++)
      {
        ble_write(*pCur);
        pCur++;
      }
      
      for (int j = 0; j < 15000; j++)
        ble_do_events();
  
      iRemaining -= iEnd;
    }
  #endif

#else
  LogSerialResp("#");
  Serial.write(pData, u16Size);
#endif
  
 
}

