﻿#include "ui_Server.h"
#include "Server.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <fstream>
#include <thread>
#include <deque>
#include <fstream>
#include <QPushButton>
#include <QSlider>
#include <QFileDialog>
#include <QApplication>
#include "GlobalDefine.h"
using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

Server::Server(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::Server)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-Server.png")));

}
