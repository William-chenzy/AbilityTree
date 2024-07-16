#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <atomic>
#include "CreateRes.hpp"
#include <qmainwindow.h>

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
    void on_toolButton_help_clicked();
    void on_toolButton_min_clicked();
    void on_toolButton_max_clicked();
    void on_toolButton_close_clicked();

private:
    void BindSlot();
    void SwitchLanguage();

private:
    Ui::MainWindow* ui;

};

#define PRCESS_WAIT(A) std::this_thread::sleep_for(std::chrono::milliseconds(A)), QApplication::processEvents();

#endif // MAINWINDOW_H
