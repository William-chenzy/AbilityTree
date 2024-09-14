#include "execute.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <map>
using namespace std;
DWORD current;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam){
	KBDLLHOOKSTRUCT* ks = (KBDLLHOOKSTRUCT*)lParam;
	DWORD code = ks->vkCode;
	DWORD processId = 0;
	GetWindowThreadProcessId(GetForegroundWindow(), &processId);

	//if (wParam == WM_KEYDOWN);
	//else if (wParam == WM_KEYUP);

	if (processId == current) return -1;
	else return CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam){
	DWORD processId = 0;
	MOUSEHOOKSTRUCT* ms = (MOUSEHOOKSTRUCT*)lParam;
	GetWindowThreadProcessId(WindowFromPoint(ms->pt), &processId);

	//if (wParam == WM_LBUTTONDOWN);
	//else if (wParam == WM_LBUTTONUP);
	//else if (wParam == WM_RBUTTONDOWN);
	//else if (wParam == WM_RBUTTONUP);
	//else if (wParam == WM_MOUSEMOVE);

	if (processId == current) return -1;
	else return CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK DebugProc(int nCode, WPARAM wParam, LPARAM lParam) {
	MOUSEHOOKSTRUCT* ms = (MOUSEHOOKSTRUCT*)lParam;
	DWORD threadId = GetCurrentThreadId();

	DWORD processId = 0;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	if (hProcess) {
		processId = GetProcessId(hProcess);
		CloseHandle(hProcess);
	}

	if (processId == current) return -1;
	else return CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK DialogProc(int nCode, WPARAM wParam, LPARAM lParam) {
	MOUSEHOOKSTRUCT* ms = (MOUSEHOOKSTRUCT*)lParam;
	DWORD threadId = GetCurrentThreadId();

	DWORD processId = 0;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	if (hProcess) {
		processId = GetProcessId(hProcess);
		CloseHandle(hProcess);
	}
	MSG* pMsg = (MSG*)lParam;
	LONG_PTR style = GetWindowLongPtr(pMsg->hwnd, GWL_STYLE);
	bool is_dialog = (style & WS_POPUP) || (style & WS_DLGFRAME);

	MessageBox(NULL, std::to_string(processId).c_str(), std::to_string(processId).c_str(), 0);

	if (is_dialog && processId == current) return -1;
	else return CallNextHookEx(NULL, nCode, wParam, lParam);
}
LRESULT CALLBACK ALLProc(int nCode, WPARAM wParam, LPARAM lParam) {
	MOUSEHOOKSTRUCT* ms = (MOUSEHOOKSTRUCT*)lParam;
	DWORD threadId = GetCurrentThreadId();

	DWORD processId = 0;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	if (hProcess) {
		processId = GetProcessId(hProcess);
		CloseHandle(hProcess);
	}

	MessageBox(NULL, std::to_string(processId).c_str(), std::to_string(processId).c_str(), 0);

	if (processId == current) return -1;
	else return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Hook(DWORD pid, TYPE type) {
	MSG msg;
	current = pid;
	HHOOK g_Hook = NULL;
	HINSTANCE hM = GetModuleHandle(NULL);

	switch (type) {
		case MOUSE:g_Hook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hM, 0); break;
		case KEY:g_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hM, 0); break;
		case DEBUG:g_Hook = SetWindowsHookEx(WH_DEBUG, DebugProc, hM, 0); break;
		case DIALOG:g_Hook = SetWindowsHookEx(WH_MSGFILTER, DialogProc, hM, 0); break;
		case ALL:g_Hook = SetWindowsHookEx(WH_MSGFILTER, ALLProc, hM, 0); break;
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(g_Hook);
}


void HookThread(DWORD pid, TYPE type) {
	std::thread* t = new thread(Hook, pid, type);
	t->detach();
	while (1)this_thread::sleep_for(chrono::milliseconds(100));
}