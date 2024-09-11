#include "NetConfig.h"
#include "NetConfigTool.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QCheckBox>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMessageBox>
#include <QApplication>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <ShellAPI.h>
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

NetConfigTool::NetConfigTool(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::NetConfigTool)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	if (deskRect.width() < width() || deskRect.height() < height()) this->setBaseSize(deskRect.width(), deskRect.height());
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	this->setWindowFlags(this->windowFlags() &~Qt::WindowMaximizeButtonHint);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap(":/img/res/LOGO-AT-NTC.ico")));

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr != RPC_E_CHANGED_MODE && FAILED(hr)) {
		QMessageBox::information(nullptr, LU_STR44, LU_STR157);
		exit(0);
	}

	HINSTANCE  hDevMgr = LoadLibrary(_TEXT("devmgr.dll"));
	if (!hDevMgr) {
		QMessageBox::information(nullptr, LU_STR44, LU_STR158);
		exit(0);
	}
	pDeviceProperties = (PDEVICEPROPERTIES)GetProcAddress((HMODULE)hDevMgr, DeviceProperties_RunDLL);

	foreach(QCheckBox * i, this->findChildren<QCheckBox*>())
		connect(i, &QCheckBox::clicked, this, &NetConfigTool::NetCardRule);
	connect(ui->lineEdit, &QLineEdit::editingFinished, this, &NetConfigTool::NetCardRuleWord);

	LoadConfigure configure();
	NetCardRuleWord();
	SwitchLanguage();
	NetCardRule();
}
void NetConfigTool::SwitchLanguage(){
	ui->label_2->setText(LU_STR423);
	ui->label_3->setText(LU_STR424);
	ui->label_4->setText(LU_STR426);
	ui->pushButton_attr->setText(LU_STR425);
	ui->pushButton_ip_attr->setText(LU_STR425);
	ui->pushButton_set_firewall->setText(LU_STR427);
	ui->pushButton_task_->setText(LU_STR428);
	ui->pushButton_net_cen->setText(LU_STR429);
	ui->pushButton_fire_wall->setText(LU_STR431);
}

void NetConfigTool::NetCardRule() {
	netcard_rule[MIB_IF_TYPE_ETHERNET] = ui->checkBox_ETHERNET->isChecked();
	netcard_rule[MIB_IF_TYPE_TOKENRING] = ui->checkBox_TOKENRING->isChecked();
	netcard_rule[MIB_IF_TYPE_OTHER] = ui->checkBox_OTHER->isChecked();
	netcard_rule[MIB_IF_TYPE_FDDI] = ui->checkBox_FDDI->isChecked();
	netcard_rule[MIB_IF_TYPE_PPP] = ui->checkBox_PPP->isChecked();
	netcard_rule[MIB_IF_TYPE_LOOPBACK] = ui->checkBox_LOOPBACK->isChecked();
	netcard_rule[MIB_IF_TYPE_SLIP] = ui->checkBox_SLIP->isChecked();
	netcard_rule[IF_TYPE_IEEE80211] = ui->checkBox_WIFI->isChecked();

	EnumNetCard();
}
void NetConfigTool::NetCardRuleWord() {
	QString rule = ui->lineEdit->text();
	while (rule.size()) {
		QString arule;
		while (rule.size() && rule[0] != ',') {
			arule.push_back(rule[0]);
			rule.remove(0, 1);
		}
		rule.remove(0, 1);
		netcard_word.push_back(arule);
	}

	EnumNetCard();
}
void NetConfigTool::EnumNetCard() {
	ui->comboBox_net->clear();
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
			bool have_word = false;
			QString name = pIpInf->Description;
			for (auto i : netcard_word)have_word |= (name.lastIndexOf(i) >= 0);
			if (netcard_rule[pIpInf->Type] && !have_word) {
				ui->comboBox_net->addItem(name);
				net_hard[name.toStdString()] = pIpInf->AdapterName;
			}
			pIpInf = pIpInf->Next;
		}
	}
	if (pIpInf) delete pIpInf;
}

void NetConfigTool::on_pushButton_fire_wall_clicked() {
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
	QMessageBox::information(nullptr, LU_STR167, buf);
}

void NetConfigTool::on_pushButton_ip_attr_clicked() {
	QString text = ui->comboBox_net->currentText();
	std::string guid = net_hard[text.toStdString()];
	InvokeTCPIPProperties((HWND)this->winId(), guid.c_str());
}

void NetConfigTool::on_pushButton_attr_clicked() {
	QString text = ui->comboBox_net->currentText(); 
	std::string guid = GetDeviceID(text.toStdString());
	if (guid.empty())return;
	LPSTR pst = &guid[0];
	pDeviceProperties((HWND)this->winId(), NULL, pst, NULL);
}



void NetConfigTool::on_pushButton_set_firewall_clicked() {
	if (!is_Linux) WinExec("control /name Microsoft.WindowsFirewall /page pageConfigureSettings", SW_NORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
void NetConfigTool::on_pushButton_net_cen_clicked() {
	if (!is_Linux) WinExec("control /name Microsoft.NetworkAndSharingCenter", SW_NORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
void NetConfigTool::on_pushButton_task__clicked() {
	if (!is_Linux) ShellExecute(0, "open", "taskmgr.exe", "", "", SW_SHOWNORMAL);
	else QMessageBox::warning(nullptr, LU_STR44, LU_STR122);
}
