#include "InstallTool.h"
#include <QDesktopWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QApplication>
#include <QTextCodec>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QFileDialog>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include "miniz.c"
#include <QProcess>
#include <md5.hpp>
#include <thread>
using namespace std;

#define PACKAGE_NAME "AbilityTreePackage"
#define REG_NAME "AbilityTree"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

InstallTool::InstallTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::InstallTool)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-InstallTool.ico")));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	connect(&animation, &QTimer::timeout, this, &InstallTool::CreateAnimation);
	animation.start(25);

	CheckInstallStatus();
	set = cur = 0;
}

void InstallTool::CreateAnimation() {
	static QString raw = "#centralwidget{background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #000000, stop:0.7 #474e5e);border-radius: 10px;\
							border-bottom:2px solid qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc);}";
	static float val = 0.5;
	QString val_str = QString::number(abs(val += 0.01) , 'f', 2);
	if (val >= 0.99)val = -0.99;
	if (abs(val) < 0.1)val = 0.1;
	auto style = QString(raw).arg(val_str);
	ui->centralwidget->setStyleSheet(style);

	if (abs(cur - set) > 0.01) {
		QString val_f = QString::number(cur, 'f', 2);
		QString color = QString("qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 #42f875, stop:%1 rgba(200,200,200,120));").arg(val_f);
		QString base = QString("#widget_2{border-radius: 3px;border:1px solid #dddddd;background-color:%1;}").arg(color);
		ui->widget_2->setStyleSheet(base);
		cur = set > cur ? cur + 0.01 : cur - 0.01;
	}
	QApplication::processEvents();
}

void InstallTool::CheckInstallStatus() {
	QString name = REG_NAME;
	bool ret = GetRegistryValue(name);
	name = name.mid(0, name.lastIndexOf("/AbilityTree.ifo"));

	ui->widget_3->setVisible(!ret);
	ui->pushButton_start->setVisible(!ret);
	ui->pushButton_updata->setVisible(ret);
	ui->pushButton_delete->setVisible(ret);
	if (!ret) {
		QString doc_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		ui->lineEdit_path->setText(doc_dir + "/AbilityTree");
	}
	else ui->lineEdit_path->setText(name);
}

void InstallTool::on_toolButton_path_clicked() {
	auto raw = ui->lineEdit_path->text();
	if (raw.isEmpty())raw = "./";

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", raw, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty()) return;
	ui->lineEdit_path->setText(dir);
}

void InstallTool::on_pushButton_user_rule_clicked() {
	int res = QMessageBox::information(nullptr, "软件使用许可", "是否弹窗显示?", QMessageBox::Yes | QMessageBox::No);

	static QString rule = "AbilityTree用户许可协议\r\n\
重要：请在使用本软件之前仔细阅读本用户许可协议（“协议”）。安装或使用本软件即表示您同意接受本协议的条款。如果您不同意本协议的条款，请不要安装或使用本软件。\r\n\
1. 授权\r\n\
根据本协议，林煜涵（以下简称“授权方”）授予您一个非独占有限许可，以在个人电脑或设备上安装和使用本软件以及该软件所附带的所有免费软件（以下简称“软件”）。本许可仅限于个人、非商业用途。\r\n\
2. 使用限制\r\n\
您不得：\r\n\
商业用途：将软件用于任何商业目的，除非事先获得明确的书面许可。\r\n\
非法使用：将软件用于任何非法活动或违反适用法律的活动。\r\n\
3. 知识产权\r\n\
软件及其所有相关版权、商标、专利和其他知识产权归授权方及其许可方所有。本协议仅授予您有限的使用许可，不转让任何知识产权或所有权。\r\n\
4. 免责声明\r\n\
软件按“现状”提供，不提供任何明示或暗示的保证，包括但不限于对软件适销性、特定用途适用性或不侵权的暗示保证。在法律允许的最大范围内，授权方不对因使用或无法使用软件而导致的任何损害、损失或其他责任负责。\r\n\
5. 责任限制\r\n\
在法律允许的最大范围内，授权方不对因使用软件而导致的任何直接、间接、附带、特殊或后果性损害负责，包括但不限于数据丢失或利润损失，即使授权方已被告知可能发生此类损害。\r\n\
6. 终止\r\n\
如果您违反本协议的任何条款，授权方可以立即终止本协议，并要求您停止使用软件并销毁所有软件副本。终止本协议不影响授权方在法律允许的最大范围内的权利。\r\n\
7. 适用法律\r\n\
本协议应受中国法律的管辖，并按照该法律解释。任何因本协议引起的争议应提交至中国管辖法院。\r\n\
8. 完整协议\r\n\
本协议构成您与授权方之间关于软件的完整协议，取代所有之前的口头或书面协议和沟通。任何对本协议的修改或补充应以书面形式进行，并由双方签署。\r\n\
9. 联系信息\r\n\
如有任何关于本协议的问题或需要联系授权方，请通过以下方式联系我们：\r\n\
名称：林煜涵\r\n\
电子邮件：862167084@qq.com\r\n\
安装和使用本软件即表示您同意本协议的所有条款。";

	system("start http://121.40.55.218:8080/UserLicense.html");
	if (res == QMessageBox::Yes)QMessageBox::information(nullptr, "软件使用许可", rule);
}

void InstallTool::SetProcessBar(float val) {
	set = val;
	while (abs(set - cur) > 0.01) QApplication::processEvents();
}

bool InstallTool::UnPackage() {
	auto path = QCoreApplication::applicationFilePath();
	std::fstream app_file(path.toLocal8Bit().toStdString(), ios::in | ios::binary);

	SetProcessBar(0.1);
	std::string app_str;
	while (!app_file.eof())app_str.push_back(app_file.get());
	app_str.pop_back();
	app_file.close();
	SetProcessBar(0.2);

	QString flag = app_str.substr(app_str.size() - 25).c_str();
	if (flag != "PackagedTool PACKAGE FLAG") {
		QMessageBox::warning(nullptr, "错误", "未找到标志位，请重新下载安装包!");
		return false;
	}

	QString package_md5 = app_str.substr(app_str.size() - 57, 32).c_str();

	SetProcessBar(0.3);
	unsigned long long package_size = 0;
	auto size = app_str.substr(app_str.size() - 65, 8);
	memcpy(&package_size, size.data(), 8);
	if (package_size >= app_str.size()) {
		QMessageBox::warning(nullptr, "错误", "安装包大小异常，请重新下载安装包!");
		return false;
	}

	SetProcessBar(0.4);

	auto package_str = app_str.substr(app_str.size() - 65 - package_size, package_size);
	auto md5 = MD5_32Bit(package_str);
	if (md5 != package_md5.toStdString()) {
		QMessageBox::warning(nullptr, "错误", QString("MD5码校验失败，请重新下载安装包!\r\nraw: % 1, now : % 2").arg(package_md5).arg(md5.c_str()));
		return false;
	}
	SetProcessBar(0.5);

	QFile package_file(PACKAGE_NAME);
	package_file.open(QFile::WriteOnly);
	for (auto i : package_str)package_file.write(&i, 1);
	package_file.close();
	SetProcessBar(0.6);
	return true;
}

bool extract_zip_file(const char *zip_filename, const char *output_dir) {
	mz_zip_archive zip_archive = { 0 };
	mz_bool status;
	size_t uncomp_size;
	void *p;
	int i;

	status = mz_zip_reader_init_file(&zip_archive, zip_filename, 0);
	if (!status)return false;

	int file_count = mz_zip_reader_get_num_files(&zip_archive);
	for (i = 0; i < file_count; i++) {
		mz_zip_archive_file_stat file_stat;

		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
			mz_zip_reader_end(&zip_archive);
			return false;
		}

		char file_path[1024];
		snprintf(file_path, sizeof(file_path), "%s/%s", output_dir, file_stat.m_filename);

		if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) QDir().mkdir(file_path);
		else {
			p = mz_zip_reader_extract_file_to_heap(&zip_archive, file_stat.m_filename, &uncomp_size, 0);
			if (!p) {
				mz_zip_reader_end(&zip_archive);
				return false;
			}

			char dir_path[1024];
			snprintf(dir_path, sizeof(dir_path), "%s/%s", output_dir, file_stat.m_filename);
			char *last_slash = strrchr(dir_path, '/');
			if (last_slash) {
				*last_slash = '\0';
				QDir().mkdir(dir_path);
			}

			FILE *output_file = fopen(file_path, "wb");
			if (!output_file) {
				mz_free(p);
				mz_zip_reader_end(&zip_archive);
				return false;
			}

			fwrite(p, 1, uncomp_size, output_file);
			fclose(output_file);

			mz_free(p);
		}
	}
	mz_zip_reader_end(&zip_archive);
	return true;
}
bool InstallTool::UnZip(QString path) {
	const char *dir = path.toLocal8Bit().toStdString().c_str();

	bool result = extract_zip_file(PACKAGE_NAME, path.toStdString().c_str());
	if (!result) QMessageBox::warning(nullptr, "错误", "包解压缩失败!");
	else SetProcessBar(0.9);

	QFile(PACKAGE_NAME).remove();
	return result;
}
void InstallTool::on_pushButton_start_clicked() {
	if (!ui->checkBox_user_rule->isChecked()) {
		QMessageBox::information(nullptr, "安装", "请先同意AbilityTree用户许可协议!");
		return;
	}

	ui->toolButton_close->setEnabled(false);
	ui->pushButton_start->setEnabled(false);
	ui->pushButton_start->setText("正在安装...");

	QString dir_str = ui->lineEdit_path->text();
	if (!QDir(dir_str).exists()) QDir().mkpath(dir_str);

	if (!UnPackage())exit(0);
	if (!UnZip(dir_str))exit(0);

	QFile file(QCoreApplication::applicationFilePath());
	file.copy(dir_str + "/uninstall.exe");

	QString name = REG_NAME, key_res;
	QString key_value = dir_str + "/AbilityTree.ifo";
	while (key_res != key_value) {
		key_res = name;
		GetRegistryValue(key_res);
		SetRegistryValue(name, key_value);
		key_res = key_res.mid(0, key_value.size());
		this_thread::sleep_for(chrono::milliseconds(500));
	}

	this_thread::sleep_for(chrono::milliseconds(500));
	auto exe_path = dir_str + "/AbilityTreeViewer.exe";
	if (ui->checkBox_auto_start->isChecked()) {
		QProcess* process = new QProcess();
		process->start(exe_path);
	}
	if (ui->checkBox_ln->isChecked()) {
		QString tar_dir_str;
		if (is_Linux)tar_dir_str = "/usr/local/bin";
		else tar_dir_str = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

		auto tar_path = tar_dir_str + "/AbilityTreeViewer.lnk";
		CreateShortcut(QStringToStdWstring(tar_path), QStringToLPCSTR(exe_path));
	}

	SetProcessBar(1);
	QMessageBox::information(nullptr, "安装", "安装完成!");
	exit(0);
}
void InstallTool::on_pushButton_updata_clicked() {
	ui->toolButton_close->setEnabled(false);
	ui->pushButton_delete->setVisible(false);
	ui->pushButton_updata->setEnabled(false);
	ui->pushButton_updata->setText("正在修复/升级...");

	QString dir_str = ui->lineEdit_path->text();
	if (!QDir(dir_str).exists()) QDir().mkpath(dir_str);

	if (!UnPackage())exit(0);
	if (!UnZip(dir_str))exit(0);

	SetProcessBar(1);
	QMessageBox::information(nullptr, "修复/升级", "修复/升级 完成!");
	exit(0);
}

bool deleteFilesInDirectory(const QString &dirPath, QString dir_mask) {
	QDir dir(dirPath);
	if (!dir.exists()) return false;

	QStringList entries = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
	foreach(const QString &entry, entries) {
		QString filePath = dir.filePath(entry);
		if (!QFile::remove(filePath)) return false;
	}

	QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(const QString &subDir, subDirs) {
		if (subDir == dir_mask)continue;
		QString subDirPath = dir.filePath(subDir);
		if (!deleteFilesInDirectory(subDirPath, dir_mask)) return false;
		if (!QDir(subDirPath).rmdir(subDirPath)) return false;
	}
	return true;
}
void InstallTool::on_pushButton_delete_clicked() {
	auto res = QMessageBox::warning(nullptr, "卸载", "确认要卸载程序吗?", QMessageBox::Yes | QMessageBox::No);
	if (res == QMessageBox::No)return;
	auto clear = QMessageBox::warning(nullptr, "卸载", "是否清空个人数据?", QMessageBox::Yes | QMessageBox::No);
	QString mask = clear == QMessageBox::Yes ? "" : "conf";

	ui->toolButton_close->setEnabled(false);
	ui->pushButton_updata->setVisible(false);
	ui->pushButton_delete->setEnabled(false);
	ui->pushButton_delete->setText("正在卸载...");

	RemoveRegistryValue(REG_NAME);
	deleteFilesInDirectory(ui->lineEdit_path->text(), mask);

	QString tar_dir_str;
	if (is_Linux)tar_dir_str = "/usr/local/bin";
	else tar_dir_str = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QFile(tar_dir_str + "/AbilityTreeViewer.lnk").remove();

	QMessageBox::information(nullptr, "卸载", "卸载完成!");

	QString scriptPath = ui->lineEdit_path->text();
	if (is_Linux) {
		scriptPath += "delete_self.sh";
		QFile scriptFile(scriptPath);
		if (scriptFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&scriptFile);
			out << "#!/bin/sh\n";
			out << "sleep 1\n";
			out << "rm -rf \"" << ui->lineEdit_path->text() << "\"\n";
			scriptFile.close();
			QFile::setPermissions(scriptPath, QFile::permissions(scriptPath) | QFile::ExeUser);
		}
	}
	else {
		scriptPath += "delete_self.bat";
		QFile batFile(scriptPath);
		if (batFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&batFile);
			out << "@echo off\n";
			out << "timeout /t 1 /nobreak > NUL\n";
			out << "rmdir /s /q \"" << ui->lineEdit_path->text() << "\"\n";
			batFile.close();
		}
	}
	QProcess::startDetached(scriptPath);
	exit(0);
}

void InstallTool::on_toolButton_min_clicked() {
	this->showMinimized();
}
void InstallTool::on_toolButton_close_clicked() {
	auto res = QMessageBox::information(nullptr, "安装", "确认要退出安装程序吗?", QMessageBox::Yes | QMessageBox::No);
	if (res == QMessageBox::No)return;
	exit(0);
}


QPoint mouse_pos;
bool is_move_mouse;
void InstallTool::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		QRect rect_top = ui->centralwidget->rect();
		rect_top.translate(ui->centralwidget->pos());

		if (rect_top.contains(event->pos())) {
			is_move_mouse = true;
			mouse_pos = event->screenPos().toPoint();
		}
	}

	QWidget::mousePressEvent(event);
}

void InstallTool::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}

void InstallTool::mouseMoveEvent(QMouseEvent* event) {
	if (!is_move_mouse)return QWidget::mouseMoveEvent(event);
	move(this->pos() + (event->screenPos().toPoint() - mouse_pos));
	mouse_pos = event->screenPos().toPoint();
	QWidget::mouseMoveEvent(event);
}