#ifndef INITIALLIZE_WINDOWS_H
#define INITIALLIZE_WINDOWS_H

#include <QTimer>
#include <qmainwindow.h>
#include "ui_InitializeWindow.h"

namespace Ui {
    class InitializeWindow;
}
class InitializeWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit InitializeWindow(QWidget* parent = nullptr);
	bool IsInitDone() { return init_done; }
	~InitializeWindow();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;;

private slots:
	void ReloadParam();
	void RefreshAnimation();

private:
	void RefreshStatus(QString msg);
    void SwitchLanguage();

private:
	int g = 70;
	bool add_type;
	bool init_done;
	QTimer refresh;
	QTimer animation;
	QPoint mouse_pos;
	bool is_move_mouse;
    Ui::InitializeWindow* ui;
};

#define SOFTWARE_INFORMATION "This software is based on GPL\r\n\
Can be used for learning, research, and commercial purposes\r\n\
But it must be open-source\r\n\
After modification, the original author's name cannot be used for promotion\r\n\
Last modified by: Lin Yuhan\r\n"

#endif // INITIALLIZE_WINDOWS_H
