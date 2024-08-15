#include <ModuleCard.h>
#include <QApplication>
#include <QDebug>
#include <random>
#include <QPainter>
#include "define.hpp"


static long long card_index = 0;
static GlobalImage* ptr = nullptr;
ModuleCard::ModuleCard(QWidget *parent) : QWidget(parent) {
	card = new QWidget();
	mask = new QWidget();
	name_lab = new QLabel();
	icon_lab = new QLabel();
	background = new QLabel();
	main_layout = new QVBoxLayout();
	card_layout = new QGridLayout();
	this->setLayout(main_layout);
	card->setLayout(card_layout);
	name_lab->setWordWrap(true);

	obj_name = QString("ModuleCard_%1").arg(card_index++);
	icon_lab->setFixedWidth(64), icon_lab->setFixedHeight(64);
	name_lab->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	card_layout->setHorizontalSpacing(6);
	card_layout->setVerticalSpacing(6);
	card->setObjectName(obj_name);
	main_layout->setMargin(0);

	AddStartModifyButton();

	name_lab->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Maximum);
	card_layout->addWidget(icon_lab, 0, 0, 1, 2, Qt::AlignHCenter | Qt::AlignVCenter);
	card_layout->addWidget(background, 0, 0, 3, 2);
	card_layout->addWidget(name_lab, 1, 0, 2, 2);
	card_layout->addWidget(mask, 0, 0, 3, 2);
	main_layout->addWidget(card);
	background->lower();

	QString border("qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #1edffc, stop:0.3 transparent, stop:0.7 transparent, stop:1 #1edffc)");
	card->setStyleSheet(WidgetStyle + QString("#%1{border-radius: 6px;border:2px solid %2;}").arg(obj_name).arg(border));
	name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 24px;font-weight: bold;color: #AAAAAA;");

	program = new QProcess();

	connect(&in_out, &QTimer::timeout, this, &ModuleCard::InOutAnimation);
	connect(&refresh, &QTimer::timeout, this, &ModuleCard::CreateAnimation);
	connect(program, static_cast<void (QProcess::*)(int)>(&QProcess::finished), [&](int) {
		start->setText("运行");
		background->setVisible(false);
		GlobalImage::getInstance()->UnInstallEvent(background, card);
		QString border("qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #1edffc, stop:0.3 transparent, stop:0.7 transparent, stop:1 #1edffc)");
		card->setStyleSheet(WidgetStyle + QString("#%1{border-radius: 6px;border:2px solid %2;}").arg(obj_name).arg(border));
		});
}
ModuleCard::~ModuleCard() {
	disconnect(&in_out, &QTimer::timeout, this, &ModuleCard::InOutAnimation);
	disconnect(&refresh, &QTimer::timeout, this, &ModuleCard::CreateAnimation);
	DeleteButtonClicked(true);
	refresh.stop();
	in_out.stop();
}

void ModuleCard::BeginAnimation() {
	if (refresh.isActive())return;
	mask->raise();
	mask_val = 99;
	mask_val2 = 99;
	mask->setVisible(true);
	if (LoadConfigure::getInstance()->GetAnimation())refresh.start(16);
}
void ModuleCard::CreateAnimation() {
	auto style = QString("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #aaaaaa, stop:0.%1 #aaaaaa, stop:0.%2 transparent)")
				.arg(mask_val).arg(mask_val2);
	mask->setStyleSheet(style);
	mask_val -= 6;
	if (mask_val < 10)mask_val = 0;
	if (mask_val <= 50)mask_val2 -= 6;
	if (mask_val <= 10 && mask_val2 <= 10) {
		mask->setStyleSheet("background: transparent");
		mask->setVisible(false);
		refresh.stop();
		mask->lower();
	}
}
void ModuleCard::InOutAnimation() {
	auto style = QString("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 rgba(30,223,252,48), stop:0.%1 transparent)").arg(mask_val2);
	mask->setStyleSheet(style);
	if ((mask_val2 += 8) >= 99)in_out.stop();
}

void ModuleCard::SetParam(QString name, QString icon, int _width, int _height) {
	if (_width > 0) {
		this->setFixedWidth(_width);
		name_lab->setFixedWidth(_width - card_layout->margin() * 2);
	}
	if (_height > 0)this->setFixedHeight(_height);
	GlobalImage::getInstance()->SetParam(_width, _height);

	if (!name.isEmpty())name_lab->setText(name);
	if (!icon.isEmpty())icon_lab->setStyleSheet(QString("border-image:url(%1);").arg(icon));
}

void ModuleCard::enterEvent(QEvent* event) {
	mask_val2 = 10;
	in_out.start(16);
	mask->setVisible(true);
	if (refresh.isActive())refresh.stop();

	mask->lower();
	card->setStyleSheet(WidgetStyle + QString("#%1{border-radius: 6px;border:2px solid #1edffc;}").arg(obj_name));
}
void ModuleCard::leaveEvent(QEvent* event) {
	mask->setVisible(false);
	QString border("qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #1edffc, stop:0.3 transparent, stop:0.7 transparent, stop:1 #1edffc)");
	card->setStyleSheet(WidgetStyle + QString("#%1{border-radius: 6px;border:2px solid %2;}").arg(obj_name).arg(border));
}

void ModuleCard::AddStartModifyButton() {
	modify_widget = new QWidget();
	modify = new QPushButton("•••");
	start = new QPushButton("运行");
	go_back = new QPushButton("返回");
	delete_button = new QPushButton("删除");
	modify_widget_layout = new QVBoxLayout();
	modify_widget->setLayout(modify_widget_layout);
	modify_widget_layout->setAlignment(Qt::AlignVCenter);

	card_layout->addWidget(modify, 2, 0);
	card_layout->addWidget(start, 2, 1);
	main_layout->addWidget(modify_widget);
	modify_widget_layout->addWidget(delete_button);
	modify_widget_layout->addWidget(go_back);
	modify_widget->setVisible(false);

	QString styleSheet = "\
        QPushButton {\
            font-family: \"华文楷体\";\
            font-size: 16px;\
            font-weight: bold;\
            color: #AAAAAA;\
            border-radius: 1px;\
            border: 0px solid #474e5e;\
            background-color: transparent;\
        }\
        QPushButton:hover {\
            background: qlineargradient(x1:1, y1:1, x2:1, y2:0, stop:0 #f4716e, stop:1 transparent);\
        }\
        QPushButton:pressed {\
            background: qlineargradient(x1:1, y1:1, x2:1, y2:0, stop:0 #42f875, stop:1 transparent);\
        }\
        QPushButton::disabled {\
            background-color: #7d7d7d;\
        }";
	QString sheet = "font-family: \"华文楷体\";font-size: 16px;font-weight: bold;color: #AAAAAA;";
	modify_widget->setStyleSheet(styleSheet);
	modify->setStyleSheet(sheet);
	start->setStyleSheet(sheet);

	connect(modify, &QPushButton::clicked, this, [&]() {
		modify_widget->setVisible(true);
		card->setVisible(false);
		});
	connect(go_back, &QPushButton::clicked, this, [&]() {
		modify_widget->setVisible(false);
		card->setVisible(true);
		});
	connect(start, &QPushButton::clicked, this, &ModuleCard::StartButtonClicked);
	connect(delete_button, &QPushButton::clicked, this, &ModuleCard::DeleteButtonClicked);
}
void ModuleCard::StartButtonClicked() {
	if (program->state() != QProcess::Running) {
		QString raw_path = QCoreApplication::applicationDirPath() + "/";
		QString check_str = raw_path + exe_path + (is_Linux ? "" : ".exe");
		if (!CheckFile(check_str)) {
			if (exe_path == "Locked")QMessageBox::information(nullptr, "提示", "功能被锁定，将会在后续更新中开放!");
			else QMessageBox::warning(nullptr, "错误", "文件损坏或不存在，请重新安装或检查文件路径\r\n\部分功能只能在Windows或Linux下运行!");
			return;
		}
		int max_wait = 100;
		program->start(check_str);
		for (; max_wait && program->waitForStarted(10); max_wait--);
		if (program->state() != QProcess::Running) {
			QMessageBox::warning(nullptr, "错误", "程序启动失败!");
			return;
		}

		GlobalImage::getInstance()->GlobalImage::InstallEvent(background, card);
		background->setVisible(true);
		start->setText("终止");
		mask_val = 0;
	}
	else program->kill();
}
void ModuleCard::DeleteButtonClicked(bool is_exit) {
	if (program->state() == QProcess::Running) {
		QString msg = "模块: " + name_lab->text() + " 正在运行，是否结束?";
		auto res = QMessageBox::information(nullptr, LU_STR44, msg, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (res == QMessageBox::Cancel)return;
		disconnect(program, static_cast<void (QProcess::*)(int)>(&QProcess::finished), nullptr, nullptr);
		if (res == QMessageBox::Yes)program->kill(), delete program;
	}
	else if(!is_exit) {
		auto res = QMessageBox::information(nullptr, LU_STR44, "是否确认删除模块?", QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::No)return;
	}
	GlobalImage::getInstance()->GlobalImage::UnInstallEvent(background, card);
	if (!is_exit)emit deleteModule(name_lab->text(), this);
}
