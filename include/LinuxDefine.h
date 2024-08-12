#ifndef __LINUX_DEFINE__H__
#define __LINUX_DEFINE__H__


static const bool is_Linux = true;
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/can/raw.h>
#include <linux/can.h>
#include <errno.h>
#include <net/if.h>
#include <dlfcn.h>
#define closesocket(a) ::close(a)
#define openCom(A) ::open(A, O_RDWR)
#define closeCom(A) ::close(A)
#define writeCom(A,B,C,D) ::write(A, B, C)
#define readCom(A,B,C,D) ::read(A, B, C)
typedef int HANDLE;
typedef unsigned long DWORD;
typedef unsigned long long Ulong;
#define INVALID_HANDLE_VALUE -1
#define localtime_s(A,B) {A = localtime(B);}
#pragma comment(lib, "Inferencer.lib")
#define GetCurrentDirectory(A,B)
#define ShellExecute(a,b,c,d,e,f)
#define SW_SHOW 1
#define WinExec(a, b) system(a)
#define SetWindowPos(a, b, c, d, e, f, g) std::cout<<"²»Ö§³Ö"<<std::endl


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
		QString = _file.readAll();
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

bool CreateShortcut(QString targetPath, QString targetName) {
	QString cmd = QString("sudo ln -s %1 /usr/local/bin/%2").arg(targetPath).arg(targetName);
	system(cmd);
}

#endif //__LINUX_DEFINE__H__