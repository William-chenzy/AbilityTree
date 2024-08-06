#include "ClickPoint.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <QIcon>
#include <QApplication>

ClickPoint::ClickPoint(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
	this->resize(24, 24);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	setWindowOpacity(1);
	setAttribute(Qt::WA_TranslucentBackground);
	ui.label_img->setStyleSheet("border-image:url(./res/mouseCircle.png);");
	::SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
}

static QPoint mouse_pos;
static bool is_move_mouse;
void ClickPoint::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		is_move_mouse = true;
		mouse_pos = event->screenPos().toPoint();
	}
	QWidget::mousePressEvent(event);
}

void ClickPoint::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}

void ClickPoint::mouseMoveEvent(QMouseEvent* event) {
	setCursor(Qt::SizeAllCursor);
	if (is_move_mouse) {
		QPoint newPos = this->pos() + (event->screenPos().toPoint() - mouse_pos);
		move(newPos);
		mouse_pos = event->screenPos().toPoint();
	}

	QWidget::mouseMoveEvent(event);
}
