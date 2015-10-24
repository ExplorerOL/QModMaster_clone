#include <QtDebug>
#include <QtWidgets/QMessageBox>

#include "QsLog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "eutils.h"

const QString PACKETS="Packets : ";
const QString ERRORS="Errors : ";

MainWindow::MainWindow(QWidget *parent, ModbusAdapter *adapter, ModbusCommSettings *settings) :
    QMainWindow(parent), m_modbus(adapter), m_modbusCommSettings(settings),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //UI - dialogs
    m_dlgAbout = new About();
    connect(ui->actionAbout,SIGNAL(triggered()),m_dlgAbout,SLOT(show()));
    m_dlgModbusRTU = new SettingsModbusRTU(this,m_modbusCommSettings);
    connect(ui->actionSerial_RTU,SIGNAL(triggered()),this,SLOT(showSettingsModbusRTU()));
    m_dlgModbusTCP = new SettingsModbusTCP(this,m_modbusCommSettings);
    connect(ui->actionTCP,SIGNAL(triggered()),this,SLOT(showSettingsModbusTCP()));
    m_dlgSettings = new Settings(this,m_modbusCommSettings);
    connect(ui->actionSettings,SIGNAL(triggered()),this,SLOT(showSettings()));
    m_busMonitor = new BusMonitor(this, m_modbus->rawModel);
    connect(ui->actionBus_Monitor,SIGNAL(triggered()),this,SLOT(showBusMonitor()));

    //UI - connections
    connect(ui->cmbModbusMode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedModbusMode(int)));
    connect(ui->cmbFunctionCode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedFunctionCode(int)));
    connect(ui->cmbBase,SIGNAL(currentIndexChanged(int)),this,SLOT(changedBase(int)));
    connect(ui->sbNoOfCoils,SIGNAL(valueChanged(int)),this,SLOT(changedNoOfRegs(int)));
    connect(ui->sbStartAddress,SIGNAL(valueChanged(int)),this,SLOT(changedStartAddress(int)));
    connect(ui->spInterval,SIGNAL(valueChanged(int)),this,SLOT(changedScanRate(int)));
    connect(ui->btAddItems,SIGNAL(clicked(bool)),this,SLOT(addItems()));
    connect(ui->btClear,SIGNAL(clicked(bool)),this,SLOT(clearItems()));
    connect(ui->actionRead_Write,SIGNAL(triggered()),this,SLOT(request()));
    connect(ui->actionScan,SIGNAL(toggled(bool)),this,SLOT(scan(bool)));
    connect(ui->actionConnect,SIGNAL(toggled(bool)),this,SLOT(changedConnect(bool)));
    connect(ui->actionReset_Counters,SIGNAL(triggered()),this,SIGNAL(resetCounters()));

    //UI - status
    m_statusInd = new QLabel;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;
    m_statusPackets = new QLabel(PACKETS + "0");
    m_statusPackets->setStyleSheet("QLabel {color:blue;}");
    m_statusErrors = new QLabel(ERRORS + "0");
    m_statusErrors->setStyleSheet("QLabel {color:red;}");
    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 10);
    ui->statusBar->addWidget(m_statusPackets, 10);
    ui->statusBar->addWidget(m_statusErrors, 10);
    m_statusInd->setPixmap(QPixmap(":/img/ballorange-16.png"));

    //Populate function name combo box
    ui->cmbFunctionCode->clear();
    for(int i=0; i<8; i++)
        ui->cmbFunctionCode->addItem(EUtils::ModbusFunctionName(i));

    //Setup Toolbar
    ui->mainToolBar->addAction(ui->actionConnect);
    ui->mainToolBar->addAction(ui->actionRead_Write);
    ui->mainToolBar->addAction(ui->actionScan);
    ui->mainToolBar->addAction(ui->actionReset_Counters);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionBus_Monitor);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionSerial_RTU);
    ui->mainToolBar->addAction(ui->actionTCP);
    ui->mainToolBar->addAction(ui->actionSettings);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionAbout);
    ui->mainToolBar->addAction(ui->actionExit);

    //Init models
    ui->tblRegisters->setItemDelegate(m_modbus->regModel->itemDelegate());
    ui->tblRegisters->setModel(m_modbus->regModel->model);
    m_modbus->regModel->setBase(EUtils::UInt);

    //Update UI
    ui->sbNoOfCoils->setEnabled(true);
    ui->actionRead_Write->setEnabled(false);
    ui->actionScan->setEnabled(false);
    updateStatusBar();

    QLOG_INFO()<<  "Start Program" ;

}

MainWindow::~MainWindow()
{

    delete ui;

    QLOG_INFO()<<  "Stop Program" ;

}

void MainWindow::showSettingsModbusRTU()
{

    //Show RTU Settings Dialog

    if (m_dlgModbusRTU->exec()==QDialog::Accepted) {
        QLOG_INFO()<<  "RTU settings changes accepted ";
        updateStatusBar();
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_INFO()<<  "RTU settings changes rejected ";

}

void MainWindow::showSettingsModbusTCP()
{

    //Show TCP Settings Dialog

    if (m_dlgModbusTCP->exec()==QDialog::Accepted) {
        QLOG_INFO()<<  "TCP settings changes accepted ";
        updateStatusBar();
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_INFO()<<  "TCP settings changes rejected ";

}

void MainWindow::showSettings()
{

    //Show General Settings Dialog

    if (m_dlgSettings->exec()==QDialog::Accepted) {
        QLOG_INFO()<<  "Settings changes accepted ";
        m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->maxNoOfLines().toInt());
        m_modbus->setTimeOut(m_modbusCommSettings->timeOut().toInt());
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_INFO()<<  "Settings changes rejected ";

}

void MainWindow::showBusMonitor()
{

    //Show Bus Monitor

    m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->maxNoOfLines().toInt());
    m_busMonitor->show();

}

void MainWindow::changedModbusMode(int currIndex)
{

    //Show-Hide Slave IP

    QLOG_INFO()<<  "Modbus Mode changed. Index = " << currIndex;

    if (currIndex == 0) { //RTU

    }
    else { //TCP

    }

    updateStatusBar();

}

void MainWindow::changedFunctionCode(int currIndex)
{

    //Enable-Disable number of coils or registers

    QLOG_INFO()<<  "Function Code changed. Index = " << currIndex;

    const int funcionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());

    switch(funcionCode)//Label = Read Request, Write Request
    {
        case _FC_READ_COILS:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Coils");
                break;
        case _FC_READ_DISCRETE_INPUTS:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Coils");
                break;
        case _FC_READ_HOLDING_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Registers");
                break;
        case _FC_READ_INPUT_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Registers");
                break;
        case _FC_WRITE_SINGLE_COIL:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfCoils->setValue(1);
                ui->sbNoOfCoils->setEnabled(false);
                ui->lblNoOfCoils->setText("Number of Coils");
                break;
        case _FC_WRITE_MULTIPLE_COILS:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfCoils->setValue(2);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Coils");
                break;
        case _FC_WRITE_SINGLE_REGISTER:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfCoils->setValue(1);
                ui->sbNoOfCoils->setEnabled(false);
                ui->lblNoOfCoils->setText("Number of Registers");
                break;
        case _FC_WRITE_MULTIPLE_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfCoils->setValue(2);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Registers");
                break;
        default:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfCoils->setValue(1);
                ui->sbNoOfCoils->setEnabled(true);
                ui->lblNoOfCoils->setText("Number of Coils");
                break;
     }

    m_modbus->setNumOfRegs(ui->sbNoOfCoils->value());
    addItems();

}

void MainWindow::changedBase(int currIndex)
{

    //Change Base

    QLOG_INFO()<<  "Base changed. Index = " << currIndex;

    switch(currIndex)
    {
        case 0:
                m_modbus->regModel->setBase(EUtils::Bin);
                break;
        case 1:
                m_modbus->regModel->setBase(EUtils::UInt);
                break;
        case 2:
                m_modbus->regModel->setBase(EUtils::Hex);
                break;
        default:
                m_modbus->regModel->setBase(EUtils::UInt);
                break;
     }

}

void MainWindow::changedScanRate(int value)
{

    //Enable-Disable Time Interval

    QLOG_INFO()<<  "ScanRate changed. Value = " << value;

    m_modbus->setScanRate(value);

}

void MainWindow::changedConnect(bool value)
{

    //Connect - Disconnect

    if (value) { //Connected
        modbusConnect(true);
        QLOG_INFO()<<  "Connected ";
    }
    else { //Disconnected
        modbusConnect(false);
        QLOG_INFO()<<  "Disconnected ";
    }

    m_modbus->resetCounters();
    refreshView();

}

void MainWindow::changedSlaveIP()
{

    // NOT USED

}

void MainWindow::changedStartAddress(int value)
{

    //Start Address changed
    QLOG_INFO()<<  "Start Address changed. Value = " << value;

    m_modbus->setStartAddr(value);
    addItems();

}

void MainWindow::changedNoOfRegs(int value)
{

    //No of regs changed
    QLOG_INFO()<<  "No Of Regs changed. Value = " << value;

    m_modbus->setNumOfRegs(value);
    addItems();

}

void MainWindow::updateStatusBar()
{

    //Update status bar

    QString msg;

    if(ui->cmbModbusMode->currentIndex() == 0) { //RTU
        msg = "RTU : ";
        msg += m_modbusCommSettings->serialPort() + " | ";
        msg += m_modbusCommSettings->baud() + ",";
        msg += m_modbusCommSettings->dataBits() + ",";
        msg += m_modbusCommSettings->stopBits() + ",";
        msg += m_modbusCommSettings->parity();
    }
    else {
        msg = "TCP : ";
        msg += m_modbusCommSettings->slaveIP() + ":";
        msg += m_modbusCommSettings->TCPPort();
    }

    m_statusText->clear();
    m_statusText->setText(msg);

    //Connection is valid
    if (m_modbus->isConnected()) {
        m_statusInd->setPixmap(QPixmap(":/img/ballgreen-16.png"));
    }
    else {
        m_statusInd->setPixmap(QPixmap(":/img/ballorange-16.png"));
    }

}

void MainWindow::addItems()
{

    //Add items to registers model and to modbus adapter only if we are connected
    //if (!m_modbus->isConnected())
        //return;

    m_modbus->setSlave(ui->sbSlaveID->value());
    m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    m_modbus->setStartAddr(ui->sbStartAddress->value());
    m_modbus->setNumOfRegs(ui->sbNoOfCoils->value());

    QLOG_INFO()<<  "Add Items. Function Code = " << QString::number(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()),16);

    m_modbus->addItems();

}

void MainWindow::clearItems()
{

    //Clear items from registers model

    QLOG_INFO()<<  "clearItems" ;

    m_modbus->regModel->clear();

}

void MainWindow::request()
{

     //Request items from modbus adapter and add raw data to raw data model
    int rowCount = m_modbus->regModel->model->rowCount();
    QLOG_INFO()<<  "Request transaction. No or registers = " <<  rowCount;

    if (rowCount == 0) {
        QMessageBox::critical(this, "Request failed","Add items to Registers Table.");
        QLOG_WARN()<<  "Request failed. No items in registers table ";
        return;
    }

    m_modbus->setSlave(ui->sbSlaveID->value());
    m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    m_modbus->setStartAddr(ui->sbStartAddress->value());
    m_modbus->setNumOfRegs(ui->sbNoOfCoils->value());

    //Modbus data
    m_modbus->modbusTransaction();

}

void MainWindow::scan(bool value)
{

    //Request items from modbus adapter and add raw data to raw data model
   int rowCount = m_modbus->regModel->model->rowCount();

   if (value && rowCount == 0) {
       QMessageBox::critical(this, "Request failed","Add items to Registers Table.");
       QLOG_WARN()<<  "Request failed. No items in registers table ";
       ui->actionScan->setChecked(false);
       return;
   }

   m_modbus->setSlave(ui->sbSlaveID->value());
   m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
   m_modbus->setStartAddr(ui->sbStartAddress->value());
   m_modbus->setNumOfRegs(ui->sbNoOfCoils->value());

    //Start-Stop poll timer
    QLOG_INFO()<<  "Scan time = " << value;
    if (value){
        m_modbus->setScanRate(ui->spInterval->value());
        m_modbus->startPollTimer();
    }
    else
        m_modbus->stopPollTimer();

    ui->btAddItems->setEnabled(!value);
    ui->btClear->setEnabled(!value);
    ui->cmbBase->setEnabled(!value);
    ui->cmbFunctionCode->setEnabled(!value);
    ui->sbSlaveID->setEnabled(!value);
    ui->sbStartAddress->setEnabled(!value);
    ui->spInterval->setEnabled(!value);
    if (!value)
        changedFunctionCode(ui->cmbFunctionCode->currentIndex());
    else
        ui->sbNoOfCoils->setEnabled(false);

}

void MainWindow::modbusConnect(bool connect)
 {

    //Modbus connect - RTU/TCP
    QLOG_INFO()<<  "Modbus Connect. Value = " << connect;

    if (connect) { //RTU
        if (ui->cmbModbusMode->currentIndex() == EUtils::RTU) {
            m_modbus->modbusConnectRTU(m_modbusCommSettings->serialPort(),
                                        m_modbusCommSettings->baud().toInt(),
                                        EUtils::parity(m_modbusCommSettings->parity()),
                                        m_modbusCommSettings->dataBits().toInt(),
                                        m_modbusCommSettings->stopBits().toInt(),
                                        m_modbusCommSettings->RTS().toInt(),
                                        m_modbusCommSettings->timeOut().toInt()
                                        );
        }
        else { //TCP
            m_modbus->modbusConnectTCP(m_modbusCommSettings->slaveIP(),
                                       m_modbusCommSettings->TCPPort().toInt(),
                                       m_modbusCommSettings->timeOut().toInt());
        }
    }
    else { //Disconnect
        m_modbus->modbusDisConnect();
        ui->actionScan->setChecked(false);
    }

    updateStatusBar();

    //Update UI
    ui->actionConnect->setChecked(m_modbus->isConnected());
    ui->actionRead_Write->setEnabled(m_modbus->isConnected());
    ui->actionScan->setEnabled(m_modbus->isConnected());
    ui->cmbModbusMode->setEnabled(!m_modbus->isConnected());

 }

 void MainWindow::refreshView()
 {

     QLOG_INFO()<<  "Packets sent / received = " << m_modbus->packets() << ", errors = " << m_modbus->errors();
     ui->tblRegisters->resizeColumnsToContents();

     m_statusPackets->setText(PACKETS + QString("%1").arg(m_modbus->packets()));
     m_statusErrors->setText(ERRORS + QString("%1").arg(m_modbus->errors()));

 }
