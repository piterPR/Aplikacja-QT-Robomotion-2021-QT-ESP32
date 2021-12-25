#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //style sheet "C:/Users/piotr/Desktop/QTROBO/img/robomotion.png";
//    QFile styleSheetFile("C:/Users/piotr/Desktop/QTROBO/Diffnes.qss");
//    styleSheetFile.open(QFile::ReadOnly);
//    QString styleSheet = QLatin1String(styleSheetFile.readAll());
//    a.setStyleSheet(styleSheet);

    //a.setWindowIcon(QIcon("./windowIcon"));
    //    QIcon icon(":./rfid.png");
    //    a.setWindowIcon(QIcon(":./rfid.png"));
    MainWindow w;
    w.resize(1080,820);
    //a.setWindowIcon(icon);
    w.show();

    return a.exec();
}
