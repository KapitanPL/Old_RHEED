#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setStyleSheet("QPushButton { border-style: outset; border-width: 2px; border-radius: 10px; border-color: beige; padding: 2px; background-color:lightgray; min-width: 8em;} QPushButton:pressed {background-color:white}");
    return a.exec();
}
