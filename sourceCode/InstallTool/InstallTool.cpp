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
#include <fstream>
#include <iostream>
#include "miniz.c"
#include <QDebug>
#include <QProcess>
#include <md5.hpp>
#include <thread>
using namespace std;

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
	static QString raw = "#centralwidget{background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #000000, stop:0.7 #474e5e);\
							border-top-left-radius: 10px;border-top-right-radius: 10px;\
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
	bool ret = ret = GetRegistryValue(name);
	if (!QDir(name.mid(0, name.lastIndexOf("/AbilityTree.ifo"))).exists()) {
		QFile file_(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/AbilityTree.ifo");
		if (file_.open(QIODevice::ReadOnly)) name = file_.readAll();
	}
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
	std::string cmd = (is_Linux?"xdg-open ":"start ");
	system((cmd+"http://www.abilitytree.cn/UserLicense").c_str());
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

	package_str = app_str.substr(app_str.size() - 65 - package_size, package_size);
	auto md5 = MD5_32Bit(package_str);
	if (md5 != package_md5.toStdString()) {
		QMessageBox::warning(nullptr, "错误", QString("MD5码校验失败，请重新下载安装包!\r\nraw: % 1, now : % 2").arg(package_md5).arg(md5.c_str()));
		return false;
	}
	SetProcessBar(0.6);
	return true;
}

bool extract_zip_file(std::string* zip_data, const char *output_dir) {
	mz_zip_archive zip_archive = { 0 };
	mz_bool status;
	size_t uncomp_size;
	void *p;
	int i;

	status = mz_zip_reader_init_mem(&zip_archive, zip_data->c_str(), zip_data->size(), 0);
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

	bool result = extract_zip_file(&package_str, path.toStdString().c_str());
	if (!result) QMessageBox::warning(nullptr, "错误", "包解压缩失败!");
	else SetProcessBar(0.9);
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
    auto exe_path = dir_str + "/AbilityTreeViewer.exe";

	if(!is_Linux){
		for (int i = 0; i < 3 && key_res != key_value; i++) {
				key_res = name;
				GetRegistryValue(key_res);
				SetRegistryValue(name, key_value);
				key_res = key_res.mid(0, key_value.size());
				this_thread::sleep_for(chrono::milliseconds(500));
		}
		QString ini_f = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/AbilityTree.ifo";
		fstream _file(ini_f.toLocal8Bit().toStdString(), ios::out | ios::trunc);
		_file << key_value.toLocal8Bit().toStdString();
		_file.close();

		this_thread::sleep_for(chrono::milliseconds(500));
		if (ui->checkBox_ln->isChecked()) {
				QString tar_dir_str;
				if (is_Linux)tar_dir_str = "/usr/local/bin";
				else tar_dir_str = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

				auto tar_path = tar_dir_str + "/AbilityTreeViewer.lnk";
				CreateShortcut(QStringToStdWstring(tar_path), QStringToLPCSTR(exe_path));
		}
	}
	else qInfo()<<"Linux can not create reg and lnk!";

	SetProcessBar(1);
	if (ui->checkBox_auto_start->isChecked()) {
			QProcess* process = new QProcess();
			process->start(exe_path);
	}
	else QMessageBox::information(nullptr, "安装", "安装完成!");
	exit(0);
}
void InstallTool::on_pushButton_updata_clicked() {
	QString install_path = ui->lineEdit_path->text();
	if (!QDir(install_path).exists()) {
		QMessageBox::warning(nullptr, "卸载", "目录不存在，请确认安装目录");
		return;
	}

	ui->toolButton_close->setEnabled(false);
	ui->pushButton_delete->setVisible(false);
	ui->pushButton_updata->setEnabled(false);
	ui->pushButton_updata->setText("正在修复/升级...");

	if (!UnPackage())exit(0);
	if (!UnZip(install_path))exit(0);

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
	QString install_path = ui->lineEdit_path->text();
	if (!QDir(install_path).exists()) {
		QMessageBox::warning(nullptr, "卸载", "目录不存在，请确认安装目录");
		return;
	}

	auto res = QMessageBox::warning(nullptr, "卸载", "确认要卸载程序吗?", QMessageBox::Yes | QMessageBox::No);
	if (res == QMessageBox::No)return;
	auto clear = QMessageBox::warning(nullptr, "卸载", "是否清空个人数据?", QMessageBox::Yes | QMessageBox::No);
	QString mask = clear == QMessageBox::Yes ? "" : "conf";

	ui->toolButton_close->setEnabled(false);
	ui->pushButton_updata->setVisible(false);
	ui->pushButton_delete->setEnabled(false);
	ui->pushButton_delete->setText("正在卸载...");

	RemoveRegistryValue(REG_NAME);
	deleteFilesInDirectory(install_path, mask);

	QString tar_dir_str;
	if (is_Linux)tar_dir_str = "/usr/local/bin";
	else tar_dir_str = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QFile(tar_dir_str + "/AbilityTreeViewer.lnk").remove();

	QMessageBox::information(nullptr, "卸载", "卸载完成!");

	if (is_Linux) {
		install_path += "delete_self.sh";
		QFile scriptFile(install_path);
		if (scriptFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&scriptFile);
			out << "#!/bin/sh\n";
			out << "sleep 1\n";
			out << "rm -rf \"" << install_path << "\"\n";
			scriptFile.close();
			QFile::setPermissions(install_path, QFile::permissions(install_path) | QFile::ExeUser);
		}
	}
	else {
		install_path += "delete_self.bat";
		QFile batFile(install_path);
		if (batFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&batFile);
			out << "@echo off\n";
			out << "timeout /t 1 /nobreak > NUL\n";
			out << "rmdir /s /q \"" << install_path << "\"\n";
			batFile.close();
		}
	}
	QProcess::startDetached(install_path);
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
