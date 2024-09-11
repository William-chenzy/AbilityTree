#ifndef CLOUD_ANIMATION_H
#define CLOUD_ANIMATION_H

#include <opencv2/opencv.hpp>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTimer>
namespace Ui {
    class ImageFilterTool;
}

class ImageFilterTool : public QMainWindow{
	Q_OBJECT

public:
    ImageFilterTool(QWidget *parent = 0);
	~ImageFilterTool() = default;

private slots:

protected:
	void resizeEvent(QResizeEvent *event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private:

private:
	bool user_size = false;
    Ui::ImageFilterTool* ui;
};


#endif // CLOUD_ANIMATION_H
