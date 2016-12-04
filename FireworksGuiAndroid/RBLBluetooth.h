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

#ifndef RBLBLuetooth_h
#define RBLBLuetooth_h

#include <QMainWindow>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include <qbluetoothuuid.h>

#include "utils.h"

#define RBL_SERVICE_UUID    "713D0000-503E-4C75-BA94-3148F18D941E"
#define RBL_CHAR_TX_UUID    "713D0002-503E-4C75-BA94-3148F18D941E"
#define RBL_CHAR_RX_UUID    "713D0003-503E-4C75-BA94-3148F18D941E"

#define RADIO_SIZE          32

class RadioBuffer: public QObject
{
    Q_OBJECT

public:
    RadioBuffer(uint8_t *p, uint16_t u16Size)
    {
        m_size = u16Size;
        m_buffer = new uint8_t[m_size];
        memcpy(m_buffer, p, m_size);
    }

    ~RadioBuffer()
    {
        delete m_buffer;
    }

    uint8_t *m_buffer;
    uint16_t m_size;
};

class DeviceInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)
public:
    DeviceInfo(const QBluetoothDeviceInfo &device)
        : QObject(), m_device(device)
    {

    }

    void setDevice(const QBluetoothDeviceInfo &device)
    {
        m_device = device;
        emit deviceChanged();
    }

    QString getName() const { return m_device.name(); }
    QString getAddress() const
    {
    #ifdef Q_OS_MAC
        // workaround for Core Bluetooth:
        return m_device.deviceUuid().toString();
    #else
        return m_device.address().toString();
    #endif
    }

    QBluetoothDeviceInfo getDevice() const
    {
        return m_device;
    }


signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo m_device;
};


class RBLBluetooth : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant name READ name NOTIFY nameChanged)

public:
    RBLBluetooth();
    ~RBLBluetooth();

public:
    void init(QComboBox *pbluebox, QPushButton *pconnect);  //TARDIS ??
    uint8_t* loop();

    void clear();

    void startScanning();
    void connectDevice();

    void sendData(void *pData, uint16_t u16Size);

public:
    QVariant name();

signals:
    void bluetoothDeviceReady();

public slots:
    void connectToService(const QString &address);
    void disconnectService();

private slots:

    //QBluetothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo &device);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    //QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();


    //QLowEnergyService
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void readChar(const QLowEnergyCharacteristic &c,
                              const QByteArray &value);

    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value);
    void serviceError(QLowEnergyService::ServiceError e);


Q_SIGNALS:
    void nameChanged();

private:
    QComboBox *         m_pbluebox;
    QPushButton *       m_pconnect;

    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QList<QObject*>     m_devices;
    DeviceInfo          m_currentDevice;
    QLowEnergyController *m_control;
    QLowEnergyService *m_service;
    QLowEnergyDescriptor m_notificationDesc;
    bool                m_RBLServiceFound;


    bool    m_bReading;
    uint8_t m_arrBuffer[128 + 1];
    uint8_t m_u16BufferPosition;
    uint8_t m_u8RadioIndex;
    uint8_t m_arrRadioBuffer[RADIO_SIZE];

    QList<QObject*> m_buffers;
    uint8_t m_arrRadioLoopBuffer[RADIO_SIZE];
};


#endif // #ifndef RBLBLuetooth_h


