#include "Wireshark.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Wireshark w(0);
    w.show();
    return a.exec();
}
