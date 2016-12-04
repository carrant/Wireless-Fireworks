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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "SerialCommand.h"
#include "utils.h"
#include "linkedlist.h"

class FireShowEvent
{
public:
  FireShowEvent(uint32_t u32Time, uint8_t u8Slat, uint8_t u8Cue)
    { m_u32Time = u32Time; m_u8Slat = u8Slat; m_u8Cue = u8Cue; }

  bool operator==(FireShowEvent& rhs)
    { return operator==(&rhs); }

  bool operator==(FireShowEvent *rhs)
      { return m_u8Slat == rhs->m_u8Slat && m_u8Cue == rhs->m_u8Cue; }


  uint32_t m_u32Time;       // time the event is to fire
  uint8_t m_u8Slat;         // thought about arrays, we'll see if we get to that point...
  uint8_t m_u8Cue;
};


class FireShow
{
public:
    FireShow(SerialCommand& serial);
    virtual ~FireShow();

public:
    void init();
    void loop();

public:
    bool readFile(const char *pFile);
    bool addEvent(uint32_t u32Time, uint8_t u8Slat, uint8_t u8Cue);
    void printEvents();

public:
    FireShowEvent *findEvent(uint8_t u8Slat, uint8_t u8Cue);

//    bool removeEvent(uint8_t u8Slat);
//    bool removeEvent(uint8_t u8Slat, uint8_t u8Cue);

private:
    linkedlist<FireShowEvent *> m_lsEvents;

    SerialCommand& m_serial;
};
