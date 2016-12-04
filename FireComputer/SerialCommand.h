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

#ifndef SerialCommand_h
#define SerialCommand_h

#include <stdio.h>

#include "ManagerSlat.h"
#include "HardwareSerial.h"

#define CMD_ID_NONE     0
#define CMD_ID_FIRE     1
#define CMD_ID_STATUS   2
#define CMD_ID_SETCTRL  3
#define CMD_ID_SETPIN   4

//server side
#define CMD_ID_RESP_START     50
#define CMD_ID_ERROR          51
#define CMD_ID_STATUSALL      52
#define CMD_ID_STATUSSLAT     53
#define CMD_ID_STATUSSLATCUE  54
#define CMD_ID_FIRED          55

typedef struct
{
  const char *pCmd;
  uint8_t u8Len;
  uint8_t u8Id;
} FireCommand;

// Commands
// Commands end with ';'
// FIRE,slat#,cue#;
// STATUS;
// STATUS,slat#;
// STATUS,slat,cue#;

#define CMD_TERMINATOR    ';'

//response format:
// RESP_START,CMD,data;

#define RESP_START            "RESP_START;"
#define RESP_END              ";\r\n"


class SerialCommand
{
public:
  SerialCommand(HardwareSerial& s, ManagerSlat& m);
  virtual ~SerialCommand();

public:
  void init();
  void loop();

// message sent from other sources
public:
  void processRespFired(const char *pCmd, uint16_t u16Len);
  void processRespError(const char *pCmd, uint16_t u16Len);

  void processRespStatusAll(const char *pCmd, uint16_t u16Len);
  void processRespStatusSlat(const char *pCmd, uint16_t u16Len);
  void processRespStatusSlatCue(const char *pCmd, uint16_t u16Len);

private:
  uint8_t checkMessage();

public:
  void sendRespError(const char *pCmd, const char *pError);

  void sendCmdFire(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues);
  void sendCmdFire(uint8_t u8Slat, uint8_t u8Cues);

  void sendCmdStatusAll();
  void sendCmdStatusSlat(uint8_t u8Slat);
  void sendCmdStatusSlatCue(uint8_t u8Slat, uint8_t u8Cue);

  void sendCmd(const char *pCmd);

private:
  uint16_t m_u16BufferSize;
  uint16_t m_u16BufferPosition;
  uint8_t *m_arrBuffer;
  bool m_bReadingCommand;
  HardwareSerial& m_serial;
  ManagerSlat& m_manager;
  uint16_t m_u16CmdStartIndex;

};


#endif // #ifndef SerialCommand_h


