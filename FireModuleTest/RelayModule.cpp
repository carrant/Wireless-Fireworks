/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireModuleTest.

    FireModuleTest is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireModuleTest is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireModuleTest.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RelayModule.h"
#include "utils.h"

RelayModule::RelayModule()
{
  m_lsActiveRelays  = NULL;
  m_arrRelayPins    = NULL;
  m_numRelays       = 0;
  m_tRelayOnDuration= 0;
}

RelayModule::~RelayModule()
{
  RelayActiveItem *pTemp = NULL ;
  while(m_lsActiveRelays)
  {
    pTemp = m_lsActiveRelays;
    m_lsActiveRelays = m_lsActiveRelays->pNext;
    
    delete pTemp;
  }
  
  delete m_arrRelayPins;
}

void RelayModule::setDuration(bool bEMatchMode)
{
  if (bEMatchMode)
    m_tRelayOnDuration = RELAY_ON_EMATCH; // ms
   else
    m_tRelayOnDuration = RELAY_ON_TALON; //ms
}

void RelayModule::init(int *arrPins, int iNumPins, bool bEMatchMode)
{
  m_lsActiveRelays  = NULL;
  m_arrRelayPins    = new int[iNumPins];
  m_numRelays       = iNumPins;
  memcpy(m_arrRelayPins, arrPins, sizeof(int) * iNumPins);

  setDuration(bEMatchMode);
  
  for(int x=0; x < m_numRelays; x++)
    initPin(m_arrRelayPins[x]);  
  
}
void RelayModule::loop()
{
  checkRelay();
}

void RelayModule::initPin(int iPin)
{
  if (iPin == -1)
    return;
    
  pinMode(iPin, OUTPUT);
  digitalWrite(iPin,HIGH);
}


RelayActiveItem* RelayModule::findRelay(uint8_t idRelay)
{
  if (!m_lsActiveRelays)
    return NULL;

  RelayActiveItem *pCur = m_lsActiveRelays;

  while(pCur)
  {
    if (pCur->idRelay == idRelay)
      return pCur;

    pCur = pCur->pNext;
  }

  return NULL;
}

RelayActiveItem* RelayModule::addRelay(uint8_t idRelay)
{
  LogDebug("idRelay %d", idRelay);
  
  RelayActiveItem *pNew = findRelay(idRelay);
  if (pNew)
  {
    LogDebug("idRelay %d Already exists in list, updating time", idRelay);
    
    pNew->tStartTime = millis();
    return pNew;
  }

  LogDebug("idRelay %d Creating new relay", idRelay);
  pNew = new RelayActiveItem(idRelay);

  if (m_lsActiveRelays)
  {
    RelayActiveItem *pCur = m_lsActiveRelays;
    while(pCur->pNext)
      pCur = pCur->pNext;

    LogDebug("idRelay %d Adding relay to end of list", idRelay);
      
    pCur->pNext = pNew;
    pNew->pPrev = pCur;
  }
  else
  {
    LogDebug("idRelay %d Add to head", idRelay);
    m_lsActiveRelays = pNew;
  }
}

bool RelayModule::delRelay(uint8_t idRelay)
{
  LogDebug("idRelay %d", idRelay);
  RelayActiveItem *pRelay = findRelay(idRelay);

  return delRelay(pRelay);
}

bool RelayModule::delRelay(RelayActiveItem *pRelay)
{
  if (!pRelay)
    return false;
    
  LogDebug("idRelay %d", pRelay->idRelay);

  if (pRelay == m_lsActiveRelays)
  {
    LogDebug("idRelay %d Head of list", pRelay->idRelay);
    
    m_lsActiveRelays = m_lsActiveRelays->pNext;
    m_lsActiveRelays->pPrev = NULL;
    delete pRelay;
  }
  else
  {
    LogDebug("idRelay %d Middle of list", pRelay->idRelay);
    
    pRelay->pPrev->pNext = pRelay->pNext;
    pRelay->pNext->pPrev = pRelay->pPrev;
    delete pRelay;
  }

  return true;
}

void RelayModule::checkRelay()
{
  if (!m_lsActiveRelays)
    return;
    
  RelayActiveItem *pCur = m_lsActiveRelays;
  unsigned long tTime = millis();
  
  while(pCur)
  {
    if ((tTime - pCur->tStartTime) > m_tRelayOnDuration)
    {
      uint8_t arridRelayFired[] = { pCur->idRelay };
      LogDebug("idRelay %d del relay from list", pCur->idRelay);
      
      relayOff(pCur->idRelay);

      RelayActiveItem *pTemp = pCur->pNext;
      delRelay(pCur->idRelay);
      pCur = pTemp;

        
      LogDebug("head %p pCur %p", m_lsActiveRelays, pCur);
    }
    else
      return;
  }
  
}

int RelayModule::relayOn(uint8_t idRelay)
{
  if (idRelay < 0 || idRelay > m_numRelays )
    return 0;

  LogDebug("idRelay %2d, pin %d", idRelay, m_arrRelayPins[idRelay]);
    
  digitalWrite(m_arrRelayPins[idRelay],LOW);           // Turns ON Relays 1
  addRelay(idRelay);
  return m_arrRelayPins[idRelay]; //for debugging
}

void RelayModule::relayOff(uint8_t idRelay)
{
  if (idRelay < 0 || idRelay > m_numRelays )
    return;
    
  LogDebug("idRelay %2d, pin %d", idRelay, m_arrRelayPins[idRelay]);
  digitalWrite(m_arrRelayPins[idRelay],HIGH);           // Turns ON Relays 1
}

void RelayModule::toggleAllRelays(int iState)
{
  for(int x=0; x < m_numRelays; x++)
  {
    LogDebug("Toggle Relay %2d pin %2d %s", x, m_arrRelayPins[x], iState == LOW ? "LOW" : "HIGH");
    if (iState == LOW)
      relayOn(x);
    else
      relayOff(x);
  }
}

