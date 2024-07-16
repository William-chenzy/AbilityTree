#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PEReaderTool w(0);
    w.show();
    return a.exec();
}
