#include "execute.h"
#include "GlobalDefine.h"
using namespace std;

DWORD __stdcall MyFunction(LPVOID data) {
	exit(0);								//退出当前被注册线程
	return *(DWORD*)data;
}
void afterFunc() {

}

bool EnableDebugPriv()			//提权函数
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return false;

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))return false;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))return false;

	return true;
}

DWORD GetProcessID(LPCWSTR lpName)				//通过进程名获取ID
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		_bstr_t b(pe.szExeFile);
		cout << (char*)b << endl;
		//CloseHandle(hSnapshot);
		//return pe.th32ProcessID;
	}
	return NULL;
}

int main()
{
	if (!EnableDebugPriv()) {
		cout << "提权失败!" << endl;
		return -1;
	}
	else cout << "提权成功!" << endl;

	GetProcessID(TEXT("123"));

	DWORD PID = 51340;//输入线程PID来注入线程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (hProcess == NULL)cout << "open process error: " << GetLastError() << endl;

	char szBuffer[10];
	*(DWORD*)szBuffer = 1000;//for test
	void* pDataRemote = (char*)VirtualAllocEx(hProcess, 0, sizeof(szBuffer), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pDataRemote, szBuffer, (sizeof(szBuffer)), nullptr);

	DWORD cbCodeSize = ((LPBYTE)afterFunc - (LPBYTE)MyFunction);
	PDWORD pCodeRemote = (PDWORD)VirtualAllocEx(hProcess, 0, cbCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pCodeRemote, MyFunction, cbCodeSize, nullptr);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pCodeRemote, pDataRemote, 0, NULL);
	DWORD h;
	if (hThread) {
		::WaitForSingleObject(hThread, INFINITE);
		::GetExitCodeThread(hThread, &h);
		::CloseHandle(hThread);
		cout << "Done!" << endl;
	}
	else cout << "error: " << GetLastError() << endl;

	system("PAUSE");
}