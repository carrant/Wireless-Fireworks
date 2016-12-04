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

#ifndef ProcessMessage_h
#define ProcessMessage_h

#include <Arduino.h>

#include "RadioComm.h"
#include "linkedlist.h"
#ifdef USE_SEGMENT
  #include "SegmentConfig.h"
#endif

#include "ManagerSlat.h"
#include "SerialCommand.h"

class RelayModule;

#define FW_MSG_RESEND_TIMEOUT       2 * 1000 //2000ms
#define FW_MSG_STATION_FCM    0xff

#define FW_MSG_CMD_ACK              0   // Acknowledgement of message
#define FW_MSG_CMD_HELLO_MODULE     1   // Hello, are you there? from module to contoller
#define FW_MSG_CMD_HELLO_CONTROL    2   // Hello, are you there? from controller to module
#define FW_MSG_CMD_FIRE             3   // Fire specified channel from controller to module
#define FW_MSG_CMD_FIRED            4   // Fire specified channel from module to controller
#define FW_MSG_CMD_ERROR            5   // Error!

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
  uint8_t arrCues[1];         // bit array of active cues variably sized bases on number of cues
} FireworksMessageHelloModule;

typedef struct
{
  FireworksMessageBase base;  // 11 bytes
} FireworksMessageHelloControl;

typedef struct
{
  FireworksMessageBase base;  // 11 bytes
  uint8_t u8NumCues;          // 1 Total number of cues in list to fire 
  uint8_t arrCues[1];         // variably sized array of cues to fire
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
  ProcessMessage(RadioComm& radio, ManagerSlat& manager, SerialCommand& serial
#ifdef USE_SEGMENT
    , SegmentConfig &segment
#endif
  );
  virtual ~ProcessMessage();

public:
  void init();
  void loop(uint8_t *pu8Data, uint8_t u8Size);

public:
  void setControlId(uint8_t u8ControlId);
  void setPin(uint16_t u16Pin);
  

private:  
  void processAck(FireworksMessageAck *pMsg, uint8_t u8Size);
  void processHelloModule(FireworksMessageHelloModule *pMsg, uint8_t u8Size);
  void processFired(FireworksMessageFired *pMsg, uint8_t u8Size);

public:
  FireworksMessageBase* allocateMessage(uint8_t u8Size=RADIO_FRAME_SIZE);
  bool sendAck(uint16_t u16SeqToAck);
  bool sendHelloControl();
  bool sendFire(uint8_t *arrCue, uint8_t u8Size);

private:
  void clearAckList();
  
private:
  RadioComm& m_radio;
#ifdef USE_SEGMENT
  SegmentConfig &m_segment;
#endif  
  ManagerSlat& m_manager;
  SerialCommand& m_serial;
  
  uint16_t  m_u16MySeq;
  uint16_t  m_u16OtherSeq;

  linkedlist<MessageAckItem *> m_lsPendingAck;
};

#endif // #ifndef ProcessMessage_h


