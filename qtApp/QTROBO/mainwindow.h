#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSearch_clicked();

    void on_pushButtonConnect_clicked();

    void readFromPort();

    void setClipboard(QString uid);

    void saveTag(QString uid,int copyThing);

    void on_pushButtonCloseConnection_clicked();

    void on_pushButtonLedOn_clicked();

    void on_pushButtonLedOff_clicked();

    void on_pushButtonREAD_clicked();

    void on_savenfc_clicked();

    void on_SaveON_clicked();

    void on_pushButtonNEW_clicked();

    void on_copyToClipboard_clicked(QString uidd);

    void on_pushButtonCopyTime_clicked(QString time);

    void on_pushButtonOneGate_clicked();

    void on_pushButtonDualGate_clicked();

    void on_copyToClipboardTime_clicked();

    void on_pushButtonCopyTime_clicked();

private:
    Ui::MainWindow *ui;

    void addToLogs(QString message);

    QSerialPort *device;
    void sendMessageToDevice(QString message);

     QString clipboardText;

};

#endif // MAINWINDOW_H
