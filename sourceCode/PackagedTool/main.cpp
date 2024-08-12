#include "PackagedTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PackagedTool w(0);
    w.show();
    return a.exec();
}
