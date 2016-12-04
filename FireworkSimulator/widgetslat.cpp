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

#include "widgetslat.h"

#include <QPainter>
#include <QDateTime>
#include <QTimer>

//extern qint64 g_ClockTimeInMs;

WidgetSlat::WidgetSlat(int x, int y, int iSlat, int iNumCues, QWidget *parent)
    : QWidget(parent)
{
    qDebug("iSlat=%d %p", iSlat, this);
    m_iSlat         = iSlat;
    m_iNumCues      = iNumCues;
    m_ix            = x;
    m_iy            = y;
    m_iradius       = 10;
    m_ispacex       = 4;
    m_ispacey       = 4;

    m_irows         = 3;
    m_icols         = 6;

    char sTemp[64];
    QLabel *pSlat   = new QLabel(this);
    int iSlatLabelx = m_ix;
    int iSlatLabely = m_iy;
    int iSlatLabelWidth = 60;
    int iSlatLabelHeight = 30;

    sprintf(sTemp, "Slat %02d", m_iSlat);
    pSlat->setGeometry(QRect(iSlatLabelx, iSlatLabely, iSlatLabelWidth, iSlatLabelHeight));
    pSlat->setText(sTemp);

    m_icuex         = m_ix;
    m_icuey         = m_iy + iSlatLabelHeight;

    int labelWidth  = 90;
    int labelHeight = 30;
    // check labelwidth is less than control width
    m_iWidth        = m_icols * ( (1 * m_iradius) + m_ispacex) + m_ispacex;
    m_iHeight       = m_irows * ( (2 * m_iradius) + m_ispacey) + labelHeight + m_ispacey;

    //qDebug("x=%d y=%d w=%d h=%d", m_icuex, m_icuey, m_iWidth, m_iHeight);
    setGeometry(QRect(m_icuex, m_icuey , m_icuex + m_iWidth, m_icuey + m_iHeight));

    int labelx = m_icuex;
    int labely = m_icuey + 3 * (m_iradius+m_ispacey);
    m_pRemaining  = new QLabel(this);
//    m_ipRemaining->setGeometry(QRect(labelx, labely, labelx + labelWidth, labely + labelHeight));
    m_pRemaining->setGeometry(QRect(labelx, labely,  labelWidth, labelHeight));
    m_pRemaining->setText("hello");


    m_arrCues = new cueinfo[m_iNumCues];

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(100);
}


void WidgetSlat::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int iCueIndex = 0;
    for(int row=0; row < m_irows; row++)
    {
        int y = m_icuey + row * (m_iradius+m_ispacey);
        for(int col=0; col < m_icols; col++)
        {
            if(m_arrCues[iCueIndex].tEndTime)
            {
                if (QDateTime::currentMSecsSinceEpoch() > m_arrCues[iCueIndex].tEndTime)
                {
                    m_arrCues[iCueIndex].color = Qt::darkGray;
                    m_arrCues[iCueIndex].tEndTime = 0;
                    m_arrCues[iCueIndex].bDone = true;
                    emit notifyDoneFiring(m_arrCues[iCueIndex].sName.c_str(), m_iSlat, iCueIndex+1);
                }
            }

            if (!m_arrCues[iCueIndex].bDone)
            {
                int x = m_icuex + col * (m_iradius+m_ispacex);
                if (m_arrCues[iCueIndex].color == Qt::red)
                    m_arrCues[iCueIndex].color = Qt::blue;
                else if (m_arrCues[iCueIndex].color == Qt::blue)
                    m_arrCues[iCueIndex].color = Qt::red;

                painter.setBrush(m_arrCues[iCueIndex].color); //Qt::red);
                painter.drawEllipse(x,y, m_iradius, m_iradius);
            }
            iCueIndex++;
        }
    }
#if 1

    qint64 tRemaining = getRemainingTime();
    if (tRemaining == 0)
        m_pRemaining->setText("OFF");
    else
    {
        //qDebug("Remaining %lld", tRemaining);
        QTime qd1;
        qd1.setHMS(0,0,0); //(QDateTime::currentDateTime());
        QTime qd2 = qd1.addMSecs(tRemaining);
        m_pRemaining->setText(qd2.toString("hh:mm:ss:zzz"));
    }
#endif
}

qint64 WidgetSlat::getRemainingTime()
{
    qint64 ret = 0;
    for(int iCueIndex=0; iCueIndex < MAX_SLATS; iCueIndex++)
    {
        if (m_arrCues[iCueIndex].tEndTime && m_arrCues[iCueIndex].tEndTime > QDateTime::currentMSecsSinceEpoch())
        {
            if (m_arrCues[iCueIndex].tEndTime - QDateTime::currentMSecsSinceEpoch() > ret)
                ret = m_arrCues[iCueIndex].tEndTime - QDateTime::currentMSecsSinceEpoch();
        }
    }

    return ret;
}

int WidgetSlat::numFiring()
{
    int ret = 0;
    for(int iCueIndex=0; iCueIndex < MAX_SLATS; iCueIndex++)
    {
        if (m_arrCues[iCueIndex].tEndTime)
            ret++;
    }

    return ret;
}

void WidgetSlat::fireCue(int iCue, qint64 secduration, std::string& sName)
{
    iCue--;
    if (iCue > m_iNumCues || iCue < 0)
        return;

    if (m_arrCues[iCue].bDone)
    {
        qDebug("*************************************");
        qDebug("** already fired!!!");
        qDebug("**Fire slat=%d Cue=%d", m_iSlat, iCue+1);
        qDebug("*************************************");
        return;
    }
    //qDebug("Fire slat=%d Cue=%d", m_iSlat, iCue+1);
    m_arrCues[iCue].tEndTime = QDateTime::currentMSecsSinceEpoch() + secduration*1000;
    m_arrCues[iCue].color = Qt::red;
    m_arrCues[iCue].sName = sName;

    emit notifyFiring(sName.c_str(), m_iSlat, iCue+1, secduration);
}
