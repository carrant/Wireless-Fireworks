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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QStringListModel>

#include <string.h>

#include "widgetslat.h"
#include "fireshow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void updateTimelabel();
    void updateFiring(const char * sName, int iSlat, int iCue, qint64 tDuration);
    void doneFiring(const char * sName, int iSlat, int iCue);
    void showOver();

public:
    void fireCue(int slat, int cue, int duration, std::string& sName)
    {
        if (slat >=100)
            qDebug("Recv fire all");
        else
            m_arrSlats[slat-1]->fireCue(cue,duration, sName);
    }
private slots:
    void on_pushButtonStart_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QTimer* m_timer;
    QTime*  m_timeValue;

    WidgetSlat **m_arrSlats;
    std::string m_sShowFile;

    FireShow *m_pFireShow;
    QStringListModel *m_Firing;
    int m_iTotalFired;
};

#endif // MAINWINDOW_H
