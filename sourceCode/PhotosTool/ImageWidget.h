#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <opencv2/opencv.hpp>
#include <QMainWindow>
#include <Qtimer>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>

class ImageWidget : public QWidget
{
	Q_OBJECT

public:
    ImageWidget(QWidget *parent = 0);
	~ImageWidget();

	void clear();
	void ClearFouce();
	void ClickedThis();
	void SetNewImage(QString path, QString index);

signals:
	void clicked(QString p, QString n, QString s, QString i);

protected:
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void GetNewImage();

private:
	bool fouce;
	QWidget* bk;
	QString path;
	QString name;
	QString index;
	QCheckBox* box;
	QLabel* img_lab;
	QString obj_name;
	QString file_size;
	QTimer thread_detach;
};


#endif // IMAGE_WIDGET_H
