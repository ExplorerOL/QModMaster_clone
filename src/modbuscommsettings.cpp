#include "modbuscommsettings.h"

ModbusCommSettings::ModbusCommSettings(const QString &fileName, Format format , QObject *parent)
    : QSettings(fileName, format, parent)
{
    this->loadSettings();
}

void  ModbusCommSettings::loadSettings()
{

    if (this->value("TCPPort").isNull())
        m_TCPPort = "502";
    else
        m_TCPPort = this->value("TCPPort").toString();

    if (this->value("SlaveIP").isNull())
        m_slaveIP = "127.000.000.001";
    else
        m_slaveIP = this->value("SlaveIP").toString();

    if (this->value("SerialDev").isNull())
        m_serialDev = "/dev/ttyS";
    else
        m_serialDev = this->value("SerialDev").toString();

    if (this->value("SerialPort").isNull())
    {
        m_serialPort = "1";
        #ifdef Q_OS_WIN32
            m_serialPortName = "COM" + m_serialPort;
        #else
            m_serialPortName = m_serialDev;
            m_serialPortName += QStringLiteral("%1").arg(m_serialPort.toInt() - 1);
        #endif
    }
    else {
        m_serialPort = this->value("SerialPort").toString();
        m_serialPortName = this->value("SerialPortName").toString();
    }

    if (this->value("Baud").isNull())
        m_baud = "9600";
    else
        m_baud = this->value("Baud").toString();

    if (this->value("DataBits").isNull())
        m_dataBits = "8";
    else
        m_dataBits = this->value("DataBits").toString();

    if (this->value("StopBits").isNull())
        m_stopBits = "1";
    else
        m_stopBits = this->value("StopBits").toString();

    if (this->value("Parity").isNull())
        m_parity = "None";
    else
        m_parity = this->value("Parity").toString();

    if (this->value("RTS").isNull())
        #ifdef Q_OS_WIN32
            m_RTS = "Disable";
        #else
            m_RTS = "None";
        #endif
    else
        m_RTS = this->value("RTS").toString();

    if (this->value("MaxNoOfLines").toInt() == 0 ||
            this->value("MaxNoOfLines").isNull())
        m_maxNoOfLines = "50";
    else
        m_maxNoOfLines = this->value("MaxNoOfLines").toString();

    if (this->value("BaseAddr").isNull())
        m_baseAddr = "0";
    else
        m_baseAddr = this->value("BaseAddr").toString();

    if (this->value("TimeOut").isNull())
        m_timeOut = "0";
    else
        m_timeOut = this->value("TimeOut").toString();

    if (this->value("LoggingLevel").isNull())
        m_loggingLevel = 3; //warning level
    else
        m_loggingLevel = this->value("LoggingLevel").toInt();

}

void  ModbusCommSettings::saveSettings()
{

    this->setValue("TCPPort",m_TCPPort);
    this->setValue("SlaveIP",m_slaveIP);
    this->setValue("SerialDev",m_serialDev);
    this->setValue("SerialPort",m_serialPort);
    this->setValue("SerialPortName",m_serialPortName);
    this->setValue("Baud",m_baud);
    this->setValue("DataBits",m_dataBits);
    this->setValue("StopBits",m_stopBits);
    this->setValue("Parity",m_parity);
    this->setValue("RTS",m_RTS);
    this->setValue("MaxNoOfLines",m_maxNoOfLines);
    this->setValue("BaseAddr",m_baseAddr);
    this->setValue("TimeOut",m_timeOut);
    this->setValue("LoggingLevel",m_loggingLevel);

}

QString  ModbusCommSettings::TCPPort()
{
    return m_TCPPort;
}

void ModbusCommSettings::setTCPPort(QString tcpPort)
{
    m_TCPPort = tcpPort;
}

void ModbusCommSettings::setSlaveIP(QString IP)
{
    m_slaveIP = IP;
}

QString  ModbusCommSettings::slaveIP()
{
    return m_slaveIP;
}

QString  ModbusCommSettings::serialDev()
{
    return m_serialDev;
}

QString  ModbusCommSettings::serialPort()
{
    return m_serialPort;
}

QString  ModbusCommSettings::serialPortName()
{
    return m_serialPortName;
}

void ModbusCommSettings::setSerialPort(QString serialPort, QString serialDev)
{
int serialPortNo;

    m_serialDev = serialDev;
    m_serialPort = serialPort;
    serialPortNo = serialPort.toInt();
    #ifdef Q_OS_WIN32
    if (serialPortNo > 9)
        m_serialPortName = "\\\\.\\COM" + serialPort;
    else
        m_serialPortName = "COM" + serialPort;
    #else
        m_serialPortName = serialDev;
        m_serialPortName += QStringLiteral("%1").arg(serialPort.toInt() - 1);
    #endif
}

QString  ModbusCommSettings::baud()
{
    return m_baud;
}

void ModbusCommSettings::setBaud(QString baud)
{
    m_baud = baud;
}

QString  ModbusCommSettings::dataBits()
{
    return m_dataBits;
}

void ModbusCommSettings::setDataBits(QString dataBits)
{
    m_dataBits = dataBits;
}

QString  ModbusCommSettings::stopBits()
{
    return m_stopBits;
}

void ModbusCommSettings::setStopBits(QString stopBits)
{
    m_stopBits = stopBits;
}

QString  ModbusCommSettings::parity()
{
    return m_parity;
}

void ModbusCommSettings::setParity(QString parity)
{
    m_parity = parity;
}

QString  ModbusCommSettings::RTS()
{
    return m_RTS;
}

void ModbusCommSettings::setRTS(QString RTS)
{
    m_RTS = RTS;
}

QString  ModbusCommSettings::maxNoOfLines()
{
    return m_maxNoOfLines;
}

void ModbusCommSettings::setMaxNoOfLines(QString maxNoOfLines)
{
    m_maxNoOfLines = maxNoOfLines;
}

QString  ModbusCommSettings::baseAddr()
{
    return m_baseAddr;
}

void ModbusCommSettings::setBaseAddr(QString baseAddr)
{
    m_baseAddr = baseAddr;
}
QString  ModbusCommSettings::timeOut()
{
    return m_timeOut;
}

void ModbusCommSettings::setTimeOut(QString timeOut)
{
    m_timeOut = timeOut;
}

int  ModbusCommSettings::loggingLevel()
{
    return m_loggingLevel;
}
