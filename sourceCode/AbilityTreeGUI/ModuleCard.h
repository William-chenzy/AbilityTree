#ifndef MODULE_CARD_H
#define MODULE_CARD_H

#include "opencv2/opencv.hpp"
#include "GlobalDefine.h"
#include <QGridLayout>
#include <QPushButton>
#include <QApplication>
#include <QProcess>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QTimer>

class ModuleCard : public QWidget
{
	Q_OBJECT
public:
	explicit ModuleCard(QWidget *parent = nullptr);
	~ModuleCard();

	void BeginAnimation();
	void SetProgramPath(QString path) { exe_path = path; }
	void SetParam(QString name = "", QString icon = "", int _width = -1 , int _height = -1);

signals:
	void deleteModule(QString,void*);

protected:
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;

private slots:
	void InOutAnimation();
	void CreateAnimation();
	void StartButtonClicked();
	void DeleteButtonClicked(bool is_exit = false);

private:
	void AddStartModifyButton();

private:
	bool show_button;
	QString obj_name;
	QTimer refresh;
	QTimer in_out;
	int mask_val;
	int mask_val2;

	QString exe_path;
	QProcess* program;

	QWidget* card = nullptr;
	QWidget* mask = nullptr;
	QLabel* name_lab = nullptr;
	QLabel* icon_lab = nullptr;
	QLabel* background = nullptr;
	QVBoxLayout* main_layout = nullptr;
	QGridLayout* card_layout = nullptr;

	QPushButton* start = nullptr;
	QPushButton* modify = nullptr;
	QPushButton* go_back = nullptr;
	QWidget* modify_widget = nullptr;
	QPushButton* delete_button = nullptr;
	QVBoxLayout* modify_widget_layout = nullptr;
};

static QString WidgetStyle = QString("QWidget{background:transparent;}") + PUSH_BUTTON_STYLE;

class GlobalImage :public QObject {
	Q_OBJECT
public:
	static GlobalImage* getInstance() {
		static GlobalImage* ptr = new GlobalImage();
		return ptr;
	}
	void InstallEvent(QLabel* lab, QWidget* wid) {
		if (!global_timer.isActive())UpdataImage(), global_timer.start(30);
		refresh_wid[wid] = true;
		refresh_lab[lab] = true;
		install_num++;
	}
	void UnInstallEvent(QLabel* lab, QWidget* wid) {
		if (--install_num <= 0 && global_timer.isActive()) {
			circle_point_size = { width / 2, height / 2,0 };
			global_timer.stop();
			line_width = 0;
			mask_val = 0;
		}
		refresh_wid[wid] = false;
		refresh_lab[lab] = false;
	}
	void SetParam(int w, int h) {
		width = w, height = h;
		max_radius = pow(pow(w, 2) + pow(h, 2), 0.5);
		if (!global_timer.isActive())circle_point_size = { width / 2, height / 2,0 };
		global_img = DrawRGBBcakGround(w, h);
	}
	GlobalImage(const GlobalImage&) = delete;
	GlobalImage& operator=(const GlobalImage&) = delete;
private:
	GlobalImage() {
		QObject::connect(&global_timer, &QTimer::timeout, this, &GlobalImage::UpdataImage);
		mask_val = install_num = 0;
	}
	~GlobalImage() = default;
private slots:
	void UpdataImage() {
		if (!LoadConfigure::getInstance()->GetAnimation()) return;
		if (!QApplication::activeWindow())return;

		int _val = mask_val++;
		if (_val >= 200) _val = mask_val = 0;
		else if (_val >= 100) _val = 200 - _val;

		float v1 = _val / 100.f;
		float v2 = (_val - 20) / 100.f;
		float v3 = (_val + 20) / 100.f;
		if (v2 < 0)v3 = v3 - v2, v2 = 0;
		if (v3 > 1)v2 = v2 - 1 + v3, v3 = 1;

		QString border = "border-radius: 6px;border:2px solid qlineargradient(x1:0,y1:0,x2:1,y2:1,";
		auto v1_s = QString::number(v1, 'f', 2), v2_s = QString::number(v2, 'f', 2), v3_s = QString::number(v3, 'f', 2);
		border += QString("stop:%1 #1edffc, stop:%2 #ffffff, stop:%3 #1edffc);").arg(v2_s).arg(v1_s).arg(v3_s);
		for (auto i : refresh_wid) {
			if (!i.second)continue;
			i.first->setStyleSheet(WidgetStyle + QString("#%1{%2}").arg(i.first->objectName()).arg(border));
		}

		cv::Point pt = cv::Point(circle_point_size[0], circle_point_size[1]);
		cv::Mat _img = cv::Mat(global_img.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
		cv::circle(_img, pt, circle_point_size[2], cv::Scalar{ 1,1,1,24 }, line_width, 120);

		for (int i = 0; i < _img.rows; i++) {
			for (int j = 0; j < _img.cols; j++) {
				if (!_img.at<cv::Vec4b>(i, j)[3])continue;
				auto& val = _img.at<cv::Vec4b>(i, j);
				auto& val_r = global_img.at<cv::Vec4b>(i, j);
				val = { val_r[0],val_r[1],val_r[2],val[3] };
			}
		}

		auto pixmap = QPixmap::fromImage(QImage(_img.data, _img.cols, _img.rows, (int)_img.step, QImage::Format_ARGB32));
		for (auto i : refresh_lab) if (i.second)i.first->setPixmap(pixmap);

		if (circle_point_size[2] - line_width >= max_radius) {
			int w = rand() % width, h = rand() % height;
			circle_point_size[0] = h;
			circle_point_size[1] = w;
			circle_point_size[2] = 0;
			line_width = 0;
		}
		circle_point_size[2] += 2;
		line_width += 0.2f;
	}
private:
	int mask_val;
	int max_radius;
	int install_num;
	float line_width;
	int width, height;
	cv::Mat global_img;
	QTimer global_timer;
	cv::Vec3i circle_point_size;
	std::map<QLabel*, bool>refresh_lab;
	std::map<QWidget*, bool>refresh_wid;
};

#endif // MODULE_CARD_H