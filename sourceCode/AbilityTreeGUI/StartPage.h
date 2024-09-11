#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <qpushbutton.h>
#include <opencv2/opencv.hpp>

namespace ATS {
	class StartPage : public QObject {
		Q_OBJECT

	public:
		StartPage();
		~StartPage() = default;
		QWidget* GetWidget() { return widget; }

	private:
		void ReSizeCenter(QSize);
		void ReSizeCenterLabel(QSize);

	signals:
		void StartDone();

	private slots:
		void AnimationRefresh();
		bool eventFilter(QObject* watched, QEvent* event) override;

	private:
		QTimer animation;
		QLabel* info = nullptr;
		QLabel* contin = nullptr;
		QLabel* top_line = nullptr;
		QLabel* letf_line = nullptr;
		QLabel* bottom_line = nullptr;
		QPushButton* confirm = nullptr;
		QToolButton* close = nullptr;
		QLabel* logo_img = nullptr;
		QLabel* welcome = nullptr;
		QWidget* widget = nullptr;
		QWidget* title = nullptr;
		QWidget* main_wid = nullptr;
		QWidget* center_wid = nullptr;
		QGridLayout* layout = nullptr;
		QHBoxLayout *title_layout = nullptr;
		QGridLayout* main_layout = nullptr;
		QGridLayout* center_layout = nullptr;
		QLabel* welcome_null = nullptr;

		QWidget* welcome_wid = nullptr;
		QHBoxLayout *welcome_layout = nullptr;

		QWidget* logo_wid = nullptr;
		QGridLayout *logo_layout = nullptr;

		QWidget* logo_img_wid = nullptr;
		QHBoxLayout *logo_img_layout = nullptr;

		QWidget* top_line_wid = nullptr;
		QHBoxLayout *top_line_layout = nullptr;

		QWidget* bottom_line_wid = nullptr;
		QHBoxLayout *bottom_line_layout = nullptr;

		bool first_animation = false;
		bool second_animation = false;
	};
};

#endif //STARTPAGE_H