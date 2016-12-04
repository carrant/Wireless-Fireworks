/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireComputer.

    FireComputer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireComputer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireComputer.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>



// command line csv script player
// used to test, replaced buy QT gui


const char *pSerial0 = "/dev/ttyACM0";
const char *pSerial1 = "/dev/ttyACM1";
const char *pSerial2 = "/dev/ttyACM2";

#include "HardwareSerial.h"
HardwareSerial hwserial;

#include "ManagerSlat.h"
#include "utils.h"
#include "SerialCommand.h"

ManagerSlat manager;
SerialCommand serial(hwserial, manager);

void setup();
void loop();

int kbhit()
{
    int fd = fileno(stdin);
    fd_set readSet;

    FD_ZERO(&readSet);
    FD_SET(fd, &readSet);
    int fdMax = fd;

    //  select should timeout every so often so we can check IsRunning
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    int rc = select(fdMax+1, &readSet, NULL, NULL, &tv);

    //  timeout..
    if (rc == 0)
        return -1;

    //  If select returns a non-recoverable error, exit
    if (rc < 0)
    {
        if (errno == EINTR)
            return -1;

    }

    if (FD_ISSET(fd, &readSet))
    {
        return fgetc(stdin);
/*        
        unsigned char strInput[512]="";
        uint32_t u32InputLen = sizeof(strInput) - 1;
        int n = (int)fread(strInput, 1, u32InputLen, stdin);
        if (n < 0)
            return "";
            
        u32InputLen = (uint32_t)n;
        strInput[u32InputLen] = '\0';

        fflush(stdin);
        
        return strInput;
        */
    }
    
    return -1;
}


bool g_bReadLocalRunning = false;
void *readLocal(void *parg)
{
    char sCommand[1024];
    int iCommandIndex = 0;
    while(g_bReadLocalRunning)
    {
        int ch = fgetc(stdin);
        if (ch == -1)
            continue;
            
        if (ch == '\n' || ch == '\r')
        {
            sCommand[iCommandIndex] = 0;
            
            int iLen = iCommandIndex;
            iCommandIndex =0;
            
            // maybe an empty line
            if (!sCommand[0])
                continue;
              
            serial.sendCmd(sCommand);  
//            printf("command %s\n", sCommand);
        }
        else
            sCommand[iCommandIndex++] = ch;
    }
    
    return 0;
}

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
      m_serial.sendCmdFire(pCur->m_u8Slat, pCur->m_u8Cue);
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


FireShow fireShow(serial);

int main(int argc, char **argv)
{

    fireShow.readFile("showtest.txt");
    fireShow.printEvents();

    LogDebug("Starting!");
    if (!hwserial.init(pSerial0))
    {
        if (!hwserial.init(pSerial1))
        {
            if (!hwserial.init(pSerial2))
            {
                printf("No file %s or %s\n", pSerial1, pSerial2);
                return 1;
            }
        }
    }    
    pthread_t pidLocal;
    g_bReadLocalRunning = true;
    int ret =  pthread_create(&pidLocal, NULL,readLocal, 0);    
    if (ret != 0)
    {
        printf("Error opening local input thread: %s\n", strerror(errno));
        return 1;
    }
    
    // just like Arduino
    // this code might be ported to a fire control module
    // this can be in addition to the computer - kind of like a dead air filling remote..
    setup();
    
    while(1)
    {
        loop();
        usleep(1000); // 1ms
    }
    
    g_bReadLocalRunning = false;
    return 0;
}


void setup()
{
    serial.init();
    fireShow.init();
}

void loop()
{
    fireShow.loop();
    serial.loop();
}

