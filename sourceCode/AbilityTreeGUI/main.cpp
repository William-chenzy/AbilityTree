#include <QDir>
#include <define.hpp>
#include "mainwindow.h"
#include "GlobalDefine.h"
#include <QApplication>

std::vector<ModuleMember>moduleList;
int main(int argc, char* argv[]) {
#ifndef __linux__
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
#endif
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));

	MainWindow mw;
	mw.showNormal();
	mw.setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));

	return a.exec();
}
