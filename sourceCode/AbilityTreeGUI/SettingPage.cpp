#include <QTextCodec>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <qdesktopwidget.h>
#include "SettingPage.h"
#include "ui_SettingPage.h"
#include "GlobalDefine.h"
#include "define.hpp"
#include <QIcon>
#include <thread>
#include <QDebug>
using namespace std;

SettingPage::SettingPage(Method _method, QWidget* parent) :ui(new Ui::SettingPage) {
	ui->setupUi(this);
	this->setWindowIcon(QIcon(":/img/res/LOGO-AT.ico"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);

	ui->widget_language_set->setVisible(false);
	ui->widget_main_setting->setVisible(false);
	ui->widget_module_stting->setVisible(false);

	if (_method == Main_Set)ui->widget_main_setting->setVisible(true);
	if (_method == Module_Set)ui->widget_module_stting->setVisible(true);
}
SettingPage::~SettingPage() { delete ui; }
void SettingPage::closeEvent(QCloseEvent *event) {
	this->hide();
}

void SettingPage::SetModuleParam(MainConfig val){
	ui->radioButton_en->setChecked(val.language);
	ui->radioButton_ch->setChecked(!val.language);
	ui->radioButton_ani_low->setChecked(!val.animation);
	ui->radioButton_ani_height->setChecked(val.animation);
}
void SettingPage::on_pushButton_main_confirm_clicked() {
	MainConfig cfg;
	cfg.language = ui->radioButton_en->isChecked();
	cfg.animation = ui->radioButton_ani_height->isChecked();
	emit MainConfigChange(cfg);
	this->hide();
}

void SettingPage::on_pushButton_module_confirm_clicked() {
	auto name = ui->lineEdit_module_name->text();
	auto describe = ui->textEdit_mdule_describe->toPlainText();
	ModuleModify(name, describe, false);
}
void SettingPage::on_pushButton_module_delete_clicked() {
	ModuleModify("", "", true);
}
void SettingPage::SetModuleParam(QString name, QString describe) {
	ui->lineEdit_module_name->setText(name);
	ui->textEdit_mdule_describe->setText(describe);
}
