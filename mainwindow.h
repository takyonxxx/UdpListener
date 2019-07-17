#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkInterface>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QUdpSocket m_Socket;
    bool m_Hex;

    double red_avg = 0;
    double green_avg = 0;
    double blue_avg = 0;

    void initUDP(int port);
    void deInitUDP();
    void setIpAddress();

private slots:
    void readData();
    void sendData(int port, QByteArray &arrayData);
    void onStateChanged(QAbstractSocket::SocketState);

    void on_pushButton_Connect_clicked();
    void on_pushButton_Send_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_Exit_clicked();

    void on_checkBox_Hex_stateChanged(int arg1);
    void on_pushButton_SetIP_clicked();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
