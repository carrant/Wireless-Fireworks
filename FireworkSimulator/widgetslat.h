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

#ifndef WIDGETSLAT_H
#define WIDGETSLAT_H

#include <QWidget>
#include <QLabel>

#define MAX_SLATS   18
class cueinfo
{
public:
    cueinfo()
    {
        tEndTime = 0;
        bDone = false;
        color = Qt::green;
    }

public:
    qint64 tEndTime;
    bool bDone;
    Qt::GlobalColor color;
    std::string sName;
};

class WidgetSlat : public QWidget
{
    Q_OBJECT

public:
    WidgetSlat(int x, int y, int iSlat, int iNumCues, QWidget *parent = 0);

signals:
    void notifyFiring(const char *sName, int iSlat, int iCue, qint64 tDuration);
    void notifyDoneFiring(const char *sName, int iSlat, int iCue);

public:
    void fireCue(int iSlat, qint64 secduration, std::string& sName);
    qint64 getRemainingTime();
    int numFiring();

    int Width() { return m_iWidth; }
    int Height() { return m_iHeight; }

    void cueOff(int iCue) { m_arrCues[iCue].bDone = true; }
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    int m_ix;
    int m_iy;
    int m_iSlat;
    int m_iNumCues;
    QLabel *m_pRemaining;
    int m_icuex;
    int m_icuey;
    int m_iradius;
    int m_ispacex;
    int m_ispacey;
    int m_irows;
    int m_icols;

    int m_iWidth;
    int m_iHeight;

    cueinfo *m_arrCues;
};

#endif // WIDGETSLAT_H
