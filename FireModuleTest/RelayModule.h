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

#ifndef RelayModule_h
#define RelayModule_h

#include <inttypes.h>
#include <Arduino.h>

#define RELAY_ON_EMATCH   50 //ms
#define RELAY_ON_TALON    2100 //ms

class ProcessMessage;

class RelayActiveItem
{
public:
  RelayActiveItem(int i)
  {
    idRelay = i;
    pNext   = NULL;
    pPrev   = NULL;
    tStartTime = millis();
  }
  
  unsigned long tStartTime;
  uint8_t idRelay;
  RelayActiveItem *pNext;
  RelayActiveItem *pPrev;
};

class RelayModule
{
public:
  RelayModule();

  virtual ~RelayModule();

public:
  void setDuration(bool bEMatchMode);

  void init(int *arrPins, int iNumPins, bool bEMatchMode);
  void loop();

private:
  void initPin(int iPin);

  RelayActiveItem* findRelay(uint8_t idRelay);
  RelayActiveItem* addRelay(uint8_t idRelay);
  bool delRelay(uint8_t idRelay);
  bool delRelay(RelayActiveItem* pRelay);
  void checkRelay();

public:
  int relayOn(uint8_t idRelay);
  void relayOff(uint8_t idRelay);
  void toggleAllRelays(int iState);

private:
  int *m_arrRelayPins;
  int m_numRelays;
  unsigned long m_tRelayOnDuration;

  RelayActiveItem *m_lsActiveRelays;
};
//cda

#endif // #ifndef RelayModule_h


