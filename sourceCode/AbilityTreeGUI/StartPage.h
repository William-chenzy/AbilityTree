#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QLabel>
#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QToolButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <qpushbutton.h>
#include <CloudWidget.h>
#include <opencv2/opencv.hpp>


namespace ATS {
	class PAGE: public QObject {
	public:
		PAGE() {};
		~PAGE() {};

		QWidget* GetWidgte() {
			return center_wid; 
		}
		bool IsAnimationDone() {
			return animation_done;
		}
		bool IsVisiable() {
			return center_wid->isVisible(); 
		}
		virtual void SetShow() {
			center_wid->setVisible(true);
			cloud->SetVisible(cloud_name, true);
		}
		virtual void Resize(int width, int height) = 0;
		virtual void Animation(int width, int height) = 0;
		virtual void OutAnimation(int width, int height) = 0;

	protected:
		void RotateCamera(int width, int height) {
			if (!QApplication::activeWindow())return;
			QRect rect = QApplication::activeWindow()->rect();
			rect.translate(QApplication::activeWindow()->pos());
			if (!rect.contains(QCursor::pos()))return;

			auto now_pos = QCursor::pos();
			float half_width = width / 2, half_height = height / 2;
			float per_width = (now_pos.x() - rect.x() - half_width) / half_width;
			float per_height = (now_pos.y() - rect.y() - half_height) / half_height;

			float base_angle = 25.f;
			float theta = (per_width * base_angle) * M_PI / 180.f;
			float theta2 = (per_height * base_angle) * M_PI / 180.f;
			float z = 0 + 120 * cos(theta), y = 0 - 120 * sin(theta), x = 0 - 120 * sin(theta2);
			cloud->SetCameraPosition({ x , y , z }, { 0, 0, 0 }, { -1000, 0, 0 });
		}
		virtual void SetHide() {
			center_wid->setVisible(false);
			cloud->SetVisible(cloud_name, false);
		}

	protected:
		std::string cloud_name;
		bool animation_done = false;
		CloudViewer* cloud = nullptr;
		QWidget* center_wid = nullptr;
		bool is_out_animation = false;
		QGridLayout* center_layout = nullptr;
	};
	class Welcome : public PAGE {
		Q_OBJECT

	public:
		Welcome(CloudViewer*);
		void SetShow() override;
		void Resize(int width, int height) override;
		void Animation(int width, int height) override;
		void OutAnimation(int width, int height) override;

	signals:
		void Done();

	private:
		QLabel* info = nullptr;
		QLabel* contin = nullptr;
		QLabel* top_line = nullptr;
		QLabel* left_line = nullptr;
		QLabel* bottom_line = nullptr;
		QLabel* logo_img = nullptr;
		QLabel* welcome = nullptr;
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

		float text_pos = 0;
		cv::Mat logo, canny;
		std::list<std::pair<int,int>>all_ptc;
	};
	class Notice : public PAGE {
		Q_OBJECT

	public:
		Notice(CloudViewer*);
		void SetShow() override;
		void Resize(int width, int height) override;
		void Animation(int width, int height) override;
		void OutAnimation(int width, int height) override;

	signals:
		void Done();

	private:
		void SetHide() override;

	private:
		QStringList line;
		QStringList group;
		QStringList child;
		std::string cloud_box;
		QLabel* info = nullptr;
		QScrollArea* area = nullptr;
		QWidget* area_wid = nullptr;
		QVBoxLayout *area_layout = nullptr;
		QPushButton* confirm = nullptr;
		QPushButton* user_proto = nullptr;
	};
	class Set : public PAGE {
		Q_OBJECT

	public:
		Set(CloudViewer*);
		void Resize(int width, int height) override;
		void Animation(int width, int height) override;
		void OutAnimation(int width, int height) override;

	private:

	signals:
		void Done();

	private:
		bool set_done = false;
		QLabel* info = nullptr;
		QLabel* descripe = nullptr;
		QPushButton* in_viwewer = nullptr;
		QPushButton* height = nullptr;
		QPushButton* low = nullptr;
	};

	class StartPage : public QWidget {
		Q_OBJECT

	public:
		StartPage();
		~StartPage() = default;

	signals:
		void StartDone();

	private slots:
		void AnimationRefresh();
		void DoneOfWelcomePage();
		void mouseReleaseEvent(QMouseEvent* event) override;

	private:
		QTimer animation;
		bool all_done = false;
		QWidget* title = nullptr;
		QWidget* main_wid = nullptr;
		QWidget* center_box = nullptr;
		QToolButton* close = nullptr;
		QPushButton* skip = nullptr;
		QGridLayout* layout = nullptr;
		QHBoxLayout *title_layout = nullptr;
		QGridLayout* main_layout = nullptr;
		QGridLayout* center_box_layout = nullptr;

		QLabel* backGround = nullptr;
		CloudViewer* cloud = nullptr;
		Notice* notice = nullptr;
		Welcome* wel = nullptr;
		Set* set = nullptr;
	};
};

#endif //STARTPAGE_H