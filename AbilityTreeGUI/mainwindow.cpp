#include <QTextCodec>
#include <QApplication>
#include <qdesktopwidget.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QRect deskRect = QApplication::desktop()->availableGeometry();//获取可用桌面大小
    if (deskRect.width() < width() || deskRect.height() < height()) {
        this->setMinimumSize(deskRect.width(), deskRect.height());
        this->setFixedSize(deskRect.width(), deskRect.height());
    }
    this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    qRegisterMetaType<std::string>("std::string");
    SwitchLanguage();
	DrawAllImg();
}

MainWindow::~MainWindow() {

}

void MainWindow::SwitchLanguage() {

}

void MainWindow::BindSlot() {
    //todo:: bind slot in here
}

void MainWindow::on_toolButton_help_clicked() {
    //todo::
}
void MainWindow::on_toolButton_min_clicked() {
    this->showMinimized();
}
void MainWindow::on_toolButton_max_clicked() {
    if (MainWindow::isMaximized()) this->showNormal();
    else this->showMaximized();
    auto img_str = this->isMaximized() ? "full" : "max";
	auto normal = QString("#toolButton_max{border-image:url(./res/%1.png);}").arg(img_str);
    auto hover = QString("#toolButton_max:hover{border-image:url(./res/%1_hover.png);}").arg(img_str);
	ui->toolButton_max->setStyleSheet(normal + hover);
}
void MainWindow::on_toolButton_close_clicked() {
    exit(0);
}



void MainWindow::mousePressEvent(QMouseEvent* event) {

}
void MainWindow::mouseMoveEvent(QMouseEvent* event) {

}
void MainWindow::mouseReleaseEvent(QMouseEvent* event) {

}
void MainWindow::mouseDoubleClickEvent(QMouseEvent* event) {

}
bool MainWindow::eventFilter(QObject* watched, QEvent* event) {

    return true;
}