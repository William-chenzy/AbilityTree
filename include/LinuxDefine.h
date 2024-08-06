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





#endif //__LINUX_DEFINE__H__