/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModuleTest.

    FireModuleTest is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModuleTest is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModuleTest.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SegmentConfig_h
#define SegmentConfig_h

#include <inttypes.h>
#include <Arduino.h>
#include <EEPROM.h> // must include this in the main project file otherwise compile error!

#include <TM1637.h>
#include <ButtonHandler.h>

//config to add
// power level
// signal strength

class SegmentConfig
{
public:  
  SegmentConfig(TM1637& t, int iPinUp, int iPinDown);
  virtual ~SegmentConfig();

public:
  void init();
  void loop();

public:
  void printSegment(uint8_t *arr);
  void toggleSegment(bool bOn);
  void printControl();
  void printSlat();

  void printUser(uint8_t *arr, unsigned long tTimeout);
  void commitConfig();

public:
  int controlId() { return m_iFireControlId; }
  int slatId()    { return m_iSlatId; }

  bool isIdle() { return m_segmentState == e_segmentState_Idle;}

private:
  int incValue(int i);
  int decValue(int i);
  
  /**
  * processSegmentstate
  *
   * @brief Segment up/down buttons
   *
   *
   * Called for each iteration of main loop
   * If the 7 segment display is inactive and a button is pressed
   *      show the current control group id
   *      up/down will change the value
   *      After 3seconds of idle show the slat id
   *
   *      up/down will change the slat id
   *      after 3seconds of idle revert back to inactive segment state
   *
   * @param [in] iPin     Pin pressed (-1 for none)
   */
  void processSegmentstate(int iPin1, int iPin2);

private:
  typedef enum  
  {
      e_segmentState_Idle,                // nothing on segment
      e_segmentState_Slat,                // setting slat within controller group
      e_segmentState_Ctrl,                // setting fire controller id
      e_segmentState_Commit,              // write settings to EEPROM
      e_segmentState_ShowUser,            // Show user data
  } e_segmentState;

  TM1637&           m_tm1637;
  e_segmentState    m_segmentState;
  unsigned char     m_iFireControlId;
  unsigned char     m_iSlatId;
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

  int               m_addrControl;
  int               m_addrSlat;

  unsigned long     m_tUserFinished;

};

#endif // #ifndef SegmentConfig_h


