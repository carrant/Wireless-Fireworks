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

#include "Segment.h"
#include "utils.h"

Segment::Segment(TM1637& t) 
    : m_tm1637(t)
{
  m_state           = e_segmentState_Idle;
  m_tFinishTime     = millis();
  m_tFlashInterval  = 0;
  
  m_tm1637.point(false);
}
  
Segment::~Segment()
{
}


void Segment::init()
{
  setBrightness(2);
  clear();
}

void Segment::loop()
{
  switch( m_state)
  {
    case e_segmentState_Idle:
      break;

    case e_segmentState_Displaying:
      if (m_tFinishTime && millis() >= m_tFinishTime )
        toggleSegment(false);
    break;

    case e_segmentState_Flashing_On:
      if (m_tFinishTime && millis() >= m_tFinishTime )
        toggleSegment(false);
      else if (millis() >= m_tFlashTime )
      {
        clear();
        m_state = e_segmentState_Flashing_Off;
        m_tFlashTime = millis() + m_tFlashInterval;
      }
    break;
    
    case e_segmentState_Flashing_Off:
      if (m_tFinishTime && millis() >= m_tFinishTime )
        toggleSegment(false);
      else if (millis() >= m_tFlashTime )
      {
        toggleSegment(true);
        printSegment(m_arrFlash);
        m_state = e_segmentState_Flashing_On;
        m_tFlashTime = millis() + m_tFlashInterval;
      }
    break;
  }
}

void Segment::clear()
{
  m_tm1637.clearDisplay();
  toggleSegment(false);
}


void Segment::printSegment(uint8_t *arr)
{
  for (int x=0; x < 4; x++)
  {
    LogDebug("%d %02X bSegMode %d u8 %02x", x, arr[x], (arr[x] & SEG_MODE) != 0, arr[x] & ~SEG_MODE);
    bool bSegMode = (arr[x] & SEG_MODE) != 0;
    uint8_t u8 = arr[x] & ~SEG_MODE;

    if (u8 == LET_BLANK)
        m_tm1637.display(x, 0x7f);
    else if (bSegMode)
    {
      m_tm1637.display_raw(x, u8);
    }
    else if (u8 >= 0 && u8 <= 9)
      m_tm1637.display(x, u8);
    else
      m_tm1637.display_raw(x, u8);
  }
}

void Segment::toggleSegment(bool bOn)
{
  if (bOn == false)
  {
      m_tm1637.clearDisplay();
      m_state = e_segmentState_Idle;
  }
  else
      m_state = e_segmentState_Displaying;
  
}

void Segment::flash(uint8_t *arr, unsigned long tFlashInterval, unsigned long tTimeout)
{
  print(arr, tTimeout);

  memcpy(m_arrFlash, arr, 4);
  m_tFlashInterval = tFlashInterval;
  m_tFlashTime = millis() + m_tFlashInterval;
  m_state = e_segmentState_Flashing_On;
}

void Segment::flashstr(char *p, unsigned long tFlashInterval, unsigned long tTimeout)
{
  uint8_t arr[4];
  for (int x=0; x < 4; x++)
    arr[x] = charToSegment(p[x]);

  flash(arr,tFlashInterval,tTimeout);
}

void Segment::print(uint8_t *arr, unsigned long tTimeout)
{
  toggleSegment(true);
  printSegment(arr);

  if (tTimeout)
    m_tFinishTime = millis() + tTimeout;
  else
    m_tFinishTime = 0;
    
  LogDebug("%d %d %d %d", arr[0], arr[1], arr[2], arr[3]);
  m_state = e_segmentState_Displaying;
  
}

void Segment::printstr(char *p, unsigned long tTimeout)
{
  uint8_t arr[4];
  for (int x=0; x < 4; x++)
    arr[x] = charToSegment(p[x]);

  print(arr,tTimeout);
}

void Segment::printu8(uint8_t v, unsigned long tTimeout)
{
  uint8_t arr[4] = {0,0,0,0};
  if (v > 99)
    formatu8(arr+1,v);
  else
    formatu8(arr+2,v);
  print(arr, tTimeout);
}

void Segment::printhexu8(uint8_t v, unsigned long tTimeout)
{
  uint8_t arr[4] = {0,0,0,0};
  formathexu8(arr+2,v);
  print(arr, tTimeout);
  
}

void Segment::printhexu16(uint16_t v, unsigned long tTimeout)
{
  uint8_t arr[4] = {0,0,0,0};
  formathexu16(arr,v);
  print(arr, tTimeout);
  
}

void Segment::formatu8(uint8_t *pdest, uint8_t v)
{
  if (v > 99)
  {
    *pdest++ = v / 100;
    v = v - ( (v/100) * 100);
  }
  
  *pdest++ = v / 10;
  v = v - ( (v/10) * 10);

  *pdest = v;
}

void Segment::formathexu8(uint8_t *pdest, uint8_t v)
{
  char s[5];
  sprintf(s,"%02X",v);
  for (int x=0; x < 2; x++)
    if (s[x] == 'B')
      s[x] = 'b';
    else if (s[x] == 'D')
      s[x] = 'd';
      
  *pdest++ = charToSegment(s[0]);
  *pdest = charToSegment(s[1]);
}

void Segment::formathexu16(uint8_t *pdest, uint16_t v)
{
  char s[5];
  sprintf(s,"%04X",v);
  for (int x=0; x < 4; x++)
    if (s[x] == 'B')
      s[x] = 'b';
    else if (s[x] == 'D')
      s[x] = 'd';
      
  *pdest++ = charToSegment(s[0]);
  *pdest++ = charToSegment(s[1]);
  *pdest++ = charToSegment(s[2]);
  *pdest++ = charToSegment(s[3]);
}
/*
 * 
 *       A
 *    -------
 *    |     |
 *   F|     |B
 *    |     |
 *    ---G---
 *    |     |
 *   E|     |C
 *    |     |
 *    -------
 *       D
#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
 */
uint8_t Segment::charToSegment(char c)
{
  switch(c)
  {
    case ' ': return LET_BLANK;
    case '-': return LET_DASH;
    case '_': return LET_LOW;
    case '=': return LET_EQUAL;
    case '[': return LET_BRACKET_LEFT;
    case ']': return LET_BRACKET_RIGHT;
    
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    
    case 'A': return LET_A;
    case 'B': return 8;
    case 'C': return LET_C;
    case 'D': return 0;
    case 'E': return LET_E;
    case 'F': return LET_F;
    case 'H': return LET_H;
    case 'J': return LET_J;
    case 'L': return LET_L;
    case 'N': return LET_N;
    case 'O': return LET_O;
    case 'P': return LET_P;
    case 'S': return LET_S;
    case 'U': return LET_U;
    case 'Y': return LET_Y; //actually a 4...
    case 'Z': return LET_Z;

    case 'b': return LET_b;
    case 'c': return LET_c;
    case 'd': return LET_d;
    case 'h': return LET_h;
    case 'l': return LET_l;
    case 'o': return LET_o;
    case 'r': return LET_r;
    case 'u': return LET_u;
  
    
    case 'a': 
    case 'e':
    case 'f':
    case 'g':
    case 'i': 
    case 'j':
    case 'k':
    case 'm':
    case 'n':
    case 'p':
    case 'q':
    case 's': 
    case 't':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'G': 
    case 'I': 
    case 'K': 
    case 'M': 
    case 'Q': 
    case 'R': 
    case 'T':
    case 'V': 
    case 'W': 
    case 'X': 
    default:
      return 0;
  }
}

void Segment::setBrightness(uint8_t u8)
{
  if (u8 > BRIGHTEST)
    u8 = BRIGHTEST;
    
  m_tm1637.set(u8);
}

