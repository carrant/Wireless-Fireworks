/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGui.

    FireworksGui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGui.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Messages_h
#define Messages_h

#define NUM_CUES  18
#define SLAT_BROADCAST              0xfe

#define FW_MSG_CMD_ACK              0   // Acknowledgement of message
#define FW_MSG_CMD_HELLO_MODULE     1   // Hello, are you there? from module to contoller
#define FW_MSG_CMD_HELLO_CONTROL    2   // Hello, are you there? from controller to module
#define FW_MSG_CMD_FIRE             3   // Fire specified channel from controller to module
#define FW_MSG_CMD_FIRED            4   // Fire specified channel from module to controller
#define FW_MSG_CMD_ERROR            5   // Error!
#define FW_MSG_CMD_GOODBYE          6   // Change of control or slat id
#define FW_MSG_CMD_SET_SIGNAL_STR   7   // Set local signal strength  (ONLY for slave controller, NOT for slats)
#define FW_MSG_CMD_RESET_CUES       8   // Reset all cues for a slat
#define FW_MSG_CMD_RESET_BOARD      9   // Reset board
#define FW_MSG_CMD_FIRE_ALL         10  // Reset board

//stripped down messaging, out of space on nano and uno!

//32 bytes
typedef struct __attribute__((__packed__))
{
  uint16_t  u16Pin;         // 2  Security pin
  uint8_t   u8Size;         // 1  message size
  uint16_t  u16SendSeq;     // 2  Message sequence number
  uint16_t  u16RecvSeq;     // 2  Message sequence number
  uint16_t  u16Cmd;         // 2  Message command
  uint8_t   u8ControlId;    // 1  Fire group control id
  uint8_t   u8SlatId;       // 1  Slate ID (0xff for fire control module)
  
} FireworksMessageBase;

typedef struct __attribute__((__packed__))
{
  FireworksMessageBase base;  // 11 bytes
  uint16_t u16AckSeq;         // 2  sequence number to ack
} FireworksMessageAck;


typedef struct __attribute__((__packed__))
{
  FireworksMessageBase base;  // 11 bytes
  uint8_t u8NumCues;          // 1 Total number of cues on this slate
  uint32_t u32CueState;         // bit array of active cues 0 - fired, 1 - ready
} FireworksMessageHelloModule;

typedef struct __attribute__((__packed__))
{
  FireworksMessageBase base;  // 11 bytes
} FireworksMessageHelloControl;

typedef struct __attribute__((__packed__))
{
  FireworksMessageBase base;  // 11 bytes
  uint32_t u32Cues;             // bit array of cues to fire 1 = fire
} FireworksMessageFire;

typedef struct __attribute__((__packed__))
{
  FireworksMessageBase base;  // 9 bytes
  uint8_t u8NumCues;          // 1 Total number of cues in list that fired 
  uint8_t arrCues[1];         // variably sized array of cues to fire
} FireworksMessageFired;

typedef struct __attribute__((__packed__))
{
  FireworksMessageBase *pMsg;
  uint16_t u16ErrorCode;
  uint8_t arrData[1]; // extra error data
} FireworksMessageError;

#endif // #ifndef Messages_h

