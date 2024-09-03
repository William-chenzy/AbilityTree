#ifndef __CPP_WINDOWS_DEFINE__H__
#define __CPP_WINDOWS_DEFINE__H__

#include <Pdh.h>
#include <tchar.h>
#include <Psapi.h>
#include <WbemCli.h>
#include <winternl.h>
#include <processthreadsapi.h>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <winuser.h>
#include <DXGI.h>
#include <TlHelp32.h>
#include <comdef.h>
#include <shlobj.h>
#include <shobjidl.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DXGI.lib")
typedef unsigned __int64 Ulong;
typedef int socklen_t;
static const bool is_Linux = false;
#define openCom(A)  CreateFileA(A, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)
#define closeCom(A) CloseHandle(A)
#define writeCom(A,B,C,D) WriteFile(A, B, C, D, NULL)
#define readCom(A,B,C,D) ReadFile(A, B, C, D, NULL)

#endif //__CPP_WINDOWS_DEFINE__H__