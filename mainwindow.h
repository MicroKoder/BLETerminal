#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtBluetooth>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;

    void addDevice(const QBluetoothDeviceInfo &info);
    QVector<QBluetoothDeviceInfo*> deviceList;
    QVector<QBluetoothUuid> services;

    QBluetoothDeviceInfo *selectedDeviceInfo= nullptr;
    QLowEnergyService *m_service=nullptr;
    QLowEnergyController *m_controller;
    QLowEnergyCharacteristic m_characteristic;
public slots:
    void Start();
    void OnConnectBtn();
    void OnSelect();
    void serviceDiscovered(const QBluetoothUuid &newService);
    void discoveryDone();
    void OnConnect();
    void OnBtnSend();
    void errorOccurred(QLowEnergyService::ServiceError newError);
    void OnTimer();
 //   void OnServiceBtn();
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
