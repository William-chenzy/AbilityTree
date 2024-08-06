#include "ThreadInjection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ThreadInjection w(0);
    w.show();
    return a.exec();
}
