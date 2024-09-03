#include "ui_InjectionTool.h"
#include <InjectionTool.h>
#include <qmessagebox.h>
#include <QFileDialog>
#include <QProcess>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#define DLL_PATH "./InjectionTool_dll.dll"

void InjectionTool::on_pushButton_kill_self_clicked() {
	auto res = QMessageBox::question(nullptr, "提示", QString("确认击杀进程: %1吗?").arg(current_pid));
	if (res != QMessageBox::Yes)return;

	QStringList arguments;
	arguments.push_back(QString::number(current_pid));
	arguments.push_back(QString::number(0));

	this->setCursor(Qt::WaitCursor);
	QProcess process(this);
	process.start("InjectionTool_execute.exe", arguments);
	process.waitForStarted();
	process.waitForFinished();

	this->setCursor(Qt::ArrowCursor);
	QMessageBox::information(nullptr, "提示", "注入成功");
}

void InjectionTool::DllInjectionExecute() {
	if (dll_file.isEmpty()) {
		QMessageBox::warning(nullptr, "错误", "文件名不能为空");
		return;
	}

	auto dll_path = dll_file.toLocal8Bit().toStdString().c_str();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, current_pid);
	if (hProcess == NULL) {
		QMessageBox::warning(nullptr, "错误", "OpenProcess failed");
		return;
	}

	LPVOID StartAddress = VirtualAllocEx(hProcess, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (StartAddress == NULL) {
		QMessageBox::warning(nullptr, "错误", "VirtualAllocEx failed");
		return;
	}

	if (!WriteProcessMemory(hProcess, StartAddress, (BYTE*)dll_path, strlen(dll_path) + 1, nullptr)) {
		QMessageBox::warning(nullptr, "错误", "WriteProcessMemory failed");
		return;
	}

	PTHREAD_START_ROUTINE pfnStartAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	if (!pfnStartAddress) {
		QMessageBox::warning(nullptr, "错误", "GetProcAddress failed");
		return;
	}

	HANDLE hThread = CreateRemoteThreadEx(hProcess, NULL, NULL, pfnStartAddress, StartAddress, NULL, NULL, NULL);
	if (!hThread) {
		QMessageBox::warning(nullptr, "错误", "CreateRemoteThreadEx failed");
		return;
	}

	WaitForSingleObject(hThread, INFINITE);
	::CloseHandle(hThread);
	QMessageBox::information(nullptr, "提示", "注入成功");
}
void InjectionTool::on_pushButton_dll_file_clicked() {
	dll_file = QFileDialog::getOpenFileName(this,"选择","./","DLL(*.dll);");
	ui->label_9->setText("当前DLL: " + dll_file.mid(dll_file.lastIndexOf('/') + 1));
	ui->lineEdit_dll_name->setText(dll_file.mid(dll_file.lastIndexOf('/') + 1));
	ui->label_9->setToolTip(dll_file);
}
void InjectionTool::on_pushButton_dll_injection_clicked() {
	auto res = QMessageBox::question(nullptr, "提示", QString("确认注入DLL到：%1吗?\r\n本操作不可逆!").arg(current_pid));
	if (res != QMessageBox::Yes)return;

	DllInjectionExecute();
}
void InjectionTool::on_pushButton_dll_injection_apc_clicked() {
	auto res = QMessageBox::question(nullptr, "提示", QString("确认注入DLL到：%1吗?\r\n本操作不可逆!").arg(current_pid));
	if (res != QMessageBox::Yes)return;
	if (dll_file.isEmpty())return;

	const char *szDllName = dll_file.toLocal8Bit().toStdString().c_str();
	int nDllLen = lstrlen(szDllName) + sizeof(char);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, current_pid);
	if (hProcess == NULL) return;

	PVOID pDllAddr = VirtualAllocEx(hProcess, NULL, nDllLen, MEM_COMMIT, PAGE_READWRITE);
	if (pDllAddr == NULL){
		CloseHandle(hProcess);
		return;
	}
	SIZE_T dwWriteNum = 0;
	WriteProcessMemory(hProcess, pDllAddr, szDllName, nDllLen, &dwWriteNum);
	CloseHandle(hProcess);

	THREADENTRY32 te = { 0 };
	te.dwSize = sizeof(THREADENTRY32);
	HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (INVALID_HANDLE_VALUE == handleSnap){
		CloseHandle(hProcess);
		return;
	}

	FARPROC pFunAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	DWORD dwRet = 0;
	if (Thread32First(handleSnap, &te)){
		do{
			if (te.th32OwnerProcessID != current_pid)continue;
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
			if (hThread){
				dwRet = QueueUserAPC((PAPCFUNC)pFunAddr, hThread, (ULONG_PTR)pDllAddr);
				CloseHandle(hThread);
			}
		} while (Thread32Next(handleSnap, &te));
	}
	CloseHandle(handleSnap);
	QMessageBox::information(nullptr, "提示", "注入成功");
}
void InjectionTool::on_pushButton_dll_uninjection_clicked() {
	auto res = QMessageBox::question(nullptr, "提示", QString("确认从 %1 卸载DLL吗?\r\n本操作不可逆!\r\n可能导致目标进程出现运行问题.").arg(current_pid));
	if (res != QMessageBox::Yes)return;

	QString dll_name = ui->lineEdit_dll_name->text();
	if (dll_name.isEmpty())return;

	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, current_pid);
	bMore = Module32First(hSnapshot, &me);
	const char* dll_ptr = dll_name.toStdString().c_str();
	for (; !bFound && bMore; bMore = Module32Next(hSnapshot, &me))
		bFound = !_tcsicmp((LPCTSTR)me.szModule, dll_ptr) || !_tcsicmp((LPCTSTR)me.szExePath, dll_ptr);

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, current_pid)))return;

	hModule = GetModuleHandle("kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	if (!hThread)return;
	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);
	QMessageBox::information(nullptr, "提示", "卸载成功");
}

void InjectionTool::on_pushButton_dll_hook_clicked() {
	bool uninstall = ui->pushButton_dll_hook->text() == "解绑";
	if (!uninstall) {
		auto res = QMessageBox::question(nullptr, "提示", QString("确认注入DLL到：%1吗?").arg(current_pid));
		if (res != QMessageBox::Yes)return;
	}

	dll_file = DLL_PATH;
	ui->label_9->setToolTip(dll_file);
	ui->lineEdit_dll_name->setText(dll_file.mid(dll_file.lastIndexOf('/') + 1));
	ui->label_9->setText("当前DLL: " + dll_file.mid(dll_file.lastIndexOf('/') + 1));
	if (!uninstall) {
		hook_map[current_pid].recvb = ui->checkBox_recv->isChecked();
		hook_map[current_pid].sendb = ui->checkBox_send->isChecked();

		int val = 0;
		std::string pid = std::to_string(current_pid);
		if (hook_map[current_pid].recvb)val = val | 0x01;
		if (hook_map[current_pid].sendb)val = val | 0x02;
		WritePrivateProfileString("Main", pid.c_str(), std::to_string(val).c_str(), "./conf/InjectionTool.param");
		DllInjectionExecute();
	}
	else on_pushButton_dll_uninjection_clicked();

	ui->checkBox_recv->setEnabled(uninstall);
	ui->checkBox_send->setEnabled(uninstall);
	hook_map[current_pid].install = !uninstall;
	ui->pushButton_dll_hook->setText(uninstall ? "注入原生DLL" : "解绑");
}


QProcess* InjectionTool::HookInjection(QPushButton* but, QProcess* proc, int* val, int num) {
	if (proc) {
		(*val)--;
		proc->kill();
		proc->deleteLater();
		but->setText("注入");
		QMessageBox::information(nullptr, "提示", "成功");
		return nullptr;
	}

	auto res = QMessageBox::question(nullptr, "提示", QString("确认拦截进程: %1 的消息吗?").arg(current_pid));
	if (res != QMessageBox::Yes)return nullptr;

	QStringList arguments;
	arguments.push_back(QString::number(current_pid));
	arguments.push_back(QString::number(num));

	this->setCursor(Qt::WaitCursor);
	QProcess* process = new QProcess(this);
	process->start("InjectionTool_execute.exe", arguments);
	process->waitForStarted();

	(*val)++;
	but->setText("取消拦截");
	this->setCursor(Qt::ArrowCursor);
	QMessageBox::information(nullptr, "提示", "注入成功");
	return process;
}
void InjectionTool::on_pushButton_key_hook_clicked() {
	auto process = HookInjection(ui->pushButton_key_hook, hook_map[current_pid].key, &hook_map[current_pid].count, 2);
	hook_map[current_pid].key = process;
}
void InjectionTool::on_pushButton_mouse_hook_clicked() {
	auto process = HookInjection(ui->pushButton_mouse_hook, hook_map[current_pid].mouse, &hook_map[current_pid].count, 1);
	hook_map[current_pid].mouse = process;
}
void InjectionTool::on_pushButton_debug_hook_clicked() {
	auto process = HookInjection(ui->pushButton_debug_hook, hook_map[current_pid].debug, &hook_map[current_pid].count, 3);
	hook_map[current_pid].debug = process;
}
void InjectionTool::on_pushButton_dialog_hook_clicked() {
	auto process = HookInjection(ui->pushButton_dialog_hook, hook_map[current_pid].dialog, &hook_map[current_pid].count, 4);
	hook_map[current_pid].dialog = process;
}
