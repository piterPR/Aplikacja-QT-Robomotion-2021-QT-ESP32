#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1080,820);
    w.show();

    return a.exec();
}
