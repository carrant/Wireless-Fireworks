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

#ifndef SerialCommand_h
#define SerialCommand_h

#include <stdio.h>
#include <stdint.h>

#include <HardwareSerial.h>

#include "qtextedit.h"


#define RADIO_SIZE            32
#define BUFFER_SIZE           128
class SerialCommand
{
public:
  SerialCommand(HardwareSerial& s);
  virtual ~SerialCommand();

public:
  void init(QTextEdit *pedit);
  uint8_t* loop();

public:
  void sendData(uint8_t *pData, uint16_t u16Size);

private:
  uint8_t m_arrBuffer[128 + 1];
  uint8_t m_u16BufferPosition;
  uint8_t m_u8RadioIndex;
  uint8_t m_arrRadioBuffer[RADIO_SIZE];

  bool m_bReading;
  HardwareSerial& m_serial;

  QTextEdit *m_pedit;
};


#endif // #ifndef SerialCommand_h

