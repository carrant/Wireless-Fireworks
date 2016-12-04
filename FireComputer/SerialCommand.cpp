/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireComputer.

    FireComputer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireComputer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireComputer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SerialCommand.h"
#include "utils.h"

#include "ManagerSlat.h"

#include <string>

#define RESP_ERROR            "ERROR"
#define RESP_STATUS_ALL       "STATUSALL"
#define RESP_STATUS_SLAT      "STATUSSLAT"
#define RESP_STATUS_SLAT_CUE  "STATUSSLATCUE"
#define RESP_FIRED            "FIRED"
#define RESP_FIRE             "FIRE"

FireCommand g_Commands[] = { {"RESP_START",     10, CMD_ID_RESP_START },
                             {"ERROR",          5,  CMD_ID_ERROR },  
                             {"STATUSALL",      9,  CMD_ID_STATUSALL },
                             {"STATUSSLAT",     10, CMD_ID_STATUSSLAT },
                             {"STATUSSLATCUE",  13, CMD_ID_STATUSSLATCUE }
                           };
int g_CommandsSize = sizeof(g_Commands) / sizeof(FireCommand);


SerialCommand::SerialCommand(HardwareSerial& s, ManagerSlat& m)
  : m_serial(s),
    m_manager(m)
{
  m_u16BufferSize       = 128;
  m_u16BufferPosition   = 0;
  m_arrBuffer           = NULL;
  m_bReadingCommand     = false;
  m_u16CmdStartIndex    = 1;
}

SerialCommand::~SerialCommand()
{
  delete m_arrBuffer;
}


void SerialCommand::init()
{
  m_arrBuffer           = new uint8_t[m_u16BufferSize + 1];
}

void SerialCommand::loop()
{
  uint8_t u8CmdId = CMD_ID_NONE;
  char ch = 0;
  
  while( m_serial.available() > 0 && (ch = m_serial.read()) > 0)
  {
//printf("%c", ch);
    if (ch == '\n' || ch == '\r')
      continue;

    if (m_u16BufferPosition < m_u16BufferSize)
    {
      m_arrBuffer[m_u16BufferPosition++] = ch;
    }
    else
    {
      if (m_bReadingCommand)
      {
          uint16_t tempBufferSize = (m_u16BufferSize * 2) + 1;
          uint8_t *ptemp = new uint8_t[tempBufferSize];
          memcpy(ptemp, m_arrBuffer, m_u16BufferSize);
          delete m_arrBuffer;

          m_arrBuffer = ptemp;
          m_u16BufferSize = tempBufferSize;
      }
      else
          m_u16BufferPosition = 0;
          
      m_arrBuffer[m_u16BufferPosition++] = ch;
    }

    if (!m_bReadingCommand)
    {
        if (strncmp((const char *)m_arrBuffer, RESP_START, m_u16CmdStartIndex) == 0)
        {
            if (m_u16CmdStartIndex == strlen(RESP_START))
            {
                m_bReadingCommand = true;
                m_u16BufferPosition = 0;
                m_u16CmdStartIndex =1;
            }
            else
                m_u16CmdStartIndex++;
            
        }
        else
        {
            m_u16CmdStartIndex = 1;
            m_u16BufferPosition = 0;
        }
    }
    else    
    {
        if (m_arrBuffer[m_u16BufferPosition-1] == CMD_TERMINATOR)
        {
            u8CmdId = checkMessage();
            m_u16BufferPosition = 0;
            m_u16CmdStartIndex = 1;
            m_bReadingCommand = false;
        }
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

  // check if we are looking for the start of the command string
  if (!m_bReadingCommand)
  {
    if (strncmp("RESP_START", sCommand, u16BufLen - 10) == 0)
    {
    printf("\n\nlen-10 %c cmd='%s'\n\n", u16BufLen > 10 ? sCommand[u16BufLen-10] : '.', sCommand);
        return CMD_ID_RESP_START;
    }
  }
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
          case CMD_ID_RESP_START:
            return CMD_ID_RESP_START;
            
          case CMD_ID_ERROR:
            processRespError(sCommand, u16BufLen);
            break;

          case CMD_ID_STATUSALL:
            processRespStatusAll(sCommand, u16BufLen);
            break;

          case CMD_ID_STATUSSLAT:
            processRespStatusSlat(sCommand, u16BufLen);
            break;

          case CMD_ID_STATUSSLATCUE:
            processRespStatusSlatCue(sCommand, u16BufLen);
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

//ERROR,cmd, error text
void SerialCommand::processRespError(const char *pCmd, uint16_t u16Len)
{
  uint8_t u8CueIndex = 1;  
  std::string sCmd  = GetCSV(pCmd, u8CueIndex++);
  std::string sText = GetCSV(pCmd, u8CueIndex++);
  
  LogError("Error cmd '%s' '%s'", sCmd.c_str(), sText.c_str());
}


//FIRED,num cues,cue1, cue2, cueN-1
void SerialCommand::processRespFired(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Fired");
  uint8_t u8CueIndex = 1;  
  std::string sSlat     = GetCSV(pCmd, u8CueIndex++);
  std::string sNumCues  = GetCSV(pCmd, u8CueIndex++);

  if (sSlat.length() == 0 || sNumCues.length() == 0)
  {
    LogDebug("Invalid arguments for FIRE command '%s'", pCmd);
    return;
  }

//  LogDebug("Fired slat=%-2s cue=%-2s", sSlat.c_str(), sCue.c_str());

  uint8_t u8NumCues = atoi(sNumCues.c_str());
  uint8_t *arrCues = new uint8_t[u8NumCues];
  memset(arrCues, 0xff, sizeof(uint8_t) *u8NumCues); 
  for(int x=0; x < u8NumCues; x++)
  {
      std::string sCue  = GetCSV(pCmd, u8CueIndex++);
      if (sCue.length() == 0)
        break;
       
      arrCues[x] = atoi(sCue.c_str());
  } 

  m_manager.setStatusSlatCue(atoi(sSlat.c_str()), arrCues, u8NumCues, ListItemSlat::e_CueStatus_Fired);

}

//STATUSALL,slat count,slat id,cue count,cue1,cue1 status, cue2, cue2 status, ..,cueN, cueN status, slat id, cue count, cue1, cue1 status ...
void SerialCommand::processRespStatusAll(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Status");
  uint16_t u16CsvIndex = 1;
  std::string sSlatCount = GetCSV(pCmd, u16CsvIndex++);

  if (sSlatCount.length() == 0)
  {
    LogError("Invalid slat count '%s', csvIndex=%d, '%s' ", sSlatCount.c_str(), u16CsvIndex - 1, pCmd);
    return;
  }

  uint8_t u8NumSlats = atoi(sSlatCount.c_str());
  for(int iSlat=0; iSlat < u8NumSlats; iSlat++)
  {
      std::string sSlatId  = GetCSV(pCmd, u16CsvIndex++);
      if (sSlatId.length() == 0)
      {
        LogError("Invalid slat id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
        return;
      }
      
      std::string sCueCount  = GetCSV(pCmd, u16CsvIndex++);

      if (sCueCount.length() == 0)
      {
        LogError("Invalid cue count, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
        return;
      }

// may not need this for the read HW ??
// added here so when testing with the fake preloaded slats this will work
m_manager.addSlat(atoi(sSlatId.c_str()), atoi(sCueCount.c_str()));
      
      uint8_t u8CueCount = atoi(sCueCount.c_str());
      for(int iCue = 0; iCue < u8CueCount; iCue++)
      {
        std::string sCueId = GetCSV(pCmd, u16CsvIndex++);
        if (sCueId.length() == 0)
        {
            LogError("Invalid cue id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
            return;
        }
        
        std::string sCueStatus = GetCSV(pCmd, u16CsvIndex++);
        if (sCueStatus.length() == 0)
        {
            LogError("Invalid cue sCueStatus, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
            return;
        }
        
        printf("Slat=%2d Cue=%2d Status=%d\n", atoi(sSlatId.c_str()), atoi(sCueId.c_str()), atoi(sCueStatus.c_str()));
        m_manager.setStatusSlatCue(atoi(sSlatId.c_str()), atoi(sCueId.c_str()), static_cast<ListItemSlat::eCueStatus>(atoi(sCueStatus.c_str())) );
      }
  }
}

//STATUSSLAT,slat id,cue count,cue1,cue1 status, cue2, cue2 status, ..,cueN, cueN status
void SerialCommand::processRespStatusSlat(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Status");
  uint16_t u16CsvIndex = 1;

  std::string sSlatId  = GetCSV(pCmd, u16CsvIndex++);
  if (sSlatId.length() == 0)
  {
    LogError("Invalid slat id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
    return;
  }

  std::string sCueCount  = GetCSV(pCmd, u16CsvIndex++);

  if (sCueCount.length() == 0)
  {
    LogError("Invalid cue count, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
    return;
  }
  
  uint8_t u8CueCount = atoi(sCueCount.c_str());
  for(int iCue = 0; iCue < u8CueCount; iCue++)
  {
    std::string sCueId = GetCSV(pCmd, u16CsvIndex++);
    if (sCueId.length() == 0)
    {
        LogError("Invalid cue id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
        return;
    }
    
    std::string sCueStatus = GetCSV(pCmd, u16CsvIndex++);
    if (sCueStatus.length() == 0)
    {
        LogError("Invalid cue sCueStatus, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
        return;
    }
    
    printf("Slat=%2d Cue=%2d Status=%d\n", atoi(sSlatId.c_str()), atoi(sCueId.c_str()), atoi(sCueStatus.c_str()));
    m_manager.setStatusSlatCue(atoi(sSlatId.c_str()), atoi(sCueId.c_str()), static_cast<ListItemSlat::eCueStatus>(atoi(sCueStatus.c_str())) );
  }
}

//STATUSSLATCUE,slat id,cue,cue status
void SerialCommand::processRespStatusSlatCue(const char *pCmd, uint16_t u16Len)
{
  LogDebug("Status");
  uint16_t u16CsvIndex = 1;

  std::string sSlatId  = GetCSV(pCmd, u16CsvIndex++);
  if (sSlatId.length() == 0)
  {
    LogError("Invalid slat id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
    return;
  }

  std::string sCueId = GetCSV(pCmd, u16CsvIndex++);
  if (sCueId.length() == 0)
  {
      LogError("Invalid cue id, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
      return;
  }

  std::string sCueStatus = GetCSV(pCmd, u16CsvIndex++);
  if (sCueStatus.length() == 0)
  {
      LogError("Invalid cue sCueStatus, csvIndex=%d, '%s' " , u16CsvIndex - 1, pCmd);
      return;
  }
    
  printf("Slat=%2d Cue=%2d Status=%d\n", atoi(sSlatId.c_str()), atoi(sCueId.c_str()), atoi(sCueStatus.c_str()));
  m_manager.setStatusSlatCue(atoi(sSlatId.c_str()), atoi(sCueId.c_str()), static_cast<ListItemSlat::eCueStatus>(atoi(sCueStatus.c_str())) );
}

//FIRE,slat num,num cues,cue1, cue2, cueN
void SerialCommand::sendCmdFire(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_FIRE);
  LogSerialResp(",%d,%d,", u8Slat, u8NumCues);
  for(int x=0; x < u8NumCues; x++)
  {
    if ( x+1 == u8NumCues)
    {
      LogSerialResp("%d", arrCues[x]);
    }
    else
    {
      LogSerialResp("%d,", arrCues[x]);
    }
  }
  
  m_manager.setStatusSlatCue(u8Slat, arrCues, u8NumCues, ListItemSlat::e_CueStatus_Firing);
  LogSerialResp(RESP_END);
}

//FIRE,slat num,1,cue
void SerialCommand::sendCmdFire(uint8_t u8Slat, uint8_t u8Cue)
{
  uint8_t arr[1] = {u8Cue};
  sendCmdFire(u8Slat, arr, 1);
}


//ERROR,cmd, error text
void SerialCommand::sendRespError(const char *pCmd, const char *pError)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_ERROR);
  LogSerialResp(",");
  LogSerialResp("%s,%s", pCmd, pError);
  LogSerialResp(RESP_END);
}

//STATUSALL
void SerialCommand::sendCmdStatusAll()
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_ALL);
  LogSerialResp(RESP_END);
}

//STATUSSLAT,slat id
void SerialCommand::sendCmdStatusSlat(uint8_t u8Slat)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_SLAT);
  LogSerialResp(",%d", u8Slat);
  LogSerialResp(RESP_END);
}

//STATUSSLATCUE,slat id, cue Id
void SerialCommand::sendCmdStatusSlatCue(uint8_t u8Slat, uint8_t u8Cue)
{
  LogSerialResp(RESP_START);
  LogSerialResp(RESP_STATUS_SLAT_CUE);
  LogSerialResp(",%d,%d", u8Slat,u8Cue);
  LogSerialResp(RESP_END);
}

void SerialCommand::sendCmd(const char *pCmd)
{
  LogDebug("Sendcommand %s", pCmd);
  LogSerialResp("%s", pCmd);
  LogSerialResp(RESP_END);
}

