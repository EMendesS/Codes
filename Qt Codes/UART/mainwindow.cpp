#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->serialPort = new QSerialPort; //Instantiate serialPort

    QStringList ports;  //to put the value on the combo box related to the serialport
    const auto serialPortInfos = QSerialPortInfo::availablePorts();    //catch all the available ports on the system
    int i = 0;
    for(const QSerialPortInfo &serialPortInfo : serialPortInfos ){
        ports.insert(i, serialPortInfo.portName());
        i++;
    }
    ui->comboBox_port->insertItems(0, ports);
    QStringList bauds;
    bauds << "1200" << "2400" << "4800" << "9600" << "19200" << "38400" << "57600" << "115200";
    ui->comboBox_baudrate->insertItems(0, bauds);

    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), this, SLOT(connectToSerial()));  //connect to serial
    connect(this->serialPort,SIGNAL(readyRead()),this,SLOT(startReading()));
}

void MainWindow::startReading(){
    QByteArray readData = this->serialPort->readAll();
    qDebug() << readData;

    //A cada chamada enviamos um estado diferente para o LED, para fazê-lo piscar.
    if (this->ON){
        this->ON = false;
        this->serialPort->write("1\n");
    }
    else{
        this->ON = true;
        this->serialPort->write("0\n");
    }


//    while(this->serialPort->waitForReadyRead(5000)){
//        readData = this->serialPort->readAll();
//        qDebug() << readData;
//    }

}

void MainWindow::connectToSerial(){
    if(this->serialPort->isOpen()){
        this->serialPort->close();
        ui->label_status->setText("Disconnected");
        ui->pushButton_connect->setText("Connect");
        return;
    }

    this->serialPort->setPortName(ui->comboBox_port->currentText());
    this->serialPort->setBaudRate(ui->comboBox_baudrate->currentText().toUInt());

    if(!this->serialPort->open(QIODevice::ReadOnly)){
        ui->label_status->setText("Failed");
        return;
    }
    ui->label_status->setText("Connected");
    ui->pushButton_connect->setText("Disconnect");

    emit startReading(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
