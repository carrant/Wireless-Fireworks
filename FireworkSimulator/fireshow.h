/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksSimulator.

    FireworksSimulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksSimulator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksSimulator.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FIRESHOW_H
#define FIRESHOW_H

#include <QObject>
#include <QTimer>

class FireShowEvent
{
public:
    FireShowEvent(uint64_t tTimeToPlay, uint32_t u32OffsetTime, uint8_t u8Slat, uint8_t u8Cue, uint32_t u32Duration, const std::string& sName)
    {
        m_tTimeToPlay   = tTimeToPlay;
        m_u32OffsetTime = u32OffsetTime;
        m_u8Slat        = u8Slat;
        m_u8Cue         = u8Cue;
        m_u32Duration   = u32Duration;
        m_sName         = sName;

        m_pNext         = NULL;
    }

    virtual ~FireShowEvent() { }

public:
    void print(bool bHeader=false)
    {
        if(bHeader)
        {
        printf("TimePlay   Offset Slat Cue  Dur    Name\n");
        printf("---------- ------ ---- ---- ------ ----\n");
              //1234567890 123456 1234 1234 123456
        }
        printf("%10lld %6d %4d %4d %6d %s\n",
            m_tTimeToPlay,
            m_u32OffsetTime,
            m_u8Slat,
            m_u8Cue,
            m_u32Duration,
            m_sName.c_str()
            );

    }
public:
    uint64_t    m_tTimeToPlay; // abs time
    uint32_t    m_u32OffsetTime;
    uint8_t     m_u8Slat;
    uint8_t     m_u8Cue;
    uint32_t    m_u32Duration;
    std::string m_sName;

    FireShowEvent *m_pNext;
};



class MainWindow;

class FireShow : public QObject
{
    Q_OBJECT
public:
    explicit FireShow(MainWindow *ui, QObject *parent = 0);
    ~FireShow();

signals:
    void fireShowComplete();
public slots:
    void loop();

public:
    void start();

public:
    bool readShow(const char *sFileName);
    void print();

private:
    FireShowEvent * m_pEventList;
    FireShowEvent * m_pEventListLast;
    uint32_t        m_u32NumEvents;
    QTimer *        m_timer;
    MainWindow *    m_ui;

    qint64        m_tStart;
};

#endif // FIRESHOW_H
