#include "LxWireshark.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LxWireshark w(0);
    w.show();
    return a.exec();
}
