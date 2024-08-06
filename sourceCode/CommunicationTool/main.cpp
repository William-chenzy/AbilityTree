﻿#include <QDir>
#include <windows.h>
#include <QApplication>
#include "CommunicationTool.h"

bool is_EN = false;
int main(int argc, char* argv[]) {
#ifndef __linux__
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
#endif
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/img/res/CommunicationTool.ico"));

	CommunicationTool mw;
	mw.showNormal();

	return a.exec();
}