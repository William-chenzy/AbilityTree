#include <QTextCodec>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QSpinbox>
#include <qdesktopwidget.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "InitializeWindow.h"
#include "GlobalDefine.h"
#include "define.hpp"
#include <thread>
#include <QDebug>
using namespace std;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
	this->setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));

	language = LoadConfigure::getInstance()->IS_EN();
	animation = LoadConfigure::getInstance()->GetAnimation();

	help_page = new HelpPage();
	set_page = new SettingPage(Main_Set);
	connect(set_page, &SettingPage::MainConfigChange, this, &MainWindow::MainConfigChange);

	InitializeWindow iw(this);
	iw.showNormal();
	while(!iw.IsInitDone()){
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		QApplication::processEvents();
	}
	iw.close();

	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	ui->centralwidget->setAttribute(Qt::WA_TranslucentBackground);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setStyleSheet(this->styleSheet() + PUSH_BUTTON_STYLE + SCROLL_AREA_STYLE);

    QRect deskRect = QApplication::desktop()->availableGeometry();
    if (deskRect.width() < width() || deskRect.height() < height()) {
        this->setMinimumSize(deskRect.width(), deskRect.height());
        this->setFixedSize(deskRect.width(), deskRect.height());
    }
    this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);

	main_layout = static_cast<QGridLayout*>(ui->widget_center->layout());
	module_layout = static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());

	connect(&refresh_viewer, &QTimer::timeout, this, &MainWindow::RefreshViewer);
	refresh_viewer.start(30);
	RefreshViewer();

	SwitchStartPage();
    SwitchLanguage();

	SetStatusBarText("状态：正常", STYLE_COROR_G, false);
}
MainWindow::~MainWindow() { delete ui; }

void MainWindow::SwitchLanguage() {

}

void MainWindow::on_toolButton_main_setting_clicked() {
	MainConfig val{ language,animation };
	set_page->SetModuleParam(val);
	set_page->showNormal();
}
void MainWindow::on_toolButton_help_clicked() {
	help_page->showNormal();
}
void MainWindow::on_toolButton_min_clicked() {
    this->showMinimized();
}
void MainWindow::on_toolButton_max_clicked() {
    if (MainWindow::isMaximized()) this->showNormal();
    else this->showMaximized();
    auto img_str = this->isMaximized() ? "full" : "max";
	auto nor = QString("QToolButton{border-image:url(./res/%1.png);}").arg(img_str);
    auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
	ui->toolButton_max->setStyleSheet(nor + hover + pressed);
}
void MainWindow::on_toolButton_close_clicked() {
	for (auto i : moduleList) delete i.widget;
    exit(0);
}

void MainWindow::MainConfigChange(MainConfig val) {
	bool old = language;
	language = val.language;
	animation = val.animation;
	if (old != val.language)SwitchLanguage();
	LoadConfigure::getInstance()->SetEN(language);
	LoadConfigure::getInstance()->SetAnimation(animation);
}
void MainWindow::on_spinBox_list_show_valueChanged(int val) {
	for (auto& i : moduleList) i.widget->SetMaxListShow(val);
	if (current_m)current_m->widget->GetFouce();
}
void MainWindow::SetStatusBarText(QString msg, QString color, bool help) {
	ui->label_status_text->setText(msg);
	ui->pushButton_status_help->setVisible(help);
	QString style = QString("background: %1;border-radius: 12px;border:0px solid #272e2e;").arg(color);
	if(!color.isEmpty())ui->label_status_bar->setStyleSheet(style);
}


QPoint mouse_pos;
bool is_move_mouse;
int size_m_type = 0;
void MainWindow::mousePressEvent(QMouseEvent* event){
	if (MainWindow::isMaximized()) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	if (event->button() == Qt::LeftButton) {
		QRect rect_top = ui->widget_title->rect();
		rect_top.translate(ui->widget_title->pos());

		int _x = event->screenPos().x();
		int _y = event->screenPos().y();
		if (abs(_x - pos().x()) <= 4 && abs(_y - pos().y()) <= 4) size_m_type = 1;
		else if (abs(_x - pos().x() - width()) <= 4 && abs(_y - pos().y() - height()) <= 4) size_m_type = 2;
		else if (abs(_x - pos().x() - width()) <= 4 && abs(_y - pos().y()) <= 4)size_m_type = 3;
		else if (abs(_x - pos().x()) <= 4 && abs(_y - pos().y() - height()) <= 4) size_m_type = 4;
		else if (abs(_x - this->pos().x()) <= 4) size_m_type = 5;
		else if (abs(_y - pos().y() - height()) <= 4) size_m_type = 6;
		else if (abs(_x - pos().x() - width()) <= 4) size_m_type = 7;
		else if (abs(_y - this->pos().y()) <= 4)size_m_type = 8;
		else size_m_type = 0;

		if (size_m_type) mouse_pos = event->screenPos().toPoint();
		else if (rect_top.contains(event->pos())) {
			is_move_mouse = true;
			mouse_pos = event->screenPos().toPoint();
		}
	}

	QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event){
	if (event->button() == Qt::LeftButton)is_move_mouse = false, size_m_type = 0;
	QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event){
	if (MainWindow::isMaximized()) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	if (is_move_mouse){
		move(this->pos() + (event->screenPos().toPoint() - mouse_pos));
		mouse_pos = event->screenPos().toPoint();
	}
	else if (size_m_type){
		QPoint movePos = event->screenPos().toPoint() - mouse_pos;
		QRect g_Geometry = frameGeometry();

		if (size_m_type == 1)g_Geometry.setLeft(g_Geometry.left() + movePos.x()), g_Geometry.setTop(g_Geometry.top() + movePos.y());
		if (size_m_type == 2)g_Geometry.setRight(g_Geometry.right() + movePos.x()), g_Geometry.setBottom(g_Geometry.bottom() + movePos.y());
		if (size_m_type == 3)g_Geometry.setRight(g_Geometry.right() + movePos.x()), g_Geometry.setTop(g_Geometry.top() + movePos.y());
		if (size_m_type == 4)g_Geometry.setLeft(g_Geometry.left() + movePos.x()), g_Geometry.setBottom(g_Geometry.bottom() + movePos.y());
		if (size_m_type == 5)g_Geometry.setLeft(g_Geometry.left() + movePos.x());
		if (size_m_type == 6) g_Geometry.setBottom(g_Geometry.bottom() + movePos.y());
		if (size_m_type == 7)g_Geometry.setRight(g_Geometry.right() + movePos.x());
		if (size_m_type == 8)g_Geometry.setTop(g_Geometry.top() + movePos.y());

		if (g_Geometry.size().height() > this->minimumSize().height())
			if (g_Geometry.size().width() > this->minimumSize().width()) setGeometry(g_Geometry);

		mouse_pos = event->screenPos().toPoint();
	}

	int _x = abs(event->screenPos().x() - pos().x());
	int _y = abs(event->screenPos().y() - pos().y());

	if (_x <= 4 && _y <= 4)setCursor(Qt::SizeFDiagCursor);
	else if (abs(_x - width()) <= 4 && abs(_y - height()) <= 4)setCursor(Qt::SizeFDiagCursor);
	else if (abs(_x - width()) <= 4 && _y <= 4)setCursor(Qt::SizeBDiagCursor);
	else if (_x <= 4 && abs(_y - height()) <= 4)setCursor(Qt::SizeBDiagCursor);
	else if (_x <= 4)setCursor(Qt::SizeHorCursor);
	else if (abs(_y - height()) <= 4)setCursor(Qt::SizeVerCursor);
	else if (abs(_x - width()) <= 4)setCursor(Qt::SizeHorCursor);
	else if (_y <= 4)setCursor(Qt::SizeVerCursor);
	else setCursor(Qt::ArrowCursor);

	QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event){
	QRect rect_top = ui->widget_title->rect();
	rect_top.translate(ui->widget_title->pos());
	if (rect_top.contains(event->pos())) on_toolButton_max_clicked();
	QWidget::mousePressEvent(event);
}
bool MainWindow::eventFilter(QObject* watched, QEvent* event) {

	return MainWindow::eventFilter(watched, event);
}