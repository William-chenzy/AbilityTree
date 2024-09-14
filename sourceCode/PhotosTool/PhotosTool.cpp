#include "ui_PhotosTool.h"
#include "PhotosTool.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <iostream>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDesktopServices>
#include <QScrollBar>
#include <QDropEvent>
#include <QMimeData>
#include <QDateTime>
#include <fstream>
#include <thread>
#include <deque>
#include <fstream>
#include <QPushButton>
#include <QPainter>
#include <QSlider>
#include <QFileDialog>
#include <QApplication>
#include "GlobalDefine.h"
#include <../3rdParty/stb_image.h>
using namespace std;
using namespace chrono;

#define STB_IMAGE_IMPLEMENTATION
#define ANIMATION_TIMES 15.f;
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

PhotosTool::PhotosTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::PhotosTool)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	QRect deskRect = QApplication::desktop()->availableGeometry();
	if (deskRect.width() < width() || deskRect.height() < height()) this->setMaximumSize(deskRect.width(), deskRect.height());
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-PhotosTool.png")));

	img_list_layout = new QHBoxLayout();
	img_list_layout->setSpacing(12);
	img_list_layout->setMargin(0);
	AddNextPrevButton();
	drop_path.clear();

	offset_x = offset_y = 0;
	img_move_refresh = 0;
	img_move = false;
	refresh_bar = 0;
	t_multiple = 0;
	multiple = 0;

	connect(&load, &QTimer::timeout, this, [&]() {if (!drop_path.isEmpty()) LoadDir(drop_path), drop_path.clear(); });
	connect(&monitor, &QTimer::timeout, this, &PhotosTool::MonitorEvent);
	ui->scrollAreaWidgetContents->setLayout(img_list_layout);
	ui->widget_image_list_show->installEventFilter(this);
	ui->scrollArea_image_list->installEventFilter(this);
	ui->widget_image_list->installEventFilter(this);
	ui->label_main_image->installEventFilter(this);
	ui->widget_image_all_info->setVisible(false);
	ui->widget_image_tools->setVisible(false);
	ui->widget_image_list->setVisible(false);
	ui->widget_main_tools->setVisible(false);
	monitor.start(16);
	load.start(100);

	float last = 0;
	for (float i = 0.05f; i < 1; i *= 1.15f) {
		if (i - last < 0.01f)i += 0.01f;
		mutiple_list.push_back(i);
		last = i;
	}
	for (float i = 1; i < 50; i *= 1.15f) mutiple_list.push_back(i);
	mutiple_list.push_back(50);
}

void PhotosTool::AddNextPrevButton() {
	QGridLayout* layout = static_cast<QGridLayout*>(ui->widget_center->layout());
	QHBoxLayout* l_lay = new QHBoxLayout(), *r_lay = new QHBoxLayout();
	prev_button_wid = new QWidget();
	next_button_wid = new QWidget();
	next_button = new QToolButton();
	prev_button = new QToolButton();

	l_lay->setMargin(0);
	l_lay->setSpacing(0);
	r_lay->setMargin(0);
	r_lay->setSpacing(0);

	prev_button_wid->setFixedWidth(64);
	next_button_wid->setFixedWidth(64);
	prev_button_wid->setCursor(Qt::ArrowCursor);
	next_button_wid->setCursor(Qt::ArrowCursor);

	next_button->setFixedWidth(36);
	next_button->setFixedHeight(36);
	next_button->setVisible(false);
	next_button->setCursor(Qt::ArrowCursor);
	connect(next_button, &QToolButton::clicked, this, &PhotosTool::on_toolButton_image_next_clicked);
	next_button->setStyleSheet("border-image:url(./res/right_img.png) 0 0 0  stretch stretch;background-color:rgba(255,255,255,64)");
		
	prev_button->setFixedWidth(36);
	prev_button->setFixedHeight(36);
	prev_button->setVisible(false);
	prev_button->setCursor(Qt::ArrowCursor);
	connect(prev_button, &QToolButton::clicked, this, &PhotosTool::on_toolButton_image_prev_clicked);
	prev_button->setStyleSheet("border-image:url(./res/left_img.png) 0 0 0  stretch stretch;background-color:rgba(255,255,255,64);");

	prev_button_wid->setLayout(l_lay);
	next_button_wid->setLayout(r_lay);
	l_lay->addWidget(prev_button);
	r_lay->addWidget(next_button);

	next_button_wid->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	prev_button_wid->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

	layout->addWidget(prev_button_wid, 0, 0, 1, 1);
	layout->addWidget(next_button_wid, 0, 2, 1, 1);
	layout->addWidget(ui->label_main_image, 0, 0, 1, 3);

	prev_button_wid->setVisible(false);
	next_button_wid->setVisible(false);
	prev_button_wid->installEventFilter(this);
	next_button_wid->installEventFilter(this);
}

void PhotosTool::MonitorEvent() {
	static int lab_w = ui->label_main_image->width();
	static int lab_h = ui->label_main_image->height();
	if (lab_w != ui->label_main_image->width() || lab_h != ui->label_main_image->height()) {
		lab_w = ui->label_main_image->width();
		lab_h = ui->label_main_image->height();
		if (image_max)RefreshImage();
	}
	if (refresh_bar > 0)MoveHScrollBar(current), refresh_bar--;

	if (t_multiple != multiple) {
		float setp = abs(multiple - t_multiple) < abs(multiple_step) ? multiple - t_multiple : multiple_step;
		t_multiple += setp;

		img_width = raw_img.cols* t_multiple;
		img_height = raw_img.rows* t_multiple;
		img_move = img_height > lab_h || img_width > lab_w;
		if (!img_move) {
			cv::resize(raw_img, cv_img, { 0,0 }, t_multiple, t_multiple);
			ui->label_main_image->setCursor(Qt::ArrowCursor);
			img_move_refresh = offset_x = offset_y = 0;
			img_move = false;
		}
		else {
			auto _pos = QCursor::pos() - ui->label_main_image->mapToGlobal(QPoint(0, 0));

			float last_mul = t_multiple - setp;
			int a_img_w = raw_img.cols* last_mul;
			int a_img_h = raw_img.rows* last_mul;
			float step_cols = raw_img.cols * setp;
			float step_rows = raw_img.rows * setp;
			float pos_offset_x = offset_x + _pos.x();
			float pos_offset_y = offset_y + _pos.y();
			float pos_per_x = pos_offset_x / a_img_w;
			float pos_per_y = pos_offset_y / a_img_h;

			offset_x += pos_per_x * step_cols, offset_y += pos_per_y * step_rows;

			offset_x = offset_x + lab_w > img_width ? img_width - lab_w : offset_x;
			offset_y = offset_y + lab_h > img_height ? img_height - lab_h : offset_y;
			offset_x = offset_x < 0 ? 0 : offset_x;
			offset_y = offset_y < 0 ? 0 : offset_y;

			ui->label_main_image->setCursor(Qt::OpenHandCursor);
			img_move_refresh++;
		}
		ShowImageToLabel();
	}

	if (img_move_refresh) {
		int roi_x, roi_y, roi_w = 1, roi_h = 1;
		for (int i = 0; i < raw_img.cols; i++) {
			if (i*t_multiple > offset_x)break;
			roi_x = i;
		}
		for (int i = 0; i < raw_img.rows; i++) {
			if (i*t_multiple > offset_y)break;
			roi_y = i;
		}
		while (roi_w++*t_multiple < lab_w);
		while (roi_h++*t_multiple < lab_w);

		if (roi_w + roi_x > raw_img.cols) {
			int val = roi_w + roi_x - raw_img.cols;
			if (roi_x - val >= 0)roi_x -= val;
			else roi_w -= val;
		}
		if (roi_h + roi_y > raw_img.rows) {
			int val = roi_h + roi_y - raw_img.rows;
			if (roi_y - val >= 0)roi_y -= val;
			else roi_h -= val;
		}

		cv::Mat mul_img;
		cv::Mat roi_img = raw_img(cv::Rect(roi_x, roi_y, roi_w, roi_h));
		cv::resize(roi_img, mul_img, { 0,0 }, t_multiple, t_multiple);

		int now_offset_x = roi_x * t_multiple;
		int now_offset_y = roi_y * t_multiple;
		int now_w = lab_w > mul_img.cols ? mul_img.cols : lab_w;
		int now_h = lab_h > mul_img.rows ? mul_img.rows : lab_h;

		int move_x = offset_x - now_offset_x;
		int move_y = offset_y - now_offset_y;
		while (now_w + move_x > mul_img.cols)move_x--;
		while (now_h + move_y > mul_img.rows)move_y--;

		cv_img = mul_img(cv::Rect(move_x, move_y, now_w, now_h)).clone();

		img_move_refresh--;
		ShowImageToLabel();
	}
}

void PhotosTool::MultipleImage(bool bigger) {
	if (image_max) {
		image_max = !image_max;
		QString img_str = image_max ? "image_max_cancel" : "image_max";
		auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
		auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
		auto nor = QString("QToolButton{border-image:url(./res/%1.png);background-color:transparent;}").arg(img_str);
		ui->toolButton_image_max->setStyleSheet(nor + hover + pressed);
	}

	float b_last = mutiple_list[0];
	float s_last = mutiple_list[0];
	for (auto i : mutiple_list) {
		b_last = i;
		if (i > multiple)break;
		if (!bigger && i == multiple)break;
		s_last = i;
	}
	multiple = bigger ? b_last : s_last;
	multiple_step = (multiple - t_multiple) / ANIMATION_TIMES;
}

void PhotosTool::RefreshImage() {
	int ch_rows = raw_img.rows, ch_cols = raw_img.cols;
	int now_h = ui->label_main_image->height(), now_w = ui->label_main_image->width();

	if (!image_max)multiple = 1;
	else {
		float mul_w = now_w * 1.f / ch_cols * 1.f;
		float mul_h = now_h * 1.f / ch_rows * 1.f;
		multiple = (mul_w < mul_h ? mul_w : mul_h);
	}
	multiple_step = (multiple - t_multiple) / ANIMATION_TIMES;
}


void PhotosTool::ShowImageToLabel() {
	//int width, height, channels;
	//unsigned char *img = stbi_load(curr_path.toLocal8Bit().toStdString().c_str(), &width, &height, &channels, 0);
	//auto c_n = channels == 4 ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
	//ui->label_main_image->setPixmap(QPixmap::fromImage(QImage(img, width, height, width * channels, QImage::Format_RGBA8888)));

	auto c_n = cv_img.channels() == 4 ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
	QImage q_img = QImage(cv_img.data, cv_img.cols, cv_img.rows, cv_img.cols * cv_img.channels(), c_n);
	ui->label_multiple->setText(QString::number(t_multiple * 100, 'f', 0) + "%");
	ui->label_main_image->setPixmap(QPixmap::fromImage(q_img));
}
void PhotosTool::MoveHScrollBar(ImageWidget* wid) {
	int now_w = wid->pos().x();
	int show_width = ui->scrollArea_image_list->width();
	auto bar = ui->scrollArea_image_list->horizontalScrollBar();

	if (abs(now_w - show_width - bar->value()) < 100)bar->setValue(now_w - show_width + 100);
	else if (abs(now_w - bar->value()) < 100)bar->setValue(now_w);
	else if (now_w > show_width + bar->value())bar->setValue(now_w);
	else if (now_w < bar->value())bar->setValue(now_w);
}

void PhotosTool::LoadDir(QString path_name) {
	int pos = path_name.lastIndexOf('/') + 1;
	QString path = path_name.mid(0, pos);
	QString name = path_name.mid(pos);

	QDir dir(path);
	if (!dir.exists())return;

	QStringList filename;
	filename << "*.png" << "*.jpg" << "*.jepg" << "*.bmp" << "*.tif" << "*.tiff" << "*.jfif";
	all_file = dir.entryList(filename, QDir::Files | QDir::Readable, QDir::Name);
	for (auto& i : all_file)i = path + i, qInfo() << i;

	int index = 0;
	for (auto i : all_file) {
		if (i == path_name)break;
		else index++;
	}
	LoadDirImage(index);
}
void PhotosTool::LoadDirImage(int now_index) {
	for (int i = all_file.size(); i < img_list.size(); i++) {
		img_list_layout->removeWidget(img_list[i]);
		img_list[i]->clear();
	}

	refresh_bar = 0;
	img_move = false;
	offset_x = offset_y = 0;
	t_multiple = multiple = 1;
	ui->label->setVisible(false);
	ui->label_2->setVisible(false);
	prev_button_wid->setVisible(true);
	next_button_wid->setVisible(true);
	ui->widget_main_tools->setVisible(true);
	ui->widget_image_list->setVisible(all_file.size() - 1);

	if (img_list.size() <= all_file.size())img_list.resize(all_file.size());
	for (auto& i : img_list) {
		if (i == nullptr)i = new ImageWidget();
		connect(i, &ImageWidget::clicked, this, &PhotosTool::LoadIamge);
	}

	auto idx_str = QString("%1/%2").arg(now_index + 1).arg(all_file.size());
	img_list[now_index]->SetNewImage(all_file[now_index], idx_str);
	img_list[now_index]->ClickedThis();
	QApplication::processEvents();

	unsigned long beg = now_index - 100 < 0 ? 0 : now_index - 100;
	unsigned long end = now_index + 100 >= all_file.size() ? all_file.size() - 1 : now_index + 100;

	for (; beg < end; beg++) {
		auto index_str = QString("%1/%2").arg(beg + 1).arg(all_file.size());
		img_list[beg]->SetNewImage(all_file[beg], index_str);
		img_list_layout->addWidget(img_list[beg]);
		QApplication::processEvents();
	}

	unsigned long index = 0;
	auto it = img_list.begin();
	for (auto i : all_file) {
		auto index_str = QString("%1/%2").arg(index + 1).arg(all_file.size());
		img_list[index]->SetNewImage(i, index_str);
		img_list_layout->addWidget(*(it++));
		QApplication::processEvents();
		index++;
	}
	refresh_bar = 2;
}
void PhotosTool::LoadIamge(QString path, QString name, QString size, QString index) {
	raw_img = cv::imread(path.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
	if (raw_img.empty())return;

	qInfo() << raw_img.rows << raw_img.cols;
	cv::imshow("raw_img", raw_img);
	cv::waitKey(1);

	ui->label_image_all_path->setText(path);
	QDateTime created = QFileInfo(path).created();
	ui->label_image_date->setText(created.toString());

	curr_path = path;
	t_multiple = multiple = 1;
	image_max = image_full = false;
	current = static_cast<ImageWidget*>(sender());

	auto c_t = raw_img.channels() == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;
	cv::cvtColor(raw_img, raw_img, c_t);

	cv_img = raw_img.clone();
	int now_h = ui->label_main_image->height();
	int now_w = ui->label_main_image->width();
	if (cv_img.cols > now_w || cv_img.rows > now_h)on_toolButton_image_max_clicked();

	ShowImageToLabel();
	MoveHScrollBar(current);

	ui->widget_image_tools->setVisible(true);
	ui->label_image_number->setText(index);
	ui->lineEdit_image_name->setText(name);
	ui->pushButton_image_size->setText(size);
	ui->pushButton_image_pix->setText(QString(" %1 * %2 ").arg(cv_img.cols).arg(cv_img.rows));
}

void PhotosTool::on_toolButton_image_full_clicked() {
	image_full = !image_full;
	if (image_full)this->showNormal(), this->showFullScreen();
	if (!image_full) {
		if(!main_full)this->showNormal();
		if(main_full)this->showMaximized();
	}
	ui->widget_title->setVisible(!image_full);

	QString img_str = image_full ? "image_full_cancel" : "image_full";
	auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
	auto nor = QString("QToolButton{border-image:url(./res/%1.png);background-color:transparent;}").arg(img_str);
	ui->toolButton_image_full->setStyleSheet(nor + hover + pressed);
}
void PhotosTool::on_toolButton_image_max_clicked() {
	image_max = !image_max;
	QString img_str = image_max ? "image_max_cancel" : "image_max";
	auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
	auto nor = QString("QToolButton{border-image:url(./res/%1.png);background-color:transparent;}").arg(img_str);
	ui->toolButton_image_max->setStyleSheet(nor + hover + pressed);
	RefreshImage();
}
void PhotosTool::on_toolButton_image_bigger_clicked() {
	MultipleImage(true);
}
void PhotosTool::on_toolButton_image_small_clicked() {
	MultipleImage(false);
}
void PhotosTool::on_toolButton_image_prev_clicked() {
	ImageWidget* last = nullptr;
	for (auto i : img_list) {
		if (i == current)break;
		last = i;
	}
	if (last)last->ClickedThis();
}
void PhotosTool::on_toolButton_image_next_clicked() {
	ImageWidget* prev = nullptr;
	ImageWidget* last = nullptr;
	for (auto i : img_list) {
		if (prev) {
			last = i;
			break;
		}
		if (i == current)prev = i;
	}
	if (last)last->ClickedThis();
}
void PhotosTool::on_toolButton_left_rotate_clicked() {
	cv::Mat t_img;
	cv::rotate(raw_img, raw_img, cv::ROTATE_90_COUNTERCLOCKWISE);

	auto c_t = raw_img.channels() == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;
	cv::cvtColor(raw_img, t_img, c_t);

	if (image_max)on_toolButton_image_max_clicked();
	else cv_img = raw_img.clone(), ShowImageToLabel();

	cv::imwrite(curr_path.toStdString(), t_img);
	current->SetNewImage(curr_path, "");
}
void PhotosTool::on_toolButton_right_rotate_clicked() {
	cv::Mat t_img;
	cv::rotate(raw_img, raw_img, cv::ROTATE_90_CLOCKWISE);

	auto c_t = raw_img.channels() == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;
	cv::cvtColor(raw_img, t_img, c_t);

	if (image_max)on_toolButton_image_max_clicked();
	else cv_img = raw_img.clone(), ShowImageToLabel();

	cv::imwrite(curr_path.toStdString(), t_img);
	current->SetNewImage(curr_path, "");
}
void PhotosTool::on_toolButton_image_delete_clicked() {
	auto res = QMessageBox::information(nullptr, "提示", "确认删除吗?", QMessageBox::Ok | QMessageBox::No);
	if (res == QMessageBox::No)return;

	ImageWidget* cur = current;
	on_toolButton_image_next_clicked();
	if (cur == current)on_toolButton_image_prev_clicked();
	QFile::remove(curr_path);
	cur->clear();
}
void PhotosTool::on_toolButton_image_info_clicked() {
	bool vis = ui->widget_image_all_info->isVisible();
	QString img_str = !vis ? "image_info_cancel" : "image_info";
	auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
	auto nor = QString("QToolButton{border-image:url(./res/%1.png);background-color:transparent;}").arg(img_str);
	ui->toolButton_image_info->setStyleSheet(nor + hover + pressed);
	ui->widget_image_all_info->setVisible(!vis);
}

void PhotosTool::on_comboBox_wheel_type_currentIndexChanged(int val) {
	switch (ui->comboBox_key->currentIndex()) {
	case 0:wheel_key = val; break;
	case 1:up_down_key = val; break;
	case 2:left_right_key = val; break;
	}
}
void PhotosTool::on_comboBox_key_currentIndexChanged(int val) {
	ui->comboBox_wheel_type->blockSignals(true);
	switch (val) {
	case 0:ui->comboBox_wheel_type->setCurrentIndex(wheel_key); break;
	case 1:ui->comboBox_wheel_type->setCurrentIndex(up_down_key); break;
	case 2:ui->comboBox_wheel_type->setCurrentIndex(left_right_key); break;
	}
	ui->comboBox_wheel_type->blockSignals(false);
}

void PhotosTool::on_lineEdit_image_name_editingFinished() {
	if (ui->lineEdit_image_name->hasFocus()) {
		ui->lineEdit_image_name->clearFocus();
		return;
	}

	QString name = curr_path.mid(curr_path.lastIndexOf('/') + 1);
	QString new_name = ui->lineEdit_image_name->text();
	if (new_name == name)return;

	QString new_path = curr_path.mid(0, curr_path.lastIndexOf('/') + 1) + new_name;
	if (QFile(curr_path).rename(new_path)) LoadDir(new_path);
	else QMessageBox::warning(nullptr, "错误", "修改失败"); 
}

void PhotosTool::on_toolButton_open_dir_clicked() {
	if (curr_path.isEmpty())return;
	QString path = curr_path.mid(0, curr_path.lastIndexOf('/') + 1);
	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
void PhotosTool::on_toolButton_save_as_clicked() {
	if (raw_img.empty())return;
	QString filePath = QFileDialog::getSaveFileName(nullptr, "保存文件", curr_path, "*.*");
	if (filePath.isEmpty())return;

	cv::Mat t_img;
	auto c_t = raw_img.channels() == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;
	cv::cvtColor(raw_img, t_img, c_t);
	cv::imwrite(filePath.toLocal8Bit().toStdString(), t_img);
}

void PhotosTool::on_toolButton_min_clicked() {
	this->showMinimized();
}
void PhotosTool::on_toolButton_max_clicked() {
	if (PhotosTool::isMaximized()) this->showNormal();
	else this->showMaximized();
	auto img_str = this->isMaximized() ? "full" : "max";
	auto nor = QString("QToolButton{border-image:url(./res/%1.png);}").arg(img_str);
	auto hover = QString("QToolButton:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	auto pressed = QString("QToolButton:pressed{ border-image:url(./res/%1_pressed.png); }").arg(img_str);
	ui->toolButton_max->setStyleSheet(nor + hover + pressed);
	main_full = PhotosTool::isMaximized();
}
void PhotosTool::on_toolButton_close_clicked() {
	exit(0);
}

void PhotosTool::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls()) event->acceptProposedAction();
	else event->ignore();
}
void PhotosTool::dropEvent(QDropEvent* event) {
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) return;

	cv::Mat img = cv::imread(urls[0].toLocalFile().toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
	if (img.empty()) {
		QMessageBox box;
		box.setWindowTitle("提示");
		box.setText("此文件暂时无法识别\r\n您可以登录 http://www.abilitytree.cn/ProblemFeedback 反馈此问题");
		box.setTextInteractionFlags(Qt::TextSelectableByMouse);
		box.exec();
		return;
	}

	drop_path = urls[0].toLocalFile();
	ui->label_main_image->setText("加载中...");
}


QPoint mouse_pos;
bool is_move_mouse;
int size_m_type = 0;
void PhotosTool::mousePressEvent(QMouseEvent* event) {
	if (PhotosTool::isMaximized()) {
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
void PhotosTool::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false, size_m_type = 0;
	QWidget::mouseReleaseEvent(event);
}
void PhotosTool::mouseMoveEvent(QMouseEvent* event) {
	if (PhotosTool::isMaximized()) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	if (is_move_mouse) {
		move(this->pos() + (event->screenPos().toPoint() - mouse_pos));
		mouse_pos = event->screenPos().toPoint();
	}
	else if (size_m_type) {
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
void PhotosTool::mouseDoubleClickEvent(QMouseEvent* event) {
	QRect rect_top = ui->widget_title->rect();
	rect_top.translate(ui->widget_title->pos());
	if (rect_top.contains(event->pos())) on_toolButton_max_clicked();
	QWidget::mousePressEvent(event);
}
bool PhotosTool::eventFilter(QObject* obj, QEvent* event) {
	if (obj == ui->widget_image_list) {
		if (event->type() == QEvent::Enter) {
			ui->label_2->setVisible(true);
			ui->label->setVisible(true);
		}
		if (event->type() == QEvent::Leave) {
			ui->label_2->setVisible(false);
			ui->label->setVisible(false);
		}
	}

	if (obj == prev_button_wid) {
		if (event->type() == QEvent::Enter) prev_button->setVisible(true);
		if (event->type() == QEvent::Leave) prev_button->setVisible(false);
	}
	if (obj == next_button_wid) {
		if (event->type() == QEvent::Enter) next_button->setVisible(true);
		if (event->type() == QEvent::Leave) next_button->setVisible(false);
	}

	if (obj == ui->scrollArea_image_list && event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		auto bar = ui->scrollArea_image_list->horizontalScrollBar();

		int step = 30;
		if (wheelEvent->delta() > 0)step *= -1;
		bar->setValue(bar->value() + step);
	}

	if (obj == ui->widget_image_list_show && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
			bool vis = ui->scrollArea_image_list->isVisible();
			QString str = !vis ? "./res/down_img.png" : "./res/up_img.png";
			auto style = QString("border-image:url(%1) 0 0 0  stretch stretch;background-color:transparent;").arg(str);

			ui->label_2->setStyleSheet(style);
			ui->scrollArea_image_list->setVisible(!vis);
		}
	}


	if (obj != ui->label_main_image || raw_img.empty())return QMainWindow::eventFilter(obj, event);

	static QPoint old_pos;
	static bool move_image = false;
	if (event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		if (wheel_key == 0 && wheelEvent->delta() > 0)on_toolButton_image_bigger_clicked();
		else if (wheel_key == 0 && wheelEvent->delta() < 0)on_toolButton_image_small_clicked();
		else if (wheel_key == 1 && wheelEvent->delta() > 0)on_toolButton_image_prev_clicked();
		else if (wheel_key == 1 && wheelEvent->delta() < 0)on_toolButton_image_next_clicked();
		else if (wheel_key == 2 && wheelEvent->delta() > 0)on_toolButton_left_rotate_clicked();
		else if (wheel_key == 2 && wheelEvent->delta() < 0)on_toolButton_right_rotate_clicked();
	}
	else if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		auto key = keyEvent->key();
		if (key == Qt::Key_Left || key == Qt::Key_Right) {
			if (left_right_key == 0 && key == Qt::Key_Left)on_toolButton_image_bigger_clicked();
			else if (left_right_key == 0 && key == Qt::Key_Right)on_toolButton_image_small_clicked();
			else if (left_right_key == 1 && key == Qt::Key_Left)on_toolButton_image_prev_clicked();
			else if (left_right_key == 1 && key == Qt::Key_Right)on_toolButton_image_next_clicked();
			else if (left_right_key == 2 && key == Qt::Key_Left)on_toolButton_left_rotate_clicked();
			else if (left_right_key == 2 && key == Qt::Key_Right)on_toolButton_right_rotate_clicked();
		}
		if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
			if (up_down_key == 0 && key == Qt::Key_Up)on_toolButton_image_bigger_clicked();
			else if (up_down_key == 0 && key == Qt::Key_Down)on_toolButton_image_small_clicked();
			else if (up_down_key == 1 && key == Qt::Key_Up)on_toolButton_image_prev_clicked();
			else if (up_down_key == 1 && key == Qt::Key_Down)on_toolButton_image_next_clicked();
			else if (up_down_key == 2 && key == Qt::Key_Up)on_toolButton_left_rotate_clicked();
			else if (up_down_key == 2 && key == Qt::Key_Down)on_toolButton_right_rotate_clicked();
		}
	}
	else if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (img_move && mouseEvent->button() == Qt::LeftButton) {
			ui->label_main_image->setCursor(Qt::ClosedHandCursor);
			old_pos = mouseEvent->pos();
			move_image = true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
			if(img_move)ui->label_main_image->setCursor(Qt::OpenHandCursor);
			else ui->label_main_image->setCursor(Qt::ArrowCursor);
			move_image = false;
		}
	}
	else if (event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (move_image) {
			auto now_pos = mouseEvent->pos();
			int lab_w = ui->label_main_image->width();
			int lab_h = ui->label_main_image->height();
			int _offset_x = offset_x + old_pos.x() - now_pos.x();
			int _offset_y = offset_y + old_pos.y() - now_pos.y();

			_offset_x = _offset_x + lab_w > img_width ? img_width - lab_w : _offset_x;
			_offset_y = _offset_y + lab_h > img_height ? img_height - lab_h : _offset_y;
			_offset_x = _offset_x < 0 ? 0 : _offset_x;
			_offset_y = _offset_y < 0 ? 0 : _offset_y;

			offset_x = _offset_x, offset_y = _offset_y;
			old_pos = mouseEvent->pos();
			img_move_refresh++;
		}
	}


	return QMainWindow::eventFilter(obj, event);
}
