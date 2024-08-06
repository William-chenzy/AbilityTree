#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qtimer>
namespace Ui {
    class ThreadInjection;
}

class ThreadInjection : public QMainWindow
{
	Q_OBJECT

public:
    ThreadInjection(QWidget *parent = 0);
	~ThreadInjection() = default;

protected:

private:

private slots:

private:
    Ui::ThreadInjection* ui;
};


#endif // MAINWINDOW_H
