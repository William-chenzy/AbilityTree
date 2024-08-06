#include "PhotosTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PhotosTool w(0);
    w.show();
    return a.exec();
}
