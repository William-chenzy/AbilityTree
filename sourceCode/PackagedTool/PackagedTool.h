#ifndef MAINWINDOW_H_LN
#define MAINWINDOW_H_LN

#include <QMainWindow>
#include "GlobalDefine.h"
#include "ui_PackagedTool.h"
namespace Ui {
    class PackagedTool;
}

class PackagedTool : public QMainWindow
{
    Q_OBJECT

public:
    PackagedTool(QWidget *parent = 0);
    ~PackagedTool() = default;

private:
	void GetIngnoreFile();
	void GetIngnoreDir();
	void CreateScript();
	void ZipAllFiles();

private slots:
	void on_toolButton_qt_dir_clicked();
	void on_toolButton_install_dir_clicked();
	void on_toolButton_package_dir_clicked();
	void on_pushButton_start_package_clicked();
	void on_toolButton_output_dir_clicked();

private:
    Ui::PackagedTool* ui;
};

#endif // MAINWINDOW_H_LN
