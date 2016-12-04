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

#include <ButtonHandler.h>

#include <TM1637.h>

#ifndef Segment_h
#define Segment_h

#include <inttypes.h>
#include <Arduino.h>

#include <TM1637.h>


/*

 * #define SEG_MODE   0b10000000 // Enter segment mode (i.e. SEG_A is the top line, not '1')
/*    
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
// for display_raw
#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000

#define LET_BLANK   0x7f // space, empty, none
#define LET_BRACKET_LEFT LET_C
#define LET_BRACKET_RIGHT (SEG_MODE | SEG_A | SEG_B | SEG_C | SEG_D)
#define LET_EQUAL  (SEG_MODE | SEG_G | SEG_D)
#define LET_HIGH   (SEG_MODE | SEG_A)
#define LET_MED    (SEG_MODE | SEG_G)
#define LET_LOW    (SEG_MODE | SEG_D)
#define LET_MINUS  (SEG_MODE | SEG_G)
#define LET_DASH   LET_MINUS
#define LET_LEFT1  (SEG_MODE | SEG_F)
#define LET_LEFT2  (SEG_MODE | SEG_F | SEG_E)
#define LET_RIGHT1 (SEG_MODE | SEG_B)
#define LET_RIGHT2 (SEG_MODE | SEG_B | SEG_C)
#define LET_A (SEG_MODE | SEG_A | SEG_F | SEG_E | SEG_G | SEG_B | SEG_C)
#define LET_C (SEG_MODE | SEG_A | SEG_F | SEG_E | SEG_D)
#define LET_E (SEG_MODE | SEG_A | SEG_F | SEG_G | SEG_E | SEG_D)
#define LET_F (SEG_MODE | SEG_A | SEG_F | SEG_E | SEG_E | SEG_G)
#define LET_H (SEG_MODE | SEG_F | SEG_E | SEG_G | SEG_B | SEG_C)
#define LET_J (SEG_MODE | SEG_B | SEG_C | SEG_D)
#define LET_L (SEG_MODE | SEG_F | SEG_E | SEG_D)
#define LET_N (SEG_MODE | SEG_A | SEG_F | SEG_B | SEG_E | SEG_C)
#define LET_O (SEG_MODE | SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define LET_P (SEG_MODE | SEG_A | SEG_B | SEG_F | SEG_E | SEG_G)
#define LET_S (SEG_MODE | SEG_A | SEG_F | SEG_G | SEG_C | SEG_D)
#define LET_U (SEG_MODE | SEG_F | SEG_E | SEG_D | SEG_C | SEG_B)
#define LET_Z (SEG_MODE | SEG_A | SEG_B | SEG_G | SEG_E | SEG_D)


#define LET_b (SEG_MODE | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G)
#define LET_c (SEG_MODE | SEG_G | SEG_E | SEG_D)
#define LET_d (SEG_MODE | SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)
#define LET_h (SEG_MODE | SEG_F | SEG_E | SEG_G | SEG_C)
#define LET_l (SEG_MODE | SEG_F | SEG_E)
#define LET_n (SEG_MODE | SEG_E | SEG_G | SEG_C)
#define LET_o (SEG_MODE | SEG_G | SEG_E | SEG_D | SEG_C)
#define LET_r (SEG_MODE | SEG_E | SEG_G)
#define LET_u (SEG_MODE | SEG_E | SEG_D | SEG_C)

 */
class Segment
{
public:  
  Segment(TM1637& t);
  virtual ~Segment();

public:
  void init();
  void loop();

public:
  void clear();
  void print(uint8_t *arr, unsigned long tTimeout);
  void printstr(char *p, unsigned long tTimeout);
  void flash(uint8_t *arr, unsigned long tFlashInterval, unsigned long tTimeout);
  void flashstr(char *p, unsigned long tFlashInterval, unsigned long tTimeout);

  void printu8(uint8_t v,unsigned long tTimeout);
  void printhexu8(uint8_t v,unsigned long tTimeout);
  void printhexu16(uint16_t v,unsigned long tTimeout);

  void formatu8(uint8_t *pdest, uint8_t v);
  void formathexu8(uint8_t *pdest, uint8_t v);
  void formathexu16(uint8_t *pdest, uint16_t v);

  uint8_t charToSegment(char c);

  void setBrightness(uint8_t u8);
public:

  bool isIdle() { return m_state == e_segmentState_Idle;}

private:
  typedef enum  
  {
      e_segmentState_Idle,                // nothing on segment
      e_segmentState_Displaying,          // setting slat within controller group
      e_segmentState_Flashing_On,         // setting fire controller id
      e_segmentState_Flashing_Off         // setting fire controller id
  } e_segmentState;

  e_segmentState m_state;
  void printSegment(uint8_t *arr);
  void toggleSegment(bool bOn);

private:
  TM1637&           m_tm1637;
  int               m_iConfigTimeout;

  unsigned long     m_tFinishTime;
  unsigned long     m_tFlashTime;
  unsigned long     m_tFlashInterval;
  uint8_t           m_arrFlash[4];
};

#endif // #ifndef Segment_h


