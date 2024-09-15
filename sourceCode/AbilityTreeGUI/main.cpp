#include <QDir>
#include <define.hpp>
#include "mainwindow.h"
#include "GlobalDefine.h"
#include <QMessageBox>
#include <QApplication>
#include <QSurfaceFormat>

std::vector<ModuleMember>moduleList;
bool SetGlVersion(int ver_j, int ver_n) {
	QSurfaceFormat format;
	format.setVersion(ver_j, ver_n);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	QOpenGLContext context;
	context.setFormat(format);
	return context.create();
}

int main(int argc, char* argv[]) {
#ifndef __linux__
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
#define GL_VER 3
#define GL_VER_2 0
#else
#define GL_VER 0
#define GL_VER_2 3
#endif

	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
	a.setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));

	if (!SetGlVersion(3, GL_VER)) {
		if (!SetGlVersion(3, GL_VER_2)) {
			QMessageBox box;
			box.setWindowTitle("错误");
			box.setIcon(QMessageBox::Warning);
			box.setText("软件无法适用当前系统OpenGL版本\r\n反馈此问题来获得支持!");
			QPushButton *openWeb = new QPushButton("反馈问题");
			box.addButton(openWeb, QMessageBox::ActionRole);
			box.addButton(QMessageBox::Ok);
			box.exec();

			if (box.clickedButton() == openWeb) {
				std::string cmd = (is_Linux ? "xdg-open " : "start ");
				system((cmd + "http://www.abilitytree.cn/ProblemFeedback").c_str());
			}
			exit(0);
		}
	}
	MainWindow mw;
	mw.showNormal();
	mw.setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));

	return a.exec();
}
