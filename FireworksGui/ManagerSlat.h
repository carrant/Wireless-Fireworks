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

#ifndef ManagerSlat_h
#define ManagerSlat_h

#include <stdio.h>
#include <string.h>

#include "linkedlist.h"
#include "utils.h"

#ifndef ARDUINO
class MainWindow;
#endif

// ListItemSlat::eCueStatus
class ListItemSlat
{
public:
  enum  eCueStatus
  {
    e_CueStatus_Unknown=0,
    e_CueStatus_Armed=1,
    e_CueStatus_Firing=3,
    e_CueStatus_Fired=4,
  };

  ListItemSlat(uint8_t u8SlatId, uint8_t u8NumCues)
  {
    m_u8SlatId      = u8SlatId;
    m_u8NumCues     = u8NumCues;
    m_arrStatusCues = NULL;

    createCues();
  }

  virtual ~ListItemSlat()
    { delete m_arrStatusCues; }

  bool operator==(ListItemSlat& rhs)
    { return m_u8SlatId == rhs.m_u8SlatId; }

  eCueStatus& operator[](uint8_t index) 
    { return m_arrStatusCues[index]; }
    
  void createCues()
  {
    if (m_arrStatusCues)
      return;
      
    m_arrStatusCues = new eCueStatus[m_u8NumCues];

    memset(m_arrStatusCues, e_CueStatus_Unknown, sizeof(eCueStatus) * m_u8NumCues);
  }
  
  void setCues(eCueStatus *arrCues)
  {
    createCues();
    memcpy(m_arrStatusCues, arrCues, sizeof(eCueStatus) * m_u8NumCues);
  }

  void setCue(uint8_t iCue, eCueStatus eStatus)
  {
    createCues();

    m_arrStatusCues[iCue] = eStatus;
  }

  void setStatusCue(uint8_t *arrCues, uint8_t u8NumCues, eCueStatus eStatus)
  {
    for(int x=0; x< u8NumCues; x++)
    {
      if (arrCues[x] < numCues())
      {
        m_arrStatusCues[arrCues[x]] = eStatus;
      }
    }
  }

public:
  uint8_t slatId()          { return m_u8SlatId; }
  uint8_t numCues()         { return m_u8NumCues; }
  eCueStatus* statusCues()  { return m_arrStatusCues; }
  
private:
  uint8_t m_u8SlatId;
  uint8_t m_u8NumCues;
  eCueStatus *m_arrStatusCues;
};

class ManagerSlat
{
public:  
#ifndef ARDUINO
    ManagerSlat(MainWindow *pMain);
#else
    ManagerSlat();
#endif
  virtual ~ManagerSlat();

public:
  bool addSlat(uint8_t u8SlatId, uint8_t u8NumCues); 

  ListItemSlat *findSlat(uint8_t u8SlatId);

  void setStatusSlatCue(uint8_t u8Slat, uint8_t u8Cues, ListItemSlat::eCueStatus eStatus);
  void setStatusSlatCue(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues, ListItemSlat::eCueStatus eStatus);
  
public:
  linkedlist<ListItemSlat *> getList() { return m_lsSlats; }
  
  uint8_t controlId() { return m_u8ControlId; }
  uint8_t pin()       { return m_u8Pin; }

  
  void    setControlId(uint8_t u) { m_u8ControlId = u; }
  void    setPin(uint8_t u)       { m_u8Pin = u; }
  
private:
  linkedlist<ListItemSlat *>  m_lsSlats;
  uint8_t                     m_u8ControlId;
  uint8_t                     m_u8Pin;

#ifndef ARDUINO
    MainWindow *m_pMain;
#endif
};

#endif // #ifndef ManagerModule_h


