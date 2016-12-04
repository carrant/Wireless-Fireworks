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

#ifndef Config_h
#define Config_h

#include <inttypes.h>
#include <Arduino.h>
#include <EEPROM.h> // must include this in the main project file otherwise compile error!

#include "Segment.h"
#include "RadioComm.h"
#include <ButtonHandler.h>
#include <ReadWriteEEPROM.h>


#define VERSION 0x0004

//config to add
// power level
// signal strength

class Config
{
public:  
  Config(Segment& s, RadioComm& r, int iPinUp, int iPinDown);
  virtual ~Config();

public:
  void init();
  void loop();

public:
  void printSegment(uint8_t *arr);
  void toggleSegment(bool bOn);
  void printControl();
  void printSlat();
  void printPower();
  void printInfo();
  
  void commitConfig();

public:
  int controlId() { return m_iFireControlId; }
  int slatId()    { return m_iSlatId; }
  int radioPower();

  bool isIdle() { return m_segmentState == e_segmentState_Idle;}

  bool setTalking(bool b) { m_bTalking = b; }

private:
  int incValue(int i);
  int decValue(int i);
  
  void processSegmentstate(int iPin1, int iPin2);

private:
  typedef enum  
  {
      e_segmentState_Idle,                // nothing on segment
      e_segmentState_Info,                // setting slat within controller group
      e_segmentState_Slat,                // setting slat within controller group
      e_segmentState_Ctrl,                // setting fire controller id
      e_segmentState_Power,               // setting fire controller id
      e_segmentState_Commit,              // write settings to EEPROM
      e_segmentState_ShowUser,            // Show user data
  } e_segmentState;

  RadioComm&        m_radio;
  Segment           m_segment;
  e_segmentState    m_segmentState;
  unsigned long     m_tLastButton;
  int               m_iConfigTimeout;
  int               m_iPinUp;
  int               m_iPinDown;
  ButtonHandler     m_buttonUp;
  ButtonHandler     m_buttonDown;

  unsigned long     m_tDebounceTime;
  unsigned long     m_tDebounceTimeInit;
  unsigned long     m_tLastCheck;
  unsigned char     m_MaxValue;

  uint8_t           m_iFireControlId_EEPROM;
  uint8_t           m_iSlatId_EEPROM;
  uint8_t           m_u8Power_EEPROM;

  uint8_t           m_iFireControlId;
  uint8_t           m_iSlatId;
  uint8_t           m_u8Power;

  ReadWriteEEPROM   m_eeSlat;
  ReadWriteEEPROM   m_eeControl;
  ReadWriteEEPROM   m_eePower;
  
  uint8_t           m_u8MaxPower;

  bool              m_bTalking;
};

#endif // #ifndef Config_h


