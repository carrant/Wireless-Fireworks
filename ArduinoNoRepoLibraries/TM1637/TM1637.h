//  Author:Fred.Chu
//  Date:9 April,2013
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Modified record:
//
/*******************************************************************************/

#ifndef TM1637_h
#define TM1637_h
#include <inttypes.h>
#include <Arduino.h>
//************definitions for TM1637*********************
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44

#define STARTADDR  0xc0 
/**** definitions for the clock point of the digit tube *******/
#define POINT_ON   1
#define POINT_OFF  0
/**************definitions for brightness***********************/
#define  BRIGHT_DARKEST 0
#define  BRIGHT_TYPICAL 2
#define  BRIGHTEST      7

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
 */
// for display_raw
#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000

#define SEG_MODE   0b10000000 // Enter segment mode (i.e. SEG_A is the top line, not '1')

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
#define LET_Y 4
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
class TM1637
{
  public:
    uint8_t Cmd_SetData;
    uint8_t Cmd_SetAddr;
    uint8_t Cmd_DispCtrl;
    boolean _PointFlag;     //_PointFlag=1:the clock point on
    TM1637(uint8_t, uint8_t);
    void init(void);        //To clear the display
    void writeByte(int8_t wr_data);//write 8bit data to tm1637
    void start(void);//send start bits
    void stop(void); //send stop bits
    void display(int8_t DispData[]);
    void display(uint8_t BitAddr,int8_t DispData);
    void display_raw(uint8_t BitAddr,int8_t DispData);
    void clearDisplay(void);
    void set(uint8_t = BRIGHT_TYPICAL,uint8_t = 0x40,uint8_t = 0xc0);//To take effect the next time it displays.
    void point(boolean PointFlag);//whether to light the clock point ":".To take effect the next time it displays.
    void coding(int8_t DispData[]); 
    int8_t coding(int8_t DispData); 
  private:
    uint8_t Clkpin;
    uint8_t Datapin;
};
#endif
