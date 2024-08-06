#pragma once

#include <QDialog>
#include <Windows.h>
#include <atltypes.h>
#include "ui_ClickPoint.h"

class ClickPoint : public QDialog
{
	Q_OBJECT

public:
	ClickPoint(QWidget *parent = nullptr);
	~ClickPoint() = default;
	QPoint GetPoint() { return { this->pos().x() + 12,this->pos().y() + 12 }; }
	void SetPoint(QPoint _p) { this->move(_p.x() + 12, _p.y() + 12); }
	void Wheel() {ui.label_img->setStyleSheet("border-image:url(./res/mouseMiddle.png);");}
	void LeftPush() { ui.label_img->setStyleSheet("border-image:url(./res/mouseLeft.png);"); }
	void LeftUp() { ui.label_img->setStyleSheet("border-image:url(./res/mouseCircle.png);"); }
	void RightPush() { ui.label_img->setStyleSheet("border-image:url(./res/mouseRight.png);"); }
	void RightUp() { ui.label_img->setStyleSheet("border-image:url(./res/mouseCircle.png);"); }

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	Ui::ClickPointClass ui;
};
