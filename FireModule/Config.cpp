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

#include "Config.h"
#include "utils.h"

#define EEPROM_ADDRESS_CONTROL  0
#define EEPROM_ADDRESS_SLAT     1
#define EEPROM_ADDRESS_POWER    2

Config::Config(Segment& s, RadioComm& r, int iPinUp, int iPinDown) 
    : m_segment(s),
      m_radio(r),
      m_buttonUp(iPinUp),
      m_buttonDown(iPinDown),
      m_eeControl(EEPROM_ADDRESS_CONTROL,1),
      m_eeSlat(EEPROM_ADDRESS_SLAT,1),
      m_eePower(EEPROM_ADDRESS_POWER,1)
      
{
    m_iPinUp          = iPinUp;
    m_iPinDown        = iPinDown;

    m_segmentState    = e_segmentState_Idle;
    m_iFireControlId  = 0;
    m_tLastButton     = 0;
    m_iConfigTimeout  = 3 * 1000; // 3 seconds

    m_u8Power         = 1;
    m_u8MaxPower      = 4; // 0 = low, 1 = med, 2 = high, 3 = MAX

    m_MaxValue        = 99;

    m_tDebounceTimeInit= 250; //100ms
    m_tDebounceTime   = m_tDebounceTimeInit; //100ms
    m_tLastCheck      = 0;

    m_bTalking        = false;
    
    m_iFireControlId  = (unsigned char)m_eeControl;
    m_iSlatId         = (unsigned char)m_eeSlat;
    m_u8Power         = (unsigned char)m_eePower;

    if (m_iFireControlId > m_MaxValue || m_iFireControlId < 0)
      m_iFireControlId = 0;
      
    if (m_iSlatId > m_MaxValue || m_iSlatId < 0)
      m_iSlatId = 0;

    if (m_u8Power > m_u8MaxPower || m_u8Power < 0)
      m_u8Power = 1;
}
  
Config::~Config()
{
}


void Config::init()
{
    m_buttonUp.init();
    m_buttonDown.init();

    m_segment.clear();
}

#define IS_PRESS(x)  ((x) == EV_SHORTPRESS || (x) == EV_LONGPRESS || (x) == EV_STILLPRESS)

void Config::loop()
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




void Config::printControl()
{
    uint8_t arr[4];
    arr[0] = LET_C;
    arr[1] = LET_L;
    arr[2] = m_iFireControlId / 10;
    arr[3] = m_iFireControlId - (arr[2] * 10);
    m_segment.print(arr, 0);
}


void Config::printSlat()
{
    uint8_t arr[4];
    arr[0] = LET_S;
    arr[1] = LET_L;
    arr[2] = m_iSlatId / 10;
    arr[3] = m_iSlatId - (arr[2] * 10);
    m_segment.print(arr, 0);
}

void Config::printPower()
{
    uint8_t arr[4] = {0,0,0,0};
    switch(m_u8Power)
    {
      //LOW
      case 0:
        arr[0] = LET_L;
        arr[1] = LET_O;
        arr[2] = SEG_F | SEG_E | SEG_D | SEG_C;
        arr[3] = SEG_E | SEG_D | SEG_C | SEG_B;
      break;

      //MED
      case 1:
        arr[0] = SEG_E | SEG_F | SEG_A | SEG_B;
        arr[1] = SEG_F | SEG_A | SEG_B | SEG_C;
        arr[2] = LET_E;
        arr[3] = LET_d;
      break;

      //HIGH
      case 2:
        arr[0] = LET_H;
        arr[1] = 1;
        arr[2] = LET_BLANK;
        arr[3] = LET_LOW;

        // tried to do a "G"
        //arr[2] = SEG_A | SEG_F | SEG_E | SEG_D;
        //arr[3] = SEG_D | SEG_C | SEG_G;
      break;

      //MAX
      case 3:
        arr[0] = SEG_E | SEG_F | SEG_A | SEG_B;
        arr[1] = SEG_F | SEG_A | SEG_B | SEG_C;
        arr[2] = LET_A;
        arr[3] = LET_BLANK; // leave blank
      break;

      default:
        arr[0] = LET_E;
        arr[1] = LET_r;
        arr[2] = LET_r;
        arr[3] = LET_BLANK; // leave blank
      break;        
    }
    m_segment.print(arr, 0);
}

void Config::printInfo()
{
  uint8_t arr[4] = {0,0,0,0};
  //TODO add multiline segment print (aka swap displays)
  m_segment.printhexu16(VERSION, 500);
  delay(500);
  
  if (m_bTalking)
  {
    arr[0] = LET_Y;
    arr[1] = LET_E;
    arr[2] = LET_S;
    arr[3] = LET_BLANK; // leave blank
  }
  else
  {
    arr[0] = LET_BLANK;
    arr[1] = LET_n;
    arr[2] = LET_o;
    arr[3] = LET_BLANK; // leave blank
  }
  
  m_segment.print(arr, 0);
}

int Config::incValue(int i)
{
    return (i +1) % (m_MaxValue+1);
}

int Config::decValue(int i)
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
void Config::processSegmentstate(int iEventUp, int iEventDown)
{
  switch (m_segmentState)
  {
      // check if button to enable config
      case e_segmentState_Idle:
          if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Info;
              printInfo();
              //m_segmentState = e_segmentState_Power;
              //printPower();
              
              m_tLastButton = millis();
          }
      break;

      case e_segmentState_Info:
          if (IS_PRESS(iEventUp) && IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Slat;
              printSlat();
              m_tDebounceTime = m_tDebounceTimeInit;
              m_tLastButton = millis();
          }
          else if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Slat;
              printSlat();
              
              m_tLastButton = millis();
          }
          else
          {
              if( millis() - m_tLastButton >= m_iConfigTimeout/2)
              {
                  m_segmentState = e_segmentState_Slat;
                  printSlat();
                  m_tLastButton = millis();
              }
          }

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
              m_segmentState = e_segmentState_Power;
              printPower();
              m_tDebounceTime = m_tDebounceTimeInit;
              m_tLastButton = millis();
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
                  m_segmentState = e_segmentState_Power;
                  printPower();
                  m_tLastButton = millis();
              }
          }
      break;

      case e_segmentState_Power:
          if (IS_PRESS(iEventUp) && IS_PRESS(iEventDown))
          {
              m_segmentState = e_segmentState_Commit;
//              toggleSegment(false);

//              m_tDebounceTime = m_tDebounceTimeInit;
          }
          else if (IS_PRESS(iEventUp) || IS_PRESS(iEventDown))
          {
            if (IS_PRESS(iEventUp))
              m_u8Power = (m_u8Power+1) % m_u8MaxPower;
            else
              m_u8Power = m_u8Power ? (m_u8Power - 1) : m_u8MaxPower-1;
              
              printPower();
              
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

  }
}
void Config::commitConfig()
{
  m_segmentState = e_segmentState_Idle;
  m_segment.clear();

  m_tDebounceTime = m_tDebounceTimeInit;

  m_eeControl.writeValue((unsigned char)m_iFireControlId);
  m_eeSlat.writeValue((unsigned char)m_iSlatId);
  bool bRadioChanged = m_eePower.writeValue((unsigned char)m_u8Power);

  if (bRadioChanged)
  {
    m_radio.radio().setPALevel(radioPower());
    m_radio.radio().printDetails();
  }
}

int Config::radioPower()
{
  switch(m_u8Power)
  {
    case 0: return RF24_PA_MIN;
    default:
    case 1: return RF24_PA_LOW;
    case 2: return RF24_PA_HIGH;
    case 3: return RF24_PA_MAX;
  }
}

