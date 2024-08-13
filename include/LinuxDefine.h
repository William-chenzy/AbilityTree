#ifndef __LINUX_DEFINE__H__
#define __LINUX_DEFINE__H__

#include <QFile>
#include <QTextStream>
#include "CppLinuxDefine.h"

static QString QStringToLPCSTR(const QString& qstr) {
	return qstr;
}
static QString QStringToStdWstring(const QString& qstr) {
	return qstr;
}

static bool GetRegistryValue(QString& key_val) {
	QString key = "/usr/local/etc/" + key_val;
	QFile _file(key);
	if (_file.open(QFile::ReadOnly)) {
		key_val = _file.readAll();
		_file.close();
	}
	else key_val = "";
	return !key_val.isEmpty();
}
static bool SetRegistryValue(QString& key_val, QString val) {
	QString key = "/usr/local/etc/" + key_val;
	QFile _file(key);
	if (_file.open(QFile::ReadOnly)) {
		_file.write(val.toLocal8Bit());
		_file.close();
	}
	else return false;
	return true;
}
static bool RemoveRegistryValue(QString key_val) {
	QString key = "/usr/local/etc/" + key_val;
	QFile(key).remove();
	return true;
}

static bool CreateShortcut(QString targetPath, QString targetName) {
	QString cmd = QString("sudo ln -s %1 /usr/local/bin/%2").arg(targetPath).arg(targetName);
	system(cmd.toStdString().c_str());
}

#endif //__LINUX_DEFINE__H__
