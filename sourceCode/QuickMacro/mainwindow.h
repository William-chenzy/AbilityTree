#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qtimer>
#include <ClickPoint.h>
namespace Ui {
    class QuickMacro;
}

class QuickMacro : public QMainWindow
{
	Q_OBJECT

public:
    QuickMacro(QWidget *parent = 0);
    ~QuickMacro(){ UnhookWindowsHookEx(mouse_Hook); UnhookWindowsHookEx(mouse_Hook);}

protected:
	void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
	void closeEvent(QCloseEvent *event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void ErrExit(QString msg);
	void SaveConfigure();
    void HotKetBack();
    void RefreshView();

private slots:
    void SaveFile();
    void ReadFile();
    void on_pushButton_top_clicked();
    void on_pushButton_show_clicked();
    void on_pushButton_point_clicked();
	void on_pushButton_set_done_clicked();
	void on_pushButton_left_start_clicked();
	void on_pushButton_mdify_video_clicked();
	void on_pushButton_modify_start_clicked();
    void on_horizontalSlider_action_valueChanged(int val);
	void on_pushButton_set_cancel_clicked(bool init = false);

private:
	int hotK_1, hotK_2, hotK_3;
	bool lf_execute, mf_execute;
    ClickPoint cp;
    QPoint cp_p;
	QString global_info;
    Ui::QuickMacro* ui;
    HHOOK mouse_Hook = NULL;
    HHOOK key_Hook = NULL;
    QTimer re;
};


#endif // MAINWINDOW_H
