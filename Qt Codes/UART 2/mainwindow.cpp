#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QImageReader>
#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>
#include <QDateTime>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Serial Terminal");
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

    /*Conexão do clique do botão com o slot que inicia a conexão*/
    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), this, SLOT(connectToSerial()));  //connect to serial
    /*Conexão do sinal "pronto para leitura" com o slot que faz a leitura. Desse modo, quando
    é feita uma leitura, o método sai ao terminá-la. Adicionalmente estamos escrevendo para a
    porta serial, enviando um Byte para acender ou apagar o LED no Arduino.
    Em alguns casos pode ser necessário utilizar threads, depende de  diversos  fatores,  mas
    normalmente dá pra resolver muita coisa só utilizando SIGNAL & SLOT.*/
    connect(this->serialPort,SIGNAL(readyRead()),this,SLOT(startReading()));

    /*Quando pressionado enter, tem o mesmo efeito do botão*/
    connect(ui->lineEdit_type,SIGNAL(returnPressed()),this,SLOT(feedChat()));

    connect(ui->lineEdit_type, SIGNAL(returnPressed()), this, SLOT(feedChat()));
    connect(ui->pushButton_send, SIGNAL(clicked(bool)), this, SLOT(feedChat()));
    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), this, SLOT(enableWidgets()));

    ui->pushButton_send->setEnabled(false);
    ui->lineEdit_type->setEnabled(false);
    ui->plainTextEdit->setEnabled(false);

    this->enableWidgets(false);

    ui->plainTextEdit->setReadOnly(true);

    bool ok;
    this->username = QInputDialog::getText(this, tr("Entre com um username"),
                                             tr("User name:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
}

void MainWindow::enableWidgets(bool activate){
    if (!activate){
        ui->plainTextEdit->clear();
    }
    ui->lineEdit_type->setEnabled(activate);
    ui->pushButton_send->setEnabled(activate);
    ui->plainTextEdit->setEnabled(activate);
}

void MainWindow::feedChat(){
    ui->plainTextEdit->appendPlainText(ui->lineEdit_type->text());  //imprime oq foi escrito
//    ui->plainTextEdit->clear();

    QString msg = QDateTime::currentDateTime().toString() + "\n";

    msg        += this->username.remove("\r\n") + "> " + ui->lineEdit_type->text().remove("\r\n");

    QString val = msg.split('\n').at(1) + '\n';
    qDebug() << val;
    //this->serialPort->write(">>>");
    this->serialPort->write(val.toUtf8());

    ui->lineEdit_type->clear();
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::feedChatFromSerial(QByteArray msg)
{
/* Coloca data antes do que foi printado
    QString dt = QDateTime::currentDateTime().toString() + "\n";
    dt += msg;
*/
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::startReading(){
    QByteArray readData = this->serialPort->readAll();
    qDebug() << readData;

    this->feedChatFromSerial(readData); //added

/*    //A cada chamada enviamos um estado diferente para o LED, para fazê-lo piscar.
    if (this->ON){
        this->ON = false;
        this->serialPort->write("1\n");
    }
    else{
        this->ON = true;
        this->serialPort->write("0\n");
    }

*/
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
        this->enableWidgets(false);
        return;
    }

    //se for para abrir, pegamos os parâmetros de velocidade e porta a abrir.
    this->serialPort->setPortName(ui->comboBox_port->currentText());
    this->serialPort->setBaudRate(ui->comboBox_baudrate->currentText().toUInt());

    if(!this->serialPort->open(QIODevice::ReadWrite)){
        ui->label_status->setText("Failed");
        return;
    }
    ui->label_status->setText("Connected");
    ui->pushButton_connect->setText("Disconnect");

    emit startReading(true);
    enableWidgets(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
