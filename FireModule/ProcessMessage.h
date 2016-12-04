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

#ifndef ProcessMessage_h
#define ProcessMessage_h

#include <Arduino.h>

#define USE_SEGMENT_CONFIG
#define USE_SEGMENT

#include "RadioComm.h"
#include "linkedlist.h"
#include "Config.h"
#include "Segment.h"

#define NUM_CUES  18
#define SLAT_BROADCAST              0xfe

class RelayModule;

#define FW_MSG_RESEND_TIMEOUT       2 * 1000 //2000ms
#define FW_MSG_STATION_FCM          0xff

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
#define FW_MSG_CMD_FIRE_ALL         10  // Fire all queues

//stripped down messaging, out of space on nano and uno!

//32 bytes
typedef struct 
{
  uint16_t  u16Pin;         // 2  Security pin
  uint8_t   u8Size;         // 1  message size
  uint16_t  u16SendSeq;     // 2  Message sequence number
  uint16_t  u16RecvSeq;     // 2  Message sequence number
  uint16_t  u16Cmd;         // 2  Message command
  uint8_t   u8ControlId;    // 1  Fire group control id
  uint8_t   u8SlatId;       // 1  Slate ID (0xff for fire control module)
  
} FireworksMessageBase;

typedef struct
{
  FireworksMessageBase base;  // 11 bytes
  uint16_t u16AckSeq;         // 2  sequence number to ack
} FireworksMessageAck;


typedef struct
{
  FireworksMessageBase base;  // 11 bytes
  uint8_t u8NumCues;          // 1 Total number of cues on this slate
  uint16_t u16CueStateLo;     // bit array of active cues 0 - fired, 1 - ready   1-16
  uint16_t u16CueStateHi;     // bit array of active cues 0 - fired, 1 - ready  17-32
} FireworksMessageHelloModule;

typedef struct
{
  FireworksMessageBase base;  // 11 bytes
} FireworksMessageHelloControl;

typedef struct
{
  FireworksMessageBase base;  // 11 bytes
  uint16_t u16CueStateLo;     // bit array of active cues 0 - fired, 1 - ready   1-16
  uint16_t u16CueStateHi;     // bit array of active cues 0 - fired, 1 - ready  17-32
} FireworksMessageFire;

typedef struct
{
  FireworksMessageBase base;  // 9 bytes
  uint8_t u8NumCues;          // 1 Total number of cues in list that fired 
  uint8_t arrCues[1];         // variably sized array of cues to fire
} FireworksMessageFired;

typedef struct
{
  FireworksMessageBase *pMsg;
  uint16_t u16ErrorCode;
  uint8_t arrData[1]; // extra error data
} FireworksMessageError;


typedef struct 
{
  FireworksMessageBase *pMsg;
  uint32_t tTimestamp;
  int32_t  i32ResendCount;
  uint32_t u32Timeout;
} MessageAckItem;

class ProcessMessage
{
public:
  ProcessMessage(RadioComm& radio, RelayModule& relay, Segment &segment, Config &cfg);
  virtual ~ProcessMessage();

public:
  void init(uint16_t u16Pin);
  uint16_t loop(uint8_t *pu8Data, uint8_t u8Size);

public:
  void setPin(uint16_t u16Pin);
  

private:  
  void processAck(FireworksMessageAck *pMsg, uint8_t u8Size);
  void processHelloControl(FireworksMessageHelloControl *pMsg, uint8_t u8Size);
  void processFire(FireworksMessageFire *pMsg, uint8_t u8Size);
  void processResetCues(FireworksMessageBase *pMsg, uint8_t u8Size);
  void processResetBoard(FireworksMessageBase *pMsg, uint8_t u8Size);
  void processFireAll(FireworksMessageBase *pMsg, uint8_t u8Size);
  
public:
  FireworksMessageBase* allocateMessage(uint8_t u8Size=RADIO_FRAME_SIZE);
  bool sendAck(uint16_t u16SeqToAck);
  bool sendHelloModule();
  bool sendFired(uint8_t *arrCue, uint8_t u8Size);
  bool sendGoodbye(uint8_t u8ControlId, uint8_t u8SlatId);

  void checkChange();

  bool talking() { return m_bRecvFirstMsg; }
  
private:
  RadioComm&    m_radio;
  RelayModule&  m_relay;
  Config&       m_config;
  Segment&      m_segment;
  uint16_t      m_u16MySeq;
  uint16_t      m_u16OtherSeq;

  uint16_t      m_u16Pin;

  uint32_t      m_tHelloInterval;
  uint32_t      m_tLastHello;

  uint16_t      m_u16CueStateLo; //  1-16
  uint16_t      m_u16CueStateHi; // 17-32

  uint8_t       m_u8PrevSlatId;
  uint8_t       m_u8PrevControlId;
  bool          m_bRecvFirstMsg;
};

#endif // #ifndef ProcessMessage_h


