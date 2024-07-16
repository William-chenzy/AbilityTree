#include "LxNetCfgTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LxNetcfgTool w(0);
    w.show();
    return a.exec();
}
