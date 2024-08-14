#include "define.hpp"
#include <QTextCodec>
#include <QApplication>
#include <QMouseEvent>
#include <qdesktopwidget.h>
#include "InitializeWindow.h"
#include "GlobalDefine.h"
#include <thread>
#include <QDebug>
using namespace std;

InitializeWindow::InitializeWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::InitializeWindow) {
    ui->setupUi(this);

	this->setAttribute(Qt::WA_TranslucentBackground);
    QRect deskRect = QApplication::desktop()->availableGeometry();

	int _width = deskRect.width() / 3;
	int _height = deskRect.height() / 3;
	int f_val = _width < _height ? _width : _height;
	this->setFixedWidth(f_val), this->setFixedHeight(f_val);
    this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

	connect(&animation, &QTimer::timeout, this, &InitializeWindow::RefreshAnimation);
	connect(&refresh, &QTimer::timeout, this, &InitializeWindow::ReloadParam);
	refresh.setSingleShot(true);
	animation.start(16);
	refresh.start(100);

	init_done = false;
	add_type = true;

    SwitchLanguage();
}
InitializeWindow::~InitializeWindow() { delete ui; }

void InitializeWindow::SwitchLanguage() {
	ui->label_info->setText(SOFTWARE_INFORMATION);
}

void InitializeWindow::ReloadParam() {
	RefreshStatus("正在加载模块资源...");
	if (!LoadModuleNumbers())exit(0);
	RefreshStatus("正在预制图像资源...");
	DrawAllImg(QCoreApplication::applicationDirPath());

	RefreshStatus("正在加载用户模块...");

	RefreshStatus("正在加载插件...");

	RefreshStatus("正在检测电脑配置...");

	for (int i = 0; i < 100; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		QApplication::processEvents();
	}

	init_done = true;
}
void InitializeWindow::RefreshStatus(QString msg) {
	ui->label_status->setText(msg);
	QApplication::processEvents();
}
void InitializeWindow::RefreshAnimation() {
	if (add_type) {
		if (g < 90)g += 1;
		else add_type = false, g -= 1;
	}
	if (!add_type) {
		if (g > 10)g -= 1;
		else add_type = true, g += 1;
	}
	QString color = QString("stop:0 #f4716e, stop:0.%2 #42f875, stop:1 #1edffc").arg(g);
	QString style = QString("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,%1);").arg(color);
	ui->widget_3->setStyleSheet(style);
}

void InitializeWindow::mousePressEvent(QMouseEvent* event) {
	if (event->button() != Qt::LeftButton) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	QRect rect_top = ui->centralwidget->rect();
	rect_top.translate(ui->centralwidget->pos());
	if (rect_top.contains(event->pos())) {
		is_move_mouse = true;
		mouse_pos = event->screenPos().toPoint();
	}

	QWidget::mousePressEvent(event);
}
void InitializeWindow::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}
void InitializeWindow::mouseMoveEvent(QMouseEvent* event) {
	if (!is_move_mouse) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	move(this->pos() + (event->screenPos().toPoint() - mouse_pos));
	mouse_pos = event->screenPos().toPoint();
	QWidget::mouseMoveEvent(event);
}
