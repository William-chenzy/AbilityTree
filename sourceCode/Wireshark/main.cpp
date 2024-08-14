#include "Wireshark.h"
#include "GlobalDefine.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
    Wireshark w(0);
    w.show();
    return a.exec();
}
