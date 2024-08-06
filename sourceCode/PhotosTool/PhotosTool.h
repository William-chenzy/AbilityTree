#ifndef PHOTOS_TOOL_H
#define PHOTOS_TOOL_H

#include <opencv2/opencv.hpp>
#include <ImageWidget.h>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTimer>
namespace Ui {
    class PhotosTool;
}

class PhotosTool : public QMainWindow
{
	Q_OBJECT

public:
    PhotosTool(QWidget *parent = 0);
	~PhotosTool() = default;

private slots:
	void MonitorEvent();
	void on_toolButton_min_clicked();
	void on_toolButton_max_clicked();
	void on_toolButton_close_clicked();
	void LoadIamge(QString path, QString name, QString size, QString index);

	void on_toolButton_image_full_clicked();
	void on_toolButton_image_bigger_clicked();
	void on_toolButton_image_small_clicked();
	void on_toolButton_image_prev_clicked();
	void on_toolButton_image_next_clicked();
	void on_toolButton_left_rotate_clicked();
	void on_toolButton_right_rotate_clicked();
	void on_toolButton_image_delete_clicked();
	void on_toolButton_image_max_clicked();
	void on_toolButton_image_info_clicked();

	void on_toolButton_save_as_clicked();
	void on_toolButton_open_dir_clicked();

	void on_lineEdit_image_name_editingFinished();
	void on_comboBox_key_currentIndexChanged(int val);
	void on_comboBox_wheel_type_currentIndexChanged(int val);

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private:
	void AddNextPrevButton();
	void RefreshImage();
	void ShowImageToLabel();
	void LoadDirImage(int index);
	void MultipleImage(bool bigger);
	void LoadDir(QString path_name);
	void MoveHScrollBar(ImageWidget* wid);

private:
	int wheel_key = 0;
	int up_down_key = 1;
	int left_right_key = 2;

	QTimer load;
	QTimer monitor;
	float multiple;
	cv::Mat cv_img;
	cv::Mat raw_img;
	int refresh_bar;
	float t_multiple;
	QString curr_path;
	float multiple_step;
	QStringList all_file;
	ImageWidget* current;
	QHBoxLayout* img_list_layout;
	std::vector<ImageWidget*> img_list;
	std::vector<float>mutiple_list;
	QString drop_path;
	QRegion clipRegion;

	int offset_x;
	int offset_y;
	bool img_move;
	int img_width;
	int img_height;
	int img_move_refresh;

	bool image_max = false;
	bool main_full = false;
	bool image_full = false;

	QToolButton* prev_button;
	QToolButton* next_button;
	QWidget* prev_button_wid;
	QWidget* next_button_wid;

    Ui::PhotosTool* ui;
};


#endif // PHOTOS_TOOL_H
