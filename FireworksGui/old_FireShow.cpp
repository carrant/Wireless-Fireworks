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

#include "FireShow.h"

FireShow::FireShow(SerialCommand& serial)
    : m_serial(serial)
{
}

FireShow::~FireShow()
{
  FireShowEvent *pEvent = NULL;
  while(m_lsEvents.count())
  {
    pEvent = m_lsEvents.removeHead();
    delete pEvent;
  }
}

bool FireShow::readFile(const char *pFile)
{
  FILE *f = fopen(pFile, "r");
  if (!f)
    return false;

  char sLine[1024];

  int32_t u32Time=0;
  uint8_t u8Slat = 0;
  uint8_t u8Cue = 0;

  while(fgets(sLine, 1024, f) != NULL)
  {
    sscanf(sLine, "%d,%d,%d", &u32Time, (int *)&u8Slat, (int *)&u8Cue);
    addEvent(u32Time, u8Slat, u8Cue);
  }

  return true;
}

void FireShow::init()
{
}

void FireShow::loop()
{
  uint32_t u32CurrentTime = millis() / 1000; //seconds

/*
static uint32_t ols = 0;
if (ols == 0)
 ols = u32CurrentTime;
else
{
    if (u32CurrentTime != ols)
    {
        ols = u32CurrentTime;
        printf("time %d %d\n", ols, m_lsEvents.count());
      printEvents();
    }
}
*/

  FireShowEvent *pCur = NULL;
  while(m_lsEvents.count())
  {
    pCur = m_lsEvents.removeHead();
    if (u32CurrentTime >= pCur->m_u32Time)
    {
      LogDebug("Send fire u8Slat=%d u8Cue=%d", pCur->m_u8Slat, pCur->m_u8Cue);
     // m_serial.sendCmdFire(pCur->m_u8Slat, pCur->m_u8Cue);
      delete pCur;
    }
    else
    {
      m_lsEvents.addHead(pCur);
      return;
    }
  }

}

FireShowEvent *FireShow::findEvent(uint8_t u8Slat, uint8_t u8Cue)
{
  linkedlistitem<FireShowEvent *> *pCur;

  m_lsEvents.first();
  while(!m_lsEvents.end())
  {
    pCur = m_lsEvents.next();

    if (pCur->m_item->m_u8Slat == u8Slat && pCur->m_item->m_u8Cue == u8Cue)
      return pCur->m_item;
  }

  return NULL;
}

bool FireShow::addEvent(uint32_t u32Time, uint8_t u8Slat, uint8_t u8Cue)
{
  if (findEvent(u8Slat, u8Cue))
  {
  LogError("Already in list u8Slat=%d u8Cue=%d", u8Slat, u8Cue);
    return false;
    }

  FireShowEvent *pNew = new FireShowEvent(u32Time, u8Slat, u8Cue);
  linkedlistitem<FireShowEvent *> *pCur;

  if (m_lsEvents.count() == 0)
  {
    m_lsEvents.addHead(pNew);
    return true;
  }

  m_lsEvents.first();
  while(!m_lsEvents.end())
  {
    pCur = m_lsEvents.next();

    if (u32Time < pCur->m_item->m_u32Time)
    {
      m_lsEvents.insertBefore(pCur, pNew);
      return true;
    }
  }

  m_lsEvents.addTail(pNew);
  return true;
}

void FireShow::printEvents()
{
  linkedlistitem<FireShowEvent *> *pCur;

  printf("Time       Slat Cue\n");
        //---------- ---- ----
        //1234567890 1234 1234
  m_lsEvents.first();
  while(!m_lsEvents.end())
  {
    pCur = m_lsEvents.next();
    FireShowEvent *pitem = pCur->m_item;
    if (!pitem)
    {
    printf("no item***************\n");
    }
    printf("%10d %4d %4d\n", pitem->m_u32Time, pitem->m_u8Slat, pitem->m_u8Cue);
  }
}
