#include "IPv4cfgUi.h"
#include "LxNetCfgTool.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <QMessageBox>
#include <tchar.h>
#include <fstream>
#include <iostream>
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

LxNetcfgTool::LxNetcfgTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::LxNetcfgTool)
{
	ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	QRect deskRect = QApplication::desktop()->availableGeometry();//获取可用桌面大小
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);//让窗口居中显示
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap("./res/title.png")));
	::SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr != RPC_E_CHANGED_MODE && FAILED(hr)) {
		QMessageBox::information(this, LU_STR156, LU_STR157);
		exit(0);
	}

	HINSTANCE  hDevMgr = LoadLibrary(_TEXT("devmgr.dll"));
	if (hDevMgr) pDeviceProperties = (PDEVICEPROPERTIES)GetProcAddress((HMODULE)hDevMgr, DeviceProperties_RunDLL);
	else {
		QMessageBox::information(this, LU_STR156, LU_STR158);
		exit(0);
	}

	std::fstream* conf_file = new std::fstream;
	conf_file->open("./conf/LxViewerParam.lvp", std::ios::in | std::ios::binary);
	if (!conf_file->is_open()) is_zh_CN = 1;
	else is_zh_CN = (bool)conf_file->get();

	FindAllNetHard();
	SwitchLanguage();
}
void LxNetcfgTool::SwitchLanguage(){
	ui->label->setText(LU_STR422);
	ui->label_2->setText(LU_STR423);
	ui->label_3->setText(LU_STR424);
	ui->pushButton_attr->setText(LU_STR425);
	ui->pushButton_ip_attr->setText(LU_STR425);
	ui->label_4->setText(LU_STR426);
	ui->pushButton_set_firewall->setText(LU_STR427);
	ui->pushButton_task_->setText(LU_STR428);
	ui->pushButton_net_cen->setText(LU_STR429);
	ui->pushButton_ok->setText(LU_STR430);
	ui->label_5->setText(LU_STR167);
	ui->pushButton_fire_wall->setText(LU_STR431);
}

void LxNetcfgTool::FindAllNetHard() {
	PIP_ADAPTER_INFO pIpInf = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpInf, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		delete pIpInf;
		pIpInf = (PIP_ADAPTER_INFO)new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpInf, &stSize);
	}
	if (ERROR_SUCCESS == nRel) {
		while (pIpInf) {
			std::string name = pIpInf->Description;
			if ((pIpInf->Type == IF_TYPE_IEEE80211 || pIpInf->Type == MIB_IF_TYPE_ETHERNET) 
				&& (name.find("Virtual") == std::string::npos) && (name.find("Bluetooth") == std::string::npos)) {
				NETAttr na{ pIpInf->AdapterName ,pIpInf->Type == MIB_IF_TYPE_ETHERNET };
				net_hard[name] = na;
				ui->comboBox_net->addItem(name.c_str());
			}
			pIpInf = pIpInf->Next;
		}
	}
	if (pIpInf) delete pIpInf;
}

void LxNetcfgTool::on_pushButton_fire_wall_clicked() {
	char buf[256];
	sprintf_s(buf, LU_STR159);
	INetFwPolicy2* pNetFwPolicy2 = NULL;
	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	if (hr != RPC_E_CHANGED_MODE && FAILED(hr)) {
		sprintf_s(buf, LU_STR160, hr);
		return;
	}

	hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
	if (FAILED(hr)) goto Cleanup;

	VARIANT_BOOL status_do;
	pNetFwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, &status_do);
	hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, !status_do);
	if (FAILED(hr))  sprintf_s(buf, LU_STR161, hr);
	hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, !status_do);
	if (FAILED(hr)) sprintf_s(buf, LU_STR162, hr);
	hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, !status_do);
	if (FAILED(hr)) sprintf_s(buf, LU_STR163, hr);
	sprintf_s(buf, LU_STR164, status_do ? LU_STR165 : LU_STR166);
Cleanup:
	if (pNetFwPolicy2 != NULL)pNetFwPolicy2->Release();
	if (SUCCEEDED(hr))CoUninitialize();
	QMessageBox::information(this, LU_STR167, buf);
}

void LxNetcfgTool::on_pushButton_ip_attr_clicked() {
	QString text = ui->comboBox_net->currentText();
	std::string guid = net_hard[text.toStdString()].guid;
	InvokeTCPIPProperties((HWND)this->winId(), guid.c_str());
}

void LxNetcfgTool::on_pushButton_attr_clicked() {
	QString text = ui->comboBox_net->currentText(); 
	std::string guid = GetDeviceID(text.toStdString());
	if (guid.empty())return;
	LPSTR pst = &guid[0];
	pDeviceProperties((HWND)this->winId(), NULL, pst, NULL);
}

void LxNetcfgTool::on_pushButton_ok_clicked() { exit(0); }
void LxNetcfgTool::on_toolButton_close_clicked() { exit(0); }



static QPoint mouse_pos;
static bool is_move_mouse;
void LxNetcfgTool::mousePressEvent(QMouseEvent* event){
	if (event->button() == Qt::LeftButton){
		QRect rect = ui->widget->rect();
		rect.translate(ui->widget->pos());

		if (rect.contains(event->pos())){
			is_move_mouse = true;
			mouse_pos = event->screenPos().toPoint();
		}
	}

	QWidget::mousePressEvent(event);
}

void LxNetcfgTool::mouseReleaseEvent(QMouseEvent* event){
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}

void LxNetcfgTool::mouseMoveEvent(QMouseEvent* event){
	if (is_move_mouse) {
		QPoint newPos = this->pos() + (event->screenPos().toPoint() - mouse_pos);
		move(newPos);
		mouse_pos = event->screenPos().toPoint();
	}

	QWidget::mouseMoveEvent(event);
}

void LxNetcfgTool::on_pushButton_set_firewall_clicked() {
	if (!is_Linux) WinExec("control /name Microsoft.WindowsFirewall /page pageConfigureSettings", SW_NORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
void LxNetcfgTool::on_pushButton_net_cen_clicked() {
	if (!is_Linux) WinExec("control /name Microsoft.NetworkAndSharingCenter", SW_NORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
void LxNetcfgTool::on_pushButton_task__clicked() {
	if (!is_Linux) ShellExecute(0, "open", "taskmgr.exe", "", "", SW_SHOWNORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
