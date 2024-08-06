#include "PeFileAnalyzer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PeFileAnalyzer w(0);
    w.show();
    return a.exec();
}
