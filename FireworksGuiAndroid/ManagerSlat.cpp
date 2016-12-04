/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ManagerSlat.h"
#include "mainwindow.h"

#ifndef ARDUINO
ManagerSlat::ManagerSlat(MainWindow *pMain)
{
    m_pMain = pMain;
}
#else
ManagerSlat::ManagerSlat()
{

}
#endif

ManagerSlat::~ManagerSlat()
{
  ListItemSlat *pSlat = NULL;
  while(m_lsSlats.count())
  {
    pSlat = m_lsSlats.removeHead();
    delete pSlat;
  }

}


bool ManagerSlat::addSlat(uint8_t u8SlatId, uint8_t u8NumCues)
{
  ListItemSlat *pSlat = new ListItemSlat(u8SlatId, u8NumCues);
  if (m_lsSlats.findpointers(pSlat))
  {
    LogError("ERROR Slat %d already exists!", u8SlatId);
    delete pSlat;
    return true;
  }

  m_lsSlats.addTail(pSlat);

  m_pMain->addSlat(u8SlatId);

  return true;
}

ListItemSlat *ManagerSlat::findSlat(uint8_t u8SlatId)
{
//  LogDebug("Look for slat %d", u8SlatId);
  linkedlistitem<ListItemSlat *> *pCur;
  
  m_lsSlats.first();
  while(!m_lsSlats.end())
  {
    pCur = m_lsSlats.next();

    if (pCur->m_item->slatId() == u8SlatId)
      return pCur->m_item;
  }

  LogDebug("Slat %d not found", u8SlatId);
  return NULL;
  
}

void ManagerSlat::setStatusSlatCue(uint8_t u8Slat, uint8_t u8Cue, ListItemSlat::eCueStatus eStatus)
{
  ListItemSlat *pSlat = findSlat(u8Slat);
  if (!pSlat)
  {
    LogError("Unable to find slat %d", u8Slat);
    return;
  }

  pSlat->statusCues()[u8Cue] = eStatus;

#ifndef ARDUINO
  switch(eStatus)
  {
  case ListItemSlat::e_CueStatus_Unknown:
      return;
  case ListItemSlat::e_CueStatus_Armed:
      m_pMain->readyCue(u8Slat, u8Cue);
      break;

  case ListItemSlat::e_CueStatus_Firing:
      m_pMain->fireCue(u8Slat, u8Cue);
      break;

    case ListItemSlat::e_CueStatus_Fired:
      m_pMain->idleCue(u8Slat, u8Cue);
      break;
  }

#endif
}

void ManagerSlat::setStatusSlatCue(uint8_t u8Slat, uint8_t *arrCues, uint8_t u8NumCues, ListItemSlat::eCueStatus eStatus)
{
  ListItemSlat *pSlat = findSlat(u8Slat);
  if (!pSlat)
  {
    LogError("Unable to find slat %d", u8Slat);
    return;
  }

  pSlat->setStatusCue(arrCues, u8NumCues, eStatus);
}

