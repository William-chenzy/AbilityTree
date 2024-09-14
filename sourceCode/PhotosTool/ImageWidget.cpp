#include "ImageWidget.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <iostream>
#include <QMessageBox>
#include <QMimeData>
#include <thread>
#include <fstream>
#include <QGridLayout>
#include <QFileDialog>
#include <QApplication>
#include "GlobalDefine.h"
using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

static map<int, QString>lev{
	{0," B "},
	{1," KB "},
	{2," MB "},
	{3," GB "},
};

static ImageWidget* curr = nullptr;
static int obj_num = 0;
ImageWidget::ImageWidget(QWidget *parent){
	QGridLayout* layout = new QGridLayout();
	QGridLayout* bk_layout = new QGridLayout();
	obj_name = QString("ImageWidget_%1").arg(obj_num++);
	this->setObjectName(obj_name);
	this->setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);

	img_lab = new QLabel("加载中...");
	box = new QCheckBox();
	bk = new QWidget();

	bk_layout->setMargin(2);
	bk_layout->setSpacing(0);
	bk->setLayout(bk_layout);

	this->setFixedWidth(88);
	this->setFixedHeight(64);
	img_lab->setFixedWidth(80);
	img_lab->setFixedHeight(60);
	img_lab->setAlignment(Qt::AlignCenter);

	bk->setStyleSheet("border-radius: 6px;background-color:rgba(255,255,255,66);border:0px solid #ffffff;");
	img_lab->setStyleSheet("background-color:transparent;border-radius: 0px;border:0px solid #ffdffc;");
	box->setStyleSheet("background-color:transparent;border-radius: 2px;border:0px solid #1edffc;");
	
	fouce = false;
	box->setVisible(false);
	layout->addWidget(bk, 0, 0, 1, 1);
	bk_layout->addWidget(img_lab, 0, 0, 2, 2, Qt::AlignHCenter);
	bk_layout->addWidget(box, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

	thread_detach.setSingleShot(true);
	connect(&thread_detach, &QTimer::timeout, this, &ImageWidget::GetNewImage);
}
ImageWidget::~ImageWidget() { if (curr == this)curr = nullptr; }

void ImageWidget::clear() {
	img_lab->setText("加载中...");
	this->setVisible(false);
	box->setChecked(false);
	file_size.clear();
	index.clear();
	path.clear();
	name.clear();
}
void ImageWidget::GetNewImage() {
	cv::Mat img = cv::imread(path.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
	if (img.empty()) {
		qInfo() << path << " is empty";
		clear();
		return;
	}

	float mul_w = 80.f / img.cols * 1.f, mul_h = 60.f / img.rows * 1.f;
	float mul = mul_w < mul_h ? mul_w : mul_h;
	cv::resize(img, img, { 0,0 }, mul, mul);

	auto c_t = img.channels() == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;
	cv::cvtColor(img, img, c_t);

	auto c_n = img.channels() == 4 ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
	auto _img = QImage(img.data, img.cols, img.rows, img.cols * img.channels(), c_n);
	img_lab->setPixmap(QPixmap::fromImage(_img));
	box->setChecked(false);
}
void ImageWidget::SetNewImage(QString _path, QString _index) {
	path = _path;
	if (!_index.isEmpty())index = _index;
	name = path.mid(path.lastIndexOf('/') + 1);

	int level = 0;
	double f_val = QFile(path).size();
	while (f_val >= 1000)f_val /= 1000, level++;
	file_size = " " + QString::number(f_val, 'f', 2) + lev[level];
	thread_detach.start(10);
}
void ImageWidget::ClickedThis() {
	if (path.isEmpty())return;
	if (curr)curr->ClearFouce();
	curr = this;

	fouce = true;
	box->setVisible(false);
	emit clicked(path, name, file_size, index);
	bk->setStyleSheet("border-radius: 6px;background-color:transparent;border:1px solid #1edffc;");
}
void ImageWidget::ClearFouce() {
	fouce = false;
	bk->setStyleSheet("border-radius: 6px;background-color:rgba(255,255,255,66);border:0px solid #666666;");
}


void ImageWidget::enterEvent(QEvent* event) {
	if (!fouce)box->setVisible(true);
	if (!fouce)bk->setStyleSheet("border-radius: 6px;background-color:rgba(255,255,255,66);border:1px solid #666666;");
}
void ImageWidget::leaveEvent(QEvent* event) {
	box->setVisible(false);
	if (!fouce)bk->setStyleSheet("border-radius: 6px;background-color:rgba(255,255,255,66);border:0px solid #666666;");
}

void ImageWidget::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) ClickedThis();
	QWidget::mousePressEvent(event);
}
bool ImageWidget::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) qInfo() << "ImageWidget LeftButton: " << path;
		if (mouseEvent->button() == Qt::RightButton) qInfo() << "ImageWidget RightButton: " << path;
	}

	return false;
}