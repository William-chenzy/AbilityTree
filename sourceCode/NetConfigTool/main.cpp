#include "NetConfigTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetConfigTool w(0);
    w.show();
    return a.exec();
}
