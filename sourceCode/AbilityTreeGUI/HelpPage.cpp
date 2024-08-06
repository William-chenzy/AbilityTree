#include <QTextCodec>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QSpinbox>
#include <qdesktopwidget.h>
#include "HelpPage.h"
#include "ui_HelpPage.h"
#include "GlobalDefine.h"
#include "define.hpp"
#include <QIcon>
#include <thread>
#include <QDebug>
using namespace std;

HelpPage::HelpPage(QWidget* parent) :ui(new Ui::HelpPage) {
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
}
HelpPage::~HelpPage() { delete ui; }
void HelpPage::closeEvent(QCloseEvent *event) {
	this->hide();
}