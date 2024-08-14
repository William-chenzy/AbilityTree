#include "PhotosTool.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
    PhotosTool w(0);
    w.show();
    return a.exec();
}
