#include "blueterm.h"
#include "ui_blueterm.h"

BlueTerm::BlueTerm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BlueTerm)
{
    ui->setupUi(this);

    // Disable maximizing
    setFixedSize(width(), height());

    // Adding title for widget
    QWidget::setWindowTitle("BlueTerm");

    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->serial),"Serial");
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->BlueTooth),"BlueTooth");
    //ui->serial->setWindowTitle("serial");
    initSerialDefaults();
}

BlueTerm::~BlueTerm()
{
    delete ui;
}

void BlueTerm::on_connect_clicked()
{

    QString portName = ui->comboBox->currentText();
    serialPort.setPortName(portName);

    serialPort.open(QIODevice::ReadWrite);

    if(!serialPort.isOpen()){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("!!!! Something went Wrong !!!!");
        //qDebug("serial port is not opened");
        printf("serial port is not opened");
    }
    else {
        ui->textBrowser->setTextColor(Qt::green);
        ui->textBrowser->append("!!!! connected !!!!");
        QString stringbaudRate = ui->comboBox_2->currentText();
        int intbaudRate = stringbaudRate.toInt();
        serialPort.setBaudRate(intbaudRate);

        QString dataBits = ui->comboBox_3->currentText();
        if(dataBits == "5 Bits") {
            serialPort.setDataBits(QSerialPort::Data5);
        }
        else if((dataBits == "6 Bits")) {
            serialPort.setDataBits(QSerialPort::Data6);
        }
        else if(dataBits == "7 Bits") {
            serialPort.setDataBits(QSerialPort::Data7);
        }
        else if(dataBits == "8 Bits"){
            serialPort.setDataBits(QSerialPort::Data8);
        }

        QString stopBits = ui->comboBox_4->currentText();
        if(stopBits == "1 Bit") {
            serialPort.setStopBits(QSerialPort::OneStop);
        }
        else if(stopBits == "1,5 Bits") {
            serialPort.setStopBits(QSerialPort::OneAndHalfStop);
        }
        else if(stopBits == "2 Bits") {
            serialPort.setStopBits(QSerialPort::TwoStop);
        }

        QString parity = ui->comboBox_5->currentText();
        if(parity == "No Parity"){
            serialPort.setParity(QSerialPort::NoParity);
        }
        else if(parity == "Even Parity"){
            serialPort.setParity(QSerialPort::EvenParity);
        }
        else if(parity == "Odd Parity"){
            serialPort.setParity(QSerialPort::OddParity);
        }
        else if(parity == "Mark Parity"){
            serialPort.setParity(QSerialPort::MarkParity);
        }
        else if(parity == "Space Parity") {
            serialPort.setParity(QSerialPort::SpaceParity);
        }


        QString flowControl = ui->comboBox_6->currentText();
        if(flowControl == "No Flow Control") {
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
        }
        else if(flowControl == "Hardware Flow Control") {
            serialPort.setFlowControl(QSerialPort::HardwareControl);
        }
        else if(flowControl == "Software Flow Control") {
            serialPort.setFlowControl(QSerialPort::SoftwareControl);
        }

        code = "\n\r";
        codeSize = code.size();

        connect(&serialPort,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
    }


}

void BlueTerm::initSerialDefaults()
{
    // Ports
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox->addItems(stringPorts);

    // Baud Rate Ratios
    QList<qint32> baudRates = info.standardBaudRates(); // What baudrates does my computer support ?
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->comboBox_2->addItems(stringBaudRates);

    // Data Bits
    ui->comboBox_3->addItem("8");
    ui->comboBox_3->addItem("6");
    ui->comboBox_3->addItem("7");
    ui->comboBox_3->addItem("5");

    // Stop Bits
    ui->comboBox_4->addItem("1 Bit");
    ui->comboBox_4->addItem("1,5 Bits");
    ui->comboBox_4->addItem("2 Bits");

    // Parities
    ui->comboBox_5->addItem("No Parity");
    ui->comboBox_5->addItem("Even Parity");
    ui->comboBox_5->addItem("Odd Parity");
    ui->comboBox_5->addItem("Mark Parity");
    ui->comboBox_5->addItem("Space Parity");

    //Flow Controls
    ui->comboBox_6->addItem("No Flow Control");
    ui->comboBox_6->addItem("Hardware Flow Control");
    ui->comboBox_6->addItem("Software Flow Control");
}

/*
During process of making this program, i encountered a problem.
Library of QSerialPort does not guarantee that received message comes at once.It mostly comes part by part.
Sometimes it comes completely.But it is not guarantee.I tried to solve this issue by giving message a code.
I stored every received message in buffer. Whenever i store message in my buffer,i searched my code in buffer.
If it is found,i display the part of the message until the code. And i remove that message from buffer.
If it is not found i keep storing received message in buffer.
*/


void BlueTerm::receiveMessage()
{
    QByteArray dataBA = serialPort.readAll();

    QString data(dataBA); // Convert raw bytes to QString
    buffer.append(data);  // Append to the buffer

    // Split the buffer by the termination code
    QStringList messages = buffer.split(code);

    // Process all complete messages except the last part (incomplete)
    for (int i = 0; i < messages.size() - 1; ++i) {
        //QString cleanMessage = messages[i].trimmed();
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append(messages[i]);
    }

    // Retain the last incomplete part in the buffer
    buffer = messages.last();

}

void BlueTerm::on_pushButton_clicked()
{
    QString message = ui->lineEdit_2->text();
    ui->textBrowser->setTextColor(Qt::black); // Color of message to send is green.
    ui->textBrowser->append(message);
    if(ui->CRCheck->isChecked())
    {
        message.append("\n\r");
    }
    serialPort.write(message.toUtf8());
}


// Button of Disconnect
void BlueTerm::on_disconnectButton_clicked()
{
    ui->textBrowser->setTextColor(Qt::red);
    ui->textBrowser->append("!!!! Disconnect !!!!");
    serialPort.close();
}

// Button of Refresh Ports
void BlueTerm::on_pushButton_4_clicked()
{
    ui->comboBox->clear();
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox->addItems(stringPorts);
}

// Button of Clear
void BlueTerm::on_pushButton_5_clicked()
{
    ui->textBrowser->clear();
}


