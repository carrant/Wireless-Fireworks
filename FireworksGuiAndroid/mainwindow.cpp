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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
//#include <QtEndian>
#include <string>


#define SCRIPT_SKIP     257
#define SCRIPT_ALL      65000


void logtext(bool bTime, QTextEdit *ptext, char *fmt, ... )
{
  char buf[2048] = ""; // resulting string limited to 128 chars

  if (bTime)
  {
    QString sTime = QDateTime::currentDateTime().toString("hh:mm:ss:zzz  ");
    strcpy(buf, sTime.toUtf8().constData());
  }

  char *pAfterTime = buf + strlen(buf);

  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(pAfterTime, sizeof(buf) - 16, (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(pAfterTime, sizeof(buf) - 16, (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);

  char outbuf[2048];
  int iout = 0;
  if (*buf == '\n' && *(buf +1) == 0)
  {
      outbuf[iout++] = '\n';
      outbuf[iout++] = 0;
  }
  else
  {
      for (int x=0; x < strlen(buf); x++)
          if (buf[x] == '\r' || buf[x] == '\n')
              continue;
          else
              outbuf[iout++] = buf[x];

      outbuf[iout++] = 0;
    }
  QTextCursor prev_cursor = ptext->textCursor();
  ptext->moveCursor (QTextCursor::End);
  ptext->insertPlainText (outbuf);
#if 0
  ptext->moveCursor (QTextCursor::End); // autoscroll
#else
  ptext->setTextCursor (prev_cursor); // no autoscroll
#endif
}

#define LOGTEXT(f, ...) \
{ logtext(true, ui->textEditLog, f, ##__VA_ARGS__); \
    logtext(false, ui->textEditLog, "\n"); }


QTextEdit *g_textEditLog = NULL;

void g_LogText(const char *s)
{
    if (!g_textEditLog)
        return;

    if (*s=='\n' && *(s+1) == 0)
        logtext(false, g_textEditLog, "%s", s);
    else
        logtext(true, g_textEditLog, "%s", s);

}

/*****************************************************************************************
 *
 * To create the manifest -
 *
 * 1.  Compile/run using android kit
 * 2.  Go to the directory where your .pro file is
 * 3.  One level up there will be a directory for arm-debug
 *       i.e. ../build-FireworksGui-Android_for_armeabi_v7a_GCC_4_9_Qt_5_5_0-Debug
 * 4.  Under that directory there is a directory called android-build
 *       i.e. ../build-FireworksGui-Android_for_armeabi_v7a_GCC_4_9_Qt_5_5_0-Debug/android-build
 * 5.  Copy the AndroidManifest.xml to a safe place
 *       i.e. cp ../build-FireworksGui-Android_for_armeabi_v7a_GCC_4_9_Qt_5_5_0-Debug/android-build ~/AndroidManifest.xml_save
 * 6.  Go back to QT Creator
 * 7.  Click on Projects (left hand side)
 * 8.  Click on the Android kit
 *       i.e. Android for armeabi-v7a (GCC 4.9, Qt 5.5.0)
 * 9.  Under "Build Steps" there will be "Build Android APK", click on "Details"
 * 10. Under Android click on "Create Template"
 * 11. Specify where you want the template created
 *       i.e. <project dir>/android
 * 12. Now copy the saved template to the template you just created
 *       i.e. cp ~/AndroidManifest.xml_save <project dir>/android/AndroidManifest.xml
 * 13. Done!
 * ***************************************************************************************/



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));

    ui->setupUi(this);

    m_pSettings     = new QSettings("FireworksGuiAndroid", "FireworksGuiAndroidSettings");
    m_u16Pin        = 0xdead;
    m_u8ControlId   = 0x01;
    m_u16MySeq      = 0;
    m_u16OtherSeq   = 0;
    m_u8CurrentSlat = -1;

    m_arrCues = new CueButtonInfo*[18];

    // tried to get this working with a loop, but can't find the button!
    // the window isn't active yet
    /*
    QWidget *pWin = QApplication::activeWindow();
    for (int x=0; x < 18; x++)
    {
        char sButton[128];
        sprintf(sButton, "%02d", x+1);
        QPushButton* button = this->findChild<QPushButton*>(sButton);

        if (button)
            m_arrCues[x] = new CueButtonInfo(button, x);
    }
    */

    int x=0;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue01, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue02, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue03, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue04, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue05, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue06, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue07, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue08, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue09, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue10, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue11, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue12, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue13, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue14, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue15, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue16, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue17, x);  x++;
    m_arrCues[x] = new CueButtonInfo(ui->btnCue18, x);  x++;


    QObject::connect(ui->btnCue01, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue02, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue03, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue04, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue05, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue06, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue07, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue08, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue09, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue10, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue11, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue12, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue13, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue14, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue15, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue16, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue17, SIGNAL(clicked()),this, SLOT(buttonclicked()));
    QObject::connect(ui->btnCue18, SIGNAL(clicked()),this, SLOT(buttonclicked()));

    memset(m_arrSlatCueState, 0, sizeof(uint32_t) * MAX_SLAT);

    QAction *myAction = ui->lineEditScript->addAction(QIcon(":fileopen.png"), QLineEdit::TrailingPosition);
    connect(myAction, &QAction::triggered, this, &MainWindow::onScriptFileOpen);

    ui->btnStartScript->setEnabled(false);
    QIcon icon (":gray.png");
    ui->btnStartScript->setIcon(icon);
    m_bReadingScript    = false;
    m_fScript           = NULL;
    m_tScriptNextTime   = 0;
    m_u16NextSlat       = SCRIPT_SKIP;
    m_u16NextCue        = SCRIPT_SKIP;

    m_rblBluetooth      = new RBLBluetooth;
    fireShow            = new FireShow(this);

    fireShow->readFile("showtest.txt");
    fireShow->printEvents();

    connect(m_rblBluetooth, SIGNAL(bluetoothDeviceReady()), this, SLOT(bluetoothReady()));
    LogDebug("Starting!");

    // just like Arduino
    // this code might be ported to a fire control module
    // this can be in addition to the computer - kind of like a dead air filling remote..
    setup();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_loop_event()));
    timer->start(20);

    QStringList stringList;
    stringList << "Red";
    stringList << "Green";
    stringList << "Blue";

    m_Slats = new QStringListModel;

    ui->listViewSlats->setModel(m_Slats);

    char sText[128];
    sprintf(sText, "0x%04X", m_u16Pin);
    ui->labelPin->setText(sText);

    QString sSignalStrengthIndex = m_pSettings->value("SignalStrengthIndex",QVariant("Medium")).toString();

    qDebug("sSignalStrengthIndex %s", sSignalStrengthIndex.toUtf8().constData());
    ui->comboBoxSignalStrength->addItem("Low");
    ui->comboBoxSignalStrength->addItem("Medium");
    ui->comboBoxSignalStrength->addItem("High");
    ui->comboBoxSignalStrength->addItem("Max");

    int iControlGroupIndex = m_pSettings->value("ControlGroupIndex",QVariant(1)).toInt();
    qDebug("iControlGroupIndex %d", iControlGroupIndex);

    for (int x=0; x< 15; x++)
    {
        char s[32];
        sprintf(s,"%d", x);
        ui->comboBoxControlGroup->addItem(s);
    }
    ui->comboBoxControlGroup->setCurrentIndex(iControlGroupIndex);

//    ui->comboBoxSignalStrength->setCurrentIndex(ui->comboBoxSignalStrength->findText("Max"));
    ui->comboBoxSignalStrength->setCurrentIndex(ui->comboBoxSignalStrength->findText(sSignalStrengthIndex));

// add slat for testing
//slat 99 slat99
#if 0
    FireworksMessageHelloModule hello;
    hello.base.u16Pin       = m_u16Pin;
    hello.base.u8Size       = 32;
    hello.base.u16SendSeq   = 0;
    hello.base.u16RecvSeq   = 0;
    hello.base.u16Cmd       = FW_MSG_CMD_HELLO_MODULE;
    hello.base.u8ControlId  = m_u8ControlId;
    hello.base.u8SlatId     = 99;

    hello.u8NumCues         = 18;
    hello.u32CueState       = 0xffffffff;

    recvHelloModule(&hello);
#endif

}

void MainWindow::addSlat(int iSlatId)
{
    char sSlat[128];
    sprintf(sSlat,"Slat %02d", iSlatId);

    QModelIndexList qModelIndex = m_Slats->match(m_Slats->index(0, 0), Qt::DisplayRole, sSlat);
    if (qModelIndex.count() > 0)
        return;
    /*// make sure it doesn't exist in the list
    for (int i = 0; i < m_Slats->rowCount(); ++i)
    {
        if (m_Slats->data(i, Qt::DisplayRole).toInt() == iSlatId)
        {

            return;
        }
    }
*/

    m_Slats->insertRows(m_Slats->rowCount(), 1);
    m_Slats->setData(m_Slats->index(m_Slats->rowCount()-1), sSlat);

    m_Slats->sort(0);

    if (m_Slats->rowCount() == 1)
    {
        m_u8CurrentSlat = iSlatId;

        QModelIndex index = m_Slats->index(0);

        ui->listViewSlats->setCurrentIndex(index);
        //ui->listViewSlats->selectAll();
        LOGTEXT("current slat %d", m_u8CurrentSlat);
    }
}
    
MainWindow::~MainWindow()
{
    delete ui;

    delete fireShow;
    delete m_rblBluetooth;
}



void MainWindow::setup()
{
    m_rblBluetooth->init(ui->comboBoxBluetooth, ui->btnBluetoothConnect);
    fireShow->init();

    g_textEditLog = ui->textEditLog;
}

void MainWindow::loop()
{
    fireShow->loop();
    uint8_t *pData = m_rblBluetooth->loop();
    recvMessage(pData, 32);

    readScript();
}



void MainWindow::recvMessage(uint8_t *pu8Data, uint16_t u16Size)
{

    if (!pu8Data || u16Size == 0)
        return;
#if 0
    char sLine[1024];
    char *pLine = sLine;
    for (int x=0; x<16;x++)
        pLine += sprintf(pLine, "%02x ", pu8Data[x]);

    pLine += sprintf(pLine, "\n");

    for (int x=16; x<32;x++)
        pLine += sprintf(pLine, "%02x ", pu8Data[x]);
    pLine += sprintf(pLine, "\n");

    LOGTEXT("\n%s", sLine);
#endif

    FireworksMessageBase *pMsg = (FireworksMessageBase *)pu8Data;

#if 0
    LOGTEXT("Pin            0x%04x", pMsg->u16Pin);
    LOGTEXT("Size           %d", pMsg->u8Size);
    LOGTEXT("u16SendSeq     %d", pMsg->u16SendSeq);
    LOGTEXT("u16RecvSeq     %d", pMsg->u16RecvSeq);
    LOGTEXT("u16Cmd         %d", pMsg->u16Cmd);
    LOGTEXT("u8ControlId    %d", pMsg->u8ControlId);
    LOGTEXT("u8SlatId       %d", pMsg->u8SlatId);
#endif
//    LOGTEXT("Receive message %d", pMsg->u16Cmd);

    if (pMsg->u16Pin != m_u16Pin)
    {
      LOGTEXT("Invalid PIN %d %d", pMsg->u16Pin, m_u16Pin);
      return;
    }

    if (pMsg->u8ControlId != m_u8ControlId)
    {
      LOGTEXT("Invalid ControlId %d %d", pMsg->u8ControlId, m_u8ControlId);
      return;
    }

    switch(pMsg->u16Cmd)
    {
      case FW_MSG_CMD_ACK:
        LOGTEXT("Recv ack");
      break;

      case FW_MSG_CMD_GOODBYE:
      {
        LOGTEXT("Recv Goodbye %02d", pMsg->u8SlatId);
        recvGoodbye(pMsg);
      }
      break;

      case FW_MSG_CMD_HELLO_MODULE:
      {
        FireworksMessageHelloModule *pHello = (FireworksMessageHelloModule *)pMsg;
        LOGTEXT("Recv Hello %02d", pHello->base.u8SlatId);
        recvHelloModule(pHello);
      }
      break;

      case FW_MSG_CMD_FIRED:
      {
          FireworksMessageFired *pFired = (FireworksMessageFired *)pMsg;
          LOGTEXT("Recv Fired %02d %02d", pFired->base.u8SlatId, pFired->arrCues[0] + 1);
      }
      break;

    }

}



void MainWindow::on_loop_event()
{
    loop();
}

void MainWindow::fireCue(int iSlat, int iCue)
{
    if (iSlat > MAX_SLAT)
        return;

    on_btnCue_clicked(iCue);
}

void MainWindow::idleCue(int iSlat, int iCue)
{
    if (iSlat > MAX_SLAT)
        return;

    m_arrCues[iCue]->setState(e_cueState_Idle);
}

void MainWindow::readyCue(int iSlat, int iCue)
{
    if (iSlat > MAX_SLAT)
        return;

    m_arrCues[iCue]->setState(e_cueState_Ready);
}

void MainWindow::on_btnCue_clicked(int iCue)
{
    if (m_arrCues[iCue]->m_State == e_cueState_Idle)
        return;

    m_arrSlatCueState[m_u8CurrentSlat] &= ~(1<<iCue);

    blinkCueButton(iCue);

    sendFire(m_u8CurrentSlat, iCue);
//    QTimer::singleShot(FIRING_MS, this, SLOT(on_btnCue01_done_firing()));

}

void MainWindow::on_btnCue_done_firing()
{
    QTimer *pTimer = (QTimer*)QObject::sender();

    if (!pTimer)
        return;

    int iIndex = pTimer->property("button").toInt();
    int iCount = pTimer->property("count").toInt();
    int iSlat = pTimer->property("slat").toInt();

    if (iSlat != m_u8CurrentSlat)
    {
        pTimer->stop();
    }
    else if (iCount > 10)
    {
        m_arrCues[iIndex]->setState(e_cueState_Idle);
        pTimer->stop();
    }
    else
    {
        if (iCount % 2 == 0)
            m_arrCues[iIndex]->setState(e_cueState_Ready);
        else
            m_arrCues[iIndex]->setState(e_cueState_Firing);

        iCount++;
        QVariant qcount(iCount);
        if (!pTimer->setProperty("count", qcount))
            printf("Erro!!!!\n\n\n");
    }
}

void MainWindow::buttonclicked()
{
    QPushButton* pButton = (QPushButton*)QObject::sender();
    int iId = pButton->property("id").toInt();

    on_btnCue_clicked(iId);
}


void MainWindow::recvGoodbye(FireworksMessageBase *pMsg)
{
    char sSlat[128];
    sprintf(sSlat,"Slat %02d", pMsg->u8SlatId);

    QModelIndexList qModelIndex = m_Slats->match(m_Slats->index(0, 0), Qt::DisplayRole, sSlat);
    if (qModelIndex.count() == 0)
        return;

    foreach(QModelIndex index, qModelIndex)
    {
      m_Slats->removeRow(index.row());
    }
}

void MainWindow::recvHelloModule(FireworksMessageHelloModule *pMsg)
{
    if (pMsg->base.u8SlatId >= MAX_SLAT )
        return;

    addSlat(pMsg->base.u8SlatId);
    sendHelloControl(pMsg->base.u8SlatId);

    m_arrSlatCueState[pMsg->base.u8SlatId] = pMsg->u32CueState;

    syncSlatCueButton(pMsg->base.u8SlatId);
}


FireworksMessageBase* MainWindow::allocateMessage(uint8_t u8Size, uint8_t u8Slat)
{
  uint8_t *pData = new uint8_t[u8Size];

  FireworksMessageBase *pBase = (FireworksMessageBase *)pData;

  pBase->u16Pin      = m_u16Pin;
  pBase->u8Size      = 32;
  pBase->u16SendSeq  = m_u16MySeq++;
  pBase->u16RecvSeq  = m_u16OtherSeq;
  pBase->u16Cmd      = FW_MSG_CMD_ACK;
  pBase->u8ControlId = m_u8ControlId;
  pBase->u8SlatId    = u8Slat;

  return pBase;
}

void MainWindow::sendAck(uint8_t u8Slat)
{
    FireworksMessageBase *pMsg = allocateMessage(32, u8Slat);

    pMsg->u16Cmd = FW_MSG_CMD_ACK;
    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendHelloControl(uint8_t u8Slat)
{
    FireworksMessageBase *pMsg = allocateMessage(32, u8Slat);

    pMsg->u16Cmd = FW_MSG_CMD_HELLO_CONTROL;
    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendFire(uint8_t u8Slat, uint8_t u8Cue)
{
    FireworksMessageFire *pMsg = (FireworksMessageFire *)allocateMessage(32, u8Slat);

    pMsg->base.u16Cmd = FW_MSG_CMD_FIRE;
    pMsg->u32Cues = (1 << u8Cue);

    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendSignalStrength(uint8_t index)
{
    FireworksMessageBase *pMsg = (FireworksMessageBase *)allocateMessage(32, index); // index goes in slatid

    pMsg->u16Cmd = FW_MSG_CMD_SET_SIGNAL_STR;

    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendResetCues(uint8_t u8Slat)
{
    FireworksMessageBase *pMsg = (FireworksMessageBase *)allocateMessage(32, u8Slat);

    pMsg->u16Cmd = FW_MSG_CMD_RESET_CUES;

    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendResetBoard(uint8_t u8Slat)
{
    FireworksMessageBase *pMsg = (FireworksMessageBase *)allocateMessage(32, u8Slat);

    pMsg->u16Cmd = FW_MSG_CMD_RESET_BOARD;

    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}

void MainWindow::sendFireAll(uint8_t u8Slat)
{
    FireworksMessageBase *pMsg = (FireworksMessageBase *)allocateMessage(32, u8Slat);

    pMsg->u16Cmd = FW_MSG_CMD_FIRE_ALL;

    m_rblBluetooth->sendData((uint8_t *)pMsg, 32);

    delete pMsg;
}


void MainWindow::on_listViewSlats_clicked(const QModelIndex &index)
{
    QVariant qvar = m_Slats->data(index, Qt::DisplayRole);

    QString qstr = qvar.toString();

    LOGTEXT("clicked %s", qstr.toStdString().c_str());

    const char *pstr = qstr.toStdString().c_str();
    const char *p = strchr(pstr, ' ');
    if (!p)
        return;


    m_u8CurrentSlat = atoi(p+1);
    syncSlatCueButton(m_u8CurrentSlat);
    LOGTEXT("current slat %d", m_u8CurrentSlat);
}

void MainWindow::syncSlatCueButton(uint8_t u8SlatId)
{
    if (u8SlatId != m_u8CurrentSlat)
        return;

    for (uint32_t iCue=0; iCue < NUM_CUES; iCue++)
    {
        uint32_t u32CueMask = (1<<iCue);
        if ((m_arrSlatCueState[u8SlatId] & u32CueMask) == 0)
            m_arrCues[iCue]->setState(e_cueState_Idle);
        else
            m_arrCues[iCue]->setState(e_cueState_Ready);

    }
}

void MainWindow::on_comboBoxSignalStrength_currentIndexChanged(int index)
{
    sendSignalStrength(index);

    if (!ui->comboBoxSignalStrength->currentText().isEmpty())
    {
        m_pSettings->setValue("SignalStrengthIndex",ui->comboBoxSignalStrength->currentText());
        m_pSettings->sync();

        qDebug("sSignalStrengthIndex %s", ui->comboBoxSignalStrength->currentText().toUtf8().constData());
    }
}

void MainWindow::on_btnResetBoard_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset board?", "Really reset board?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    sendResetBoard(m_u8CurrentSlat);
}

void MainWindow::on_btnResetCues_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset cue?", "Really reset cue?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    sendResetCues(m_u8CurrentSlat);
}

void MainWindow::on_btnResetAll_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset all?", "Really Reset all?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    QStringList slSlats = m_Slats->stringList();
    for (int i = 0; i < slSlats.size(); ++i)
    {
        char str[64] = "";
        int iSlat = -1;
        sscanf(slSlats.at(i).toLocal8Bit().constData(), "%s %d", str, &iSlat); // todo add qvariant data for slatid

        if (iSlat == -1)
            continue;

        sendResetBoard(iSlat);

    }

    m_Slats->setStringList( QStringList() );
//    m_Slats->removeRows( 0, m_Slats->rowCount() );
}

void MainWindow::blinkCueButton(int iCue)
{

    m_arrCues[iCue]->setState(e_cueState_Firing);

    QVariant qvar(iCue);
    QVariant qcount((int)0);
    QTimer *timer = new QTimer(this);
    timer->setProperty("button", qvar);
    timer->setProperty("count", qcount);
    timer->setProperty("slat", m_u8CurrentSlat);

    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnCue_done_firing()));
    timer->start(200);
}

void MainWindow::on_btnFireAll_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Fire all?", "Really Fire all?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
      sendFireAll(SLAT_BROADCAST);
    }
}

// script file format
// time from last event in MS, firework name, slat #, cue #
//
// 257(SCRIPT_SKIP) is ignore
// 65000(SCRIPT_ALL) is all for either slat or cue
//
// i.e.
// 500, 257, 257    # WAIT 500ms line to sync up time slat and cue == 257 ignore line (this is not required, but can be used to sync with music periodically)
// 0, 15, 2         # NOW fire slat 15, cue 2
// 0, 20, 18        # NOW fire slat 20, cue 18
// 1000, 1, 2       # WAIT 1000ms to fire slat 1, cue 2
// 0, 2, 2          # NOW fire slate 2, cue 2
// 2000, 3, 6       # WAIT 2000ms fire slat 3, cue 6
// 0. 2, 65000      # NOW fire slat 2 ALL CUES
// 0. 65000, 65000  # NOW fire ALL SLATS and ALL CUES
void MainWindow::onScriptFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(
                            this,
                            "SLAT Script",
                            "/home/carrant/dev/arduino/Fireworks/FireworksGui", //QDir::currentPath(),
                            tr("Scripts (*.csv *.txt)")
                            );

    ui->lineEditScript->setText(fileName);

    if (ui->lineEditScript->text().length() > 0)
    {
        ui->btnStartScript->setEnabled(true);
        QIcon icon (":green.png");
        ui->btnStartScript->setIcon(icon);
    }
    else
    {
        ui->btnStartScript->setEnabled(false);
        QIcon icon (":gray.png");
        ui->btnStartScript->setIcon(icon);
    }
}

void MainWindow::on_btnStartScript_clicked()
{
    if (!m_fScript)
    {
        m_fScript = fopen(ui->lineEditScript->text().toUtf8().constData(), "r");
        if (!m_fScript)
            return;

        LOGTEXT("Reading from file \"%s\"", ui->lineEditScript->text().trimmed().toUtf8().constData());
    }

    m_bReadingScript = !m_bReadingScript;

    // script is read in the main loop (just like arduino)
    if (m_bReadingScript)
    {
        ui->btnStartScript->setText("Pause Script");
        ui->btnStartScript->setEnabled(true);
        QIcon icon (":red.png");
        ui->btnStartScript->setIcon(icon);

        m_tScriptNextTime = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        ui->btnStartScript->setText("Resume Script");
        if (ui->lineEditScript->text().length() > 0)
        {
            ui->btnStartScript->setEnabled(true);
            QIcon icon (":green.png");
            ui->btnStartScript->setIcon(icon);
        }
        else
        {
            ui->btnStartScript->setEnabled(false);
            QIcon icon (":gray.png");
            ui->btnStartScript->setIcon(icon);
        }
    }
}

void MainWindow::readScript()
{
    if (!m_bReadingScript)
        return;

    if (!m_fScript)
        return;

    if (QDateTime::currentMSecsSinceEpoch() < m_tScriptNextTime)
        return;

    bool bFiring = true;
    while (bFiring)
    {
        if (m_u16NextSlat == SCRIPT_SKIP)
        {
            //nop
        }
        else if (m_u16NextSlat == SCRIPT_ALL && m_u16NextCue == SCRIPT_ALL)
        {
            sendFireAll(SLAT_BROADCAST);

            QStringList slSlats = m_Slats->stringList();
            for (int i = 0; i < slSlats.size(); ++i)
            {
                char str[64] = "";
                int iSlat = -1;
                sscanf(slSlats.at(i).toLocal8Bit().constData(), "%s %d", str, &iSlat);

                if (iSlat == -1)
                    continue;

                for(int x=0; x < 18; x++)
                {
                    if ( (m_arrSlatCueState[iSlat] & (1<<x)) != 0)
                    {
                        m_arrSlatCueState[iSlat] &= ~(1<<x);
                        if ((uint8_t)iSlat == m_u8CurrentSlat)
                            blinkCueButton(x);
                    }
                }
            }


        }
        else if (m_u16NextCue == SCRIPT_ALL) // not yet
        {
            sendFireAll(m_u16NextSlat);
            for(int x=0; x < 18; x++)
                if ( (m_arrSlatCueState[m_u16NextSlat] & (1<<m_u16NextCue)) != 0)
                {
                    m_arrSlatCueState[m_u16NextSlat] &= ~(1<<m_u16NextCue);
                    if ((uint8_t)m_u16NextSlat == m_u8CurrentSlat)
                        blinkCueButton(m_u16NextCue);
                }


        }
        else
        {
            LOGTEXT("FIRE %02d  %02d", m_u16NextSlat, m_u16NextCue);
            sendFire(m_u16NextSlat, m_u16NextCue);
            m_arrSlatCueState[m_u16NextSlat] &= ~(1<<m_u16NextCue);

            if ((uint8_t)m_u16NextSlat == m_u8CurrentSlat)
                blinkCueButton(m_u16NextCue);

        }

        char sLine[2048];
        if (fgets(sLine, 2048, m_fScript) == NULL)
        {
            fclose(m_fScript);
            m_fScript = NULL;

            ui->btnStartScript->setText("Start Script");
            ui->btnStartScript->setEnabled(true);
            QIcon icon (":green.png");
            ui->btnStartScript->setIcon(icon);

            LOGTEXT("FINISHED ********");
            return;
        }

        char *p = strchr(sLine, '#');
        if (p)
            *p = 0;
        p = sLine;
        while(isspace(*p) && *p)
            p++;

        if (!(*p))
        {
            m_u16NextSlat = SCRIPT_SKIP;
            continue;
        }

        std::string strLine = sLine;
        int iTime = atoi(GetCSV(strLine, 0).c_str());;
        int iSlat = atoi(GetCSV(strLine, 2).c_str());;
        int iCue  = atoi(GetCSV(strLine, 3).c_str());;

        LOGTEXT("Read from file \"%s\"  time:%6d slat:%2d cue:%2d", sLine, iTime, iSlat, iCue);
        m_u16NextSlat = iSlat;
        m_u16NextCue  = iCue;
        if (iTime != 0)
        {
            LOGTEXT("Waiting for %dms", iTime);
            m_tScriptNextTime = QDateTime::currentMSecsSinceEpoch() + iTime;
            bFiring = false;
        }
    }
}

void MainWindow::on_btnBluetoothScan_clicked()
{
    m_Slats->setStringList( QStringList() );
    m_rblBluetooth->startScanning();
}

void MainWindow::on_btnBluetoothConnect_clicked()
{
    ui->textEditLog->clear();
    m_rblBluetooth->connectDevice();
}



void MainWindow::on_btnClear_clicked()
{
    ui->textEditLog->clear();
}


void MainWindow::on_comboBoxControlGroup_currentIndexChanged(int index)
{
    if (m_u8ControlId == index)
        return;

    m_Slats->setStringList( QStringList() );

    m_u8ControlId = index;
    LOGTEXT("New control group %d", m_u8ControlId);

    m_pSettings->setValue("ControlGroupIndex",m_u8ControlId);
    m_pSettings->sync();

    qDebug("iControlGroupIndex %d", m_u8ControlId);

}

void MainWindow::bluetoothReady()
{
    sendSignalStrength(ui->comboBoxSignalStrength->currentIndex());
    qDebug("Send signal strength on bluetooth ready %d", ui->comboBoxSignalStrength->currentIndex());
}
