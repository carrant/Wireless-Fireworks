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

#include "ManagerSlat.h"
  
ManagerSlat::ManagerSlat()
{
  
}

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
  return true;
}

ListItemSlat *ManagerSlat::findSlat(uint8_t u8SlatId)
{
  LogDebug("Look for slat %d", u8SlatId);
  linkedlistitem<ListItemSlat *> *pCur;
  
  m_lsSlats.first();
  while(!m_lsSlats.end())
  {
    pCur = m_lsSlats.next();

  LogDebug("CURR slat %d %p %p", pCur->m_item->slatId(), pCur->m_item, pCur);
    if (pCur->m_item->slatId() == u8SlatId)
      return pCur->m_item;
  }

  LogDebug("Slat %d not found", u8SlatId);
  return NULL;
  
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

