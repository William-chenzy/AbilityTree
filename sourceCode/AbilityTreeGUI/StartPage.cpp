#include <StartPage.h>
#include <QSizePolicy>
#include <QCursor>
#include <QWidget>
#include <QDebug>
#include <define.hpp>
#include <QScrollBar>
#include <QTextLayout>
#include <QTextDocument>
#include <CppGlobalDefine.h>
using namespace ATS;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

cv::Mat Canny(const cv::Mat& img) {
	cv::Mat img_gray, img_blur, edges;
	cvtColor(img, img_gray, cv::COLOR_BGRA2GRAY);
	GaussianBlur(img_gray, img_blur, cv::Size(3, 3), 0);
	cv::Canny(img_blur, edges, 100, 200, 3, false);
	return edges.clone();
}
cv::Mat QImageToMat(const QImage &img, bool color_b = false) {
	if (img.format() != QImage::Format_ARGB32) return cv::Mat();
	cv::Mat mat(img.height(), img.width(), CV_8UC4, const_cast<uchar*>(img.bits()), img.bytesPerLine());
	if (color_b)cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
	return mat.clone();
}

QFont GetFontSize(QFont font, const int width, const int height, const QString & text, int*text_h = nullptr, int*text_w = nullptr) {
	QFontMetrics metrics(font);
	int pointSize = font.pointSize();
	int text_height = metrics.height();
	int text_width = metrics.width(text);
	while (text_width < width && text_height < height) {
		font.setPointSize(pointSize++);
		metrics = QFontMetrics(font);
		text_height = metrics.height();
		text_width = metrics.width(text);
	}
	while ((text_width > width || text_height > height) && pointSize > 10) {
		font.setPointSize(pointSize--);
		metrics = QFontMetrics(font);
		text_height = metrics.height();
		text_width = metrics.width(text);
	}
	if (text_h)*text_h = text_height;
	if (text_w)*text_w = text_width;
	return font;
}
int getMultiLineTextHeight(const QString &text, const QFont &font, int width) {
	QTextDocument doc;
	doc.setDefaultFont(font);
	doc.setTextWidth(width);
	doc.setPlainText(text);

	QSizeF size = doc.size();
	return size.height();
}

Welcome::Welcome(CloudViewer* _cloud) {
	cloud = _cloud;
	cloud_name = "LOGO_CLOUD";
	logo = QImageToMat(QImage(":/img/res/LOGO-ATWR.png"));
	canny = Canny(logo);
	info = new QLabel;
	top_line = new QLabel;
	left_line = new QLabel;
	bottom_line = new QLabel;
	welcome = new QLabel(" 欢迎来到: ");
	contin = new QLabel("点击继续");
	logo_img = new QLabel;
	welcome_null = new QLabel;
	center_wid = new QWidget;
	welcome_wid = new QWidget;
	logo_wid = new QWidget;
	logo_img_wid = new QWidget;
	center_layout = new QGridLayout();
	welcome_layout = new QHBoxLayout();
	logo_layout = new QGridLayout();
	logo_img_layout = new QHBoxLayout();

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

	center_wid->setMouseTracking(true);
	logo_img->setScaledContents(true);

	contin->setAlignment(Qt::AlignCenter);
	welcome_layout->addWidget(welcome);
	contin->setVisible(false);

	left_line->setFixedWidth(1);
	top_line->setFixedHeight(1);
	contin->setFixedHeight(24);
	bottom_line->setFixedHeight(1);

	logo_layout->setMargin(0);
	logo_layout->setSpacing(0);
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
	top_line->setStyleSheet("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #f4716e, stop:0.5 #42f875, stop:1 #1edffc);");
	bottom_line->setStyleSheet("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #f4716e, stop:0.5 #42f875, stop:1 #1edffc);");
	left_line->setStyleSheet("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 transparent, stop:0.5 #1edffc, stop:1 transparent);");


	top_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
	top_line_layout->addWidget(top_line);
	top_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));

	bottom_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
	bottom_line_layout->addWidget(bottom_line);
	bottom_line_layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));

	logo_img_layout->addWidget(logo_img);
	logo_layout->addWidget(top_line_wid, 0, 0, 1, 1);
	logo_layout->addWidget(logo_img_wid, 1, 0, 1, 1);
	logo_layout->addWidget(bottom_line_wid, 2, 0, 1, 1);

	center_layout->addWidget(left_line, 0, 0);
	center_layout->addWidget(welcome_wid, 0, 1);
	center_layout->addWidget(welcome_null, 0, 2, 1, 1);
	center_layout->addWidget(logo_wid, 1, 0, 1, 3);
	center_layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 3);
	center_layout->addWidget(contin, 3, 0, 1, 3);

	cloud->MatToCloudPoint(QImageToMat(QImage(":/img/res/LOGO-AT.png"), true), cloud_name, -5, 2, 10);
	for (int i = 0; i < logo.rows; i++) {
		for (int j = 0; j < logo.cols; j++)
			if (logo.at<cv::Vec4b>(i, j)[3]) {
				if (all_ptc.size() > 10) {
					auto iter = all_ptc.begin();
					std::advance(iter, rand() % all_ptc.size());
					all_ptc.insert(iter, { i,j });
				}
				else all_ptc.push_back({ i,j });
			}
	}
	welcome->setVisible(false);
	logo_img->setVisible(false);
	top_line->setVisible(false);
	bottom_line->setVisible(false);
	left_line->setVisible(false);
	SetHide();
}
void Welcome::Animation(int width, int height) {
	if (is_out_animation) {
		OutAnimation(0, 0);
		return;
	}

	if (text_pos <= 100) {
		welcome->move({ (int)(welcome->width() * text_pos / 100.f - welcome->width()),0 });
		logo_img->move({ 0,(int)(logo_img->height() - logo_img->height() * text_pos / 100.f) });
		bottom_line->setFixedWidth(center_wid->width()*text_pos / 100.f);
		top_line->setFixedWidth(center_wid->width()*text_pos / 100.f);
		cloud->SetCameraPosition({ 0, 0, 120 * text_pos / 100 }, { 0, 0, 0 }, { -1000, 0, 0 });
	}

	if (contin->isVisible()) {
		static float val = -125;
		val = val >= 125 ? -125 : val + 1;
		contin->setStyleSheet(QString("color: rgba(212,212,212,%1);").arg(abs(val) + 130));

		static float ani = -0.99f;
		static QString base = "background: qlineargradient";
		static QString base_h = base + "(x1:0,y1:1,x2:1,y2:1,";

		ani = ani >= 0.99f ? -0.99f : ani + 0.002f;
		if (ani >= -0.1f && ani <= 0.1f)ani = 0.1f;
		QString val_str = QString::number(abs(ani), 'f', 3);
		QString val_str2 = QString::number(1 - abs(ani), 'f', 3);
		QString val_str3 = QString("rgb(%1,223,252)").arg(30 + 225 * (1 - abs(ani)));
		top_line->setStyleSheet(base_h + QString("stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc);").arg(val_str));
		bottom_line->setStyleSheet(base_h + QString("stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc);").arg(val_str2));
		left_line->setStyleSheet(base + QString("(x1:1,y1:1,x2:1,y2:0,stop:0 transparent, stop:0.5 %1, stop:1 transparent);").arg(val_str3));

		static int step = 20;
		static int now_cols = 0;
		cv::Mat _img = logo.clone();
		now_cols = now_cols + 3 >= _img.cols ? 0 : now_cols + 3;
		for (int i = 0; i < _img.rows; i++) {
			int min_cols = now_cols - step < 0 ? 0 : now_cols - step;
			int max_cols = now_cols + step > _img.cols - 1 ? _img.cols - 1 : now_cols + step;
			for (int j = min_cols; j < max_cols; j++)
				if (canny.at<uchar>(i, j))_img.at<cv::Vec4b>(i, j) = { 255,255,255,255 };
		}
		QImage img_mainicon(_img.data, _img.cols, _img.rows, (int)_img.step, QImage::Format_ARGB32);
		img_mainicon.scaled(logo_img->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		logo_img->setPixmap(QPixmap::fromImage(img_mainicon));

		RotateCamera(width, height);
	}

	if (text_pos == 0) {
		welcome->updateGeometry();
		logo_img->updateGeometry();
		logo_img->repaint();
		welcome->repaint();
		QApplication::processEvents();

		welcome->setVisible(true);
		logo_img->setVisible(true);
		top_line->setVisible(true);
		bottom_line->setVisible(true);
		left_line->setVisible(true);
	}

	if (text_pos < 100)text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;
	else if (text_pos == 100)contin->setVisible(true), text_pos++, animation_done = true;
}
void Welcome::OutAnimation(int width, int height) {
	static float text_pos = 0;
	if (text_pos < 100)text_pos = text_pos + 1.5f > 100 ? 100 : text_pos + 1.5f;

	if (!is_out_animation) {
		is_out_animation = true;
		return;
	}

	if (text_pos <= 100) {
		welcome->move({ (int)( - welcome->width() * text_pos / 100.f),0 });

		static unsigned long cur = all_ptc.size() / 60;
		unsigned long now = cur;
		while (now-- && all_ptc.size()) {
			logo.at<cv::Vec4b>(all_ptc.begin()->first, all_ptc.begin()->second)[3] = 0;
			all_ptc.erase(all_ptc.begin());
		}
		QImage img_mainicon(logo.data, logo.cols, logo.rows, (int)logo.step, QImage::Format_ARGB32);
		img_mainicon.scaled(logo_img->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		logo_img->setPixmap(QPixmap::fromImage(img_mainicon));

		top_line->setFixedWidth(center_wid->width() - center_wid->width()*text_pos / 100.f);
		bottom_line->setFixedWidth(center_wid->width() - center_wid->width()*text_pos / 100.f);
		cloud->SetCameraPosition({ 0, 0, 120 - 120 * text_pos / 100 }, { 0, 0, 0 }, { -1000, 0, 0 });
	}

	if (text_pos >= 100) SetHide(), emit Done();
}
void Welcome::SetShow() {
	PAGE::SetShow();
	cloud->SetCameraPosition({ 0, 0, 0 }, { 0, 0, 0 }, { -1000, 0, 0 });
}
void Welcome::Resize(int width, int height) {
	width = width / 5 * 4;
	height = height / 5 * 4;
	center_wid->setFixedSize({ width,height });

	int welcome_height = height / 4;
	welcome->setFixedHeight(welcome_height);
	welcome->setMaximumHeight(welcome_height);
	welcome_wid->setFixedHeight(welcome_height);
	welcome_wid->setMaximumHeight(welcome_height);
	welcome_null->setFixedHeight(welcome_height);
	welcome_null->setMaximumHeight(welcome_height);
	left_line->setMaximumHeight(welcome_height);

	int text_h, text_w;
	welcome->setFont(GetFontSize(welcome->font(), center_wid->width() / 2, welcome->height(), welcome->text(), &text_h, &text_w));
	left_line->setFixedSize({ 2,text_h });
	welcome->setFixedWidth(text_w);

	QImage img_mainicon(logo.data, logo.cols, logo.rows, (int)logo.step, QImage::Format_ARGB32);
	img_mainicon.scaled(logo_img->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	logo_img->setPixmap(QPixmap::fromImage(img_mainicon));
}

Notice::Notice(CloudViewer* _cloud) {
	cloud = _cloud;
	info = new QLabel;
	area = new QScrollArea;
	cloud_name = "Star_Center";
	cloud_box = cloud_name + "_BOX";
	confirm = new QPushButton("我已知晓");
	user_proto = new QPushButton("http://www.abilitytree.cn/UserLicense");
	center_layout = new QGridLayout();
	area_layout = new QVBoxLayout();
	center_wid = new QWidget;
	area_wid = new QWidget;

	area_wid->setMouseTracking(true);
	area_wid->setLayout(area_layout);
	center_wid->setMouseTracking(true);
	center_wid->setLayout(center_layout);

	area_layout->setMargin(0);
	area_layout->setSpacing(0);
	center_layout->setMargin(0);
	center_layout->setSpacing(0);

	info->setAlignment(Qt::AlignCenter);
	info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	info->setFont(QFont("LiShu", 10, 75));
	confirm->setFont(QFont("LiShu", 12, 75));
	user_proto->setFont(QFont("LiShu", 12, 75));
	user_proto->setStyleSheet("QPushButton{background-color:transparent;color:#ffffff;}QPushButton:hover{text-decoration: underline;}");
	info->setText("AbilityTree 是一款多功能、多平台、开源且免费的软件\r\n\r\n\
AbilityTree 致力于在统一平台上提供更方便、更全面的功能，更人性化的设计以及更强大的视觉体验\r\n\r\n\
软件现在正处于测试阶段，有许多功能等待开发完善，我们在后续的每一次更新中都会列出新增的功能以及修复的问题\r\n\r\n\
如果您能在使用后提供宝贵的意见或者新奇的想法，并反馈给我们，我们将感激不尽，并在支持者名单中加上您的大名\r\n\r\n\
AbilityTree 使用 GPL3 许可协议，任何使用或继承 AbilityTree 源码的软件，请同样使用 GPL3 许可协议\r\n\r\n\
使用 AbilityTree 之前，请您仔细阅读以下用户协议：");

	area->setWidget(area_wid);
	area_layout->addWidget(info);
	area_layout->addWidget(user_proto);
	center_layout->addWidget(area, 0, 0, 1, 3);
	center_layout->addWidget(new QLabel(), 1, 0, 1, 1);
	center_layout->addWidget(confirm, 1, 1, 1, 1);
	center_layout->addWidget(new QLabel(), 1, 2, 1, 1);

	connect(confirm, &QPushButton::clicked, this, [&]() { OutAnimation(0, 0); });
	connect(user_proto, &QPushButton::clicked, this, [&]() {
		std::string cmd = (is_Linux ? "xdg-open " : "start ");
		system((cmd + "http://www.abilitytree.cn/UserLicense").c_str());
		});

	std::vector<ModuleMember>_list;
	QString raw_path = QCoreApplication::applicationDirPath();
	QFile file(raw_path + "/conf/ModuleNumbers.json");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString str = file.readAll();
	file.close();
	QJsonArray moudle_json = QJsonDocument::fromJson(str.toUtf8()).array();
	std::map<QString, bool>ch_name_check, en_name_check;
	for (auto i : moudle_json) {
		auto _obj = i.toObject();
		ModuleMember _temp;
		auto func_arr = _obj["Func"].toArray();
		for (auto f : func_arr) _temp.func.push_back(ModuleFunc());
		_list.push_back(_temp);
	}

	int slices = 30, stacks = 30;
	auto AddNewStar = [&](float radius, float r, float g, float b, float x_offset, float y_offset)->BaseValue {
		BaseValue val;
		val.is_show = false;
		val.dtype = DQuadsStrip;
		for (int i = 0; i < slices; ++i) {
			float theta1 = i * 2 * M_PI / slices;
			float theta2 = (i + 1) * 2 * M_PI / slices;
			for (int j = 0; j <= stacks; ++j) {
				float phi = j * M_PI / stacks;
				float x1 = radius * sin(theta1) * cos(phi);
				float y1 = radius * sin(theta1) * sin(phi);
				float z1 = radius * cos(theta1);
				float x2 = radius * sin(theta2) * cos(phi);
				float y2 = radius * sin(theta2) * sin(phi);
				float z2 = radius * cos(theta2);

				val.cloud.push_back({ x1 + x_offset, y1 + y_offset, z1,r,g,b });
				val.cloud.push_back({ x2 + x_offset, y2 + y_offset, z2,r,g,b });
			}
		}
		return val;
	};

	float angle = 0;
	int line_index = 0;
	int star_index = 0;
	float angle_step = 360.f / _list.size();
	for (auto i : _list) {
		float r = (rand() % 255) / 255.f;
		float g = (rand() % 255) / 255.f;
		float b = (rand() % 255) / 255.f;
		float theta = (180 - angle) * M_PI / 180.f;
		float x = 15 * cos(theta), y = -15 * sin(theta);
		QString name = cloud_name.c_str() + QString("_%1").arg(star_index++);
		cloud->AddNewCloud(AddNewStar(2, r, g, b, x, y), name.toStdString());
		group.push_back(name);
		angle += angle_step;

		float angle2 = 0;
		int child_index = 0;
		float angle_step2 = 360.f / i.func.size();
		for (auto j : i.func) {
			float offset = (rand() % 255) / 255.f;
			float theta2 = (180 - angle2) * M_PI / 180.f;
			float x2 = 5 * cos(theta2), y2 = -5 * sin(theta2);
			QString name2 = name + QString("_%1").arg(child_index++);
			cloud->AddNewCloud(AddNewStar(0.5, r + offset, g + offset, b + offset, x2 + x, y2 + y), name2.toStdString());
			child.push_back(name2);
			angle2 += angle_step2;
		}

		BaseValue line_c;
		line_c.dtype = DLine;
		line_c.is_show = false;
		line_c.cloud.push_back({ 0,0,0,r,g,b });
		line_c.cloud.push_back({ x,y,0,r,g,b });
		QString line_name = name + QString("_L%1").arg(line_index++);
		cloud->AddNewCloud(line_c, line_name.toStdString());
		line.push_back(line_name);
	}

	auto box_val = AddNewStar(48, 0.1176f, 0.8745f, 0.9882f, 0, 0);
	box_val.draw_size = 1.5;
	box_val.dtype = DPoint;
	cloud->AddNewCloud(box_val, cloud_box);
	cloud->AddNewCloud(AddNewStar(5, 0.1176f, 0.8745f, 0.9882f, 0, 0), cloud_name);

	SetHide();
}
void Notice::Animation(int width, int height) {
	static float text_pos = 0;
	static int g_idx = 0, c_idx = 0, l_idx = 0;
	if (text_pos < 100)text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;
	else if (text_pos == 100)text_pos++, animation_done = true;

	if (is_out_animation) {
		OutAnimation(0, 0);
		return;
	}

	if (center_wid->isVisible() && text_pos <= 100) {
		uchar val = 255 * text_pos / 100;
		info->setStyleSheet("color:#" + rgbTohex({ val,127,255,212 }) + ";");
		QString color_wite = QString("color:#%1;").arg(rgbTohex({ val,255,255,255 }));
		confirm->setStyleSheet(QString("QPushButton{%1;}QPushButton:hover{color:#ffffff;}").arg(color_wite));
		user_proto->setStyleSheet(QString("QPushButton{background-color:transparent;%1}QPushButton:hover{text-decoration: underline;}").arg(color_wite));
		cloud->SetCameraPosition({ 0, 0, 1000 - 880 * text_pos / 100 }, { 0, 0, 0 }, { -1000, 0, 0 });
	}

	if (text_pos > 100) {
		if (g_idx / 10 < group.size()) cloud->SetVisible(group[g_idx++ / 10].toStdString(), true);
		if (g_idx >= 30 && c_idx / 2 < child.size()) cloud->SetVisible(child[c_idx++ / 2].toStdString(), true);
		if (c_idx >= 30 && l_idx / 5 < line.size()) cloud->SetVisible(line[l_idx++ / 5].toStdString(), true);
		RotateCamera(width, height);
	}
}
void Notice::OutAnimation(int width, int height) {
	if (!is_out_animation) {
		is_out_animation = true;
		return;
	}

	static float text_pos = 0;
	static int g_idx = 0, c_idx = 0, l_idx = 0;
	if (text_pos < 100)text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;

	if (text_pos <= 100) {
		if (l_idx / 2 < line.size()) cloud->SetVisible(line[l_idx++ / 2].toStdString(), false);
		if (c_idx / 2 < child.size()) cloud->SetVisible(child[c_idx++ / 2].toStdString(), false);
		if (g_idx / 5 < group.size()) cloud->SetVisible(group[g_idx++ / 5].toStdString(), false);

		uchar val = 255 - 255 * text_pos / 100;
		info->setStyleSheet("color:#" + rgbTohex({ val,127,255,212 }) + ";");
		QString color_wite = QString("color:#%1;").arg(rgbTohex({ val,255,255,255 }));
		confirm->setStyleSheet(QString("QPushButton{%1;}QPushButton:hover{color:#ffffff;}").arg(color_wite));
		user_proto->setStyleSheet(QString("QPushButton{background-color:transparent;%1}QPushButton:hover{text-decoration: underline;}").arg(color_wite));
		cloud->SetCameraPosition({ 0, 0, 10000 * text_pos / 100 }, { 0, 0, 0 }, { -1000, 0, 0 });
		if (text_pos == 100)SetHide(), emit Done();
	}
}
void Notice::SetHide() {
	PAGE::SetHide();
	cloud->SetVisible(cloud_box, false);
}
void Notice::SetShow() {
	PAGE::SetShow();
	cloud->SetVisible(cloud_box, true);
}
void Notice::Resize(int width, int height) {
	width = width / 8 * 7;
	height = height / 8 * 7;
	area->setFixedWidth(width);
	info->setFixedWidth(width);
	center_wid->setFixedSize({ width,height });
	confirm->setFixedSize({ width / 8, height / 8 });
	area_wid->setFixedSize({ width - 9,area->height() - 50 });

	int index = 0;
	QString raw_text = info->text(), text;
	while ((index = raw_text.indexOf("\r\n")) >= 0) {
		QString _t = raw_text.mid(0, index);
		if (_t.toLocal8Bit().toStdString().size() > text.toLocal8Bit().toStdString().size()) text = _t;
		raw_text = raw_text.mid(index + 2);
	}

	int text_w, text_h;
	info->setFont(GetFontSize(info->font(), area->width(), area->height(), text, &text_h, &text_w));
	text_h = getMultiLineTextHeight(info->text(), info->font(), text_w);
	if (text_h > area->height())area_wid->setFixedHeight(text_h + user_proto->height());
	if (text_w > area->width())area_wid->setFixedWidth(text_w), info->setFixedWidth(text_w);
	user_proto->setFixedWidth(text_w > width ? text_w : width);
	info->setFixedHeight(text_h);
}

Set::Set(CloudViewer* _cloud) {
	cloud = _cloud;
	cloud_name = "Set_Cloud";
	info = new QLabel("进行一点简单的配置:");
	descripe = new QLabel("请选择动画质量:");
	in_viwewer = new QPushButton("进入 AbilityTreeViewer");
	height = new QPushButton("高（包含所有2D动画效果，包含3D效果）");
	low = new QPushButton("低（包含部分2D动画效果，不包含3D效果）");
	center_layout = new QGridLayout();
	center_wid = new QWidget;

	low->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	height->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	info->setStyleSheet("color:#00dddddd;");
	descripe->setStyleSheet("color:#00dddddd;");

	center_layout->setMargin(0);
	center_layout->setSpacing(0);
	center_wid->setMouseTracking(true);
	center_wid->setLayout(center_layout);
	in_viwewer->setVisible(false);

	info->setFont(QFont("LiShu", 10, 75));
	descripe->setFont(QFont("LiShu", 12, 75));
	height->setFont(QFont("LiShu", 12, 75));
	low->setFont(QFont("LiShu", 12, 75));
	in_viwewer->setFont(QFont("LiShu", 12, 75));
	in_viwewer->setStyleSheet("QPushButton{background-color:transparent;color:#00dddddd;}QPushButton:hover{text-decoration: underline;}");
	height->setStyleSheet("QPushButton{background-color:transparent;color:#00dddddd;}QPushButton:hover{text-decoration: underline;}");
	low->setStyleSheet("QPushButton{background-color:transparent;color:#00dddddd;}QPushButton:hover{text-decoration: underline;}");

	center_layout->addWidget(info, 0, 0);
	center_layout->addWidget(descripe, 1, 0);
	center_layout->addWidget(height, 2, 0);
	center_layout->addWidget(low, 3, 0);
	center_layout->addWidget(in_viwewer, 0, 0);

	connect(in_viwewer, &QPushButton::clicked, this, [&]() { OutAnimation(0, 0); });
	connect(height, &QPushButton::clicked, this, [&]() {LoadConfigure::getInstance()->SetAnimation(1); set_done = true; });
	connect(low, &QPushButton::clicked, this, [&]() {LoadConfigure::getInstance()->SetAnimation(0); set_done = true; });

	BaseValue line_c;
	line_c.draw_size = 2;
	line_c.dtype = DPoint;
	line_c.is_show = false;
	float base_y = -120, base_x = -25;
	float radius = 15, step = 3, y_step = 3;
	for (float z = -10; z < 100; z += 3) {
		float offset_y = base_y;
		float r = (rand() % 255) / 255.f;
		float g = (rand() % 255) / 255.f;
		float b = (rand() % 255) / 255.f;
		for (float i = -3.14f; i < 3.14f; i += 0.1f) {
			float y = cos(i) + offset_y;
			float x = radius * sin(i) - base_x;
			line_c.cloud.push_back({ x,y,z,r,g,b });
			offset_y += y_step;
		}
		base_y += step;
	}
	cloud->AddNewCloud(line_c, cloud_name);

	SetHide();
}
void Set::Animation(int _width, int _height) {
	static float text_pos = 0, des_pos = 0, cho_pos = 0, out_cho_pos = 0, in_pos = 0;
	if (is_out_animation) {
		OutAnimation(0, 0);
		return;
	}

	if (center_wid->isVisible()) {
		if (text_pos < 100) {
			uchar val = 255 * text_pos / 100;
			text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;
			info->setStyleSheet("color:#" + rgbTohex({ val,127,255,212 }) + ";");
		}
		else if (des_pos < 100) {
			uchar val = 255 * des_pos / 100;
			des_pos = des_pos + 2.0 > 100 ? 100 : des_pos + 2.0;
			descripe->setStyleSheet("color:#" + rgbTohex({ val,212,212,212 }) + ";");
		}
		else if (cho_pos < 100) {
			uchar val = 255 * cho_pos / 100;
			cho_pos = cho_pos + 2.0 > 100 ? 100 : cho_pos + 2.0;
			auto str_val = QString("background-color:transparent;color:#" + rgbTohex({ val,255,255,255 }) + ";");
			low->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
			height->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
		}

		if (!cho_pos) {
			float pos = -50 + 50 * (text_pos + des_pos) / 200;
			cloud->SetCameraPosition({ pos, 0, 120 }, { pos, 0, 0 }, { -1000, 0, 0 });
		}

		if (set_done) {
			if (out_cho_pos <= 100) {
				uchar val = 255 - 255 * out_cho_pos / 100;
				out_cho_pos = out_cho_pos + 2.0 > 100 ? 100 : out_cho_pos + 2.0;
				info->setStyleSheet("color:#" + rgbTohex({ val,127,255,212 }) + ";");
				descripe->setStyleSheet("color:#" + rgbTohex({ val,212,212,212 }) + ";");
				auto str_val = QString("background-color:transparent;color:#" + rgbTohex({ val,255,255,255 }) + ";");
				low->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
				height->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
				if (!val) {
					info->setVisible(val);
					descripe->setVisible(val);
					low->setVisible(val);
					height->setVisible(val);
					in_viwewer->setVisible(!val);
					out_cho_pos++;
				}
			}
			else if (in_pos < 100) {
				uchar val = 255 * in_pos / 100;
				in_pos = in_pos + 2.0 > 100 ? 100 : in_pos + 2.0;
				auto str_val = QString("background-color:transparent;color:#" + rgbTohex({ val,127,255,212 }) + ";");
				in_viwewer->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
			}
			else animation_done = true;
		}
	}

	if (des_pos >= 100) RotateCamera(_width, _height);
}
void Set::OutAnimation(int _width, int _height) {
	if (!is_out_animation) {
		is_out_animation = true;
		return;
	}

	static float text_pos = 0;
	if (center_wid->isVisible()) {
		if (text_pos < 100) {
			uchar val = 255 - 255 * text_pos / 100;
			text_pos = text_pos + 2.0 > 100 ? 100 : text_pos + 2.0;
			auto str_val = QString("background-color:transparent;color:#" + rgbTohex({ val,127,255,212 }) + ";");
			in_viwewer->setStyleSheet(QString("QPushButton{%1}QPushButton:hover{text-decoration: underline;}").arg(str_val));
		}
		float pos = -50 * text_pos / 100;
		cloud->SetCameraPosition({ pos, 0, 120 }, { pos, 0, 0 }, { -1000, 0, 0 });
	}

	if (text_pos >= 100) SetHide(), emit Done();
}
void Set::Resize(int _width, int _height) {
	_width = _width / 8 * 7;
	_height = _height / 8 * 7;
	center_wid->setFixedSize({ _width,_height });
	info->setFixedSize({ _width / 2 , _height / 5  });
	descripe->setFixedSize({ _width,_height / 20 });
	in_viwewer->setFixedSize({ _width - 5,_height - 5 });

	int i_w, i_h, d_w, d_h, v_h;
	info->setFont(GetFontSize(info->font(), info->width(), info->height(), info->text(), &i_h, &i_w));
	descripe->setFont(GetFontSize(descripe->font(), _width, descripe->height(), descripe->text(), &d_h, &d_w));
	in_viwewer->setFont(GetFontSize(in_viwewer->font(), _width, in_viwewer->height(), in_viwewer->text(), &v_h));
	height->setFont(GetFontSize(height->font(), _width / 2 , height->height(), height->text()));
	low->setFont(GetFontSize(low->font(), _width / 2, low->height(), low->text()));
	descripe->setFixedSize({ d_w , d_h });
	info->setFixedSize({ i_w , i_h });
	in_viwewer->setFixedHeight(v_h);
}

StartPage::StartPage() {
	title = new QWidget;
	main_wid = new QWidget;
	center_box = new QWidget;
	close = new QToolButton();
	skip = new QPushButton("跳过");
	main_layout = new QGridLayout();
	title_layout = new QHBoxLayout();
	center_box_layout = new QGridLayout();
	layout = new QGridLayout();
	cloud = new CloudViewer();
	backGround = new QLabel();
	notice = new Notice(cloud);
	wel = new Welcome(cloud);
	set = new Set(cloud);

	center_box->setLayout(center_box_layout);
	main_wid->setLayout(main_layout);
	title->setLayout(title_layout);
	this->setLayout(layout);

	this->setObjectName("StWid");
	this->setMouseTracking(true);
	title->setMouseTracking(true);
	main_wid->setMouseTracking(true);
	center_box->setMouseTracking(true);

	connect(skip, &QPushButton::clicked, this, &StartPage::DoneOfWelcomePage);
	connect(close, &QPushButton::clicked, this, [&]() { exit(0); });
	connect(notice, &Notice::Done, this, [&]() { set->SetShow(); });
	connect(wel, &Welcome::Done, this, [&]() { notice->SetShow(); });
	connect(set, &Set::Done, this, &StartPage::DoneOfWelcomePage);
	skip->setFixedWidth(48);
	title->setFixedHeight(24);
	close->setFixedSize({ 24,24 });

	layout->setMargin(0);
	layout->setSpacing(0);
	title_layout->setMargin(0);
	title_layout->setSpacing(0);
	center_box_layout->setMargin(0);
	center_box_layout->setSpacing(0);

	close->setStyleSheet("QToolButton{border-image:url(./res/close.png);background-color:transparent;}\n\
							QToolButton:hover{border-image:url(./res/close_hover.png);}\n\
							QToolButton:pressed{border-image:url(./res/close_pressed.png);}");
	this->setStyleSheet("#StWid{background: transparent;}");
	backGround->setStyleSheet("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 rgba(0,0,0,0), stop:0.7 rgba(71,78,94,168));");

	title_layout->addWidget(skip, Qt::AlignRight);
	title_layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	title_layout->addWidget(close, Qt::AlignRight);

	layout->addWidget(title, 0, 0);
	layout->addWidget(main_wid, 1, 0);
	layout->addWidget(backGround, 0, 0, 2, 1);
	layout->addWidget(cloud->GlPtr(), 0, 0, 2, 1);
	backGround->lower();
	cloud->GlPtr()->lower();

	main_layout->addWidget(center_box, 0, 0);
	center_box_layout->addWidget(wel->GetWidgte(), 0, 0, 1, 1);
	center_box_layout->addWidget(notice->GetWidgte(), 0, 0, 1, 1);
	center_box_layout->addWidget(set->GetWidgte(), 0, 0, 1, 1);

	connect(&animation, &QTimer::timeout, this, &StartPage::AnimationRefresh);
	animation.start(10);
}

void StartPage::AnimationRefresh() {
	static int animation_1 = 100;
	static QSize lable_size = { 0,0 };

	if (lable_size != center_box->size()) {
		lable_size = center_box->size();
		if (wel->IsVisiable() || !wel->IsAnimationDone())wel->Resize(main_wid->width(), main_wid->height());
		if (notice->IsVisiable() || !notice->IsAnimationDone())notice->Resize(main_wid->width(), main_wid->height());
		if (set->IsVisiable() || !set->IsAnimationDone())set->Resize(main_wid->width(), main_wid->height());
	}

	if (animation_1) {
		animation_1--;
		if(!animation_1) wel->SetShow();
		return;
	}

	if (!this->isActiveWindow() || !this->isVisible())return;
	if (wel->IsVisiable())wel->Animation(this->width(), this->height());
	if (notice->IsVisiable())notice->Animation(this->width(), this->height());
	if (set->IsVisiable())set->Animation(this->width(), this->height());

	static int alpha = 255;
	if (all_done) {
		QString color = QString("stop:0 rgba(0,0,0,%1)").arg(alpha);
		QString color2 = QString("stop:0.7 rgba(71,78,94,%1))").arg(alpha);
		backGround->setStyleSheet(QString("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,%1, %2;").arg(color).arg(color2));

		alpha -= 3;
		if (alpha <= 0) this->hide(), animation.stop();
	}
}

void StartPage::DoneOfWelcomePage() {
	if (sender() == skip) {
		auto res = QMessageBox::information(nullptr, "跳过", "确认要跳过吗?\r\n所有配置将会使用默认配置!", QMessageBox::Yes | QMessageBox::No);
		if (res != QMessageBox::Yes)return;
		if (wel->IsVisiable())wel->OutAnimation(0, 0);
		if (notice->IsVisiable())notice->OutAnimation(0, 0);
		if (set->IsVisiable())set->OutAnimation(0, 0);
	}

	all_done = true;
	title->setVisible(false);
	center_box->setVisible(false);
	cloud->GlPtr()->setVisible(false);
	LoadConfigure::getInstance()->SetFirstInit(1);
	emit StartDone();
}

void StartPage::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
		if (wel->IsVisiable() && wel->IsAnimationDone())wel->OutAnimation(0, 0);
	}
	QWidget::mouseReleaseEvent(event);
}