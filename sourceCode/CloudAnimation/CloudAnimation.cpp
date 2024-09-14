#include "ui_CloudAnimation.h"
#include "CloudAnimation.h"
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
using namespace std;
using namespace chrono;

#define ANIMATION_TIMES 15.f;
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CloudAnimation::CloudAnimation(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::CloudAnimation)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-CloudAnimation.png")));
}

void CloudAnimation::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls()) event->acceptProposedAction();
	else event->ignore();
}
void CloudAnimation::dropEvent(QDropEvent* event) {
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) return;
}


QPoint mouse_pos;
bool is_move_mouse;
int size_m_type = 0;
void CloudAnimation::mousePressEvent(QMouseEvent* event) {
	if (CloudAnimation::isMaximized()) {
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
void CloudAnimation::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false, size_m_type = 0;
	QWidget::mouseReleaseEvent(event);
}
void CloudAnimation::mouseMoveEvent(QMouseEvent* event) {
	if (CloudAnimation::isMaximized()) {
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
void CloudAnimation::mouseDoubleClickEvent(QMouseEvent* event) {
	//QRect rect_top = ui->widget_title->rect();
	//rect_top.translate(ui->widget_title->pos());
	//if (rect_top.contains(event->pos())) on_toolButton_max_clicked();
	//QWidget::mousePressEvent(event);
}
bool CloudAnimation::eventFilter(QObject* obj, QEvent* event) {
	static QPoint old_pos;
	static bool move_image = false;
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
			old_pos = mouseEvent->pos();
			move_image = true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
			move_image = false;
		}
	}
	else if (event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		if (move_image) {
			auto now_pos = mouseEvent->pos();
		}
	}


	return QMainWindow::eventFilter(obj, event);
}
