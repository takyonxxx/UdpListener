#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_Hex(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setStyleSheet("font-size: 10pt; color: #cccccc; background-color: #001a1a;");

    connect(&m_Socket, &QUdpSocket::readyRead,this,&MainWindow::readData , Qt::UniqueConnection);
    connect(&m_Socket, &QAbstractSocket::stateChanged, this, &MainWindow::onStateChanged);
    setIpAddress();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setIpAddress()
{
    bool found = false;
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
            foreach (QNetworkAddressEntry entry, interface.addressEntries())
            {
                qDebug() << interface.name() + " " + entry.ip().toString() +" " + interface.hardwareAddress();
                if ( !found && interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".")
                     && !interface.humanReadableName().contains("VM") && interface.name().contains("ethernet"))
                {                    
                    ui->lineEdit_IP->setText(entry.ip().toString());
                    found = true;
                }
            }
    }
}

void MainWindow::initUDP(int port)
{
    if(m_Socket.state() != QUdpSocket::BoundState)
    {
        m_Socket.bind(QHostAddress(ui->lineEdit_IP->text()),static_cast<quint16>(port));
    }
}

void MainWindow::deInitUDP()
{
    if(m_Socket.state() == QUdpSocket::BoundState)
    {
        m_Socket.close();
    }
}

void MainWindow::readData()
{
    QHostAddress sender;
    quint16 senderPort;

    // listener for datagrams heard...
    while (m_Socket.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(static_cast<long>(m_Socket.pendingDatagramSize()));
        m_Socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if(datagram.data() != nullptr)
        {
            if(!m_Hex)
                ui->textBrowser->append(QString("<-- ") + QString::number(datagram.size()) + " : " + QString::fromLatin1(datagram.data()));
            else
                ui->textBrowser->append(QString("<-- ") + QString::number(datagram.size()) + " : " + datagram.toHex());
        }
    }
}

void MainWindow::on_pushButton_Connect_clicked()
{
    if(m_Socket.state() == QUdpSocket::BoundState)
    {
        deInitUDP();
    }
    else
    {
        initUDP(ui->lineEdit_Port->text().toInt());
    }

}

void MainWindow::sendData(int port, QByteArray &arrayData)
{
    QUdpSocket m_sSocket;
    m_sSocket.writeDatagram(arrayData, QHostAddress::LocalHost, port);
    arrayData.clear();
}

void MainWindow::onStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
    case QAbstractSocket::UnconnectedState:
        ui->pushButton_Connect->setText("Connect");
        ui->textBrowser->append(QString("Disconnected from ") + ui->lineEdit_IP->text() + QString(" Port : ") + ui->lineEdit_Port->text());
        break;

    case QAbstractSocket::ConnectedState:
        ui->pushButton_Connect->setText("Disconnect");
        ui->textBrowser->append(QString("Connected to ") + ui->lineEdit_IP->text() + QString(" Port : ") + ui->lineEdit_Port->text());
        break;

    case QAbstractSocket::BoundState:
        ui->pushButton_Connect->setText("Disconnect");
        ui->textBrowser->append(QString("Connected to ") + ui->lineEdit_IP->text() + QString(" Port : ") + ui->lineEdit_Port->text());
        break;

    default:
        break;
    }
}


void MainWindow::on_pushButton_Send_clicked()
{
    QByteArray data;
    data.append(ui->lineEdit_Send->text());
    sendData(ui->lineEdit_Port->text().toInt(), data);
    ui->textBrowser->append(QString("--> ") + ui->lineEdit_Send->text());
}

void MainWindow::on_pushButton_Clear_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_pushButton_Exit_clicked()
{
    exit(0);
}

void MainWindow::on_checkBox_Hex_stateChanged(int arg1)
{
    m_Hex = arg1;
}

void MainWindow::on_pushButton_SetIP_clicked()
{
    setIpAddress();
}
