#include "ui_QMO.h"
#include "mainwindow.h"
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
#include "GlobalDefine.h"
using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
typedef struct PoTi {
	ULONG64 type;
	CPoint Pot;
	ULONG64 sleep;
}PoTi;

std::map<int, QString>key_to_string{
	{0x01, "LBUTTON"},
	{0x02, "RBUTTON"},
	{0x03, "CANCEL"},
	{0x04, "MBUTTON"},
	{0x05, "XBUTTON1"},
	{0x06, "XBUTTON2"},
	{0x08, "BACK"},
	{0x09, "TAB"},
	{0x0C, "CLEAR"},
	{0x0D, "RETURN"},
	{0x10, "SHIFT"},
	{0x11, "CONTROL"},
	{0x12, "MENU"},
	{0x13, "PAUSE"},
	{0x14, "CAPITAL"},
	{0x1B, "ESCAPE"},
	{0x20, "SPACE"},
	{0x21, "PRIOR"},
	{0x22, "NEXT"},
	{0x23, "END"},
	{0x24, "HOME"},
	{0x25, "LEFT"},
	{0x26, "UP"},
	{0x27, "RIGHT"},
	{0x28, "DOWN"},
	{0x29, "SELECT"},
	{0x2A, "PRINT"},
	{0x2B, "EXECUTE"},
	{0x2C, "SNAPSHOT"},
	{0x2D, "INSERT"},
	{0x2E, "DELETE"},
	{0x2F, "HELP"},
	{0x30, "0"},
	{0x31, "1"},
	{0x32, "2"},
	{0x33, "3"},
	{0x34, "4"},
	{0x35, "5"},
	{0x36, "6"},
	{0x37, "7"},
	{0x38, "8"},
	{0x39, "9"},
	{0x41, "A"},
	{0x42, "B"},
	{0x43, "C"},
	{0x44, "D"},
	{0x45, "E"},
	{0x46, "F"},
	{0x47, "G"},
	{0x48, "H"},
	{0x49, "I"},
	{0x4A, "J"},
	{0x4B, "K"},
	{0x4C, "L"},
	{0x4D, "M"},
	{0x4E, "N"},
	{0x4F, "O"},
	{0x50, "P"},
	{0x51, "Q"},
	{0x52, "R"},
	{0x53, "S"},
	{0x54, "T"},
	{0x55, "U"},
	{0x56, "V"},
	{0x57, "W"},
	{0x58, "X"},
	{0x59, "Y"},
	{0x5A, "Z"},
	{0x5B, "LWIN"},
	{0x5C, "RWIN"},
	{0x5D, "APPS"},
	{0x5F, "SLEEP"},
	{0x60, "NUMPAD0"},
	{0x61, "NUMPAD1"},
	{0x62, "NUMPAD2"},
	{0x63, "NUMPAD3"},
	{0x64, "NUMPAD4"},
	{0x65, "NUMPAD5"},
	{0x66, "NUMPAD6"},
	{0x67, "NUMPAD7"},
	{0x68, "NUMPAD8"},
	{0x69, "NUMPAD9"},
	{0x6A, "MULTIPLY"},
	{0x6B, "ADD"},
	{0x6C, "SEPARATOR"},
	{0x6D, "SUBTRACT"},
	{0x6E, "DECIMAL"},
	{0x6F, "DIVIDE"},
	{0x70, "F1"},
	{0x71, "F2"},
	{0x72, "F3"},
	{0x73, "F4"},
	{0x74, "F5"},
	{0x75, "F6"},
	{0x76, "F7"},
	{0x77, "F8"},
	{0x78, "F9"},
	{0x79, "F10"},
	{0x7A, "F11"},
	{0x7B, "F12"},
	{0x7C, "F13"},
	{0x7D, "F14"},
	{0x7E, "F15"},
	{0x7F, "F16"},
	{0x80, "F17"},
	{0x81, "F18"},
	{0x82, "F19"},
	{0x83, "F20"},
	{0x84, "F21"},
	{0x85, "F22"},
	{0x86, "F23"},
	{0x87, "F24"},
	{0x90, "NUMLOCK"},
	{0x91, "SCROLL"},
	{0xA0, "LSHIFT"},
	{0xA1, "RSHIFT"},
	{0xA2, "LCONTROL"},
	{0xA3, "RCONTROL"},
	{0xA4, "LMENU"},
	{0xA5, "RMENU"},
	{0xA6, "BROWSER_BACK"},
	{0xA7, "BROWSER_FORWARD"},
	{0xA8, "BROWSER_REFRESH"},
	{0xA9, "BROWSER_STOP"},
	{0xAA, "BROWSER_SEARCH"},
	{0xAB, "BROWSER_FAVORITES"},
	{0xAC, "BROWSER_HOME"},
	{0xAD, "VOLUME_MUTE"},
	{0xAE, "VOLUME_DOWN"},
	{0xAF, "VOLUME_UP"},
	{0xB0, "MEDIA_NEXT_TRACK"},
	{0xB1, "MEDIA_PREV_TRACK"},
	{0xB2, "MEDIA_STOP"},
	{0xB3, "MEDIA_PLAY_PAUSE"},
	{0xB4, "LAUNCH_MAIL"},
	{0xB5, "LAUNCH_MEDIA_SELECT"},
	{0xB6, "LAUNCH_APP1"},
	{0xB7, "LAUNCH_APP2"},
	{0xBA, "OEM_1"},
	{0xBB, "OEM_PLUS"},
	{0xBC, "OEM_COMMA"},
	{0xBD, "OEM_MINUS"},
	{0xBE, "OEM_PERIOD"},
	{0xBF, "OEM_2"},
	{0xC0, "OEM_3"},
	{0xC1, "GAMEPAD_A"},
	{0xC2, "GAMEPAD_B"},
	{0xC3, "GAMEPAD_X"},
	{0xC4, "GAMEPAD_Y"},
	{0xC5, "GAMEPAD_R1"},
	{0xC6, "GAMEPAD_L1"},
	{0xC7, "GAMEPAD_R2"},
	{0xC8, "GAMEPAD_L2"},
	{0xC9, "GAMEPAD_OK"},
	{0xCA, "GAMEPAD_CANCEL"},
	{0xCB, "GAMEPAD_UP"},
	{0xCC, "GAMEPAD_DOWN"},
	{0xCD, "GAMEPAD_LEFT"},
	{0xCE, "GAMEPAD_RIGHT"}
};

HWND hwnd;
bool show_ = false;
bool execute_ = false;
bool begin_hook = false;
bool client_mode = false;
uint hot_1, hot_2, hot_3;
string lab_str;
milliseconds beg;
deque<PoTi>act;

ULONG64 GetSleepTime() {
	milliseconds now_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	ULONG64 val = now_.count() - beg.count();
	beg = now_;
	return val;
}

LRESULT CALLBACK LowLevelMouseProc(INT nCode, WPARAM wParam, LPARAM lParam) {
	CPoint  Pot;
	tagMSLLHOOKSTRUCT* pkbhs = (tagMSLLHOOKSTRUCT*)lParam;
	if (begin_hook && nCode == HC_ACTION){
		GetCursorPos(&Pot);
		if(client_mode) ScreenToClient(hwnd, &Pot);  //屏幕转化为客户端
		if (wParam == WM_MOUSEWHEEL) act.push_back(PoTi{ wParam, { ((short*)&pkbhs->mouseData)[1] ,0 }, GetSleepTime() });
		else act.push_back(PoTi{ wParam, Pot, GetSleepTime() });
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyProc(INT nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
	if (begin_hook && nCode == HC_ACTION) 
		if (p->vkCode != VK_LMENU && p->vkCode != VK_F1 && p->vkCode != VK_F2 && p->vkCode != VK_F3)
			act.push_back(PoTi{ wParam - 255, {(int)p->vkCode,0}, GetSleepTime() });
	return CallNextHookEx(NULL, nCode, wParam, lParam);  //  hook链
}

void CustomExecute(int whilet, int wt, float ber) {
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	while (execute_ && whilet--) {
		for (auto i : act) {
			if (!execute_)break;
			else if (i.type >= 0x0200) {
				int action;
				if (i.type == WM_MOUSEMOVE)action = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
				else if (i.type == WM_LBUTTONDOWN)action = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
				else if (i.type == WM_LBUTTONUP)action = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP;
				else if (i.type == WM_MOUSEWHEEL)action = MOUSEEVENTF_WHEEL;
				else continue;
				if (i.type != WM_MOUSEWHEEL)mouse_event(action, i.Pot.x * 65535 / cx, i.Pot.y * 65535 / cy, 0, 0);
				else mouse_event(action, 0, 0, i.Pot.x, 0);
			}
			else keybd_event(i.Pot.x, 0, i.type, 0), cout << i.Pot.x << endl;
			this_thread::sleep_for(milliseconds((int)(i.sleep * ber)));
		}
		this_thread::sleep_for(milliseconds(wt));
		lab_str = "剩余: " + to_string(whilet) + "次, Alt+" + key_to_string[hot_3].toStdString() + ": 停止执行";
	}
	execute_ = false;
	lab_str = "执行结束";
}

void LeftClickExecute(int times, ULONG sleep, int whilet, int whil_sleep, CPoint pt) {
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, pt.x * 65535 / cx, pt.y * 65535 / cy, 0, 0);
	while (execute_ && whilet--) {
		int _tim = times;
		while (execute_ && _tim--) {
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, pt.x * 65535 / cx, pt.y * 65535 / cy, 0, 0);
			this_thread::sleep_for(milliseconds(sleep));
			lab_str = "剩余: " + to_string(whilet) + "个循环, " + to_string(_tim) + "次, Alt+"+key_to_string[hot_1].toStdString() +": 停止执行";
		}
		this_thread::sleep_for(milliseconds(whil_sleep));
	}
	execute_ = false;
	lab_str = "执行结束";
}

QuickMacro::QuickMacro(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::QuickMacro)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-QuickMacro.png")));

	on_pushButton_set_cancel_clicked(true);
	mouse_Hook = SetWindowsHookExA(WH_MOUSE_LL, (HOOKPROC)(LowLevelMouseProc), NULL, NULL);
	if (mouse_Hook == NULL) {
		UnhookWindowsHookEx(mouse_Hook);
		ErrExit(QString("Mouse Hook初始化失败: %1").arg(QString::number(GetLastError())));
	}
	key_Hook = SetWindowsHookExA(WH_KEYBOARD_LL, (HOOKPROC)(LowLevelKeyProc), NULL, NULL);
	if (key_Hook == NULL) {
		UnhookWindowsHookEx(key_Hook);
		UnhookWindowsHookEx(mouse_Hook);
		ErrExit(QString("Key Hook初始化失败: %1").arg(QString::number(GetLastError())));
	}
	hwnd = (HWND)this->winId();
	DrawAllImg();

	hot_1 = hotK_1, hot_2 = hotK_2, hot_3 = hotK_3;
	std::thread hk_t(&QuickMacro::HotKetBack, this);
	hk_t.detach();
	connect(&re, &QTimer::timeout, this, &QuickMacro::RefreshView);
	re.start(100);

	connect(ui->horizontalSlider_action, &QSlider::valueChanged, this, &QuickMacro::on_horizontalSlider_action_valueChanged);
	connect(ui->pushButton_point, &QPushButton::clicked, this, &QuickMacro::on_pushButton_point_clicked);
	connect(ui->pushButton_show, &QPushButton::clicked, this, &QuickMacro::on_pushButton_show_clicked);
	connect(ui->pushButton_save, &QPushButton::clicked, this, &QuickMacro::SaveFile);
	connect(ui->pushButton_read, &QPushButton::clicked, this, &QuickMacro::ReadFile);

	ui->lineEdit_4->installEventFilter(this);
	ui->lineEdit_8->installEventFilter(this);
	ui->lineEdit_12->installEventFilter(this);
	lf_execute = mf_execute = false;
	on_pushButton_top_clicked();
}

void QuickMacro::HotKetBack() {
	int status = 0;
	int hotK_1 = -1, hotK_2 = -1, hotK_3 = -1;
	while (1) {
		if (hotK_1 != hot_1 || hotK_2 != hot_2 || hotK_3 != hot_3) {
			global_info = "";
			if (hotK_1 >= 0)UnregisterHotKey(NULL, 1);
			if (hotK_2 >= 0)UnregisterHotKey(NULL, 2);
			if (hotK_3 >= 0)UnregisterHotKey(NULL, 3);
			if (!RegisterHotKey(NULL, 1, MOD_ALT, hot_1))global_info = "左键单击快捷键初始化失败";
			if (!RegisterHotKey(NULL, 2, MOD_ALT, hot_2))global_info = "开始录制快捷键初始化失败";
			if (!RegisterHotKey(NULL, 3, MOD_ALT, hot_3))global_info = "开始执行快捷键初始化失败";
			hotK_1 = hot_1, hotK_2 = hot_2, hotK_3 = hot_3;
		}

		MSG msg = { 0 };
		GetMessageA(&msg, NULL, 0, 0);
		if (hotK_1 != hot_1 || hotK_2 != hot_2 || hotK_3 != hot_3) continue;

		if (msg.message != WM_HOTKEY)continue;
		if ((begin_hook || execute_) && status != msg.wParam)continue;
		else if (show_)continue;
		else status = msg.wParam;

		if (1 == msg.wParam) emit ui->pushButton_left_start->clicked();
		if (2 == msg.wParam) emit ui->pushButton_mdify_video->clicked();
		if (3 == msg.wParam) emit ui->pushButton_modify_start->clicked();
	}
}
void QuickMacro::on_pushButton_left_start_clicked() {
	if (execute_) {
		lf_execute = execute_ = false;
		return;
	}
	else lf_execute = execute_ = true;
	int t = ui->spinBox_left_click_t->value();
	for (int t = 150; t && execute_; t--) {
		lab_str = "即将开始: " + to_string(t) + ", 请勿操作键盘与鼠标";
		QApplication::processEvents(QEventLoop::AllEvents);
		this_thread::sleep_for(milliseconds(20));
	}
	if (!execute_)return;
	int n = ui->spinBox_left_click_m->value();
	int xt = ui->spinBox_left_click_t_2->value();
	int xn = ui->spinBox_left_click_m_2->value();
	CPoint _p{ cp_p.x(),cp_p.y() };
	std::thread _t(&LeftClickExecute, t, n, xt, xn, _p);
	_t.detach();
	lab_str = QString("Alt+%1: 停止执行").arg(ui->lineEdit_4->text()).toStdString();
	SaveConfigure();
}
void QuickMacro::on_pushButton_mdify_video_clicked() {
	begin_hook = !begin_hook;
	if (!begin_hook) {
		lab_str = "录制完成!";
		ui->horizontalSlider_action->setMaximum(act.size() - 1);
		return;
	}
	act.clear();
	lab_str = QString("Alt+%1: 停止录制").arg(ui->lineEdit_8->text()).toStdString();
	beg = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}
void QuickMacro::on_pushButton_modify_start_clicked() {
	if (execute_) {
		mf_execute = execute_ = false;
		return;
	}
	if (act.empty()) {
		lab_str = "自定义路径为空！";
		return;
	}
	else mf_execute = execute_ = true;
	for (int t = 150; t && execute_; t--) {
		lab_str = "即将开始: " + to_string(t) + ", 请勿操作键盘与鼠标";
		QApplication::processEvents(QEventLoop::AllEvents);
		this_thread::sleep_for(milliseconds(20));
	}
	if (!execute_)return;
	int t = ui->spinBox_times->value();
	int w = ui->spinBox_while->value();
	double n = ui->doubleSpinBox_speed->value();
	std::thread _t(&CustomExecute, t, w, n);
	_t.detach();
	lab_str = QString("Alt+%1: 停止执行").arg(ui->lineEdit_12->text()).toStdString();
	SaveConfigure();
}

void QuickMacro::on_pushButton_show_clicked() {
	if (act.empty()) {
		lab_str = "自定义路径为空！";
		return;
	}
	show_ = true;
	ui->pushButton_show->setEnabled(false);
	if (!cp.isVisible())on_pushButton_point_clicked();
	lab_str = "演示中...";
	float ber = ui->doubleSpinBox_speed->value();
	unsigned long long number = 0;
	for (auto i : act) {
		if (i.type == WM_MOUSEMOVE)cp.SetPoint({ i.Pot.x, i.Pot.y });
		else if (i.type == WM_RBUTTONDOWN)cp.RightPush();
		else if (i.type == WM_RBUTTONUP)cp.RightUp();
		else if (i.type == WM_LBUTTONDOWN)cp.LeftPush();
		else if (i.type == WM_LBUTTONUP)cp.LeftUp();
		else if (i.type == WM_MOUSEWHEEL)cp.Wheel();
		else if (i.type == WM_KEYDOWN || i.type == WM_KEYUP) lab_str = "键盘输入中...";
		QApplication::processEvents(QEventLoop::AllEvents);
		this_thread::sleep_for(milliseconds((int)(i.sleep * ber)));
		if (i.type == WM_MOUSEWHEEL)cp.LeftUp();
		else if (i.type == WM_KEYDOWN || i.type == WM_KEYUP)lab_str = "演示中...";
		ui->horizontalSlider_action->setValue(number++);
	}
	lab_str = "演示完成！";
	ui->pushButton_show->setEnabled(true);
	show_ = false;
}

void QuickMacro::SaveFile() {
	if (act.empty()) {
		lab_str = "自定义路径为空！";
		return;
	}
	QString filename = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("*.qmo"));
	if (filename.isEmpty()) return;
	fstream file(filename.toLocal8Bit().toStdString(), ios::in | ios::out | ios::trunc | ios::binary);
	if (!file.is_open()) {
		lab_str = "保存失败!";
		return;
	}
	file << "LxQuicMacro";
	unsigned long number = act.size();
	file.write((char*)&number, sizeof(number));
	for (auto i : act) file.write((char*)&i, sizeof(i));
	file.close();
}
void QuickMacro::ReadFile() {
	QString aFileName = QFileDialog::getOpenFileName(this, "选择文件", QCoreApplication::applicationDirPath(), "所有文件(*.qmo)");
	if (aFileName.isEmpty()) return;

	fstream file(aFileName.toLocal8Bit().toStdString(), ios::in | ios::binary);
	if (!file.is_open()) {
		lab_str = "读取失败!";
		return;
	}
	char val[12]{ 0,0,0,0,0,0,0,0,0,0,0,'\0'};
	file.read(val, 11);
	string head = val;
	if (head != "LxQuicMacro") {
		lab_str = "文件格式有误!";
		return;
	}
	unsigned long number = 0;
	file.read((char*)&number, 4);
	for (; number; number--) {
		PoTi pti;
		file.read((char*)&pti, sizeof(pti));
		act.push_back(pti);
	}
	ui->horizontalSlider_action->setMaximum(act.size() - 1);
}

void QuickMacro::SaveConfigure() {
	int l_times = ui->spinBox_left_click_t->value();
	int l_space = ui->spinBox_left_click_m->value();
	int l_loop = ui->spinBox_left_click_t_2->value();
	int l_loop_space = ui->spinBox_left_click_m_2->value();
	int r_times = ui->spinBox_times->value();
	int r_space = ui->spinBox_while->value();
	int r_speed = ui->doubleSpinBox_speed->value();

	fstream _file("./conf/QuickMacro.conf", ios::in | ios::out | ios::binary);
	if (!_file.is_open())QMessageBox::warning(this, "错误", "配置文件读写失败，请检查权限!");
	else {
		string str;
		while (!_file.eof()) str.push_back(_file.get());
		_file.close();

		if (str.size() < 44)for (int i = 0; i < 44; i++)str.push_back('\0');
		memcpy((char*)str.data(), &l_times, 4);
		memcpy((char*)str.data() + 4, &l_space, 4);
		memcpy((char*)str.data() + 8, &l_loop, 4);
		memcpy((char*)str.data() + 12, &l_loop_space, 4);

		memcpy((char*)str.data() + 20, &r_times, 4);
		memcpy((char*)str.data() + 24, &r_space, 4);
		memcpy((char*)str.data() + 28, &r_speed, 4);

		_file.open("./conf/QuickMacro.conf", ios::in | ios::out | ios::binary);
		_file.write(&str[0], 44);
		_file.close();
	}
}
void QuickMacro::on_pushButton_set_cancel_clicked(bool init) {
	string str = "";
	int r_times = 1, r_space = 1, r_speed = 1000;
	hotK_1 = VK_F1, hotK_2 = VK_F2, hotK_3 = VK_F3;
	int l_times = 50, l_space = 8, l_loop = 1, l_loop_space = 1000;

	fstream _file("./conf/QuickMacro.conf", ios::in | ios::out | ios::binary);
	if (!_file.is_open())_file.open("./conf/QuickMacro.conf", ios::in | ios::out | ios::trunc | ios::binary);
	if (!_file.is_open())QMessageBox::warning(this, "错误", "配置文件读写失败，请检查权限!");
	else while (!_file.eof()) str.push_back(_file.get());
	_file.close();

	if (str.size() >= 44) {
		memcpy(&l_times, str.data(), 4);
		memcpy(&l_space, str.data() + 4, 4);
		memcpy(&l_loop, str.data() + 8, 4);
		memcpy(&l_loop_space, str.data() + 12, 4);

		memcpy(&r_times, str.data() + 20, 4);
		memcpy(&r_space, str.data() + 24, 4);
		memcpy(&r_speed, str.data() + 28, 4);

		memcpy(&hotK_1, str.data() + 32, 4);
		memcpy(&hotK_2, str.data() + 36, 4);
		memcpy(&hotK_3, str.data() + 40, 4);
	}
	else {
		if (str.size() < 44)for (int i = 0; i < 44; i++)str.push_back('\0');
		memcpy((char*)str.data(), &l_times, 4);
		memcpy((char*)str.data() + 4, &l_space, 4);
		memcpy((char*)str.data() + 8, &l_loop, 4);
		memcpy((char*)str.data() + 12, &l_loop_space, 4);
		memcpy((char*)str.data() + 16, &l_times, 4);

		memcpy((char*)str.data() + 20, &r_times, 4);
		memcpy((char*)str.data() + 24, &r_space, 4);
		memcpy((char*)str.data() + 28, &r_speed, 4);

		memcpy(&str[0] + 32, &hotK_1, 4);
		memcpy(&str[0] + 36, &hotK_2, 4);
		memcpy(&str[0] + 40, &hotK_3, 4);

		_file.open("./conf/QuickMacro.conf", ios::in | ios::out | ios::binary);
		_file.write(str.c_str(), 44);
		_file.close();
	}

	if (init) {
		ui->spinBox_left_click_t->setValue(l_times);
		ui->spinBox_left_click_m->setValue(l_space);
		ui->spinBox_left_click_t_2->setValue(l_loop);
		ui->spinBox_left_click_m_2->setValue(l_loop_space);
		ui->spinBox_times->setValue(r_times);
		ui->spinBox_while->setValue(r_space);
		ui->doubleSpinBox_speed->setValue(r_speed);
	}

	ui->lineEdit_4->setText(key_to_string[hotK_1]);
	ui->lineEdit_8->setText(key_to_string[hotK_2]);
	ui->lineEdit_12->setText(key_to_string[hotK_3]);
}
void QuickMacro::on_pushButton_set_done_clicked() {
	fstream _file("./conf/QuickMacro.conf", ios::in | ios::out |ios::binary);
	if (!_file.is_open())QMessageBox::warning(this, "错误", "配置文件读写失败，请检查权限!");
	else {
		string str;
		while (!_file.eof()) str.push_back(_file.get());
		_file.close();

		if (str.size() < 44)for (int i = 0; i < 44; i++)str.push_back('\0');
		memcpy(&str[0] + 32, &hotK_1, 4);
		memcpy(&str[0] + 36, &hotK_2, 4);
		memcpy(&str[0] + 40, &hotK_3, 4);

		_file.open("./conf/QuickMacro.conf", ios::in | ios::out | ios::binary);
		_file.write(&str[0], 44);
		_file.close();
	}
	int old_key;
	if (hot_1 != hotK_1)old_key = hot_1;
	if (hot_2 != hotK_2)old_key = hot_2;
	if (hot_3 != hotK_3)old_key = hot_3;
	hot_1 = hotK_1, hot_2 = hotK_2, hot_3 = hotK_3;

	
	keybd_event(VK_MENU, 0, 0, 0);
	keybd_event(old_key, 0, 0, 0);
	keybd_event(old_key, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	QMessageBox::information(this, "提示", "修改完成");
}

void QuickMacro::RefreshView() {
	ui->label_info->setText(lab_str.c_str());
	cp_p = cp.GetPoint();
	if (execute_)cp.hide();
	ui->label_point->setText(QString("(%1,%2)").arg(cp_p.x()).arg(cp_p.y()));
	ui->pushButton_left_start->setText(lf_execute ? "停止执行" : "开始执行");
	ui->pushButton_mdify_video->setText(begin_hook?"停止录制":"开始录制");
	ui->pushButton_modify_start->setText(mf_execute ? "停止执行" : "开始执行");
	if (!execute_ && !begin_hook)ui->label_info->setText(global_info);
}
void QuickMacro::on_horizontalSlider_action_valueChanged(int val) {
	if (act.empty()) {
		lab_str = "自定义路径为空！";
		return;
	}
	if (show_)return;
	if (!cp.isVisible())on_pushButton_point_clicked();
	auto i = act[val];
	if (i.type == WM_MOUSEMOVE)cp.SetPoint({ i.Pot.x, i.Pot.y });
	else if (i.type == WM_LBUTTONDOWN)cp.LeftPush();
	else if (i.type == WM_LBUTTONUP)cp.LeftUp();
	else if (i.type == WM_MOUSEWHEEL)cp.Wheel();
	else if (i.type == WM_KEYDOWN || i.type == WM_KEYUP) lab_str = "键盘输入中...";
	if (i.type == WM_MOUSEWHEEL)cp.LeftUp();
}



void QuickMacro::ErrExit(QString msg) {
	QMessageBox::warning(this, "错误", msg);
	exit(-1);
}
void QuickMacro::closeEvent(QCloseEvent *event) {
	exit(0);
}
void QuickMacro::on_pushButton_top_clicked() {
	static bool is_top = false;
	::SetWindowPos((HWND)this->winId(), is_top ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
	is_top = !is_top;
	ui->pushButton_top->setText(is_top ? "取消置顶" : "置顶");
}
void QuickMacro::on_pushButton_point_clicked() {
	cp.setWindowFlags(Qt::FramelessWindowHint);
	cp.setAttribute(Qt::WA_TranslucentBackground);
	cp.showNormal();
	cp.activateWindow();
	cp.move(this->pos().x() - 24, this->pos().y() - 24);
}

static QPoint mouse_pos;
static bool is_move_mouse;
void QuickMacro::mousePressEvent(QMouseEvent* event){
	if (event->button() == Qt::LeftButton){
		is_move_mouse = true;
		mouse_pos = event->screenPos().toPoint();
	}
	QWidget::mousePressEvent(event);
}
void QuickMacro::mouseReleaseEvent(QMouseEvent* event){
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}
void QuickMacro::mouseMoveEvent(QMouseEvent* event){
	if (is_move_mouse) {
		QPoint newPos = this->pos() + (event->screenPos().toPoint() - mouse_pos);
		move(newPos);
		mouse_pos = event->screenPos().toPoint();
	}

	QWidget::mouseMoveEvent(event);
}
bool QuickMacro::eventFilter(QObject* obj, QEvent* event) {
	if (obj->parent() != ui->groupBox) return QMainWindow::eventFilter(obj, event);

	if (event->type() == QEvent::MouseButtonPress) {
		if (!ui->lineEdit_4->hasFocus())ui->lineEdit_4->setStyleSheet("");
		else ui->lineEdit_4->setStyleSheet("background-color:rgb(88, 88, 88);");
		if (!ui->lineEdit_8->hasFocus())ui->lineEdit_8->setStyleSheet("");
		else ui->lineEdit_8->setStyleSheet("background-color:rgb(88, 88, 88);");
		if (!ui->lineEdit_12->hasFocus())ui->lineEdit_12->setStyleSheet("");
		else ui->lineEdit_12->setStyleSheet("background-color:rgb(88, 88, 88);");
	}
	if (event->type() != QEvent::KeyPress) return QMainWindow::eventFilter(obj, event);

	int vkCode = 0;
	QString keyText = "Unknown Key";
	QLineEdit * e_obj = static_cast<QLineEdit *>(obj);
	QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	switch (keyEvent->key())
	{
	case Qt::Key_F1: vkCode = VK_F1; keyText = "F1"; break;
	case Qt::Key_F2: vkCode = VK_F2; keyText = "F2"; break;
	case Qt::Key_F3: vkCode = VK_F3; keyText = "F3"; break;
	case Qt::Key_F4: vkCode = VK_F4; keyText = "F4"; break;
	case Qt::Key_F5: vkCode = VK_F5; keyText = "F5"; break;
	case Qt::Key_F6: vkCode = VK_F6; keyText = "F6"; break;
	case Qt::Key_F7: vkCode = VK_F7; keyText = "F7"; break;
	case Qt::Key_F8: vkCode = VK_F8; keyText = "F8"; break;
	case Qt::Key_F9: vkCode = VK_F9; keyText = "F9"; break;
	case Qt::Key_F10: vkCode = VK_F10; keyText = "F10"; break;
	case Qt::Key_F11: vkCode = VK_F11; keyText = "F11"; break;
	case Qt::Key_F12: vkCode = VK_F12; keyText = "F12"; break;

	case Qt::Key_Alt: vkCode = VK_MENU; keyText = "Alt"; break;
	case Qt::Key_Control: vkCode = VK_CONTROL; keyText = "Ctrl"; break;
	case Qt::Key_Shift: vkCode = VK_SHIFT; keyText = "Shift"; break;
	case Qt::Key_Escape: vkCode = VK_ESCAPE; keyText = "Escape"; break;
	case Qt::Key_Space: vkCode = VK_SPACE; keyText = "Space"; break;
	case Qt::Key_Tab: vkCode = VK_TAB; keyText = "Tab"; break;
	case Qt::Key_Backspace: vkCode = VK_BACK; keyText = "Backspace"; break;
	case Qt::Key_Return: vkCode = VK_RETURN; keyText = "Enter"; break;
	case Qt::Key_Delete: vkCode = VK_DELETE; keyText = "Delete"; break;
	case Qt::Key_Home: vkCode = VK_HOME; keyText = "Home"; break;
	case Qt::Key_End: vkCode = VK_END; keyText = "End"; break;
	case Qt::Key_Left: vkCode = VK_LEFT; keyText = "Left Arrow"; break;
	case Qt::Key_Up: vkCode = VK_UP; keyText = "Up Arrow"; break;
	case Qt::Key_Right: vkCode = VK_RIGHT; keyText = "Right Arrow"; break;
	case Qt::Key_Down: vkCode = VK_DOWN; keyText = "Down Arrow"; break;

	case Qt::Key_0: vkCode = '0'; keyText = "0"; break;
	case Qt::Key_1: vkCode = '1'; keyText = "1"; break;
	case Qt::Key_2: vkCode = '2'; keyText = "2"; break;
	case Qt::Key_3: vkCode = '3'; keyText = "3"; break;
	case Qt::Key_4: vkCode = '4'; keyText = "4"; break;
	case Qt::Key_5: vkCode = '5'; keyText = "5"; break;
	case Qt::Key_6: vkCode = '6'; keyText = "6"; break;
	case Qt::Key_7: vkCode = '7'; keyText = "7"; break;
	case Qt::Key_8: vkCode = '8'; keyText = "8"; break;
	case Qt::Key_9: vkCode = '9'; keyText = "9"; break;

	case Qt::Key_A: vkCode = 'A'; keyText = "A"; break;
	case Qt::Key_B: vkCode = 'B'; keyText = "B"; break;
	case Qt::Key_C: vkCode = 'C'; keyText = "C"; break;
	case Qt::Key_D: vkCode = 'D'; keyText = "D"; break;
	case Qt::Key_E: vkCode = 'E'; keyText = "E"; break;
	case Qt::Key_F: vkCode = 'F'; keyText = "F"; break;
	case Qt::Key_G: vkCode = 'G'; keyText = "G"; break;
	case Qt::Key_H: vkCode = 'H'; keyText = "H"; break;
	case Qt::Key_I: vkCode = 'I'; keyText = "I"; break;
	case Qt::Key_J: vkCode = 'J'; keyText = "J"; break;
	case Qt::Key_K: vkCode = 'K'; keyText = "K"; break;
	case Qt::Key_L: vkCode = 'L'; keyText = "L"; break;
	case Qt::Key_M: vkCode = 'M'; keyText = "M"; break;
	case Qt::Key_N: vkCode = 'N'; keyText = "N"; break;
	case Qt::Key_O: vkCode = 'O'; keyText = "O"; break;
	case Qt::Key_P: vkCode = 'P'; keyText = "P"; break;
	case Qt::Key_Q: vkCode = 'Q'; keyText = "Q"; break;
	case Qt::Key_R: vkCode = 'R'; keyText = "R"; break;
	case Qt::Key_S: vkCode = 'S'; keyText = "S"; break;
	case Qt::Key_T: vkCode = 'T'; keyText = "T"; break;
	case Qt::Key_U: vkCode = 'U'; keyText = "U"; break;
	case Qt::Key_V: vkCode = 'V'; keyText = "V"; break;
	case Qt::Key_W: vkCode = 'W'; keyText = "W"; break;
	case Qt::Key_X: vkCode = 'X'; keyText = "X"; break;
	case Qt::Key_Y: vkCode = 'Y'; keyText = "Y"; break;
	case Qt::Key_Z: vkCode = 'Z'; keyText = "Z"; break;

	default:
		keyText = QString("Key Code: %1").arg(keyEvent->key());
		vkCode = keyEvent->key(); // 默认显示原始键码
		break;
	}
	e_obj->setText(keyText);
	if (e_obj == ui->lineEdit_4)hotK_1 = vkCode;
	if (e_obj == ui->lineEdit_8)hotK_2 = vkCode;
	if (e_obj == ui->lineEdit_12)hotK_3 = vkCode;

	return QMainWindow::eventFilter(obj, event);
}