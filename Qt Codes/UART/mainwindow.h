#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSerialPort *serialPort;   //Reservate memory for object type serialPort

    bool ON = false;

public slots:
    void connectToSerial();
    void startReading();


signals:
    void startReading(bool ok);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
