#include "ui_InjectionTool.h"
#include "InjectionTool.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <iostream>
#include <qDebug>
#include <QMessageBox>
#include <fstream>
#include <thread>
#include <deque>
#include <fstream>
#include <QPushButton>
#include <QSlider>
#include <QFileDialog>
#include <QApplication>
#include <qscrollbar.h>
#include <QFileIconProvider>
using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

std::map<int64_t, ProcessInfo>process;
InjectionTool::InjectionTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::InjectionTool)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	if (deskRect.width() < width() || deskRect.height() < height()) {
		this->setBaseSize(deskRect.width(), deskRect.height());
		this->move(0, 0);
	}
	else this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-InjectionTool.png")));
	this->setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

	ui->scrollArea_info->setVisible(false);
	ui->tabWidget_process_info->removeTab(1);
	ui->tabWidget_process_info->removeTab(0);
	ui->pushButton_find->installEventFilter(this);
	ui->treeWidget_all_process->setColumnWidth(0, 150);
	ui->treeWidget_all_process->setColumnWidth(1, 48);
	ui->treeWidget_all_process->setColumnWidth(2, 88);
	ui->treeWidget_all_process->setColumnWidth(3, 88);
	ui->treeWidget_all_process->setColumnWidth(4, 60);
	ui->treeWidget_all_process->setColumnWidth(5, 240);
	ui->treeWidget_all_process->setUniformRowHeights(true);
	ui->treeWidget_all_process->header()->setSectionsClickable(true);

	std::fstream _file("./conf/InjectionTool.param", std::ios::out | ios::trunc);
	_file.close();
	
	refresh.setSingleShot(true);
	on_pushButton_tree_clicked();
	on_pushButton_refresh_clicked();
	refresh.start(ui->spinBox_timeout->value());
	connect(&refresh, &QTimer::timeout, this, &InjectionTool::on_pushButton_refresh_clicked);
	connect(ui->treeWidget_all_process->header(), &QHeaderView::sectionClicked, this, &InjectionTool::TreeWidgetHeaderClicked);
}
InjectionTool::~InjectionTool() {
	for (auto&i : hook_map) {
		if (!i.second.count)continue;
		if (i.second.key)i.second.key->kill();
		if (i.second.mouse)i.second.mouse->kill();
	}
}

uint64_t convert_time_format(const FILETIME* ftime){
	LARGE_INTEGER li;
	li.LowPart = ftime->dwLowDateTime;
	li.HighPart = ftime->dwHighDateTime;
	return li.QuadPart;
}
void GetCpuUsageRatio(HANDLE processHandle, DWORD pid){
	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	int cpu_num = info.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&now);
	if (!GetProcessTimes(processHandle, &creation_time, &exit_time, &kernel_time, &user_time))return;
	int64_t system_time = (convert_time_format(&kernel_time) + convert_time_format(&user_time)) / cpu_num;
	int64_t time = convert_time_format(&now);

	if (process[pid].lastSystemTime && process[pid].lastTime) {
		int64_t system_time_delta = system_time - process[pid].lastSystemTime;
		int64_t time_delta = time - process[pid].lastTime;
		if (time_delta > 0) {
			float cpu_ratio = (system_time_delta * 100 + time_delta / 2.f) * 1.f / time_delta * 1.f;
			process[pid].cpu = cpu_ratio;
		}
	}
	process[pid].lastSystemTime = system_time;
	process[pid].lastTime = time;
}

bool GetMemoryUsage(HANDLE procesHandle, uint64_t& mem, uint64_t& vmem){
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(procesHandle, &pmc, sizeof(pmc))){
		mem = pmc.WorkingSetSize;
		vmem = pmc.PagefileUsage;
		return true;
	}
	return false;
}

bool GetIoBytes(HANDLE procesHandle, uint64_t& read_bytes, uint64_t& write_bytes){
	IO_COUNTERS io_counter;
	if (GetProcessIoCounters(procesHandle, &io_counter)){
		read_bytes = io_counter.ReadTransferCount;
		write_bytes = io_counter.WriteTransferCount;
		return true;
	}
	return false;
}

void GetProcessCmdline(HANDLE procesHandle, std::string& cmdline){
	PROCESS_BASIC_INFORMATION pbi = { 0 };
	PEB                       peb;
	SIZE_T                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	RTL_USER_PROCESS_PARAMETERS para;

	qInfo() << "NtQueryInformationProcess";
	if (0 != NtQueryInformationProcess(procesHandle, ProcessBasicInformation, (PVOID)&pbi, sizeof(pbi), NULL))return;
	if (pbi.PebBaseAddress == nullptr) return;
	qInfo() << "ReadProcessMemory";
	if (FALSE == ReadProcessMemory(procesHandle, pbi.PebBaseAddress, &peb, sizeof(peb), &dwDummy))return;
	qInfo() << "ReadProcessMemory";
	if (FALSE == ReadProcessMemory(procesHandle, peb.ProcessParameters, &para, sizeof(para), &dwDummy))return;
	qInfo() << "ReadProcessMemory -1";

	lpAddress = para.CommandLine.Buffer;
	dwSize = para.CommandLine.Length;
	qInfo() << "dwSize: " << dwSize;

	TCHAR* cmdLineBuffer = new TCHAR[dwSize + 1];
	ZeroMemory(cmdLineBuffer, (dwSize + 1) * sizeof(WCHAR));
	qInfo() << "ReadProcessMemory 0" ;
	if (ReadProcessMemory(procesHandle, lpAddress, (LPVOID)cmdLineBuffer, dwSize, &dwDummy))cmdline = cmdLineBuffer;
	qInfo() << "ReadProcessMemory 1";
	delete[] cmdLineBuffer;
	qInfo() << "ReadProcessMemory 2";
}

bool GetAllNetIoBytes(HANDLE procesHandle, uint64_t& sent_bytes, uint64_t& received_bytes, const std::wstring& wszProcessName){
	HQUERY  hQuery = nullptr;
	HCOUNTER hcReceived = nullptr, hcSent = nullptr;
	PDH_FMT_COUNTERVALUE cv;

	PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &hQuery);

	if (lStatus != ERROR_SUCCESS)return false;

	PdhAddCounter(hQuery, "\\Network Interface(*)\\Bytes Sent/sec", NULL, &hcSent);
	PdhAddCounter(hQuery, "\\Network Interface(*)\\Bytes Received/sec", NULL, &hcReceived);
	//PdhAddCounter(hQuery, L"\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Write", NULL, &hcWrite);
	//PdhAddCounter(hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &hcWrite);
	lStatus = PdhCollectQueryData(hQuery);
	if (lStatus != ERROR_SUCCESS)return false;
	Sleep(1000);
	lStatus = PdhCollectQueryData(hQuery);
	if (lStatus != ERROR_SUCCESS)return false;

	lStatus = PdhGetFormattedCounterValue(hcSent, PDH_FMT_LONG, NULL, &cv);
	if (lStatus == ERROR_SUCCESS)sent_bytes = cv.longValue;

	lStatus = PdhGetFormattedCounterValue(hcReceived, PDH_FMT_LONG, NULL, &cv);
	if (lStatus == ERROR_SUCCESS)received_bytes = cv.longValue;

	PdhRemoveCounter(hcSent);
	PdhRemoveCounter(hcReceived);
	PdhCloseQuery(hQuery);
	return true;
}

bool GetDiskStatus(const wchar_t* path, uint64_t& total, uint64_t& free, uint64_t& usage){
	ULARGE_INTEGER utotal;
	ULARGE_INTEGER ufree;
	ULARGE_INTEGER uavailable;
	if (FALSE == GetDiskFreeSpaceExW(path, &uavailable, &utotal, &ufree))return false;
	free = ufree.QuadPart;
	total = utotal.QuadPart;
	usage = utotal.QuadPart - ufree.QuadPart;
	return true;
}

std::string GetFileDescription(TCHAR *file_path) {
	std::string description = "";

	DWORD dwSize = GetFileVersionInfoSize(file_path, NULL);
	if (dwSize > 0) {
		TCHAR *pBuf = new TCHAR[dwSize + 1];
		memset(pBuf, 0, dwSize + 1);
		GetFileVersionInfo(file_path, NULL, dwSize, pBuf);

		LPVOID lpBuffer = NULL;
		UINT uLen = 0;

		UINT nQuerySize;
		DWORD* pTransTable;
		::VerQueryValue(pBuf, "\\VarFileInfo\\Translation", (void **)&pTransTable, &nQuerySize);
		DWORD m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

		TCHAR SubBlock[50] = { 0 };
		sprintf_s(SubBlock, 50, "\\StringFileInfo\\%08lx\\FileDescription", m_dwLangCharset);

		VerQueryValue(pBuf, SubBlock, &lpBuffer, &uLen);
		if (uLen) description = (TCHAR*)lpBuffer;
		delete[]pBuf;
	}
	return description;
}

bool TraverseProcesses(){
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) return false;

	BOOL bResult = Process32First(hProcessSnap, &pe32);

	while (bResult){
		auto name = pe32.szExeFile;
		auto id = pe32.th32ProcessID;

		auto processHaldle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
		if (!processHaldle){
			bResult = Process32Next(hProcessSnap, &pe32);
			continue;
		}

		DWORD size = MAX_PATH;
		TCHAR sExeName[MAX_PATH] = { 0 };
		if (FALSE == QueryFullProcessImageName(processHaldle, 0, sExeName, &size)){
			bResult = Process32Next(hProcessSnap, &pe32);
			continue;
		}
		auto wstrProcessPath = sExeName;
		if (nullptr != processHaldle){
			if (!process.count(id)) {
				process[id].pid = id;
				process[id].p_pid = pe32.th32ParentProcessID;
				process[id].name = QString::fromLocal8Bit(name);
				process[id].description = QString::fromLocal8Bit(GetFileDescription(wstrProcessPath).c_str());

				QFileIconProvider icon_provider;
				process[id].icon = icon_provider.icon(QString::fromLocal8Bit(wstrProcessPath)).pixmap(12,12);
			}

			uint64_t memoryInfo = 0, vmem = 0, readDiskBytes = 0, writeDiskBytes = 0;
			GetCpuUsageRatio(processHaldle, pe32.th32ProcessID);
			GetMemoryUsage(processHaldle, memoryInfo, vmem);
			GetIoBytes(processHaldle, readDiskBytes, writeDiskBytes);

			process[id].is_active = true;
			process[id].memory_byte = memoryInfo;
			int64_t io = readDiskBytes + writeDiskBytes;
			process[id].disk_byte = io - process[id].last_io;
			process[id].last_io = io;
		}
		if (processHaldle) CloseHandle(processHaldle);
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return true;
}

void InjectionTool::on_treeWidget_all_process_itemClicked(QTreeWidgetItem* item, int column) {
	current_pid = item->text(4).toULongLong();

	bool key = hook_map[current_pid].key;
	bool mouse = hook_map[current_pid].mouse;
	ui->pushButton_key_hook->setText(key ? "取消拦截" : "注入");
	ui->pushButton_mouse_hook->setText(mouse ? "取消拦截" : "注入");

	bool uninstall = !hook_map[current_pid].install;
	ui->checkBox_recv->setEnabled(uninstall);
	ui->checkBox_send->setEnabled(uninstall);
	ui->pushButton_dll_hook->setText(uninstall?"注入原生DLL":"解绑");
}
void InjectionTool::on_treeWidget_all_process_itemExpanded(QTreeWidgetItem* item) {
	int64_t pid = item->text(4).toULongLong();
	if (!process.count(pid))return;
	process[pid].is_expand = item->isExpanded();
}
void InjectionTool::on_pushButton_refresh_clicked() {
	static std::mutex lck;
	if (!lck.try_lock())return;
	for (auto& i : process)i.second.is_active = false;

	TraverseProcesses();
	std::list<ProcessInfo>filter_process;
	auto val_v = ui->treeWidget_all_process->verticalScrollBar()->value();
	auto val_h = ui->treeWidget_all_process->horizontalScrollBar()->value();

	for (auto& i : process)if (!i.second.is_active)process.erase(i.first);
	for (auto i : process) {
		if (!filter.isEmpty()) {
			bool is_have = false;
			is_have |= i.second.name.indexOf(filter) > -1;
			is_have |= i.second.pid == filter.toLongLong();
			if (!is_have)continue;
		}
		filter_process.push_back(i.second);
	}

	for (auto i = filter_process.begin(); i != filter_process.end(); i++) {
		for (auto j = i; j != filter_process.end(); j++) {
			bool is_switch = false;
			is_switch |= sort == NAME && i->name > j->name;
			is_switch |= sort == CPU && i->cpu > j->cpu;
			is_switch |= sort == MEM && i->memory_byte > j->memory_byte;
			is_switch |= sort == IO && i->disk_byte > j->disk_byte;
			is_switch |= sort == PID && i->pid > j->pid;
			is_switch |= sort == DES && i->description > j->description;
			is_switch |= sort == TREE && i->name > j->name;
			if (!is_switch)continue;
			ProcessInfo t = *j;
			*j = *i, *i = t;
		}
		QApplication::processEvents();
	}

	int index = 0;
	auto set_item_val = [&](QTreeWidgetItem* item,  ProcessInfo& val) {
		QString pid = QString::number(val.pid);
		if (item->text(4) != pid) {
			item->setText(5, val.description);
			item->setIcon(0, val.icon);
			item->setText(0, val.name);
			item->setText(4, pid);
		}
		item->setText(1, QString::number(val.cpu, 'f', 2));
		item->setText(2, b_to_tb(val.memory_byte));
		item->setText(3, b_to_tb(val.disk_byte));
	};
	auto add_a_top = [&](ProcessInfo& val)->QTreeWidgetItem* {
		QTreeWidgetItem* topLevelItem = nullptr;
		if (ui->treeWidget_all_process->topLevelItemCount() <= index) {
			topLevelItem = new QTreeWidgetItem();
			ui->treeWidget_all_process->addTopLevelItem(topLevelItem);
		}
		else topLevelItem = ui->treeWidget_all_process->topLevelItem(index);
		set_item_val(topLevelItem, val);
		index++;
		return topLevelItem;
	};
	static std::map<int64_t, bool>expand;
	std::function<void(int64_t, QTreeWidgetItem*)>add_child;
	add_child = [&](int64_t pid, QTreeWidgetItem* par) {
		int child_index = 0;
		for (auto i = filter_process.begin(); i != filter_process.end();i++) {
			if (i->p_pid != pid)continue;
			QTreeWidgetItem* temp = nullptr;
			if (par->childCount() <= child_index) {
				temp = new QTreeWidgetItem();
				temp->setExpanded(true);
				par->addChild(temp);
			}
			else temp = par->child(child_index);
			set_item_val(temp, *i);
			add_child(i->pid, temp);
			child_index++;
			temp->setExpanded(i->is_expand);
		}
		while (par->childCount() > child_index)par->takeChild(child_index);
	};

	if (sort == TREE) {
		for (auto i = filter_process.begin(); i != filter_process.end(); i++) {
			std::list<ProcessInfo>::iterator t = filter_process.end();
			for (auto j = filter_process.begin(); j != filter_process.end(); j++) if (i->p_pid == j->pid)t = j;
			if (t != filter_process.end())continue;
			add_child(i->pid, add_a_top(*i));
		}
	}
	else {
		for (auto i = 0; i < ui->treeWidget_all_process->topLevelItemCount(); i++) {
			auto temp = ui->treeWidget_all_process->takeTopLevelItem(i);
			while (temp->childCount())temp->takeChild(0);
		}
		if (!up_down) for (auto i = filter_process.begin(); i != filter_process.end(); i++)add_a_top(*i);
		else for (auto i = filter_process.rbegin(); i != filter_process.rend(); i++)add_a_top(*i);
	}
	while (ui->treeWidget_all_process->topLevelItemCount() > index)ui->treeWidget_all_process->takeTopLevelItem(index);

	if (sort != TREE)ui->treeWidget_all_process->clearSelection();
	for (int i = 0; i < ui->treeWidget_all_process->topLevelItemCount(); i++) {
		auto temp = ui->treeWidget_all_process->topLevelItem(i);
		if (temp->text(4).toULongLong() != current_pid)continue;
		ui->treeWidget_all_process->setItemSelected(temp, true);
		if (!ui->checkBox_local->isChecked())continue;
		ui->treeWidget_all_process->setCurrentItem(temp);
		break;
	}
	if (!ui->checkBox_local->isChecked()) {
		ui->treeWidget_all_process->verticalScrollBar()->setValue(val_v);
		ui->treeWidget_all_process->horizontalScrollBar()->setValue(val_h);
	}

	lck.unlock();
	refresh.start(ui->spinBox_timeout->value());
}

void InjectionTool::on_pushButton_kill_clicked() {
	auto current = ui->treeWidget_all_process->currentItem();
	auto res = QMessageBox::warning(nullptr, "结束进程", "确认结束此进程吗?", QMessageBox::Yes | QMessageBox::No);
	if (res == QMessageBox::No)return;
	if (!current)return;

	int64_t pid = current->text(4).toULongLong();
	QString cmd = QString("TASKKILL /PID %1 /T /F").arg(pid);
	WinExec(cmd.toLocal8Bit().data(), SW_HIDE);
}
void InjectionTool::on_lineEdit_filter_editingFinished() {
	if (ui->lineEdit_filter->hasFocus()) {
		ui->lineEdit_filter->clearFocus();
		return;
	}
	 filter = ui->lineEdit_filter->text();
	 on_pushButton_refresh_clicked();
}
void InjectionTool::on_pushButton_tree_clicked() {
	sort = TREE;
	up_down = false;
	QStringList header;
	header << "Process" << "CPU" << "Memory Byte" << "I/O Byte" << "PID" << "Description";
	ui->treeWidget_all_process->setHeaderLabels(header);
	ui->pushButton_tree->setEnabled(false);
	on_pushButton_refresh_clicked();
}
void InjectionTool::TreeWidgetHeaderClicked(int index) {
	QStringList header;
	header << "Process" << "CPU" << "Memory Byte" << "I/O Byte" << "PID" << "Description";
	if (sort == index)up_down = !up_down;
	else sort = (SORT)index, up_down = true;
	header[sort] += up_down ? " ↑" : " ↓";
	on_pushButton_refresh_clicked();

	ui->treeWidget_all_process->setHeaderLabels(header);
	ui->pushButton_tree->setEnabled(true);
}



bool InjectionTool::eventFilter(QObject* obj, QEvent* event) {
	if (obj != ui->pushButton_find)return false;

	QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
	if (mouseEvent->button() != Qt::LeftButton)return false;

	QTreeWidgetItem* is_find = nullptr;
	std::function<void(QTreeWidgetItem*, int64_t)>find_child_pid;
	find_child_pid = [&](QTreeWidgetItem* par, int64_t pid) {
		for (int i = 0; i < par->childCount() && !is_find; i++) {
			auto temp = par->child(i);
			if (temp->childCount())find_child_pid(temp, pid);
			if (temp->text(4).toULongLong() != pid)continue;
			is_find = temp;
			break;
		}
	};

	if (event->type() == QEvent::MouseButtonPress) {
		this->setCursor(Qt::CrossCursor);
		this->lower();
	}
	if (event->type() == QEvent::MouseButtonRelease) {
		this->setCursor(Qt::ArrowCursor);
		DWORD pid;
		auto pos = mouseEvent->globalPos();
		HWND hwnd = WindowFromPoint(POINT({ pos.x(), pos.y() }));
		GetWindowThreadProcessId(hwnd, &pid);
		for (int i = 0; i < ui->treeWidget_all_process->topLevelItemCount() && !is_find; i++) {
			auto temp = ui->treeWidget_all_process->topLevelItem(i);
			if (temp->childCount())find_child_pid(temp, pid);
			if (temp->text(4).toULongLong() != pid)continue;
			is_find = temp;
			break;
		}
		this->raise();
		if (!is_find)return false;
		ui->checkBox_local->setChecked(true);
		ui->treeWidget_all_process->setFocus();
		on_treeWidget_all_process_itemClicked(is_find, 0);
	}

	return false;
}