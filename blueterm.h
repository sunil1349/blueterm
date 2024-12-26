#ifndef BLUETERM_H
#define BLUETERM_H

#include <QMainWindow>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class BlueTerm;
}
QT_END_NAMESPACE

class BlueTerm : public QMainWindow
{
    Q_OBJECT

public:
    BlueTerm(QWidget *parent = nullptr);
    ~BlueTerm();

private slots:
    void receiveMessage();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

    void on_connect_clicked();
    void on_disconnectButton_clicked();

private:
    Ui::BlueTerm *ui;
    QSerialPort serialPort;
    QSerialPortInfo info;
    QString buffer;
    QString code;
    int codeSize;

    void initSerialDefaults();
};
#endif // BLUETERM_H
