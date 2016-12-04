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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLayout>


#define MAX_SLATS 200

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList stringList;

    m_Firing = new QStringListModel;

    ui->listViewFiring->setModel(m_Firing);

    m_timer = new QTimer(this);
    m_timeValue = new QTime(0,0,0, 0);
    ui->labelTime->setText(m_timeValue->toString("hh:mm:ss:zzz"));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateTimelabel()));

    m_arrSlats = new WidgetSlat *[MAX_SLATS];
    memset(m_arrSlats, 0, sizeof(WidgetSlat *) * MAX_SLATS);

    m_iTotalFired = 0;

    int iStartX = 10;
    int iStartY = 25;

    int iCurX = iStartX;
    int iCurY = iStartY;

    int iSlatCount = 0;
    int iSlatIndex = 0;
    int iHorzSpacing = 25;
    int iVertSpacing = 15;
    // first row 6
    for(iSlatIndex=0; iSlatIndex < 6; iSlatIndex++, iSlatCount++)
    {
        m_arrSlats[iSlatCount] = new WidgetSlat(iCurX,iCurY, iSlatCount+1, 18, ui->centralWidget);
//        m_arrSlats[iSlatCount]->setObjectName(QStringLiteral("slat"));
        m_arrSlats[iSlatCount]->show();

        connect(m_arrSlats[iSlatCount],SIGNAL(notifyFiring(const char *, int, int, qint64)),this,SLOT(updateFiring(const char *, int, int, qint64)));
        connect(m_arrSlats[iSlatCount],SIGNAL(notifyDoneFiring(const char *, int, int)),this,SLOT(doneFiring(const char *, int, int)));

        iCurX += m_arrSlats[iSlatCount]->Width() + iHorzSpacing;
    }

    iCurX = iStartX;
    iCurY += m_arrSlats[iSlatCount-1]->Height() + iVertSpacing;
    // first row 6
    for(iSlatIndex=0; iSlatIndex < 6; iSlatIndex++, iSlatCount++)
    {
        m_arrSlats[iSlatCount] = new WidgetSlat(iCurX,iCurY, iSlatCount+1, 18, ui->centralWidget);
//        m_arrSlats[iSlatCount]->setObjectName(QStringLiteral("slat"));
        m_arrSlats[iSlatCount]->show();

        connect(m_arrSlats[iSlatCount],SIGNAL(notifyFiring(const char *, int, int, qint64)),this,SLOT(updateFiring(const char *, int, int, qint64)));
        connect(m_arrSlats[iSlatCount],SIGNAL(notifyDoneFiring(const char *, int, int)),this,SLOT(doneFiring(const char *, int, int)));

        iCurX += m_arrSlats[iSlatCount]->Width() + iHorzSpacing;
    }

    iCurX = iStartX + iHorzSpacing/2;
    iCurY += m_arrSlats[iSlatCount-1]->Height() + iVertSpacing;
    // last 5
    for(iSlatIndex=0; iSlatIndex < 5; iSlatIndex++, iSlatCount++)
    {
        m_arrSlats[iSlatCount] = new WidgetSlat(iCurX,iCurY, iSlatCount+1, 18, ui->centralWidget);
//        m_arrSlats[iSlatCount]->setObjectName(QStringLiteral("slat"));
        m_arrSlats[iSlatCount]->show();

        connect(m_arrSlats[iSlatCount],SIGNAL(notifyFiring(const char *, int, int, qint64)),this,SLOT(updateFiring(const char *, int, int, qint64)));
        connect(m_arrSlats[iSlatCount],SIGNAL(notifyDoneFiring(const char *, int, int)),this,SLOT(doneFiring(const char *, int, int)));

        iCurX += m_arrSlats[iSlatCount]->Width() + iHorzSpacing;
    }

    int iCueIndex = 0;
    for(iCueIndex = 11; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[1]->cueOff(iCueIndex);

    for(iCueIndex = 11; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[3]->cueOff(iCueIndex);

    for(iCueIndex = 11; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[4]->cueOff(iCueIndex);

    for(iCueIndex = 10; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[7]->cueOff(iCueIndex);

    for(iCueIndex = 12; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[9]->cueOff(iCueIndex);

    for(iCueIndex = 11; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[10]->cueOff(iCueIndex);

    for(iCueIndex = 12; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[11]->cueOff(iCueIndex);

    for(iCueIndex = 10; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[13]->cueOff(iCueIndex);

    for(iCueIndex = 10; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[14]->cueOff(iCueIndex);

    for(iCueIndex = 10; iCueIndex <= 18; iCueIndex++)
        m_arrSlats[15]->cueOff(iCueIndex);

    m_sShowFile = "/home/carrant/dev/arduino/Fireworks/FireworksGui/show2016.csv";

    m_pFireShow = new FireShow(this, this);
    connect(m_pFireShow,SIGNAL(fireShowComplete()),this,SLOT(showOver()));
}

MainWindow::~MainWindow()
{
    delete m_timer;
    delete m_timeValue;

    delete ui;
    delete m_arrSlats;
    delete m_pFireShow;
}


void MainWindow::updateTimelabel()
{
    QTime q = m_timeValue->addMSecs(100);
    m_timeValue->setHMS(q.hour(), q.minute(), q.second(), q.msec());

    ui->labelTime->setText(m_timeValue->toString("hh:mm:ss:zzz"));

    qint64 tRemaining = 0;
    int iFiring = 0;
    for(int x=0; x < MAX_SLATS; x++)
    {
        if(m_arrSlats[x])
        {
            if (m_arrSlats[x]->getRemainingTime() > tRemaining)
                tRemaining = m_arrSlats[x]->getRemainingTime();

            iFiring += m_arrSlats[x]->numFiring();
        }
    }

    QTime qd1;
    qd1.setHMS(0,0,0); //(QDateTime::currentDateTime());
    QTime qd2 = qd1.addMSecs(tRemaining);
    ui->labelDarkSky->setText(qd2.toString("hh:mm:ss:zzz"));

    char sFiring[64];
    sprintf(sFiring, "%d", iFiring);
    ui->labelConcurrent->setText(sFiring);
}

void MainWindow::on_pushButtonStart_clicked()
{
    qDebug("Start");
    m_timer->start(100);

    m_pFireShow->readShow(m_sShowFile.c_str());
    m_pFireShow->start();

/*    m_arrSlats[0]->fireSlat(1, 10000);
    m_arrSlats[0]->fireSlat(7, 8000);
    m_arrSlats[0]->fireSlat(18,18000);
    */
}

void MainWindow::on_pushButton_clicked()
{
    qDebug("activce %d %d %d", m_timer->isActive(), m_timer->interval(), m_timer->remainingTime());
}

void MainWindow::updateFiring(const char *sName, int iSlat, int iCue, qint64 tDuration)
{
    char sData[1024];
    sprintf(sData, "%2d %2d %s", iSlat, iCue, sName);

//    qDebug("notify firing sName=\"%s\", slat=%d, cue=%d, duration=%lld", sName, iSlat, iCue, tDuration);
    m_Firing->insertRows(m_Firing->rowCount(), 1);
    m_Firing->setData(m_Firing->index(m_Firing->rowCount()-1), sData);

    m_iTotalFired++;
    char s[32];
    sprintf(s,"%d", m_iTotalFired);
    ui->labelTotalFired->setText(s);
}

void MainWindow::doneFiring(const char *sName, int iSlat, int iCue)
{
    char sData[1024];
    sprintf(sData, "%2d %2d %s", iSlat, iCue, sName);

//    qDebug("notify DONE firing sName=\"%s\", slat=%d, cue=%d", sName, iSlat, iCue);
    QModelIndexList qModelIndex = m_Firing->match(m_Firing->index(0, 0), Qt::DisplayRole, sData);
    if (qModelIndex.count() == 0)
        return;

    foreach(QModelIndex index, qModelIndex)
    {
      m_Firing->removeRow(index.row());
    }

}

void MainWindow::showOver()
{
    delete m_timer;
    m_timer = NULL;
}
