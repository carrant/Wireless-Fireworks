/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModule.

    FireModule is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModule is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModule.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ProcessMessage.h"
#include "RelayModule.h"


ProcessMessage::ProcessMessage(RadioComm& radio, RelayModule& relay, Segment& segment, Config &cfg )
  : m_radio(radio), 
    m_relay(relay),
    m_segment(segment),
    m_config(cfg)
{
  m_u16Pin        = millis(); // some quasi random number so hopefully they all don't start with the same value and compromise security
  m_u16MySeq      = 0;
  m_u16OtherSeq   = 0;
  m_tLastHello    = 0;
  m_tHelloInterval= 15000; // default to 5s until first ACK, then default to 15s
  
  m_bRecvFirstMsg = false;
  m_u8PrevControlId = 0xff;
  m_u8PrevSlatId  = 0xff;
  
  m_u16CueStateLo = 0xffff; // start out with all cues active - until we figure out the discontinuity test
  m_u16CueStateHi = (1 <<0) | (1<<1);
}

ProcessMessage::~ProcessMessage()
{
}


void ProcessMessage::init(uint16_t u16Pin)
{
  m_u16Pin      = u16Pin;
  
  sendHelloModule();
}

uint16_t ProcessMessage::loop(uint8_t *pu8Data, uint8_t u8Size)
{
  checkChange();
  
  if (pu8Data)
  {  
    FireworksMessageBase *pMsg = (FireworksMessageBase *)pu8Data;

    LogDebug("Receive message %d", pMsg->u16Cmd);

    if (pMsg->u16Pin != m_u16Pin)
    {
      LogDebug("Invalid PIN %d %d", pMsg->u16Pin != m_u16Pin);
      delete pu8Data;
      return 0;
    }

    if (pMsg->u8ControlId != m_config.controlId())
    {
      LogDebug("Invalid ControlId %d %d", pMsg->u8ControlId, m_config.controlId());
      delete pu8Data;
      return 0;
    }
    
    if (pMsg->u8SlatId != m_config.slatId() && pMsg->u8SlatId != SLAT_BROADCAST)
    {
      LogDebug("Invalid Slatid %d %d", pMsg->u8SlatId, m_config.slatId());
      delete pu8Data;
      return 0;
    }
          
    m_u16OtherSeq = pMsg->u16RecvSeq;   // update the last seq number from the far end

    bool bRecv=false;
    switch(pMsg->u16Cmd)
    {
      case FW_MSG_CMD_ACK:
        processAck((FireworksMessageAck *)pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
      break;
      
      case FW_MSG_CMD_HELLO_CONTROL:
        processHelloControl((FireworksMessageHelloControl *)pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
      break;
      
      case FW_MSG_CMD_FIRE:
        processFire((FireworksMessageFire *)pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
      break;
      
      case FW_MSG_CMD_RESET_CUES:
        processResetCues((FireworksMessageBase *)pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
      break;

      case FW_MSG_CMD_RESET_BOARD:
        processResetBoard((FireworksMessageBase *)pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
        return 0xdead; // reset board
      break;

      case FW_MSG_CMD_FIRE_ALL:
        processFireAll(pMsg, u8Size);
        bRecv=true;
        delete pu8Data;
      break;
    }


    if (bRecv)
      m_tLastHello = millis();
  }

//keep it simple and keep sending hello every 5s
  if ((millis() - m_tLastHello) >= m_tHelloInterval)
  {
    sendHelloModule();
    m_tLastHello = millis();
  }

  return 0;
}

void ProcessMessage::processAck(FireworksMessageAck *pMsg, uint8_t u8Size)
{
  LogDebug("Ack");
}

void ProcessMessage::processHelloControl(FireworksMessageHelloControl *pMsg, uint8_t u8Size)
{
  LogDebug("Hello Control");
  m_tHelloInterval = 30000; // jump to sending hello every 15s
  m_config.setTalking(true);

  if (!m_bRecvFirstMsg && m_segment.isIdle())
    m_segment.printstr(" YES", 1000);
    
  m_bRecvFirstMsg = true;
}

bool cueOn(uint8_t u8Cue, uint16_t low, uint16_t high)
{
  
  if (u8Cue < 16)
    return (low & (1 << u8Cue)) != 0;

  u8Cue -=16;
    return (high & (1 << u8Cue)) != 0;
}

void setCue(bool bOn, uint8_t u8Cue, uint16_t& low, uint16_t& high)
{
  if (u8Cue < 16)
  {
    if (bOn)
      low |= (1 << u8Cue);
     else
      low &= ~(1 << u8Cue);
  }

  u8Cue -=16;
  if (bOn)
    high |= (1 << u8Cue);
   else
    high &= ~(1 << u8Cue);
}

void ProcessMessage::processFire(FireworksMessageFire *pMsg, uint8_t u8Size)
{
  LogDebug("FIRE 0x%04x%04x", pMsg->u16CueStateHi, pMsg->u16CueStateLo);
  for(uint32_t iCue=0; iCue< NUM_CUES; iCue++)
  {
    if (!cueOn(iCue, pMsg->u16CueStateLo, pMsg->u16CueStateHi))
      continue;
      
    LogDebug("FIRE %d 0x%04x%04x", iCue, m_u16CueStateHi, m_u16CueStateLo);
    setCue(false, iCue, m_u16CueStateLo, m_u16CueStateHi);
    
    int r = m_relay.relayOn(iCue);

#if 1
    if (m_segment.isIdle())
    {
      iCue++;
      uint8_t arr[4] = {iCue/10, iCue - ((iCue/10)*10), r/10, r-((r/10)*10)};
      m_segment.print(arr,1900);
    }
#endif
  }
}

void ProcessMessage::processResetCues(FireworksMessageBase *pMsg, uint8_t u8Size)
{
  LogDebug("Reset Cues");
  m_u16CueStateLo = 0xffff;
  m_u16CueStateHi = 0xffff;
  sendHelloModule();
}


void ProcessMessage::processFireAll(FireworksMessageBase *pMsg, uint8_t u8Size)
{
  LogDebug("Fire All");
  m_relay.cycleAllRelays();
}

void ProcessMessage::processResetBoard(FireworksMessageBase *pMsg, uint8_t u8Size)
{
  LogDebug("Reset Board");
  sendGoodbye(m_config.controlId(),m_config.slatId());
  delay(100);
}

FireworksMessageBase* ProcessMessage::allocateMessage(uint8_t u8Size)
{
  uint8_t *pData = new uint8_t[u8Size];

  FireworksMessageBase *pBase = (FireworksMessageBase *)pData;
  
  pBase->u16Pin      = m_u16Pin;
  pBase->u8Size      = 32;
  pBase->u16SendSeq  = m_u16MySeq++;
  pBase->u16RecvSeq  = m_u16OtherSeq;
  pBase->u16Cmd      = FW_MSG_CMD_ACK;
  pBase->u8ControlId = m_config.controlId();
  pBase->u8SlatId    = m_config.slatId();

  return pBase;
}

void ProcessMessage::checkChange()
{
  if (!m_bRecvFirstMsg)
  {
    m_u8PrevControlId = m_config.controlId();
    m_u8PrevSlatId  = m_config.slatId();
    return;
  }

  if (m_u8PrevControlId == m_config.controlId() && m_u8PrevSlatId == m_config.slatId())
    return;

  // make sure we aren't still configuring the module!
  if (!m_config.isIdle())
    return;
    

  sendGoodbye(m_u8PrevControlId,m_u8PrevSlatId);
  m_bRecvFirstMsg   = false;
  m_u8PrevControlId = 0xff;
  m_u8PrevSlatId    = 0xff;
}

bool ProcessMessage::sendGoodbye(uint8_t u8ControlId, uint8_t u8SlatId)
{
  uint8_t pData[RADIO_FRAME_SIZE];

  FireworksMessageBase *pBase = (FireworksMessageBase *)pData;
  
  pBase->u16Pin      = m_u16Pin;
  pBase->u8Size      = 32;
  pBase->u16SendSeq  = m_u16MySeq++;
  pBase->u16RecvSeq  = m_u16OtherSeq;
  pBase->u16Cmd      = FW_MSG_CMD_GOODBYE;
  pBase->u8ControlId = u8ControlId;
  pBase->u8SlatId    = u8SlatId;

  m_radio.sendMessage((uint8_t *)pBase, RADIO_FRAME_SIZE);
  
}

bool ProcessMessage::sendAck(uint16_t u16SeqToAck) //uint16_t u16OtherSizeSeq)
{
  LogDebug("Send ACK");
  FireworksMessageAck *pMsg = (FireworksMessageAck *)allocateMessage();

  pMsg->base.u16Cmd = FW_MSG_CMD_ACK;
  pMsg->u16AckSeq   = u16SeqToAck; // this should match the sender's u16SendSeq
  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);

  delete pMsg;
}

bool ProcessMessage::sendHelloModule()
{
  LogDebug("Send Hello module");
  FireworksMessageHelloModule *pMsg = (FireworksMessageHelloModule *)allocateMessage();

  pMsg->base.u16Cmd   = FW_MSG_CMD_HELLO_MODULE;
  pMsg->u8NumCues     = NUM_CUES;

  pMsg->u16CueStateLo = m_u16CueStateLo;
  pMsg->u16CueStateHi = m_u16CueStateHi;

#if 0
  uint8_t *pData = (uint8_t *)pMsg;
    char sLine[128];
    char *pLine = sLine;
    for (int x=0; x<16;x++)
        pLine += sprintf(pLine, "%02x ", pData[x]);

    pLine += sprintf(pLine, "\n\r");

    for (int x=16; x<32;x++)
        pLine += sprintf(pLine, "%02x ", pData[x]);
    pLine += sprintf(pLine, "\n\r");

    Serial.println("SEND buffer : ");
    Serial.println(sLine);;
#endif
  
  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);
  delete pMsg;
}

bool ProcessMessage::sendFired(uint8_t *arrCue, uint8_t u8Size)
{
  LogDebug("Send Hello fired");
  FireworksMessageFired *pMsg = (FireworksMessageFired *)allocateMessage();

  pMsg->base.u16Cmd = FW_MSG_CMD_FIRED;
  pMsg->u8NumCues = u8Size;
  memcpy(pMsg->arrCues, arrCue, sizeof(uint8_t)*u8Size);
  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);

  delete pMsg;
}


