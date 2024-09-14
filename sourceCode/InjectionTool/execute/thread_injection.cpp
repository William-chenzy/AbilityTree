#include "execute.h"
#include <fstream>
#include <vector>
#include <string>
#include <thread>
using namespace std;

DWORD __stdcall kill_self(LPVOID data) {
	exit(0);
	return *(DWORD*)data;
}
void kill_self_nullFunc() {}



int thread_injection(DWORD pid, TYPE type) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProcess == NULL)return -1;

	char szBuffer[10];
	*(DWORD*)szBuffer = 1000;
	void* pDataRemote = (char*)VirtualAllocEx(hProcess, 0, sizeof(szBuffer), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pDataRemote, szBuffer, (sizeof(szBuffer)), nullptr);

	void* fun_addr = nullptr, *fun_addr_end = nullptr;
	if (type == KILL) {
		fun_addr = kill_self;
		fun_addr_end = kill_self_nullFunc;
	}

	auto cbCodeSize = ((LPBYTE)kill_self_nullFunc - (LPBYTE)kill_self);
	PDWORD pCodeRemote = (PDWORD)VirtualAllocEx(hProcess, 0, cbCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pCodeRemote, kill_self, cbCodeSize, nullptr);

	DWORD h;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pCodeRemote, pDataRemote, 0, NULL);
	if (!hThread)return -1;
	::WaitForSingleObject(hThread, INFINITE);
	::GetExitCodeThread(hThread, &h);
	::CloseHandle(hThread);
	return 0;
}