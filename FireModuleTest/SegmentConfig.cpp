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

#include "SegmentConfig.h"
#include "utils.h"

SegmentConfig::SegmentConfig(TM1637& t, int iPinUp, int iPinDown) 
    : m_tm1637(t),
      m_buttonUp(iPinUp),
      m_buttonDown(iPinDown)
{
    m_iPinUp          = iPinUp;
    m_iPinDown        = iPinDown;

    m_segmentState    = e_segmentState_Idle;
    m_iFireControlId  = 0;
    m_tLastButton     = 0;
    m_iConfigTimeout  = 3 * 1000; // 3 seconds

    m_MaxValue        = 99;

    m_tDebounceTimeInit= 175; //100ms
    m_tDebounceTime   = m_tDebounceTimeInit; //100ms
    m_tLastCheck      = 0;

    m_addrControl     = 0;
    m_addrSlat        = 1;

    m_iFireControlId  = EEPROM.read(m_addrControl);
    m_iSlatId         = EEPROM.read(m_addrSlat);

    m_tUserFinished   = 0;

    if (m_iFireControlId > m_MaxValue || m_iFireControlId < 0)
      m_iFireControlId = 0;
      
    if (m_iSlatId > m_MaxValue || m_iSlatId < 0)
      m_iSlatId = 0;
}
  
SegmentConfig::~SegmentConfig()
{
}


void SegmentConfig::init()
{
    m_buttonUp.init();
    m_buttonDown.init();

    m_tm1637.clearDisplay();
}

#define IS_PRESS(x)  ((x) == EV_SHORTPRESS || (x) == EV_LONGPRESS || (x) == EV_STILLPRESS)

void SegmentConfig::loop()
{
    if (millis() - m_tLastCheck < m_tDebounceTime)
      return;
      
    int eventUp = m_buttonUp.handlePressed();
    int eventDown = m_buttonDown.handlePressed();

    if (IS_PRESS(eventUp) || IS_PRESS(eventDown))
    {
      processSegmentstate(eventUp, eventDown);
      
      if (eventUp == EV_STILLPRESS || eventDown == EV_STILLPRESS)
        m_tDebounceTime = 50;
    }
    else
    {    
      processSegmentstate(-1, -1);
      m_tDebounceTime = m_tDebounceTimeInit;
    }

    m_tLastCheck = millis();
}



void SegmentConfig::printSegment(uint8_t *arr)
{
  if (arr[0] >= 0 && arr[0] <= 9)
    m_tm1637.display(0x00, arr[0]);
  else
    m_tm1637.display_raw(0x00, arr[0]);


  if (arr[1] >= 0 && arr[1] <= 9)
    m_tm1637.display(0x01, arr[1]);
  else
    m_tm1637.display_raw(0x01, arr[1]);

    
  if (arr[2] >= 0 && arr[2] <= 9)
    m_tm1637.display(0x02, arr[2]);
  else
    m_tm1637.display_raw(0x02, arr[2]);

   
  if (arr[3] >= 0 && arr[3] <= 9)
    m_tm1637.display(0x03, arr[3]);
  else
    m_tm1637.display_raw(0x03, arr[3]);
}

void SegmentConfig::toggleSegment(bool bOn)
{
  if (bOn == false)
      m_tm1637.clearDisplay();
}

void SegmentConfig::printControl()
{
    uint8_t arr[4];
    arr[0] = LET_C;
    arr[1] = LET_L;
    arr[2] = m_iFireControlId / 10;
    arr[3] = m_iFireControlId - (arr[2] * 10);
    printSegment(arr);
}


void SegmentConfig::printSlat()
{
    uint8_t arr[4];
    arr[0] = LET_S;
    arr[1] = LET_L;
    arr[2] = m_iSlatId / 10;
    arr[3] = m_iSlatId - (arr[2] * 10);
    printSegment(arr);
}

void SegmentConfig::printUser(uint8_t *arr, unsigned long tTimeout)
{
    toggleSegment(true);
    printSegment(arr);
    m_tUserFinished   = millis() + tTimeout;
    
    m_segmentState = e_segmentState_ShowUser;
    LogDebug("%d %d %d %d", arr[0], arr[1], arr[2], arr[3]);
}

int SegmentConfig::incValue(int i)
{
    return (i +1) % (m_MaxValue+1);
}

int SegmentConfig::decValue(int i)
{
    if (i)
      return i-1;

    return m_MaxValue;
}

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
*      After 3seconds of idle show the channel id
*
*      up/down will change the channel id
*      after 3seconds of idle revert back to inactive segment state
*
* @param [in] iPin     Pin pressed (-1 for none)
*/
void SegmentConfig::processSegmentstate(int iEventUp, int iEventDown)
{
  switch (m_segmentState)
  {
      // check if button to enable config
      case e_segmentState_Idle:
          if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
              toggleSegment(true);
              m_segmentState = e_segmentState_Slat;
              printSlat();
              m_tLastButton = millis();
          }
      break;

      case e_segmentState_Slat:
          if (IS_PRESS(iEventUp) && IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Ctrl;
              printControl();
              m_tDebounceTime = m_tDebounceTimeInit;
              m_tLastButton = millis();
          }
          else if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
              m_iSlatId = IS_PRESS(iEventUp) ? incValue(m_iSlatId) : decValue(m_iSlatId);
              printSlat();
              
              m_tLastButton = millis();
          }
          else
          {
              if( millis() - m_tLastButton >= m_iConfigTimeout)
              {
                  m_segmentState = e_segmentState_Ctrl;
                  printControl();
                  m_tLastButton = millis();
              }
          }
      break;
      
      case e_segmentState_Ctrl:
          if (IS_PRESS(iEventUp) && IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Commit;
//              toggleSegment(false);

//              m_tDebounceTime = m_tDebounceTimeInit;
          }
          else if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
              m_iFireControlId = IS_PRESS(iEventUp) ? incValue(m_iFireControlId) : decValue(m_iFireControlId);
              printControl();
              
              m_tLastButton = millis();
          }
          else
          {
              if( millis() - m_tLastButton >= m_iConfigTimeout)
              {
                  m_segmentState = e_segmentState_Commit;
              }
          }
      break;

      case e_segmentState_Commit:
      {
          commitConfig();
      }
      break;

      case e_segmentState_ShowUser:
      {
        if( millis() > m_tUserFinished)
        {
          LogDebug("here");
            m_segmentState = e_segmentState_Idle;
            toggleSegment(false);

            m_tDebounceTime = m_tDebounceTimeInit;
        }
      }
      break;
      
  }
}
void SegmentConfig::commitConfig()
{
    m_segmentState = e_segmentState_Idle;
    toggleSegment(false);

    m_tDebounceTime = m_tDebounceTimeInit;
    EEPROM.write(m_addrControl, m_iFireControlId);
    EEPROM.write(m_addrSlat, m_iSlatId);
}

