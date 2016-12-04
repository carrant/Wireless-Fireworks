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

#include "RBLBluetooth.h"
#include <QThread>

QBluetoothUuid m_uuidRBLService(QString(RBL_SERVICE_UUID));
QBluetoothUuid m_uuidRBLCharTx(QString(RBL_CHAR_TX_UUID));
QBluetoothUuid m_uuidRBLCharRx(QString(RBL_CHAR_RX_UUID));


//extern void logtext(bool bTime, QTextEdit *ptext, char *fmt, ... );


RBLBluetooth::RBLBluetooth()
    : QObject(),
      m_currentDevice(QBluetoothDeviceInfo())
{
    m_u16BufferPosition   = 0;
    m_u8RadioIndex        = 0;
    m_bReading            = false;

    m_deviceDiscoveryAgent  = new QBluetoothDeviceDiscoveryAgent(this);
    m_control               = NULL;
    m_service               = NULL;
    m_RBLServiceFound       = false;

    connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

}

RBLBluetooth::~RBLBluetooth()
{
    clear();
}

void RBLBluetooth::clear()
{
    delete m_deviceDiscoveryAgent;
    m_deviceDiscoveryAgent  = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

    delete m_control;
    delete m_service;

    m_RBLServiceFound   = false;
    m_control           = NULL;
    m_service           = NULL;

    qDeleteAll(m_devices);
    m_devices.clear();

    qDeleteAll(m_buffers);
    m_buffers.clear();

    if (m_pbluebox)
        m_pbluebox->clear();

    if (m_pconnect)
        m_pconnect->setEnabled(false);
}

void RBLBluetooth::init(QComboBox *pbluebox, QPushButton *pconnect)
{
    m_pbluebox = pbluebox;
    m_pconnect = pconnect;
}

uint8_t* RBLBluetooth::loop()
{
    if (m_buffers.size() == 0)
        return NULL;

    RadioBuffer *buf = (RadioBuffer *)m_buffers.takeFirst();

    memcpy(m_arrRadioLoopBuffer, buf->m_buffer, buf->m_size);
    delete buf;
    return m_arrRadioLoopBuffer;
}

void RBLBluetooth::startScanning()
{
    clear();

    //btnBluetoothScane
    m_deviceDiscoveryAgent->start();

    LogDebug("Scanning for devices...");
}

void RBLBluetooth::connectDevice() // connects to current device in combo box
{
    DeviceInfo *dev = m_pbluebox->currentData().value<DeviceInfo *>();
    if (dev == NULL)
        return;

    LogDebug("Connect to %s %s", dev->getName().toUtf8().constData(), dev->getAddress().toUtf8().constData())

    connectToService(dev->getAddress());
}

void RBLBluetooth::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
                   << device.address().toString();
        DeviceInfo *dev = new DeviceInfo(device);
        m_devices.append(dev);

        m_pbluebox->addItem(device.name(), QVariant::fromValue(dev));
        m_pconnect->setEnabled(true);

        LogDebug("Low Energy device found. Scanning for more...");
    }
    //...
}

void RBLBluetooth::scanFinished()
{
    if (m_devices.size() == 0)
      LogDebug("No Low Energy devices found");
    Q_EMIT nameChanged();
}

void RBLBluetooth::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
    {
        LogDebug("The Bluetooth adaptor is powered off, power it on before doing discovery.");
    }
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
    {
        LogDebug("Writing or reading from the device resulted in an error.");
    }
    else
    {
        LogDebug("An unknown error has occurred.");
    }
}

QVariant RBLBluetooth::name()
{
    return QVariant::fromValue(m_devices);
}



void RBLBluetooth::connectToService(const QString &address)
{
    bool deviceFound = false;
    for (int i = 0; i < m_devices.size(); i++)
    {
        if (((DeviceInfo*)m_devices.at(i))->getAddress() == address )
        {
            m_currentDevice.setDevice(((DeviceInfo*)m_devices.at(i))->getDevice());
            LogDebug("Connecting to device...");
            deviceFound = true;
            break;
        }
    }
    // we are running demo mode
    if (!deviceFound)
    {
        return;
    }

    if (m_control)
    {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = 0;

    }
    m_control = new QLowEnergyController(m_currentDevice.getDevice(), this);
    connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this, SLOT(serviceDiscovered(QBluetoothUuid)));
    connect(m_control, SIGNAL(discoveryFinished()),
            this, SLOT(serviceScanDone()));
    connect(m_control, SIGNAL(error(QLowEnergyController::Error)),
            this, SLOT(controllerError(QLowEnergyController::Error)));
    connect(m_control, SIGNAL(connected()),
            this, SLOT(deviceConnected()));
    connect(m_control, SIGNAL(disconnected()),
            this, SLOT(deviceDisconnected()));

    m_control->connectToDevice();
}


void RBLBluetooth::deviceConnected()
{
    m_control->discoverServices();
}

void RBLBluetooth::deviceDisconnected()
{
    LogDebug("RBL service disconnected");
    qWarning() << "Remote device disconnected";
}

void RBLBluetooth::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt == m_uuidRBLService)
    {
        m_RBLServiceFound = true;
        LogDebug("RBL service discovered. Waiting for service scan to be done...");
    }
}


void RBLBluetooth::serviceScanDone()
{
    delete m_service;
    m_service = 0;

    if (m_RBLServiceFound)
    {
        LogDebug("Connecting to service...");
        m_service = m_control->createServiceObject(m_uuidRBLService, this);
    }

    if (!m_service)
    {
        LogDebug("RBL Service not found.");
        return;
    }

    connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
            this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(readChar(QLowEnergyCharacteristic,QByteArray)));
    connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)),
            this, SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)));

    m_service->discoverDetails();
}

void RBLBluetooth::disconnectService()
{
    m_RBLServiceFound = false;

    //disable notifications
    if (m_notificationDesc.isValid() && m_service)
    {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
    }
    else
    {
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = 0;
    }
}

void RBLBluetooth::controllerError(QLowEnergyController::Error error)
{
    LogDebug("Cannot connect to remote device.");
    qWarning() << "Controller Error:" << error;
}


void RBLBluetooth::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    LogDebug("state %d", s);
    switch (s)
    {
    case QLowEnergyService::ServiceDiscovered:
    {
        const QLowEnergyCharacteristic txChar = m_service->characteristic(m_uuidRBLCharTx);
        if (!txChar.isValid())
        {
            LogDebug("RBL Data not found.");
            break;
        }

        const QLowEnergyDescriptor m_notificationDesc = txChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid())
        {
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
        }
        LogDebug("RBL found.");

        emit bluetoothDeviceReady();
        break;
    }
    default:
        //nothing for now
        break;
    }
}

void RBLBluetooth::serviceError(QLowEnergyService::ServiceError e)
{
    switch (e) {
    case QLowEnergyService::DescriptorWriteError:
        LogDebug("Cannot obtain HR notifications");
        break;
    default:
        qWarning() << "HR service error:" << e;
    }
}

void RBLBluetooth::confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                         const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray("0000"))
    {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = 0;
    }
}

// 20 bytes MAX!
void RBLBluetooth::readChar(const QLowEnergyCharacteristic &c,
                                     const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != m_uuidRBLCharTx)
        return;


    LogDebug("recv size:%d", value.size());
    const char *data = reinterpret_cast<const char *>(value.constData());
//    for (int x=0; x < value.size(); x++)
//        LogDebug("recv %02d %02x", x, value[x]);

    for (int x=0; x < value.size(); x++)
    {
        char ch = value[x];

        if (!m_bReading)
        {
            if (ch == '#')
            {
                m_u8RadioIndex = 0;
                m_u16BufferPosition = 0;
                m_bReading = true;
                continue;
            }

            continue;
        }

        m_arrRadioBuffer[m_u8RadioIndex++] = ch;
        if ( m_u8RadioIndex >= RADIO_SIZE)
        {
            m_bReading = false;
            m_u8RadioIndex = 0;
            m_buffers.append(new RadioBuffer(m_arrRadioBuffer, RADIO_SIZE));
        }
    }
//    Q_EMIT hrChanged();
}

void RBLBluetooth::sendData(void *pData, uint16_t u16Size)
{
    if (!m_service)
        return;

    const QLowEnergyCharacteristic rxChar = m_service->characteristic(m_uuidRBLCharRx);
    if (!rxChar.isValid())
    {
        LogDebug("RBL Data not found.");
        return;
    }

    LogDebug("Sending Data");
    QByteArray barray;
    barray.append('#');
    barray.append((char *)pData, u16Size+1);
    int iTotal = barray.size();
    while (iTotal > 0)
    {
        int iToSend = iTotal > 20 ? 20 : iTotal;
        m_service->writeCharacteristic(rxChar, barray, QLowEnergyService::WriteWithoutResponse);
QThread::msleep(200);
        barray = barray.mid(iToSend);
        iTotal -= iToSend;
    }

    if (m_service->error() != QLowEnergyService::NoError)
    {
        LogDebug("Error writing characteristic %d", m_service->error());
    }
}
