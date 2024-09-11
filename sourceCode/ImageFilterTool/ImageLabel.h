#ifndef IMAGE_LABEL_H
#define IMAGE_LABEL_H

#include <opencv2/opencv.hpp>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTimer>

enum M_TYPE { CIRCLE, I_CIRCLE, LINE, I_LINE, BOX, I_BOX, ANY, I_ANY };
class ImageLabel : public QLabel {
	Q_OBJECT

public:
	ImageLabel();
	~ImageLabel() = default;

	void MouseType(M_TYPE val) { m_type = val; }
	void MouseTracking(bool val) { mouse_tracking = val; }

signals:

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
	M_TYPE m_type;
	bool mouse_tracking = false;
};


#endif // IMAGE_LABEL_H
