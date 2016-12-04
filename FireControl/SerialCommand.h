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

#ifndef SerialCommand_h
#define SerialCommand_h

#include <stdio.h>
#include <HardwareSerial.h>

#include "ManagerSlat.h"

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

#define RESP_ERROR            "ERROR"
#define RESP_STATUS_ALL       "STATUSALL"
#define RESP_STATUS_SLAT      "STATUSSLAT"
#define RESP_STATUS_SLAT_CUE  "STATUSSLATCUE"
#define RESP_FIRED            "FIRED"

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
  void sendRespFired(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues);

private:
  uint8_t checkMessage();

  void processCmdFire(const char *pCmd, uint16_t u16Len);
  void processCmdStatus(const char *pCmd, uint16_t u16Len);
  void processCmdSetControlId(const char *pCmd, uint16_t u16Len);
  void processCmdSetPin(const char *pCmd, uint16_t u16Len);

  void sendRespError(const char *pCmd, const char *pError);

  void sendRespStatus();
  void sendRespStatus(uint8_t u8Slat);
  void sendRespStatus(uint8_t u8Slat, uint8_t u8Cue);


private:
  uint16_t m_u16BufferSize;
  uint16_t m_u16BufferPosition;
  uint8_t *m_arrBuffer;

  HardwareSerial& m_serial;
  ManagerSlat& m_manager;

};


#endif // #ifndef SerialCommand_h


