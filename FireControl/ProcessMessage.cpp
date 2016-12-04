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

#include "ProcessMessage.h"


ProcessMessage::ProcessMessage(RadioComm& radio, ManagerSlat& manager, SerialCommand& serial
#ifdef USE_SEGMENT
    , SegmentConfig &segment
#endif    
  )
  : m_radio(radio), 
    m_manager(manager),
    m_serial(serial)
#ifdef USE_SEGMENT
    , m_segment(segment)
#endif    
{
  m_u16MySeq      = 0;
  m_u16OtherSeq   = 0;
}

ProcessMessage::~ProcessMessage()
{
  clearAckList();
}

void ProcessMessage::clearAckList()
{
  MessageAckItem *pAck = NULL;
  while(m_lsPendingAck.count())
  {
    pAck = m_lsPendingAck.removeHead();
    delete pAck->pMsg;
    delete pAck;
  }
}

void ProcessMessage::init()
{
}

void ProcessMessage::loop(uint8_t *pu8Data, uint8_t u8Size)
{
  if (pu8Data)
  {  
    FireworksMessageBase *pMsg = (FireworksMessageBase *)pu8Data;

    LogDebug("Receive message %d", pMsg->u16Cmd);

    if (pMsg->u16Pin != m_manager.pin())
    {
      LogDebug("Invalid PIN %d %d", pMsg->u16Pin, m_manager.pin());
      delete pu8Data;
      return;
    }

    if (pMsg->u8ControlId != m_manager.controlId())
    {
      LogDebug("Invalid ControlId %d %d", pMsg->u8ControlId, m_manager.controlId());
      delete pu8Data;
      return;
    }

    m_u16OtherSeq = pMsg->u16RecvSeq;   // update the last seq number from the far end

    
    switch(pMsg->u16Cmd)
    {
      case FW_MSG_CMD_ACK:
        processAck((FireworksMessageAck *)pMsg, u8Size);
        delete pu8Data;
      break;
      
      case FW_MSG_CMD_HELLO_MODULE:
        processHelloModule((FireworksMessageHelloModule *)pMsg, u8Size);
        delete pu8Data;
      break;
      
      case FW_MSG_CMD_FIRED:
        processFired((FireworksMessageFired *)pMsg, u8Size);
        delete pu8Data;
      break;
    }
  }

  linkedlistitem<MessageAckItem *> *pItem = NULL;
  m_lsPendingAck.first();
  while(!m_lsPendingAck.end())
  {
    pItem = m_lsPendingAck.next();
    MessageAckItem *pAck = pItem->m_item;
    
    if (millis() - pAck->tTimestamp > pAck->u32Timeout )
    {
      if (pAck->i32ResendCount != 0)
      {
        // < 0 infinite retry count
        if (pAck->i32ResendCount > 0)
          pAck->i32ResendCount--;
          
        m_lsPendingAck.remove(pItem);
        pAck->tTimestamp = millis();
        m_lsPendingAck.addTail(pAck);
      }
      else
      {
        m_lsPendingAck.remove(pItem);
        delete pAck;
      }
    }
  }
}

void ProcessMessage::processAck(FireworksMessageAck *pMsg, uint8_t u8Size)
{
  LogDebug("ACK");

  linkedlistitem<MessageAckItem *> *pItem = NULL;
  m_lsPendingAck.first();
  
  while(!m_lsPendingAck.end())
  {
    pItem = m_lsPendingAck.next();
    MessageAckItem *pAck = pItem->m_item;
    
    if (pAck->pMsg->u16SendSeq == pMsg->u16AckSeq )
    {
      m_lsPendingAck.remove(pItem);
      delete pAck;
    }
  }

}

void ProcessMessage::processHelloModule(FireworksMessageHelloModule *pMsg, uint8_t u8Size)
{
  LogDebug("Hello Module");
  m_manager.addSlat(pMsg->base.u8SlatId, pMsg->u8NumCues);
  m_manager.setStatusSlatCue(pMsg->base.u8SlatId, pMsg->arrCues, pMsg->u8NumCues, ListItemSlat::e_CueStatus_Armed);
  
}

void ProcessMessage::processFired(FireworksMessageFired *pMsg, uint8_t u8Size)
{
  LogDebug("FIRED!!");
  m_manager.setStatusSlatCue(pMsg->base.u8SlatId, pMsg->arrCues, pMsg->u8NumCues, ListItemSlat::e_CueStatus_Fired);
  m_serial.sendRespFired(pMsg->base.u8SlatId, pMsg->arrCues, pMsg->u8NumCues);
}


FireworksMessageBase* ProcessMessage::allocateMessage(uint8_t u8Size)
{
  uint8_t *pData = new uint8_t[u8Size];

  FireworksMessageBase *pBase = (FireworksMessageBase *)pData;
  
  pBase->u16Pin      = m_manager.pin();
  pBase->u16SendSeq  = m_u16MySeq++;
  pBase->u16RecvSeq  = m_u16OtherSeq;
  pBase->u16Cmd      = FW_MSG_CMD_ACK;
  pBase->u8ControlId = m_manager.controlId();
  pBase->u8SlatId    = FW_MSG_STATION_FCM;

  return pBase;
}

bool ProcessMessage::sendAck(uint16_t u16SeqToAck) //uint16_t u16OtherSizeSeq)
{
  LogDebug("Send ACK");
  FireworksMessageAck *pMsg = (FireworksMessageAck *)allocateMessage();

  pMsg->u16AckSeq = u16SeqToAck; // this should match the sender's u16SendSeq
  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);

  delete pMsg;
}

bool ProcessMessage::sendHelloControl()
{
  LogDebug("Send Hello Control");
  FireworksMessageHelloControl *pMsg = (FireworksMessageHelloControl *)allocateMessage();

  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);
  
  MessageAckItem* pAck  = new MessageAckItem;
  pAck->pMsg            = (FireworksMessageBase *)pMsg;
  pAck->tTimestamp      = millis();
  pAck->i32ResendCount  = -1; // resend forever
  pAck->u32Timeout      = FW_MSG_RESEND_TIMEOUT;
}

bool ProcessMessage::sendFire(uint8_t *arrCue, uint8_t u8Size)
{
  LogDebug("Send Fire");
  FireworksMessageFired *pMsg = (FireworksMessageFired *)allocateMessage();

  pMsg->u8NumCues = u8Size;
  memcpy(pMsg->arrCues, arrCue, sizeof(uint8_t)*u8Size);
  m_radio.sendMessage((uint8_t *)pMsg, RADIO_FRAME_SIZE);

  delete pMsg;
}



