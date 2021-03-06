#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLabel>
QTimer timer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::addDevice);

    connect(ui->pbStart, &QPushButton::pressed,  this, &MainWindow::Start);
    connect(ui->pbStop,  &QPushButton::pressed,  m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::stop);
    connect(ui->pbSend, &QPushButton::pressed, this, &MainWindow::OnBtnSend);

    connect(ui->pbConnect, &QPushButton::pressed, this, &MainWindow::OnConnectBtn);

    connect(ui->deviceList, &QListWidget::itemSelectionChanged, this, &MainWindow::OnSelect);
    connect(&timer, &QTimer::timeout, this, &MainWindow::OnTimer);

    ui->statusbar->addWidget(new QLabel("no connection"));

  //  connect(ui->pbService, &QPushButton::pressed, this, &MainWindow::OnServiceBtn);
}

void MainWindow::Start()
{
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void MainWindow::addDevice(const QBluetoothDeviceInfo &info)
{
    deviceList.append(new QBluetoothDeviceInfo(info));
    ui->deviceList->addItem(info.name());
}

void MainWindow::OnConnectBtn()
{
    if (selectedDeviceInfo != nullptr)
    {
        m_controller = QLowEnergyController::createCentral(*selectedDeviceInfo);
        connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &MainWindow::serviceDiscovered);
        connect(m_controller, &QLowEnergyController::discoveryFinished, this, &MainWindow::discoveryDone);
        connect(m_controller, &QLowEnergyController::connected, this, &MainWindow::OnConnect );


        m_controller->connectToDevice();
    }
}
void MainWindow::OnConnect()
{
    qDebug() << ("connected");
    m_controller->discoverServices();

}
void MainWindow::serviceDiscovered(const QBluetoothUuid &newService)
{
   qDebug()<<QString::number( newService.toUInt16(),16);
   services.append(newService);
}
void MainWindow::OnBtnSend()
{
    if (m_service)
    {
        ui->textEdit->append(ui->lineEdit->text());
        m_service->writeCharacteristic(m_service->characteristics().at(0), ui->lineEdit->text().toUtf8());
    }

  //  services.clear();
}
void MainWindow::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
   // m_characteristic = characteristic;
    ui->textEdit->append(QString::fromUtf8(newValue));
}

void MainWindow::discoveryDone()
{
    timer.start(10);
}
void MainWindow::OnTimer()
{
    timer.stop();
    m_service = m_controller->createServiceObject(QBluetoothUuid("{0000ffe0-0000-1000-8000-00805f9b34fb}"));/*services[rows[0].row()]*/

    if (m_service)
    {
        qDebug() << ("service created");
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &MainWindow::characteristicChanged);
        connect(m_service, &QLowEnergyService::errorOccurred, this, &MainWindow::errorOccurred);

        m_service->discoverDetails();
        QLabel* stat= ui->statusbar->findChild<QLabel*>();
        if (stat)
            stat->setText("connected");

    }else
    {
        qDebug() << "no service";
    }
}
void MainWindow::errorOccurred(QLowEnergyService::ServiceError newError)
{
    qDebug() << newError;
}
void MainWindow::OnSelect()
{
    int irow=0;
    QModelIndexList rows=  ui->deviceList->selectionModel()->selectedRows();
    if (rows.count()>0)
    {
        irow = rows[0].row();

        selectedDeviceInfo = deviceList[irow];
     //   ui->textEdit->setText(QString::number(selectedDeviceInfo->majorDeviceClass()));
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

