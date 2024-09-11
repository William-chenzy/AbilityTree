#include "CloudAnimation.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
    CloudAnimation w(0);
    w.show();
    return a.exec();
}
