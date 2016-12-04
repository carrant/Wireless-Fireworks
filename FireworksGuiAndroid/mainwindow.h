/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "qpushbutton.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>


#include "utils.h"
#include "FireShow.h"
#include "qstringlistmodel.h"
#include "Messages.h"
#include "RBLBluetooth.h"

#define FIRING_MS 2000
#define MAX_SLAT    255


namespace Ui {
class MainWindow;
}

enum e_cueState {
    e_cueState_Idle,
    e_cueState_Ready,
    e_cueState_Firing
} ;



#include <QString>
#include <QObject>


class StringList : public QStringListModel
{
public:
  void append (const QString& string){
    insertRows(rowCount(), 1);
    setData(index(rowCount()-1), string);
  }
  StringList& operator<<(const QString& string){
    append(string);
    return *this;
  }
};

class CueButtonInfo
{
public:
    CueButtonInfo(QPushButton *pbtn, int iId)
    {
        m_pButton = pbtn;
        setState(e_cueState_Idle);

        QVariant qvar(iId);
        pbtn->setProperty("id", qvar);

    }

    void setState(e_cueState state)
    {
        switch(state)
        {
        case e_cueState_Idle:
        {
            m_State = state;
            QIcon icon (":gray.png");
            m_pButton->setIcon(icon);
        }
            break;

        case e_cueState_Ready:
        {
            m_State = state;
            QIcon icon (":green.png");
            m_pButton->setIcon(icon);
        }
            break;

        case e_cueState_Firing:
            {
            m_State = state;
            QIcon icon (":red.png");
            m_pButton->setIcon(icon);
            }
               break;
        }
    }

    QPushButton *m_pButton;
    e_cueState m_State;
};

QT_USE_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void on_btnCue_clicked(int iIndex);
    void fireCue(int iSlat, int iCue);
    void idleCue(int iSlat, int iCue);
    void readyCue(int iSlat, int iCue);
    void addSlat(int iSlatId);

    void recvMessage(uint8_t *pData, uint16_t u16Size);

public slots:
    void bluetoothReady();

private slots:
    void buttonclicked();

    void on_btnCue_done_firing();

    void on_loop_event();

    void on_listViewSlats_clicked(const QModelIndex &index);

    void on_comboBoxSignalStrength_currentIndexChanged(int index);

    void on_btnResetBoard_clicked();

    void on_btnResetCues_clicked();

    void on_btnResetAll_clicked();

    void on_btnFireAll_clicked();

    void onScriptFileOpen();

    void on_btnStartScript_clicked();

    void on_btnBluetoothScan_clicked();

    void on_btnBluetoothConnect_clicked();

    void on_btnClear_clicked();

    void on_comboBoxControlGroup_currentIndexChanged(int index);

Q_SIGNALS:
    void nameChanged();

private:
    void setup();
    void loop();

    void readScript();
public:
    void recvHelloModule(FireworksMessageHelloModule *pMsg);
    void recvGoodbye(FireworksMessageBase *pMsg);

    FireworksMessageBase* allocateMessage(uint8_t u8Size, uint8_t u8Slat);
    void sendAck(uint8_t u8Slat);
    void sendHelloControl(uint8_t u8Slat);
    void sendFire(uint8_t u8Slat, uint8_t u8Cue);
    void sendSignalStrength(uint8_t index);
    void sendResetCues(uint8_t u8Slat);
    void sendResetBoard(uint8_t u8Slat);
    void sendFireAll(uint8_t u8Slat);

    void syncSlatCueButton(uint8_t u8SlatId);

    void blinkCueButton(int iCue);

private:
    Ui::MainWindow      *ui;


    CueButtonInfo       **m_arrCues;
    uint32_t            m_arrSlatCueState[MAX_SLAT];

    FireShow            *fireShow;

    QStringListModel    *m_Slats;

    uint16_t            m_u16Pin;
    uint8_t             m_u8ControlId;
    uint16_t            m_u16MySeq;
    uint16_t            m_u16OtherSeq;
    uint8_t             m_u8CurrentSlat;

    bool                m_bReadingScript;
    FILE                *m_fScript;
    qint64              m_tScriptNextTime;
    uint16_t            m_u16NextSlat;
    uint16_t            m_u16NextCue;

    RBLBluetooth        *m_rblBluetooth;
    QSettings           *m_pSettings;
};




#endif // MAINWINDOW_H
