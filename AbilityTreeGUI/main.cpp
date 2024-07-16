#include <windows.h>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) 
{
#ifndef __linux__
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	HWND hWnd = GetConsoleWindow(); // 获取控制台窗口句柄
	ShowWindow(hWnd, SW_HIDE); // 隐藏控制台窗口
#endif

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(QPixmap("./res/LOGO-AT.png")));

	MainWindow mw;
	mw.showNormal();

	return a.exec();
}
