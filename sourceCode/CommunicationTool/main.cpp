﻿#include <QDir>
#include <QApplication>
#include "GlobalDefine.h"
#include "CommunicationTool.h"

bool is_EN = false;
int main(int argc, char* argv[]) {
#ifndef __linux__
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return 1;
#endif
	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
	a.setWindowIcon(QIcon(":/img/res/CommunicationTool.ico"));

	CommunicationTool mw;
	mw.showNormal();

	return a.exec();
}
