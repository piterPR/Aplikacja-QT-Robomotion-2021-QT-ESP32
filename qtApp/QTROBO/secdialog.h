#ifndef SECDIALOG_H
#define SECDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class secDialog;
}

class secDialog : public QDialog
{
    Q_OBJECT

public:
    explicit secDialog(QWidget *parent = nullptr);
    ~secDialog();

private slots:
//    void readFromPort();

private:
    Ui::secDialog *ui;
    QSerialPort *device;
//    void addUidDialog(QString message);
//    void sendMessageToDevice(QString message);

};

#endif // SECDIALOG_H
