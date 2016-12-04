/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireControl.

    FireControl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireControl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireControl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RadioComm_h
#define RadioComm_h

#include <Arduino.h>

#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>

#include <SPI.h>

#define RADIO_FRAME_SIZE    32

class RadioComm
{
public:
  RadioComm(int ipinCS, int ipinCSN, uint64_t u64ReadPipe, uint64_t u64WritePipe);
  virtual ~RadioComm();

public:
  void init();
  uint8_t* loop();
  bool sendMessage(uint8_t *pData, uint8_t u8Size);
  
private:
  RF24      m_radio;
  uint64_t  m_u64ReadPipe;
  uint64_t  m_u64WritePipe;
  bool      m_bStarted;
};

#endif // #ifndef RadioComm_h


