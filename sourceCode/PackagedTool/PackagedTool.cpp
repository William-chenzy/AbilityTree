#include "PackagedTool.h"
#include "PackagedTool.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QCheckBox>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMessageBox>
#include <QApplication>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <QIODevice>
#include <QFileDialog>
#include <zip.h>
#include <md5.hpp>
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

PackagedTool::PackagedTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::PackagedTool)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	this->setWindowFlags(this->windowFlags() &~Qt::WindowMaximizeButtonHint);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-PackagedTool.ico")));


	QFile _file("./conf/PackagedTool.conf");
	_file.open(QFile::ReadOnly);
	if (_file.isOpen() && _file.size()) {
		QStringList _dir;
		while (!_file.atEnd()) {
			QString str_ = _file.readLine();
			if (str_.size() <= 2)continue;
			_dir.push_back(str_);
		}
		if (_dir.size() > 0)ui->lineEdit_qt_dir->setText(_dir[0]);
		if (_dir.size() > 1)ui->lineEdit_install_dir->setText(_dir[1]);
		if (_dir.size() > 2)ui->lineEdit_package_dir->setText(_dir[2]);
		if (_dir.size() > 3)ui->lineEdit_output_dir->setText(_dir[3]);
		if (_dir.size() > 4)ui->lineEdit_output_name->setText(_dir[4]);
		_file.close();
	}
	else {
		if (is_Linux) ui->lineEdit_qt_dir->setText("/usr/local/bin/linuxdeployqt");
		else ui->lineEdit_qt_dir->setText("C:\\Qt\\Qt5.12.12\\5.12.12\\msvc2017_64\\bin");
	}
}

void PackagedTool::on_toolButton_qt_dir_clicked() {
	auto raw = ui->lineEdit_qt_dir->text();
	if (raw.isEmpty())raw = "./";

	QString fileName = QFileDialog::getOpenFileName(this,"Open File", raw,tr("BAT (*.bat)"));
	if (fileName.isEmpty()) return;
	ui->lineEdit_qt_dir->setText(fileName);
}
void PackagedTool::on_toolButton_install_dir_clicked() {
	auto raw = ui->lineEdit_qt_dir->text();
	if (raw.isEmpty())raw = "./";

	QString fileName = QFileDialog::getOpenFileName(this, "Open File", raw, tr("EXE (*.exe)"));
	if (fileName.isEmpty()) return;
	ui->lineEdit_install_dir->setText(fileName);
}
void PackagedTool::on_toolButton_package_dir_clicked() {
	auto raw = ui->lineEdit_qt_dir->text();
	if (raw.isEmpty())raw = "./";

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", raw, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty()) return;
	ui->lineEdit_package_dir->setText(dir);
	if (ui->lineEdit_output_dir->text().isEmpty())ui->lineEdit_output_dir->setText(dir);
}
void PackagedTool::on_toolButton_output_dir_clicked() {
	auto raw = ui->lineEdit_qt_dir->text();
	if (raw.isEmpty())raw = "./";

	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", raw, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty()) return;
	ui->lineEdit_output_dir->setText(dir);
}

void PackagedTool::CreateScript() {
	QString file_str;
	QStringList filePathList;
	QString pack_cmd = is_Linux ? "linuxdeployqt " : "windeployqt ";
	QString qt_dir = ui->lineEdit_qt_dir->text().remove('\r').remove('\n');
	QString pack_dir = ui->lineEdit_package_dir->text().remove('\r').remove('\n');
	QString file_name = pack_dir + (is_Linux ? "/PackagedTool.sh" : "/PackagedTool.bat");
	QFileInfoList fileList = QDir(pack_dir).entryInfoList(QDir::Files | QDir::NoSymLinks);

	if (is_Linux)file_str += "#!/bin/bash\r\n";
	else file_str += "@echo off\r\nset \"PATH=" + qt_dir + "\; %PATH%\"" + "\r\n";
	for (auto i : fileList)if (i.suffix() == "exe")file_str += pack_cmd + i.filePath() + "\r\n";

	QFile cmd_file(file_name);
	cmd_file.open(QFile::WriteOnly);
	if (cmd_file.isOpen()) {
		cmd_file.write(file_str.toLocal8Bit());
		cmd_file.close();
	}
	else {
		QMessageBox::warning(this, "错误", "无法写入脚本");
		return;
	}

	QString sys_cmd = (is_Linux ? "./" : "") + file_name;
	system(sys_cmd.toStdString().c_str());
	QFile::remove(file_name);
}

void addFileToZip(struct zip_t *zip, const QString &filePath, const QString &entryName) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) return;
	QByteArray fileData = file.readAll();
	file.close();

	if (zip_entry_open(zip, entryName.toUtf8().constData()) != 0) return;
	if (zip_entry_write(zip, fileData.constData(), fileData.size()) != 0) {
		zip_entry_close(zip);
		return;
	}

	zip_entry_close(zip);
}
void addDirectoryToZip(struct zip_t *zip, const QString &dirPath, const QString &basePath) {
	QDir dir(dirPath);
	if (!dir.exists())return;
	QString _basePath = (basePath.isEmpty() ? "" : basePath + "/");

	QFileInfoList dir_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QFileInfo &dir_info : dir_list) 
		addDirectoryToZip(zip, dir_info.absoluteFilePath(), _basePath + dir_info.fileName());

	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	for (const QFileInfo &file_info : file_list) 
		addFileToZip(zip, file_info.absoluteFilePath(), _basePath + file_info.fileName());
}
void PackagedTool::ZipAllFiles() {
	QFile::remove("PackagedTool.zip");
	QString pack_dir = ui->lineEdit_package_dir->text().remove('\r').remove('\n');

	struct zip_t *zip = zip_open("PackagedTool.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	if (!zip) {
		QMessageBox::warning(nullptr, "错误", "创建压缩包失败!");
		return;
	}
	addDirectoryToZip(zip, pack_dir, "");
	zip_close(zip);
}

void PackagedTool::on_pushButton_start_package_clicked() {
	auto res = QMessageBox::information(nullptr, "提示", "确认开始打包吗?", QMessageBox::Ok | QMessageBox::No);
	if (res == QMessageBox::No)return;

	QFile _file("./conf/PackagedTool.conf");
	_file.open(QFile::WriteOnly);
	if (_file.isOpen()) {
		QString str;
		str += ui->lineEdit_qt_dir->text() + "\r\n";
		str += ui->lineEdit_install_dir->text() + "\r\n";
		str += ui->lineEdit_package_dir->text() + "\r\n";
		str += ui->lineEdit_output_dir->text() + "\r\n";
		str += ui->lineEdit_output_name->text() + "\r\n";
		_file.write(str.toLocal8Bit());
		_file.close();
	}
	else QMessageBox::warning(nullptr, "错误", "无法写入配置文件");

	QString output = ui->lineEdit_output_dir->text().remove('\r').remove('\n');
	QString install = ui->lineEdit_install_dir->text().remove('\r').remove('\n');
	QString output_name = ui->lineEdit_output_name->text().remove('\r').remove('\n');
	QFile app_file(install);
	if (!app_file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(nullptr, "错误", "未找到安装程序!");
		return;
	}

	ui->pushButton_start_package->setEnabled(false);
	ui->pushButton_start_package->setText("正在执行脚本");
	QApplication::processEvents();
	CreateScript();
	ui->pushButton_start_package->setText("正在压缩打包");
	QApplication::processEvents();
	ZipAllFiles();

	std::string zip_str;
	std::fstream zip_file("./PackagedTool.zip", ios::in | ios::binary);
	while (!zip_file.eof())zip_str.push_back(zip_file.get());
	zip_str.pop_back();
	zip_file.close();

	char size_char[8];
	auto size = zip_str.size();
	memcpy(size_char, &size, 8);

	QFile::remove(output + "/" + output_name);
	QFile pack_file(output + "/" + output_name);
	pack_file.open(QIODevice::WriteOnly);

	ui->pushButton_start_package->setText("正在计算Md5值");
	QApplication::processEvents();
	auto md5 = MD5_32Bit(zip_str);

	ui->pushButton_start_package->setText("正在制作完整包");
	QApplication::processEvents();

	qint64 bytesRead;
	char buffer[1024];
	while ((bytesRead = app_file.read(buffer, 1024)) > 0)pack_file.write(buffer, bytesRead);
	for (auto i : zip_str)pack_file.write(&i, 1);
	pack_file.write(size_char, 8);
	pack_file.write(md5.c_str(), 32);
	pack_file.write("PackagedTool PACKAGE FLAG");

	pack_file.close();
	zip_file.close();
	app_file.close();
	QMessageBox::information(nullptr, "提示", "打包完成");
	ui->pushButton_start_package->setText("开始打包");
	ui->pushButton_start_package->setEnabled(true);
}