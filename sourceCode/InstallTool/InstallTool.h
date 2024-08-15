#ifndef MAINWINDOW_H_LN
#define MAINWINDOW_H_LN

#include <qtimer.h>
#include <QMainWindow>
#include "ui_InstallTool.h"

#ifdef __linux__
#include <LinuxDefine.h>
#else
#include <WindowsDefine.h>
#endif

namespace Ui {
    class InstallTool;
}

class InstallTool : public QMainWindow
{
    Q_OBJECT

public:
    InstallTool(QWidget *parent = 0);
    ~InstallTool() = default;

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:
	void SetProcessBar(float val);
	void CheckInstallStatus();
	bool UnZip(QString path);
	void CreateAnimation();
	bool UnPackage();

private slots:
	void on_toolButton_min_clicked();
	void on_toolButton_close_clicked();
	void on_toolButton_path_clicked();
	void on_pushButton_start_clicked();
	void on_pushButton_updata_clicked();
	void on_pushButton_delete_clicked();
	void on_pushButton_user_rule_clicked();

private:
	float cur, set;
	QTimer animation;
	std::string package_str;
    Ui::InstallTool* ui;
};

#endif // MAINWINDOW_H_LN
