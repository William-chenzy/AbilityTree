#include "ui_ImageFilterTool.h"
#include "ImageFilterTool.h"
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
#include<QSplitter>
#include <QSlider>
#include <QFileDialog>
#include <QApplication>
#include "GlobalDefine.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
using namespace chrono;

#define ANIMATION_TIMES 15.f;
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//-------------------------------------------傅里叶函数---------------------------------
void dftshift(Mat& ds){
	int cx = ds.cols / 2;//图像的中心点x坐标
	int cy = ds.rows / 2;//图像的中心点y坐标
	Mat q0 = ds(Rect(0, 0, cx, cy));//左上
	Mat q1 = ds(Rect(cx, 0, cx, cy));//右上
	Mat q2 = ds(Rect(0, cy, cx, cy));//左下
	Mat q3 = ds(Rect(cx, cy, cx, cy));//右下

	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}
void myimshow(const Mat& complexImg){
	Mat mag;
	//对复数图像计算幅值
	Mat planes[2];
	split(complexImg, planes);
	magnitude(planes[0], planes[1], mag);

	//对幅值去对数log
	Mat mag_log;
	mag += Scalar::all(1);
	cv::log(mag, mag_log);
	//对幅值图像做归一化处理
	Mat mag_norm;
	cv::normalize(mag_log, mag_norm, 0, 1, NORM_MINMAX);
	imshow("dft magnitud", mag_norm);
}
void myimshow2(const Mat& complexImg){
	Mat mag;
	//对复数图像计算幅值
	Mat planes[2];
	split(complexImg, planes);
	magnitude(planes[0], planes[1], mag);
	normalize(mag, mag, 1, 0, NORM_MINMAX);
	imshow("inverce dft magnitud", mag);
}
int FuLiYeFunction(){
	//1.读入灰度图像，不要读入彩色图像
	Mat iimg, img_dft, img2;
	Mat img=imread("./res/1725546147671.png",0);
	imshow("original image", img);
	//2.将单通道图像转换成双通道图像
	img.convertTo(img2, CV_32FC2);
	//3.调用dft函数实现傅里叶变换
	dft(img2, img_dft, DFT_COMPLEX_OUTPUT);
	//4.显示傅里叶频谱图
	dftshift(img_dft);//傅里叶普的中心化

	//for (int i = 0; i < img_dft.cols; i++) {
	//	for (int j = 0; j < img_dft.rows; j++) {
	//		if (abs(i - img_dft.cols / 2) > img_dft.cols / 10 || abs(j - img_dft.rows / 2) > img_dft.rows / 10) img_dft.at<Vec2f>(j, i) = 0;
	//	}
	//}

	int half_cols = img_dft.cols / 2, half_rows = img_dft.rows / 2, range = 12;
	for (int i = 0; i < img_dft.cols; i++) {
		for (int j = 0; j < img_dft.rows; j++) {
			if (abs(i - half_cols) < range && abs(j - half_rows) < range) img_dft.at<Vec2f>(j, i) = 0;
		}
	}
	img_dft.at<Vec2f>(img_dft.rows / 2, img_dft.cols / 2) = 0;

	myimshow(img_dft);

	//5.调用idft()函数，执行逆傅里叶变换
	idft(img_dft, iimg);
	myimshow2(iimg);
	waitKey();
	return 0;
}
//-------------------------------------------傅里叶函数---------------------------------

ImageFilterTool::ImageFilterTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::ImageFilterTool)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-ImageFilterTool.png")));
	ui->label_resize->installEventFilter(this);

	//FuLiYeFunction();
}

void ImageFilterTool::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls()) event->acceptProposedAction();
	else event->ignore();
}
void ImageFilterTool::dropEvent(QDropEvent* event) {
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) return;
}

QPoint mouse_pos;
bool is_move_mouse;
int size_m_type = 0;
void ImageFilterTool::mousePressEvent(QMouseEvent* event) {
	if (ImageFilterTool::isMaximized()) {
		QWidget::mouseMoveEvent(event);
		return;
	}

	//if (event->button() == Qt::LeftButton) {
	//	QRect rect_top = ui->widget_title->rect();
	//	rect_top.translate(ui->widget_title->pos());

	//	int _x = event->screenPos().x();
	//	int _y = event->screenPos().y();
	//	if (abs(_x - pos().x()) <= 4 && abs(_y - pos().y()) <= 4) size_m_type = 1;
	//	else if (abs(_x - pos().x() - width()) <= 4 && abs(_y - pos().y() - height()) <= 4) size_m_type = 2;
	//	else if (abs(_x - pos().x() - width()) <= 4 && abs(_y - pos().y()) <= 4)size_m_type = 3;
	//	else if (abs(_x - pos().x()) <= 4 && abs(_y - pos().y() - height()) <= 4) size_m_type = 4;
	//	else if (abs(_x - this->pos().x()) <= 4) size_m_type = 5;
	//	else if (abs(_y - pos().y() - height()) <= 4) size_m_type = 6;
	//	else if (abs(_x - pos().x() - width()) <= 4) size_m_type = 7;
	//	else if (abs(_y - this->pos().y()) <= 4)size_m_type = 8;
	//	else size_m_type = 0;

	//	if (size_m_type) mouse_pos = event->screenPos().toPoint();
	//	else if (rect_top.contains(event->pos())) {
	//		is_move_mouse = true;
	//		mouse_pos = event->screenPos().toPoint();
	//	}
	//}

	QWidget::mousePressEvent(event);
}
void ImageFilterTool::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false, size_m_type = 0;
	QWidget::mouseReleaseEvent(event);
}
void ImageFilterTool::mouseMoveEvent(QMouseEvent* event) {
	if (ImageFilterTool::isMaximized()) {
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
void ImageFilterTool::mouseDoubleClickEvent(QMouseEvent* event) {
	//QRect rect_top = ui->widget_title->rect();
	//rect_top.translate(ui->widget_title->pos());
	//if (rect_top.contains(event->pos())) on_toolButton_max_clicked();
	//QWidget::mousePressEvent(event);
}
bool ImageFilterTool::eventFilter(QObject* obj, QEvent* event) {
	static QPoint old_pos;
	static bool resize = false;
	if (obj == ui->label_resize) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (event->type() == QEvent::MouseMove && resize) {
			int offset = mouseEvent->globalPos().x() - old_pos.x();
			int width = ui->groupBox_info->width() - offset, width2 = ui->widget_2->width();
			if (width2 >= 100 || offset > 0)ui->groupBox_info->setFixedWidth(width <= 100 ? 100 : width);
			old_pos = mouseEvent->globalPos();
			user_size = true;
		}
		else if (event->type() == QEvent::MouseButtonPress)resize = true, old_pos = mouseEvent->globalPos();
		else if (event->type() == QEvent::MouseButtonRelease) resize = false;
		return QMainWindow::eventFilter(obj, event);
	}

	if (event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	}
	else if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		auto key = keyEvent->key();
		if (key == Qt::Key_Left || key == Qt::Key_Right) {
		}
		if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
		}
	}
	else if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
		}
	}
	else if (event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
	}


	return QMainWindow::eventFilter(obj, event);
}

void ImageFilterTool::resizeEvent(QResizeEvent *event) {
	if (!user_size)ui->groupBox_info->setFixedWidth(this->width() / 2);
}