#include "secdialog.h"
#include "ui_secdialog.h"

#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextStream>
#include <QPixmap>
#include <string>

secDialog::secDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::secDialog)
{
    ui->setupUi(this);
}

secDialog::~secDialog()
{
    delete ui;
}

//void secDialog::addUidDialog(QString message)
//{
//    //QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
//    //ui->textEditLogs->append(message);
//    //ui->UIDlabel->append(message);
//    //QString uid = message;
//    //qDebug() << "string wymyslon" << uid;
//    //qDebug() << uid;
//    ui->UidText->append(message);

//}
//void secDialog::readFromPort()
//{
//    while(this->device->canReadLine())
//    {
//        QString line = this->device->readLine();
//        qDebug() << line;

//        QString terminator = "\r";
//        int pos = line.lastIndexOf(terminator);
//        //qDebug() << line.left(pos);

//        this->addUidDialog(line.left(pos));



//    }
//}
