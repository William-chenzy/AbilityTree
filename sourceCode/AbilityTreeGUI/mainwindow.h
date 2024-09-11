#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <atomic>
#include <QWidget>
#include <qtimer.h>
#include <QGridLayout>
#include <StartPage.h>
#include <qmainwindow.h>
#include "ModuleList.h"
#include "HelpPage.h"

namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
	void StartDone();
	void DeleteModule();
	void RefreshViewer();
	void MainConfigChange(MainConfig val);
	void ModuleListClicked(void* sender);
    void on_toolButton_help_clicked();
    void on_toolButton_min_clicked();
    void on_toolButton_max_clicked();
    void on_toolButton_close_clicked();
	void on_toolButton_main_setting_clicked();
	void on_spinBox_list_show_valueChanged(int val);

private:
    void SwitchLanguage();
	void SwitchStartPage();
	void SetStatusBarText(QString msg, QString color, bool help);

private:
	float g = 0.5;
	bool language;
	bool animation;
    Ui::MainWindow* ui;
	QTimer refresh_viewer;
	ATS::StartPage* sp = nullptr;
	bool refresh_module_list = true;
	ModuleMember* current_m = nullptr;
	SettingPage* set_page = nullptr;
	HelpPage* help_page = nullptr;

	QVBoxLayout *module_layout = nullptr;
	QGridLayout *main_layout = nullptr;
	QWidget* current_widget = nullptr;
	QWidget* user_widget = nullptr;
};

#define PRCESS_WAIT(A) std::this_thread::sleep_for(std::chrono::milliseconds(A)), QApplication::processEvents();

#endif // MAINWINDOW_H
