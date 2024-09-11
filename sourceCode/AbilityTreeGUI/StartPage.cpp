#include <StartPage.h>
#include <QSizePolicy>
#include <QApplication>
#include <QDebug>
using namespace ATS;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

StartPage::StartPage() {

	info = new QLabel;
	top_line = new QLabel;
	letf_line = new QLabel;
	close = new QToolButton;
	bottom_line = new QLabel;
	confirm = new QPushButton("我已知晓");
	welcome = new QLabel(" 欢迎来到: ");
	contin = new QLabel("点击继续");
	logo_img = new QLabel;
	welcome_null = new QLabel;
	widget = new QWidget;
	title = new QWidget;
	main_wid = new QWidget;
	center_wid = new QWidget;
	welcome_wid = new QWidget;
	logo_wid = new QWidget;
	logo_img_wid = new QWidget;
	main_layout = new QGridLayout();
	center_layout = new QGridLayout();
	title_layout = new QHBoxLayout();
	welcome_layout = new QHBoxLayout();
	logo_layout = new QGridLayout();
	logo_img_layout = new QHBoxLayout();
	layout = new QGridLayout();

	top_line_wid = new QWidget;
	bottom_line_wid = new QWidget;
	top_line_layout = new QHBoxLayout();
	bottom_line_layout = new QHBoxLayout();

	logo_wid->setLayout(logo_layout);
	top_line_wid->setLayout(top_line_layout);
	bottom_line_wid->setLayout(bottom_line_layout);
	logo_img_wid->setLayout(logo_img_layout);
	welcome_wid->setLayout(welcome_layout);
	center_wid->setLayout(center_layout);
	main_wid->setLayout(main_layout);
	title->setLayout(title_layout);
	widget->setLayout(layout);

	title->setMouseTracking(true);
	widget->setObjectName("StWid");
	widget->setMouseTracking(true);
	main_wid->setMouseTracking(true);
	center_wid->setMouseTracking(true);

	title_layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	connect(close, &QPushButton::clicked, this, [&]() { exit(0); });
	title_layout->addWidget(close, Qt::AlignRight);
	contin->setAlignment(Qt::AlignCenter);
	welcome_layout->addWidget(welcome);
	contin->setVisible(false);

	letf_line->setFixedWidth(1);
	top_line->setFixedHeight(1);
	title->setFixedHeight(24);
	contin->setFixedHeight(24);
	close->setFixedSize({ 24,24 });
	bottom_line->setFixedHeight(1);

	layout->setMargin(0);
	layout->setSpacing(0);
	logo_layout->setMargin(0);
	logo_layout->setSpacing(0);
	title_layout->setMargin(0);
	title_layout->setSpacing(0);
	center_layout->setMargin(0);
	center_layout->setSpacing(0);
	welcome_layout->setMargin(0);
	welcome_layout->setSpacing(0);
	logo_img_layout->setMargin(0);
	logo_img_layout->setSpacing(0);

	top_line_layout->setMargin(0);
	top_line_layout->setSpacing(0);
	bottom_line_layout->setMargin(0);
	bottom_line_layout->setSpacing(0);

	contin->setFont(QFont("LiShu", 11, 75));
	welcome->setFont(QFont("LiShu", 10, 75));
	close->setStyleSheet("QToolButton{border-image:url(./res/close.png);background-color:transparent;}\n\
							QToolButton:hover{border-image:url(./res/close_hover.png);}\n\
							QToolButton:pressed{border-image:url(./res/close_pressed.png);}");
	widget->setStyleSheet("#StWid{background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #000000, stop:0.7 #474e5e);}");
	top_line->setStyleSheet("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #f4716e, stop:0.5 #42f875, stop:1 #1edffc);");
	bottom_line->setStyleSheet("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #f4716e, stop:0.5 #42f875, stop:1 #1edffc);");
	letf_line->setStyleSheet("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 transparent, stop:0.5 #1edffc, stop:1 transparent);");

	layout->addWidget(title, 0, 0);
	layout->addWidget(main_wid, 1, 0);

	main_layout->addWidget(center_wid, 0, 0);

	logo_img_layout->addWidget(logo_img);


	top_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
	top_line_layout->addWidget(top_line);
	top_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));

	bottom_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
	bottom_line_layout->addWidget(bottom_line);
	bottom_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));

	logo_layout->addWidget(top_line_wid, 0, 0, 1, 1);
	logo_layout->addWidget(logo_img_wid, 1, 0, 1, 1);
	logo_layout->addWidget(bottom_line_wid, 2, 0, 1, 1);

	center_layout->addWidget(letf_line, 0, 0);
	center_layout->addWidget(welcome_wid, 0, 1);
	center_layout->addWidget(welcome_null, 0, 2, 1, 1);
	center_layout->addWidget(logo_wid, 1, 0, 1, 3);
	center_layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 3);
	center_layout->addWidget(contin, 3, 0, 1, 3);

	connect(&animation, &QTimer::timeout, this, &StartPage::AnimationRefresh);
	animation.start(10);


	//center_wid->setStyleSheet("border:2px solid #ffffff;");
}

void StartPage::ReSizeCenter(QSize raw_size) {
	int width = raw_size.width() / 5 * 4;
	int height = raw_size.height() / 5 * 4;
	center_wid->setFixedSize({ width,height });

	welcome->setFixedHeight(height / 4);
	welcome->setMaximumHeight(height / 4);
	welcome_wid->setFixedHeight(height / 4);
	welcome_wid->setMaximumHeight(height / 4);
	welcome_null->setFixedHeight(height / 4);
	welcome_null->setMaximumHeight(height / 4);
	letf_line->setMaximumHeight(height / 4);
}
void StartPage::ReSizeCenterLabel(QSize lable_size) {
	QFont font = welcome->font();
	QFontMetrics metrics(font);
	int pointSize = font.pointSize();
	int text_height = metrics.height();
	int text_width = metrics.width(welcome->text());
	while (text_width < center_wid->width() / 2 && text_height < welcome->height()) {
		font.setPointSize(pointSize++);
		metrics = QFontMetrics(font);
		text_height = metrics.height();
		text_width = metrics.width(welcome->text());
	}
	while ((text_width > center_wid->width() / 2 || text_height > welcome->height()) && pointSize > 0) {
		font.setPointSize(pointSize--);
		metrics = QFontMetrics(font);
		text_height = metrics.height();
		text_width = metrics.width(welcome->text());
	}
	welcome->setFont(font);
	welcome_wid->setFixedWidth(text_width);
	welcome_wid->setMaximumWidth(text_width);
	letf_line->setFixedSize({ 2,text_height });

	QImage img_mainicon(":/img/res/LOGO-ATWR.png");
	img_mainicon.scaled(logo_img->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	logo_img->setPixmap(QPixmap::fromImage(img_mainicon));
	logo_img->setScaledContents(true);
}
void StartPage::AnimationRefresh() {
	static int animation_1 = 50;
	static QSize raw_size = { 0,0 };
	static QSize lable_size = { 0,0 };

	if (raw_size != widget->size()) ReSizeCenter(raw_size = widget->size());
	if (lable_size != welcome->size()) ReSizeCenterLabel(lable_size = welcome->size());

	static float text_pos = 0;
	if (widget->isActiveWindow() && widget->isVisible()) {
		if (animation_1 && !first_animation) {
			if (text_pos < 100)text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;
			else if (text_pos == 100)first_animation = true, contin->setVisible(true), text_pos++;
		}
		else animation_1--;
	}

	if (first_animation && !second_animation) {
		static float ani = -0.99;
		static QString base = "background: qlineargradient";
		static QString base_h = base + "(x1:0,y1:1,x2:1,y2:1,";

		ani = ani >= 0.99 ? -0.99 : ani + 0.002;
		if (ani >= -0.1 && ani <= 0.1)ani = 0.1;
		QString val_str = QString::number(abs(ani), 'f', 3);
		QString val_str2 = QString::number(1 - abs(ani), 'f', 3);
		QString val_str3 = QString("rgb(%1,223,252)").arg(30 + 225 * (1 - abs(ani)));
		top_line->setStyleSheet(base_h + QString("stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc);").arg(val_str));
		bottom_line->setStyleSheet(base_h + QString("stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc);").arg(val_str2));
		letf_line->setStyleSheet(base + QString("(x1:1,y1:1,x2:1,y2:0,stop:0 transparent, stop:0.5 %1, stop:1 transparent);").arg(val_str3));
	}

	if (welcome->isVisible()) {
		welcome->move({ (int)(welcome->width() * text_pos / 100.f - welcome->width()),0 });
		logo_img->move({ 0,(int)(logo_img->height() - logo_img->height() * text_pos / 100.f) });
		bottom_line->setFixedWidth(center_wid->width()*text_pos / 100.f);
		top_line->setFixedWidth(center_wid->width()*text_pos / 100.f);
	}

	if (contin->isVisible()) {
		static float val = -125;
		val = val >= 125 ? -125 : val + 1;
		contin->setStyleSheet(QString("color: rgba(212,212,212,%1);").arg(abs(val) + 130));
	}


	//todo else: 
}

bool StartPage::eventFilter(QObject* watched, QEvent* event) {

	return false;
}