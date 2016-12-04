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

#include "fireshow.h"
#include <QDateTime>

#if 0
std::string GetCSV(const std::string& strCSV, uint32_t u32Index, const char* pstrComma=",");
std::string Trim(const std::string& strIn);

std::string GetCSV(const std::string& strCSV, uint32_t u32Index, const char* pstrComma)
{
    if (!pstrComma || *pstrComma == '\0')
        pstrComma = ",";

    std::string strValue;
    size_t pos0 = 0;
    size_t pos1 = 0;

    uint32_t i;
    for ( i = 0; i < u32Index; i++ )
    {
        pos1 = strCSV.find_first_of(pstrComma, pos0);
        if (pos1 == std::string::npos)
            break;
        pos0 = pos1 + 1;
    }

    if (i == u32Index)
    {
        pos1 = strCSV.find_first_of(pstrComma, pos0);
        if (pos1 != std::string::npos)
            strValue = strCSV.substr(pos0, pos1-pos0);
        else
            strValue = strCSV.substr(pos0);
    }
    return Trim(strValue);
}

std::string Trim(const std::string& strIn)
{
    std::string strOut = strIn;
    const char* pstrWhiteSpace = " \r\n\t";
    strOut.erase(0, strOut.find_first_not_of(pstrWhiteSpace));
    strOut.erase(strOut.find_last_not_of(pstrWhiteSpace)+1);
    return strOut;
}
#endif

#include "mainwindow.h"

#define LogDebug qDebug
#define LogError qDebug
qint64 millis_64()
{
    static qint64 first = 0;

    if (first == 0)
        first = QDateTime::currentMSecsSinceEpoch();

    return QDateTime::currentMSecsSinceEpoch() - first;
}



FireShow::FireShow(MainWindow * ui, QObject *parent) : QObject(parent)
{
    m_ui                = ui;
    m_pEventList        = NULL;
    m_pEventListLast    = NULL;
    m_u32NumEvents      = 0;
    m_timer             = NULL;
    m_tStart            = 0;
    m_fLast             = NULL;
    m_fLast = fopen("/home/carrant/dev/arduino/Fireworks/FireworksGui/fired.txt", "w");
}

FireShow::~FireShow()
{
    while(m_pEventList)
    {
        FireShowEvent *ptemp = m_pEventList->m_pNext;
        delete m_pEventList;
        m_pEventList = ptemp;
    }
    fclose(m_fLast);
}

void FireShow::start()
{
    if (m_timer)
        delete m_timer;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(loop()));
    m_timer->start(200);
    m_tStart = millis_64();
}

void FireShow::stop()
{
    delete m_timer;
    m_timer = NULL;
}

void FireShow::print()
{
    if(!m_pEventList)
    {
        qDebug("No events");
        return;
    }

    qDebug("Total events : %d", m_u32NumEvents);

    m_pEventList->print(true);
    FireShowEvent *ptemp = m_pEventList->m_pNext;
    while(ptemp)
    {
        ptemp->print();
        ptemp = ptemp->m_pNext;
    }
}

bool FireShow::readShow(const char *sFileName)
{
    FILE *fin = fopen(sFileName, "r");
    if (!fin)
    {
        qDebug("Error opening file '%s'", sFileName);
        return false;
    }

    uint64_t tClock = 0;
    char sInputLine[2048];
    while(fgets(sInputLine, 2047, fin) != NULL)
    {
        char *p = strchr(sInputLine, '#');
        if (p)
            *p = 0;

        std::string sLine = Trim(sInputLine);

        if (sLine.empty())
            continue;

        uint32_t    u32OffsetTime   = atoi(GetCSV(sLine,0).c_str());
        uint16_t    u8Slat          = atoi(GetCSV(sLine,1).c_str());
        uint16_t    u8Cue           = atoi(GetCSV(sLine,2).c_str());
        uint32_t    u32Duration     = atoi(GetCSV(sLine,3).c_str());
        std::string sName           = GetCSV(sLine,4);

        if (u8Slat > 255 || u8Cue > 255)
            continue;

        if (u32Duration == 0)
            u32Duration = 4;

        qDebug("Add event clock=%6d offset=%4d slat=%2d cue=%2d dur=%3d name=\"%s\"", tClock, u32OffsetTime, u8Slat, u8Cue, u32Duration, sName.c_str());

        tClock += u32OffsetTime;
        FireShowEvent *pNew = new FireShowEvent(tClock, u32OffsetTime, u8Slat, u8Cue, u32Duration, sName);

        m_u32NumEvents++;
        if (!m_pEventList)
        {
            m_pEventList = m_pEventListLast = pNew;
        }
        else
        {
            m_pEventListLast->m_pNext = pNew;
            m_pEventListLast = pNew;
        }
    }

    fclose(fin);
    qDebug("Finished reading show script");
    return true;
}

void FireShow::loop()
{
  while(m_pEventList)
  {
    qint64 u32CurrentTime = (millis_64() - m_tStart) / 1000; //seconds
    uint32_t xx = m_pEventList->m_tTimeToPlay;
//    qDebug("curtime %lld  playtime %d", u32CurrentTime, m_pEventList->m_tTimeToPlay);
    if ((m_pEventList->m_tTimeToPlay) > u32CurrentTime)
        return;

    QTime qd1;
    qd1.setHMS(0,0,0); //(QDateTime::currentDateTime());
    QTime qd2 = qd1.addMSecs(millis_64());


    fprintf(m_fLast, "FIRED u8Slat=%d u8Cue=%d Name=\"%s\"\n", m_pEventList->m_u8Slat, m_pEventList->m_u8Cue, m_pEventList->m_sName.c_str());
      LogDebug("%s Send fire CurTime=%d PlayTime %d u8Slat=%d u8Cue=%d Name=\"%s\"", qd2.toString("hh:mm:ss:zzz").toUtf8().constData(), u32CurrentTime, m_pEventList->m_tTimeToPlay, m_pEventList->m_u8Slat, m_pEventList->m_u8Cue, m_pEventList->m_sName.c_str());
      m_ui->fireCue(m_pEventList->m_u8Slat, m_pEventList->m_u8Cue, m_pEventList->m_u32Duration, m_pEventList->m_sName);

      FireShowEvent *pTemp = m_pEventList->m_pNext;
      delete m_pEventList;
      m_pEventList = pTemp;
      m_u32NumEvents--;
  }

  if(!m_pEventList)
  {
      delete m_timer;
      m_timer = NULL;
      emit fireShowComplete();
    }

}

void FireShow::adjustTime(qint64 sec )
{
    qDebug("Adjust time %llds", sec);
    m_tAdjust = sec;
} // pos to speed up, neg to slow down
