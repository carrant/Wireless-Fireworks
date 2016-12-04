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

#include "SerialCommand.h"
#include "utils.h"

#include "ManagerSlat.h"

FireCommand g_Commands[] = { {"FIRE",     4,  CMD_ID_FIRE },  
                             {"STATUS",   6,  CMD_ID_STATUS },
                             {"SETCTRL",  7,  CMD_ID_SETCTRL },
                             {"SETPIN",   6,  CMD_ID_SETPIN }
                           };
int g_CommandsSize = sizeof(g_Commands) / sizeof(FireCommand);


SerialCommand::SerialCommand(HardwareSerial& s, ManagerSlat& m)
  : m_serial(s),
    m_manager(m)
{
  m_u16BufferSize       = 128;
  m_u16BufferPosition   = 0;
  m_arrBuffer           = NULL;
}

SerialCommand::~SerialCommand()
{
  delete m_arrBuffer;
}


void SerialCommand::init()
{
  m_arrBuffer           = new uint8_t[m_u16BufferSize];
}

void SerialCommand::loop()
{
  uint8_t u8CmdId = CMD_ID_NONE;
  char ch = 0;
  
  while( m_serial.available() > 0 && (ch = m_serial.read()) > 0)
  {
    if (ch == '\n' || ch == '\r')
      continue;

    if (m_u16BufferPosition < m_u16BufferSize)
    {
      m_arrBuffer[m_u16BufferPosition++] = ch;
    }
    else
    {
      uint16_t tempBufferSize = (m_u16BufferSize * 2)/3;
      uint8_t *ptemp = new uint8_t[tempBufferSize];
      memcpy(ptemp, m_arrBuffer, m_u16BufferSize);
      delete m_arrBuffer;

      m_arrBuffer = ptemp;
      m_u16BufferSize = tempBufferSize;
      
      m_arrBuffer[m_u16BufferPosition++] = ch;
    }

    if ( (u8CmdId = checkMessage()) != CMD_ID_NONE)
    {
      m_u16BufferPosition = 0;
    }
  }
}

uint8_t SerialCommand::checkMessage()
{
  if (m_arrBuffer[m_u16BufferPosition-1] != CMD_TERMINATOR)
    return CMD_ID_NONE;

  m_arrBuffer[m_u16BufferPosition-1] = 0;
  uint16_t u16BufLen = m_u16BufferPosition - 1;
  const char* sCommand = (const char *)m_arrBuffer;
  m_u16BufferPosition = 0;

  //LogDebug("Checking '%s' for command", sCommand);
  
  for (int x=0; x < g_CommandsSize; x++)
  {
    if (u16BufLen >= g_Commands[x].u8Len )
    {
      if (strncmp(g_Commands[x].pCmd, sCommand, g_Commands[x].u8Len) == 0)
      {
        LogDebug("Got command %s", sCommand);

        switch(g_Commands[x].u8Id)
        {
          case CMD_ID_FIRE:
            processCmdFire(sCommand, u16BufLen);
            break;

          case CMD_ID_STATUS:
            processCmdStatus(sCommand, u16BufLen);
            break;

          case CMD_ID_SETCTRL:
            processCmdSetControlId(sCommand, u16BufLen);
            break;

          case CMD_ID_SETPIN:
            processCmdSetPin(sCommand, u16BufLen);
            break;

          default:
            LogError("Unknown command %s", sCommand);
            break;
        }
        return g_Commands[x].u8Id;
      }
    }
  }

  return CMD_ID_NONE;
}

void SerialCommand::processCmdFire(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Fire");
  String sSlat = GetCSV(pCmd, 1);
  String sCue  = GetCSV(pCmd, 2);

  if (sSlat.length() == 0 || sCue.length() == 0)
  {
    LogDebug("Invalid arguments for FIRE command '%s'", pCmd);
    return;
  }

  ListItemSlat *pItem = m_manager.findSlat(sSlat.toInt());
  if (!pItem)
  {
    char sError[1024];
    sprintf(sError, "Slat '%s' not found", sSlat.c_str());
    sendRespError(pCmd, sError);
    return;
  }

  LogDebug("Fire slat=%-2s cue=%-2s", sSlat.c_str(), sCue.c_str());
}

void SerialCommand::processCmdStatus(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Status");
  String sSlat = GetCSV(pCmd, 1);
  String sCue  = GetCSV(pCmd, 2);

  if (sSlat.length() == 0 && sCue.length() == 0)
  {
    sendRespStatus();
    return;
  }
  
  if (sSlat.length() != 0 && sCue.length() != 0)
  {
    sendRespStatus(sSlat.toInt(), sCue.toInt());
    return;
  }

  if (sSlat.length() != 0)
  {
    sendRespStatus(sSlat.toInt());
    return;
  }

  LogDebug("Unknown status command '%s'", pCmd);
}


void SerialCommand::processCmdSetControlId(const char *pCmd, uint16_t u16Len)
{
  LogDebug("SetCtrl");
  String sCtrl = GetCSV(pCmd, 1);

  if (sCtrl.length() == 0)
  {
    LogDebug("Invalid arguments for SETCTRL command '%s'", pCmd);
    return;
  }
  
  LogDebug("SetCtrlId %s", sCtrl.c_str());
  m_manager.setControlId(atoi(sCtrl.c_str()));
}

void SerialCommand::processCmdSetPin(const char *pCmd, uint16_t u16Len)
{
  LogDebug("SetPin");
  String sPin = GetCSV(pCmd, 1);

  if (sPin.length() == 0)
  {
    LogDebug("Invalid arguments for SETPIN command '%s'", pCmd);
    return;
  }
  
  LogDebug("SetPin %s", sPin.c_str());
  m_manager.setPin(atoi(sPin.c_str()));
}

//FIRED,num cues,cue1, cue2, cueN-1
void SerialCommand::sendRespFired(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_FIRED);
  LogSerialResp(",%d,");
  for(int x=0; x < u8NumCues; x++)
  {
    if ( x+1 == u8NumCues)
    {
      LogSerialResp("%d,%d,%d", u8Slat, arrCues[x], ListItemSlat::e_CueStatus_Fired);
    }
    else
    {
      LogSerialResp("%d,%d,%d,", u8Slat, arrCues[x], ListItemSlat::e_CueStatus_Fired);
    }
    
  }
  LogSerialResp(RESP_END);
}

// ERROR,CMD,Error text
void SerialCommand::sendRespError(const char *pCmd, const char *pError)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_ERROR);
  LogSerialResp(",");
  LogSerialResp("%s,%s", pCmd, pError);
  LogSerialResp(RESP_END);
}

//STATUSALL,slat count,slat id,cue count,cue1,cue1 status, cue2, cue2 status, ..,cueN, cueN status, slat id, cue count, cue1, cue1 status ...
void SerialCommand::sendRespStatus()
{
  LogDebug("Status All");
  
  linkedlist<ListItemSlat *> ls = m_manager.getList();

  linkedlistitem<ListItemSlat *> *pCur;

  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_ALL);
  LogSerialResp(",%d,", ls.count());
  
  ls.first();
  while(!ls.end())
  {
    pCur = ls.next();

LogDebug("cur slat %d  %p %p", pCur->m_item->slatId(), pCur->m_item, pCur);
    ListItemSlat::eCueStatus* arrStatus = pCur->m_item->statusCues();
    if (!arrStatus)
    {
      LogSerialResp(RESP_END);
      return;
    }

    LogSerialResp("%d,%d,", pCur->m_item->slatId(), pCur->m_item->numCues() );
    for(int x=0; x < pCur->m_item->numCues(); x++)
    {
      if (x+1 == pCur->m_item->numCues() && pCur->m_pNext == NULL )
      {
        LogSerialResp("%d,%d", x, arrStatus[x]);
      }
      else
      {
        LogSerialResp("%d,%d,", x, arrStatus[x]);
      }
    }
  }
  
  LogSerialResp(RESP_END);
  
}

//STATUSSLAT,slat id,cue count,cue1,cue1 status, cue2, cue2 status, ..,cueN, cueN status
void SerialCommand::sendRespStatus(uint8_t u8Slat)
{
  LogDebug("Status slat=%-2d", u8Slat);
  ListItemSlat *pSlat = m_manager.findSlat(u8Slat);
  if (!pSlat)
  {
    char sError[1024];
    sprintf(sError, "Slat '%d' not found", u8Slat);
    sendRespError("STATUS slat", sError);
    return;
  }

  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_SLAT);
  LogSerialResp(",%d,%d", u8Slat, pSlat->numCues());
  
  ListItemSlat::eCueStatus* arrStatus = pSlat->statusCues();
  if (!arrStatus)
  {
    LogSerialResp(RESP_END);
    return;
  }

  for(int x=0; x < pSlat->numCues(); x++)
  {
    if ( x+1 == pSlat->numCues())
    {
      LogSerialResp("%d,%d", u8Slat, x, arrStatus[x]);
    }
    else
    {
      LogSerialResp("%d,%d,", u8Slat, x, arrStatus[x]);
    }
  }
  
  LogSerialResp(RESP_END);
  
}

//STATUSSLATCUE,slat id,cue,cue status
void SerialCommand::sendRespStatus(uint8_t u8Slat, uint8_t u8Cue)
{
  LogDebug("Status slat=%-2d cue=%-2d", u8Slat, u8Cue);

  LogDebug("Status slat=%-2d", u8Slat);
  ListItemSlat *pSlat = m_manager.findSlat(u8Slat);
  if (!pSlat)
  {
    sendRespError("STATUS slat", "slat not found");
    return;
  }

  if (u8Cue > pSlat->numCues())
  {
    sendRespError("STATUS slat", "cue not found");
    return;
  }

  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_SLAT_CUE);
  LogSerialResp(",");
  
  ListItemSlat::eCueStatus* arrStatus = pSlat->statusCues();
  if (!arrStatus)
  {
    LogSerialResp(RESP_END);
    return;
  }

  LogSerialResp("%d,%d,%d", u8Slat, u8Cue, arrStatus[u8Cue]);
  
  LogSerialResp(RESP_END);
}

