#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Josh");
    a.setApplicationName("Model Maker");

    MainWindow w;
    w.show();

    return a.exec();
}
