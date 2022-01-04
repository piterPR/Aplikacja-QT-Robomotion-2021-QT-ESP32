#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secdialog.h"


#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextStream>
#include <QPixmap>
#include <QClipboard>
#include <string>

#include <QRegularExpression>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->device = new QSerialPort(this);

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
              this, SLOT(clipboardChanged()));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSearch_clicked()
{
    ui->comboBoxDevices->clear();

    this->addToLogs("Szukam urządzeń...");

    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();

    for(int i = 0; i < devices.count(); i++)
    {
        this->addToLogs("Znalazłem urządzenie: " + devices.at(i).portName() + " " + devices.at(i).description());
        ui->comboBoxDevices->addItem(devices.at(i).portName() + "\t" + devices.at(i).description());
    }
}

//overloading to see result in console

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

void MainWindow::addToLogs(QString message)
{
    ui->textEditLogs->append(message);
    ui->textEditLogs_2->append(message);
    ui->textEditLogs_3->append(message);



}

void MainWindow::sendMessageToDevice(QString message)
{
    if(this->device->isOpen())
    {
        this->addToLogs("Wysyłam informacje do urządzenia " + message);
        this->device->write(message.toStdString().c_str());
    }
    else
    {
        this->addToLogs("Nie mogę wysłać wiadomości. Nie podłączono urządzenia do odpowiedniego portu!");
    }
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if(ui->comboBoxDevices->count() == 0)
    {
        this->addToLogs("Nie wykryto żadnych urządzeń!");
        return;
    }

    QString comboBoxQString = ui->comboBoxDevices->currentText();
    QStringList portList = comboBoxQString.split("\t");
    QString portName = portList.first();

    this->device->setPortName(portName);

    // OTWÓRZ I SKONFIGURUJ PORT:
    if(!device->isOpen())
    {
        if(device->open(QSerialPort::ReadWrite))
        {
            this->device->setBaudRate(QSerialPort::Baud115200);
            this->device->setDataBits(QSerialPort::Data8);
            this->device->setParity(QSerialPort::NoParity);
            this->device->setStopBits(QSerialPort::OneStop);
            this->device->setFlowControl(QSerialPort::NoFlowControl);

            // CONNECT:
            connect(this->device, SIGNAL(readyRead()),
                    this, SLOT(readFromPort()));

            this->addToLogs("Otwarto port szeregowy.");
        }
        else
        {
            this->addToLogs("Otwarcie porty szeregowego się nie powiodło!");
        }
    }
    else
    {
        this->addToLogs("Port już jest otwarty!");
        return;
    }
}

void MainWindow::readFromPort()
{
    while(this->device->canReadLine())
    {
        QString uidStr = "UID";
        QString timeStr = "Czas przejazdu: ";
        QString nfcTimeout = "nfcTimeout";
        QString uidSaved = "Zapisano nowe klucze i bity dostepu sektora 1 (#4 - #7)";
        QString line = this->device->readLine();
        qDebug() << line;
        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);
        if(line.contains(uidStr, Qt::CaseInsensitive)){
            qDebug() << "Read line:";
            QString participant = line;
            saveTag(participant,0); // zapisz do schowka tag
            ui->lbl_button_state->setText("Nieaktywny");
            ui->pushButtonREAD->setChecked(false);
        }

        else if(line.contains(timeStr, Qt::CaseInsensitive)){
            qDebug() << "Read line time:";
            QString time = line;
            saveTag(time,1); // zapisz do schowka czas
            ui->lbl_buton_one_gate->setText("Nieaktywny");
            ui->lbl_buton_dual_gate->setText("Nieaktywny");
            ui->pushButtonOneGate->setChecked(false);
            ui->pushButtonDualGate->setChecked(false);
            ui->pushButtonOneGate->setText("Nieaktywny");
            ui->pushButtonDualGate->setText("Nieaktywny");
        }
        else if(line.contains(nfcTimeout, Qt::CaseInsensitive)){
            qDebug() << "NFC Timeout";
            ui->lbl_button_state->setText("Nieaktywny");
            ui->pushButtonREAD->setChecked(false);
        }
        else if(line.contains(uidSaved, Qt::CaseInsensitive)){
            qDebug() << "Card wrote successfully";
            ui->labelsave->setText("Nieaktywny");
            ui->SaveON->setChecked(false);
        }


        this->addToLogs(line.left(pos));



    }
}

void MainWindow::on_pushButtonCloseConnection_clicked()
{
    if(this->device->isOpen())
    {
        this->device->close();
        this->addToLogs("Zamknięto połączenie.");
    }
    else
    {
        this->addToLogs("Port nie jest otwarty!");
        return;
    }
}

void MainWindow::on_pushButtonLedOn_clicked()
{
    this->sendMessageToDevice("1");
}

void MainWindow::on_pushButtonLedOff_clicked()
{
    this->sendMessageToDevice("0");
}


void MainWindow::on_pushButtonREAD_clicked()
{
    if (ui->pushButtonREAD->isChecked()){
        ui->lbl_button_state->setText("Aktywny");
        this->sendMessageToDevice("2");
    }
    else{
        ui->lbl_button_state->setText("Nieaktywny");

        this->sendMessageToDevice("3");
    }

}




void MainWindow::on_savenfc_clicked()
{

    qDebug("clicked");
    QString NFC_save_tag = ui->lineEdit->text(); 
    qStdOut() << NFC_save_tag;
    this->sendMessageToDevice(NFC_save_tag);

}


void MainWindow::on_SaveON_clicked()
{
    if (ui->SaveON->isChecked()){
        ui->labelsave->setText("Aktywny");
        this->sendMessageToDevice("4");
    }
    else{
        ui->labelsave->setText("Nieaktywny");
        this->sendMessageToDevice("6");
    }
}

void MainWindow::on_pushButtonNEW_clicked()
{
    secDialog secdialog;
    secdialog.setModal(true);
    secdialog.exec();
}



void MainWindow::saveTag(QString uid,int flag)
{

    if(flag == 0){
        qDebug()<<uid;
        qDebug()<<"Cut UID";
        QString uidCut = uid.mid(53,17);
        qDebug()<<uidCut;
        qDebug()<<"XD";
        if(uidCut.isEmpty()==0){
           QString uidFinal;
           QString dash = "-";
           QString firstSegment = uidCut.mid(0,8);
           QString secodSegment = uidCut.mid(8,4);
           QString thirdSegment = "11ec";
           QString fourthSegment = uidCut.mid(12,4);
           QString fifthSegment = "b8ca3a5bc7d0";
           uidFinal += firstSegment+dash+secodSegment+dash+thirdSegment+dash+fourthSegment+dash+fifthSegment;
           on_copyToClipboard_clicked(uidFinal);
        }

    }
    else if(flag == 1){
        qDebug()<<uid;
        qDebug()<<"Cut Time";
        QString timeCut = uid.mid(15,6);
        qDebug()<<timeCut;
        on_copyToClipboard_clicked(timeCut);
    }





}



void MainWindow::on_copyToClipboard_clicked(QString uidd)
{
   setClipboard(uidd);
}

void MainWindow::setClipboard(QString uid)
{

   QApplication::clipboard()->setText(uid);
}

void MainWindow::on_pushButtonOneGate_clicked()
{
    if (ui->pushButtonOneGate->isChecked()){
        ui->lbl_buton_one_gate->setText("Aktywny");
        this->sendMessageToDevice("5");
    }
    else{
        ui->lbl_buton_one_gate->setText("Nieaktywny");
        this->sendMessageToDevice("6");
    }

}


void MainWindow::on_pushButtonDualGate_clicked()
{
    if (ui->pushButtonDualGate->isChecked()){
        ui->lbl_buton_dual_gate->setText("Aktywny");
        this->sendMessageToDevice("6");
    }
    else{
        ui->lbl_buton_dual_gate->setText("Nieaktywny");
        //this->sendMessageToDevice("6");
    }
}




void MainWindow::on_pushButtonCopyTime_clicked(QString time)
{
    setClipboard(time);

}


