/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SerialCommand.h"
#include "utils.h"
#include "qthread.h"

SerialCommand::SerialCommand(HardwareSerial& s)
  : m_serial(s)
{
  m_u16BufferPosition   = 0;
  m_u8RadioIndex        = 0;
  m_bReading            = false;
}

SerialCommand::~SerialCommand()
{
}


void SerialCommand::init(QTextEdit *pedit)
{
    m_pedit = pedit;
}

// input format:
// #0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,
// ^-start                                       ^
//                                               |-end
uint8_t *SerialCommand::loop()
{
  char ch = 0;

  while( m_serial.available() > 0)
  {
    ch = m_serial.read();
    if (!m_bReading)
    {
        if (ch == '\r')
          continue;

        if (ch == '#')
        {
          m_u8RadioIndex = 0;
          m_u16BufferPosition = 0;
          m_bReading = true;
          continue;
        }

        char s[2];
        s[0]=ch; s[1]=0;
        //m_pedit->append(s);
        m_pedit->moveCursor (QTextCursor::End);
        m_pedit->insertPlainText (s);
        m_pedit->moveCursor (QTextCursor::End);

        if (ch == '\n')
          continue;

        continue;
    }

/*    char s[25];
    if (m_u8RadioIndex +1 >= RADIO_SIZE)
        sprintf(s,"%02x\r\n", ch & 0xff);
    else
        sprintf(s,"%02x, ", ch & 0xff);

    m_pedit->moveCursor (QTextCursor::End);
    m_pedit->insertPlainText (s);
    m_pedit->moveCursor (QTextCursor::End);
*/
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
  m_serial.write("#");
  m_serial.write(pData, u16Size);
/*  for(int x=0; x < u16Size; x++)
  {
//      LogDebug("%02d   %d,", x, pData[x]);
//      LogSerialResp("%d,", pData[x]);
      QThread::msleep(17);
  }
*/
}
