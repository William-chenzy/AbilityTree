﻿#include "Server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w(0);
    w.show();
    return a.exec();
}