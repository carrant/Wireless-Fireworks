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

#include "RadioComm.h"

RadioComm::RadioComm(int ipinCS, int ipinCSN, uint64_t u64ReadPipe, uint64_t u64WritePipe)
  : m_radio(ipinCS, ipinCSN)
{
  m_u64ReadPipe  = u64ReadPipe;
  m_u64WritePipe = u64WritePipe;
  m_bStarted     = false;
}

RadioComm::~RadioComm()
{

}

void RadioComm::init()
{
  LogDebug("Starting radio");
  m_bStarted = m_radio.begin();
  if (m_bStarted)
  {
    LogDebug("SUCCESS starting radio");
    m_radio.openWritingPipe(m_u64WritePipe);    // note that our pipes are the same above, but that
    m_radio.openReadingPipe(1, m_u64ReadPipe); // they are flipped between rx and tx sides.
    m_radio.startListening();
    m_radio.setDataRate( RF24_250KBPS );
    m_radio.setPALevel(RF24_PA_LOW);
    m_radio.printDetails();
  }
  else
    LogDebug("ERROR starting radio");
}

uint8_t* RadioComm::loop()
{
  if (m_bStarted && m_radio.available()) 
  {
    //LogDebug("radio available");

    uint8_t *pu8Data = new uint8_t[RADIO_FRAME_SIZE];
    // always TMRh20 RF24 lib will read setPayloadSize or 32bytes(max size)
    m_radio.read( pu8Data, RADIO_FRAME_SIZE );
//    LogDebug("Got payload @ %lu...", millis());

    return pu8Data;
  }  

  return NULL;
}

bool RadioComm::sendMessage(uint8_t *pData, uint8_t u8Size)
{
    if (!m_bStarted)
    {
      LogDebug("Radio not started");
      return false;
    }
//delay(500);
//    LogDebug("Sent Data %d", u8Size);
//for(int x=0; x < u8Size; x++)
    //LogDebug("%d  %02x", x, pData[x]);    
    
    m_radio.stopListening();
    m_radio.write( pData, u8Size );
    m_radio.startListening();

    return true;
}

void RadioComm::setSignalStrength(uint8_t u8Str)
{
  switch(u8Str)
  {
    case 0:
      m_radio.setPALevel(RF24_PA_MIN);
      LogDebug("Set signal strength RF24_PA_MIN");
    break;

    default:
    case 1:
      m_radio.setPALevel(RF24_PA_LOW);
      LogDebug("Set signal strength RF24_PA_LOW");
    break;

    case 2:
      m_radio.setPALevel(RF24_PA_HIGH);
      LogDebug("Set signal strength RF24_PA_HIGH");
    break;

    case 3:
      m_radio.setPALevel(RF24_PA_MAX);
      LogDebug("Set signal strength RF24_PA_MAX");
    break;
    
  }
  
}

// https://gist.github.com/bryanthompson/ef4ecf24ad36410f077b 

