#include "Wireshark.h"
#include <iostream>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <thread>
#include <QLineEdit>
#include <fstream>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStringListModel>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "GlobalDefine.h"
#include <qdebug.h>
#include <thread>

using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

string GetEtProtocol(ushort et) {
	ushort et_t = (et >> 8) | (et << 8);
	switch (et_t) {
	case 0x0800: return "IPV4";           // IPv4
	case 0x0806: return "ARP";            // ARP（地址解析协议）
	case 0x0808: return "X.25";           // X.25协议
	case 0x0809: return "OSPF";           // OSPF（开放最短路径优先）
	case 0x080C: return "QNX";            // QNX协议
	case 0x0810: return "IPX";            // IPX（互联网包交换）
	case 0x0816: return "AppleTalk";      // AppleTalk协议
	case 0x0819: return "IETF";           // IETF协议
	case 0x0842: return "IS-IS";          // IS-IS（中间系统到中间系统）
	case 0x0860: return "LACP";           // LACP（链路聚合控制协议）
	case 0x086D: return "IPV6";           // IPv6
	case 0x089F: return "NETBEUI";        // NETBEUI（NetBIOS扩展用户接口）
	case 0x08A3: return "SNMP";           // SNMP（简单网络管理协议）
	case 0x08F0: return "RARP";           // RARP（反向地址解析协议）
	case 0x08F5: return "DECnet";         // DECnet协议
	case 0x2000: return "MPLS";           // MPLS（多协议标签交换）
	case 0x6000: return "以太网协议";      // 以太网协议
	case 0x8035: return "RARP";           // RARP（反向地址解析协议）
	case 0x8100: return "802.1Q 标签";    // 802.1Q VLAN 标签
	case 0x8200: return "IEEE 802.1ad";   // IEEE 802.1ad (QinQ)
	case 0x86DD: return "IPV6";           // IPv6
	case 0x8808: return "以太网II";        // Ethernet II
	case 0x8809: return "EAPoL";          // EAPoL（EAP over LAN）
	case 0x880B: return "802.1x";         // 802.1x（网络接入控制）
	case 0x8847: return "MPLS 标签";      // MPLS（多协议标签交换）
	case 0x8848: return "MPLS 标签";      // MPLS（多协议标签交换）
	case 0x8863: return "PPPoE";          // PPPoE（以太网上的点对点协议）
	case 0x8864: return "PPPoE Session";  // PPPoE（以太网上的点对点协议）
	case 0x8870: return "AT&T协议";       // AT&T 网络协议
	case 0x8871: return "AT&T VPD";       // AT&T 虚拟专用数据
	case 0x888E: return "EAPoL";          // EAPoL（EAP over LAN）
	case 0x9000: return "QinQ";           // QinQ VLAN 标签
	case 0xA88A: return "X.25";           // X.25协议
	case 0xF000: return "IEEE 802.1Q VLAN 标签"; // IEEE 802.1Q VLAN Tagging
	case 0xFEFE: return "供应商专用";     // 供应商专用协议
	case 0xFEC0: return "保留";           // 保留
	default: return "Unknown";
	}
}

string GetIntProtocol(uchar pt) {
	switch (pt) {
	case 0:   return "HOPOPT";
	case 1:   return "ICMP";
	case 2:   return "IGMP";
	case 3:   return "GGP";
	case 4:   return "IPv4";
	case 5:   return "ST";
	case 6:   return "TCP";
	case 7:   return "CBT";
	case 8:   return "EGP";
	case 9:   return "IGP";
	case 10:  return "BBN-RCC-MON";
	case 11:  return "NVP-II";
	case 12:  return "PUP";
	case 13:  return "ARGUS";
	case 14:  return "EMCON";
	case 15:  return "XNET";
	case 16:  return "CHAOS";
	case 17:  return "UDP";
	case 18:  return "MUX";
	case 19:  return "DCN-MEAS";
	case 20:  return "HMP";
	case 21:  return "PRM";
	case 22:  return "XNS-IDP";
	case 23:  return "TRUNK-1";
	case 24:  return "TRUNK-2";
	case 25:  return "LEAF-1";
	case 26:  return "LEAF-2";
	case 27:  return "RDP";
	case 28:  return "IRTP";
	case 29:  return "ISO-TP4";
	case 30:  return "NETBLT";
	case 31:  return "MFE-NSP";
	case 32:  return "MERIT-INP";
	case 33:  return "DCCP";
	case 34:  return "3PC";
	case 35:  return "IDPR";
	case 36:  return "XTP";
	case 37:  return "DDP";
	case 38:  return "IDPR-CMTP";
	case 39:  return "TP++";
	case 40:  return "IL";
	case 41:  return "IPv6";
	case 42:  return "SDRP";
	case 43:  return "IPv6-Route";
	case 44:  return "IPv6-Frag";
	case 45:  return "IDRP";
	case 46:  return "RSVP";
	case 47:  return "GRE";
	case 48:  return "DSR";
	case 49:  return "BNA";
	case 50:  return "ESP";
	case 51:  return "AH";
	case 52:  return "I-NLSP";
	case 53:  return "SWIPE";
	case 54:  return "NARP";
	case 55:  return "Min-IPv4";
	case 56:  return "TLSP";
	case 57:  return "SKIP";
	case 58:  return "IPv6-ICMP";
	case 59:  return "IPv6-NoNxt";
	case 60:  return "IPv6-Opts";
	case 61:  return "Any host internal protocol";
	case 62:  return "CFTP";
	case 63:  return "Any local network";
	case 64:  return "SAT-EXPAK";
	case 65:  return "KRYPTOLAN";
	case 66:  return "RVD";
	case 67:  return "IPPC";
	case 68:  return "Any distributed file system";
	case 69:  return "SAT-MON";
	case 70:  return "VISA";
	case 71:  return "IPCV";
	case 72:  return "CPNX";
	case 73:  return "CPHB";
	case 74:  return "WSN";
	case 75:  return "PVP";
	case 76:  return "BR-SAT-MON";
	case 77:  return "SUN-ND";
	case 78:  return "WB-MON";
	case 79:  return "WB-EXPAK";
	case 80:  return "ISO-IP";
	case 81:  return "VMTP";
	case 82:  return "SECURE-VMTP";
	case 83:  return "VINES";
	case 84:  return "IPTM";
	case 85:  return "NSFNET-IGP";
	case 86:  return "DGP";
	case 87:  return "TCF";
	case 88:  return "EIGRP";
	case 89:  return "OSPFIGP";
	case 90:  return "Sprite-RPC";
	case 91:  return "LARP";
	case 92:  return "MTP";
	case 93:  return "AX.25";
	case 94:  return "IPIP";
	case 95:  return "MICP";
	case 96:  return "SCC-SP";
	case 97:  return "ETHERIP";
	case 98:  return "ENCAP";
	case 99:  return "Any private encryption scheme";
	case 100: return "GMTP";
	case 101: return "IFMP";
	case 102: return "PNNI";
	case 103: return "PIM";
	case 104: return "ARIS";
	case 105: return "SCPS";
	case 106: return "QNX";
	case 107: return "A/N";
	case 108: return "IPComp";
	case 109: return "SNP";
	case 110: return "Compaq-Peer";
	case 111: return "IPX-in-IP";
	case 112: return "VRRP";
	case 113: return "PGM";
	case 114: return "Any 0-hop protocol";
	case 115: return "L2TP";
	case 116: return "DDX";
	case 117: return "IATP";
	case 118: return "STP";
	case 119: return "SRP";
	case 120: return "UTI";
	case 121: return "SMP";
	case 122: return "SM";
	case 123: return "PTP";
	case 124: return "ISIS over IPv4";
	case 125: return "FIRE";
	case 126: return "CRTP";
	case 127: return "CRUDP";
	case 128: return "SSCOPMCE";
	case 129: return "IPLT";
	case 130: return "SPS";
	case 131: return "PIPE";
	case 132: return "SCTP";
	case 133: return "FC";
	case 134: return "RSVP-E2E-IGNORE";
	case 135: return "Mobility Header";
	case 136: return "UDPLite";
	case 137: return "MPLS-in-IP";
	case 138: return "MANET Protocols";
	case 139: return "HIP";
	case 140: return "Shim6";
	case 141: return "WESP";
	case 142: return "ROHC";
	case 143: return "Ethernet";
	case 144: return "AGGFRAG";
	case 145: return "NSH";
	case 146: case 147: case 148: case 149:
	case 150: case 151: case 152: case 153:
	case 154: case 155: case 156: case 157:
	case 158: case 159: case 160: case 161:
	case 162: case 163: case 164: case 165:
	case 166: case 167: case 168: case 169:
	case 170: case 171: case 172: case 173:
	case 174: case 175: case 176: case 177:
	case 178: case 179: case 180: case 181:
	case 182: case 183: case 184: case 185:
	case 186: case 187: case 188: case 189:
	case 190: case 191: case 192: case 193:
	case 194: case 195: case 196: case 197:
	case 198: case 199: case 200: case 201:
	case 202: case 203: case 204: case 205:
	case 206: case 207: case 208: case 209:
	case 210: case 211: case 212: case 213:
	case 214: case 215: case 216: case 217:
	case 218: case 219: case 220: case 221:
	case 222: case 223: case 224: case 225:
	case 226: case 227: case 228: case 229:
	case 230: case 231: case 232: case 233:
	case 234: case 235: case 236: case 237:
	case 238: case 239: case 240: case 241:
	case 242: case 243: case 244: case 245:
	case 246: case 247: case 248: case 249:
	case 250: case 251: case 252: return "Unassigned";
	case 253: return "Experimental/Testing";
	case 254: return "Experimental/Testing";
	case 255: return "Reserved";
	default: return "Unknown";
	}
}

QImage MatToQImage(const cv::Mat& cvImage) {
	vector<uchar> imgBuf;
	cv::imencode(".bmp", cvImage, imgBuf);
	QByteArray baImg((char*)imgBuf.data(), static_cast<int>(imgBuf.size()));
	QImage image;
	image.loadFromData(baImg, "BMP");
	return image;
}

#ifdef _WIN32
string wideCharToMultiByte(wchar_t* pWCStrKey){
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	string str_ = pCStrKey;
	delete pCStrKey;
	return str_;
}
#endif

void packet_back(u_char* pt, const struct pcap_pkthdr* header, const u_char* pkt_data) {
	auto this_ = (Wireshark*)pt;
	if (this_->stopping)return;
	PACKAGE pak;
	pak.header = *header;
	pak.msg.append((char*)pkt_data, header->caplen);
	this_->packets.insert(this_->packets.end(),pak);
}

ushort S2B_S(const char* val) {
	uchar buf[2]{ (uchar)val[1],(uchar)val[0] };
	ushort _val;
	memmove(&_val, buf, 2);
	return _val;
}
uint S2B_I(const char* val) {
	uchar buf[4]{ (uchar)val[3],(uchar)val[2],(uchar)val[1],(uchar)val[0] };
	uint _val;
	memmove(&_val, buf, 4);
	return _val;
}
ulong S2B_L(const char* val) {
	uchar buf[8]{ (uchar)val[7],(uchar)val[6],(uchar)val[5],(uchar)val[4],(uchar)val[3],(uchar)val[2],(uchar)val[1],(uchar)val[0] };
	ulong _val;
	memmove(&_val, buf, 8);
	return _val;
}

Wireshark::Wireshark(QWidget* parent) :
	QMainWindow(parent), ui(new Ui::Wireshark)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();//获取可用桌面大小
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);//让窗口居中显示
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(":/img/res/LOGO-AT-Wireshark.ico"));

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)ErrExit(LU_STR123);
#endif

	ui->treeWidget_netCard->setColumnWidth(0, 360);
	ui->treeWidget_netCard->setColumnWidth(1, 240);
	ui->treeWidget_netCard->setColumnWidth(2, 120);
	ui->treeWidget_netCard->setColumnWidth(3, 150);

	ui->widget_2->setVisible(false);
	on_pushButton_net_refresh_clicked();

	connect(ui->tableWidget_pack, &QTableWidget::currentItemChanged, this, &Wireshark::Pack_itemClicked);
	connect(ui->tableWidget_value, &QTableWidget::itemClicked, this, &Wireshark::Value_itemClicked);
	
	connect(ui->treeWidget_netCard, &QTreeWidget::itemDoubleClicked, this, &Wireshark::netCard_itemdoubleClicked);
	connect(ui->treeWidget_netCard, &QTreeWidget::itemClicked, this, &Wireshark::netCard_itemClicked);
	connect(&refresh, &QTimer::timeout, this, &Wireshark::RefreshSpeed);
	refresh.start(1000);
	connect(&packview, &QTimer::timeout, this, &Wireshark::AnalysisPackages);

	connect(ui->treeWidget_protocol, &QTreeWidget::itemClicked, this, &Wireshark::Protocol_itemClicked);
	ui->tableWidget_pack->verticalScrollBar()->installEventFilter(this);

	ui->tableWidget_pack->setColumnWidth(0, 88);
	ui->tableWidget_pack->setColumnWidth(1, 180);
	ui->tableWidget_pack->setColumnWidth(2, 120);
	ui->tableWidget_pack->setColumnWidth(3, 120);
	ui->tableWidget_pack->setColumnWidth(4, 100);
	ui->tableWidget_pack->setColumnWidth(5, 100);
	ui->tableWidget_pack->setColumnWidth(6, 66);
	ui->tableWidget_pack->setColumnWidth(7, 480);
	ui->tableWidget_pack->verticalScrollBar()->hide();
	ui->tableWidget_pack->verticalHeader()->setHidden(true);
	ui->tableWidget_pack->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: black; color: white; }");
	ui->tableWidget_pack->verticalHeader()->setStyleSheet("QHeaderView::section { background-color: black; color: white; }");

	ui->tableWidget_value->setShowGrid(false);
	for(int i=0;i<16;i++)
		ui->tableWidget_value->setColumnWidth(i, 20);
	ui->tableWidget_value->setColumnWidth(16, 220);
	ui->tableWidget_value->verticalHeader()->hide();
	ui->tableWidget_value->verticalHeader()->setDefaultSectionSize(20);

	QStringList options;
	for (auto i : command)options << i.first;
	completer = new QCompleter(options, this);
	completer->setFilterMode(Qt::MatchContains);
	ui->lineEdit_cmd->setCompleter(completer);

	SwitchLanguage();
}
void Wireshark::SwitchLanguage() {
	ui->pushButton_net_refresh->setText(LU_STR390);
	QStringList heardList;
	QString descirbe = is_Linux?"MTU":"GUID";
	if (is_CH)heardList << "网卡" << "流量" << "峰值(Mbps)" << "MAC" << descirbe;
	else heardList << "Network card" << "Flow" << "Peak value(Mbps)" << "MAC" << descirbe;
	ui->treeWidget_netCard->setHeaderLabels(heardList);

	ui->toolButton_start->setText(LU_STR414);
	ui->toolButton_stop->setText(LU_STR415);
	ui->toolButton_over->setText(LU_STR416);
	ui->toolButton_save->setText(LU_STR417);
	ui->toolButton_clear->setText(LU_STR418);
	ui->label_2->setText(LU_STR420);
}

void Wireshark::RefreshSpeed() {
	if (start_main_speed_get && !ch_guid.empty()) {
		DWORD val[3]{ 0,0,0 };
		if (!GetSpeedAndByte(ch_guid, val[0], val[1], val[2]))return;
		float fval = ((val[1] - speed_val.inout[0]) + (val[2] - speed_val.inout[1])) / 1000000.f;
		speed_val.inout[0] = val[1], speed_val.inout[1] = val[2];
		if (speed_val.first_) { speed_val.first_ = false; return; }
		else if (speed_val.val.size() == 100) speed_val.val.pop_front();
		speed_val.val.push_back(fval);

		float top = 0;
		for (auto x : speed_val.val)top = x > top ? x : top;
		cv::Mat img = cv::Mat(20, 240, CV_8UC3, cv::Scalar(33, 29, 27));
		int pos = 0;
		for (auto x : speed_val.val) {
			for (int j = 0; j < 20.f / top * x; j++)
				if (j <= 19) img.at<cv::Vec3b>(19 - j, pos) = { 255,255,0 };
			pos++;
		}

		ui->label_3->setPixmap(QPixmap::fromImage(MatToQImage(img)).scaled({ 240,15 }));
		return;
	}
	for (int i = 0; i < ui->treeWidget_netCard->topLevelItemCount(); i++) {
		auto item = ui->treeWidget_netCard->topLevelItem(i);
		DWORD val[3]{ 0,0,0 };
		string guid;
		if (!is_Linux) guid = item->data(4, 0).toString().toStdString();
		else guid = item->data(0, 0).toString().toStdString();
		if (!GetSpeedAndByte(guid, val[0], val[1], val[2]))continue;
		float fval = ((val[1] - all_speed[guid].inout[0]) + (val[2] - all_speed[guid].inout[1])) / 1000000.f;
		all_speed[guid].inout[0] = val[1], all_speed[guid].inout[1] = val[2];
		if (all_speed[guid].first_) { all_speed[guid].first_ = false; continue; }
		else if (all_speed[guid].val.size() == 100) all_speed[guid].val.pop_front();
		all_speed[guid].val.push_back(fval);

		float top = 0;
		for (auto x : all_speed[guid].val)top = (x > top ? x : top);
		if (!top)continue;
		cv::Mat img = cv::Mat(20, 100, CV_8UC3, cv::Scalar(33, 29, 27));
		int pos = 0;
		for (auto x : all_speed[guid].val) {
			for (int j = 0; j < 20.f / top * x; j++)
				if (j <= 19) img.at<cv::Vec3b>(19 - j, pos) = { 255,255,0 };
			pos++;
		}

		all_speed[guid].p11->setPixmap(QPixmap::fromImage(MatToQImage(img)).scaled({ 240,15 }));
		ui->treeWidget_netCard->setItemWidget(item, 1, all_speed[guid].widget1);
		item->setText(2, QString::number(top));
	}
}

bool Wireshark::GetNetCardList() {
	nifl.clear();
#ifdef _WIN32
	PIP_ADAPTER_INFO pA = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pA, &stSize)){
		delete pA;
		pA = (PIP_ADAPTER_INFO)new BYTE[stSize];
		if (ERROR_SUCCESS != GetAdaptersInfo(pA, &stSize)) return false;
	}

	while (pA){
		NetINFO nif{ pA->AdapterName,pA->Description };
		for (DWORD i = 0; i < pA->AddressLength; i++) {
			char buf[5];
			sprintf_s(buf, "%02X:", pA->Address[i]);
			nif.mac += buf;
		}
		nif.mac.pop_back();

		IP_ADDR_STRING* IpA = &(pA->IpAddressList);
		while (IpA) {
			NIpINFO nip{ IpA->IpAddress.String,IpA->IpMask.String,pA->GatewayList.IpAddress.String };
			nif.ipd.push_back(nip);
			IpA = IpA->Next;
		}
		pA = pA->Next;
		nifl.push_back(nif);
	}
	delete pA;
#else
	char buf[512];
	ifconf ifconf{ 512,{buf} };
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(sockfd, SIOCGIFCONF, &ifconf) == -1) return false;
	for (int i = 0; i < ifconf.ifc_len / sizeof(ifreq); i++){
		ifreq* ifr = (struct ifreq*)buf + i;
		NetINFO nif{ "", ifr->ifr_name};

		struct ifreq        ifrq;
		strcpy(ifrq.ifr_name, ifr->ifr_name);
		if (ioctl(sockfd, SIOCGIFMTU, &ifrq) < 0)continue;
		nif.name = to_string(ifrq.ifr_mtu);
		if (ioctl(sockfd, SIOCGIFHWADDR, &ifrq) < 0)continue;
		for (uchar i = 0; i < 6; i++) {
			char buf[5];
			sprintf(buf, "%02X:", (uchar)ifrq.ifr_hwaddr.sa_data[i]);
			nif.mac += buf;
		}
		nif.mac.pop_back();

		NIpINFO nip;
		if (ioctl(sockfd, SIOCGIFADDR, &ifrq) < 0)continue;
		nip.ip = inet_ntoa(((struct sockaddr_in*)&(ifrq.ifr_addr))->sin_addr);
		if (ioctl(sockfd, SIOCGIFNETMASK, &ifrq) < 0)continue;
		nip.mask = inet_ntoa(((struct sockaddr_in*)&(ifrq.ifr_addr))->sin_addr);
		nip.getway = "0.0.0.0";
		nif.ipd.push_back(nip);
		nifl.push_back(nif);
	}
	::close(sockfd);
#endif
	return true;
}

void Wireshark::RefreshListView() {
	all_speed.clear();
	ui->treeWidget_netCard->clear();
	for (auto i : nifl){
		QTreeWidgetItem* tim = new QTreeWidgetItem;
		tim->setText(0, i.description.c_str());
		tim->setText(3, i.mac.c_str());
		tim->setText(4, i.name.c_str());
		for (auto j : i.ipd) {
			QTreeWidgetItem* timc = new QTreeWidgetItem;
			timc->setText(0, j.ip.c_str());
			timc->setText(1, j.mask.c_str());
			timc->setText(2, j.getway.c_str());
			tim->addChild(timc);
		}
		ui->treeWidget_netCard->addTopLevelItem(tim);
	}
	ui->pushButton_net_start->setEnabled(false);
}

bool Wireshark::GetSpeedAndByte(string guid, DWORD& speed, DWORD& in, DWORD& out) {
#ifdef _WIN32
	MIB_IFTABLE* pIfTable = NULL;
	ULONG          dwSize = 0;
	if (GetIfTable(pIfTable, &dwSize, TRUE) != 122L)ErrExit(LU_STR124);

	pIfTable = (MIB_IFTABLE*) new char[dwSize];
	if (pIfTable == NULL)ErrExit(LU_STR125);
	if (GetIfTable(pIfTable, &dwSize, TRUE) != 0L)ErrExit(LU_STR124);

	for (int i = 0; i < pIfTable->dwNumEntries; i++) {
		string guid_ = wideCharToMultiByte((pIfTable->table[i]).wszName);
		guid_ = guid_.substr(guid_.find('{'));
		if (guid_ != guid)continue;

		speed = (pIfTable->table[i]).dwSpeed;
		in = (pIfTable->table[i]).dwInOctets * 8;
		out = (pIfTable->table[i]).dwOutOctets * 8;
	}
	free(pIfTable);
	return true;
#else
	FILE* net_dev_file;
	char buffer[1024];
	if ((net_dev_file = fopen("/proc/net/dev", "r")) == NULL)ErrExit(LU_STR124);
	size_t bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);
	fclose(net_dev_file);
	if (bytes_read == 0)return false;
	buffer[bytes_read] = '\0';
	string file_val = buffer;
	if (file_val.find(guid) == string::npos)return false;
	auto val_s = file_val.substr(file_val.find(guid.c_str()) + guid.size() + 2);
	unsigned long save_rate = atoll(val_s.c_str());
	if (!save_rate)return false;

	speed = 0;
	in = save_rate / 2;
	out = save_rate / 2;
	return true;
#endif
}

void Wireshark::netCard_itemClicked(QTreeWidgetItem* item){
	if (item->childCount()) item->setExpanded(!item->isExpanded());
	ui->pushButton_net_start->setEnabled(true);
}

void Wireshark::on_pushButton_net_start_clicked() {
	netCard_itemdoubleClicked(ui->treeWidget_netCard->currentItem());
}
void Wireshark::netCard_itemdoubleClicked(QTreeWidgetItem* item) {
	socket(AF_INET, SOCK_RAW, 0);
	if (item->parent())item = item->parent();
	if (!is_Linux) ch_guid = item->text(4).toStdString();
	else ch_guid = item->text(0).toStdString();

	IpToNet();
	char errorBuffer[PCAP_ERRBUF_SIZE];
	pcap_handle = pcap_open(ch_name.c_str(), 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errorBuffer);
	if (pcap_handle == nullptr) {
		QMessageBox::warning(nullptr, LU_STR1, QString(LU_STR126) + errorBuffer);
		ch_guid = ch_name = "";
		return;
	}
	start_main_speed_get = true;
	on_toolButton_start_clicked();
	ui->widget_6->setVisible(false);
	ui->widget_2->setVisible(true);
	speed_val.val = all_speed[ch_guid].val;
	ui->verticalScrollBar->setMaximum(0);

	auto thread_ptr = new std::thread(pcap_loop, pcap_handle, 0, packet_back, (unsigned char*)this);
	thread_ptr->detach();
	packview.start(50);
}

string Wireshark::ProtocolInfo(string& raw, string val) {
	string info = "";
	if (raw == "ICMP" && val.size() >= 35) {
		if (val.data()[34] == 0 && val.data()[35] == 0) info = LU_STR127;
		else if (val.data()[34] == 8 && val.data()[35] == 0) info = LU_STR128;
		else if (val.data()[34] == 3 && val.data()[35] == 0) info = LU_STR129;
		else if (val.data()[34] == 3 && val.data()[35] == 1) info = LU_STR130;
		else if (val.data()[34] == 3 && val.data()[35] == 2) info = LU_STR131;
		else if (val.data()[34] == 3 && val.data()[35] == 3) info = LU_STR132;
		else if (val.data()[34] == 3 && val.data()[35] == 4) info = LU_STR133;
		else if (val.data()[34] == 3 && val.data()[35] == 13) info = LU_STR134;
		else if (val.data()[34] == 5 && val.data()[35] == 1) info = LU_STR135;
		else if (val.data()[34] == 11 && val.data()[35] == 0) info = LU_STR136;
		else if (val.data()[34] == 12 && val.data()[35] == 0) info = LU_STR137;
		else info = to_string(val.data()[34]) + " : " + to_string(val.data()[35]);
	}
	else if (raw == "UDP") {
		if (val.size() < 39)return info;
		ushort src_port = S2B_S(val.data() + 34);
		ushort dst_port = S2B_S(val.data() + 36);
		if (src_port == 3956 || dst_port == 3956) {
			if (src_port == 3956) {
				ushort id = S2B_S(val.data() + 48);
				uint reg_val = S2B_I(val.data() + 50);
				if ((uchar)val.data()[45] == 0x81)info = " < READREG_ACK";
				else if ((uchar)val.data()[45] == 0x83)info = " < WRITEREG_ACK";
				else if ((uchar)val.data()[45] == 0x85)info = " < READMEME_ACK";
				else info = asciiTohex(val.substr(44, 2), false);

				info += "   ID = " + to_string(id) + "   Val =" + to_string(reg_val);
			}
			else {
				ushort id = S2B_S(val.data() + 48);
				if ((uchar)val.data()[45] == 0x80)info = " > READREG_CMD";
				else if ((uchar)val.data()[45] == 0x82)info = " > WRITEREG_CMD";
				else if ((uchar)val.data()[45] == 0x84)info = " > READMEME_CMD";
				else info = asciiTohex(val.substr(44, 2), false);

				info += "   ID = " + to_string(id) + "   [Addr: 0x" + asciiTohex(val.substr(50, 4)) + "]";
			}
			raw = "GVCP";
		}
		else if (src_port == 3959 || dst_port == 3959) {
			uchar format = val.data()[46];
			ulong block_id = S2B_L(val.data() + 50);
			uint pack_ic = S2B_I(val.data() + 58);
			if (format == 0x81)info = " LEADER";
			else if (format == 0x82)info = " TRAILER";
			else if (format == 0x83)info = " PAYLOAD";
			else info = asciiTohex(val.substr(44, 2), false);

			info += " [Block ID: " + to_string(block_id) + "   Package ID: " + to_string(pack_ic) + "]";
			raw = "GVSP";
		}
	}

	if (info.empty()) {
		if (val.size() < 39)return info;
		ushort src_port = S2B_S(val.data() + 34);
		ushort dst_port = S2B_S(val.data() + 36);
		info = to_string(src_port) + " → " + to_string(dst_port) +"  Length: " + to_string(val.size() - 42);
	}

	return info;
}

void Wireshark::on_verticalScrollBar_valueChanged(int val) {
	if (is_execute)return;
	is_execute = true;

	if (ui->tableWidget_pack->rowCount() != pack_row_num) {
		ui->tableWidget_pack->clear();
		static QStringList strs = { "No.", "Time", "Souce","Destination","EtherType","Protocol","Length","Info" };
		ui->tableWidget_pack->setHorizontalHeaderLabels(strs);
		ui->tableWidget_pack->setRowCount(pack_row_num);
		for (ushort i = 0; i < pack_row_num; i++)
			for (int j = 0; j < 8; j++)ui->tableWidget_pack->setItem(i, j, new QTableWidgetItem());
	}

	QString s_ip = command["src.ip="];
	QString d_ip = command["dst.ip="];
	QString s_mac = command["src.mac="];
	QString d_mac = command["dst.mac="];
	QString s_port = command["src.port="];
	QString d_port = command["dst.port="];
	QString d_length = command["data.length="];

	for (ushort i = 0; i < pack_row_num;i++ )
			for (int j = 0; j < 8; j++)ui->tableWidget_pack->item(i, j)->setText("");
	ui->tableWidget_pack->clearSelection();

	unsigned int index = 0;
	for (ushort i = 0; i < pack_row_num; ) {
		unsigned long long number = val * 3 + index++;
		if (number >= packets.size())break;

		auto it = packets.begin();
		std::advance(it, number);
		if (!it->head.is_analysis)AnalysisPackageHead(it);

		if (!s_ip.isEmpty() && s_ip != it->head.inter.src_ip)continue;
		if (!d_ip.isEmpty() && d_ip != it->head.inter.dst_ip)continue;
		if (!s_mac.isEmpty() && s_mac != it->head.eth.src_mac)continue;
		if (!d_mac.isEmpty() && d_mac != it->head.eth.dst_mac)continue;
		if (!s_port.isEmpty() && s_port != it->head.user.src_port)continue;
		if (!d_port.isEmpty() && d_port != it->head.user.dst_port)continue;
		if (!d_length.isEmpty() && d_length != QString::number(it->msg.size() - 42))continue;

		auto intr_protocol = it->head.inter.protocol.toStdString();
		ui->tableWidget_pack->item(i, 0)->setText(QString::number(number));
		ui->tableWidget_pack->item(i, 1)->setText(GetTime(it->header.ts).c_str());
		ui->tableWidget_pack->item(i, 2)->setText(it->head.inter.src_ip);
		ui->tableWidget_pack->item(i, 3)->setText(it->head.inter.dst_ip);
		ui->tableWidget_pack->item(i, 4)->setText(it->head.eth.protocol);
		ui->tableWidget_pack->item(i, 5)->setText(it->head.inter.protocol);
		ui->tableWidget_pack->item(i, 6)->setText(QString::number(it->header.caplen));
		ui->tableWidget_pack->item(i, 7)->setText(ProtocolInfo(intr_protocol, it->msg).c_str());

		if(current_index == number)ui->tableWidget_pack->setCurrentCell(i, 0);
		i++;
	}
	is_execute = false;
}

void Wireshark::AnalysisPackageHead(std::list<PACKAGE>::iterator package) {
	if (package->head.is_analysis)return;

	string* _val = &package->msg;
	const char* p_data = _val->data();
	//----------------------------------------------------------------------------------------------------------------------------------
	string dst_mac = asciiTohex(_val->substr(0, 6), false);
	string src_mac = asciiTohex(_val->substr(6, 6), false);
	for (int i = 10; i > 1; i -= 2)src_mac.insert(i, 1, ':'), dst_mac.insert(i, 1, ':');

	package->head.eth.src_mac = src_mac.c_str();
	package->head.eth.dst_mac = dst_mac.c_str();
	package->head.eth.protocol = GetEtProtocol(*(ushort*)&p_data[12]).c_str();
	package->head.eth.protocol_hex = asciiTohex(_val->substr(12, 2), false).c_str();
	//----------------------------------------------------------------------------------------------------------------------------------
	string net_src, net_dst, net_fag;
	char net_head = p_data[14];
	string net_ver = to_string((int)net_head >> 4);
	string net_len = to_string((int)net_head & 0x0f) + " * 4 (bytes)";
	string net_sev = "0x" + asciiTohex(_val->substr(15, 1), false);
	string net_tle = to_string(S2B_S(p_data + 16));
	string net_ide = "0x" + asciiTohex(_val->substr(18, 2), false) + "(" + to_string(S2B_S(p_data + 18)) + ")";
	for (uchar i = 0x80; i; i >>= 1)net_fag += p_data[20] & i ? "1" : "0";
	for (uchar i = 0x80; i; i >>= 1)net_fag += p_data[21] & i ? "1" : "0";
	for (int i = 26; i < 30; i++)net_src += to_string((uchar)p_data[i]) + "."; net_src.pop_back();
	for (int i = 30; i < 34; i++)net_dst += to_string((uchar)p_data[i]) + "."; net_dst.pop_back();
	string net_tim = to_string((int)p_data[22]);
	string net_pro = GetIntProtocol(p_data[23]) + "(" + to_string((uchar)p_data[23]) + ")";
	string net_chk = "0x" + asciiTohex(_val->substr(24, 2), false);
	string internet = "Internet Protocol Version  " + net_ver + ", Ip: " + net_src + " → " + net_dst;

	package->head.inter.version = net_ver.c_str();
	package->head.inter.head_len = net_len.c_str();
	package->head.inter.services = net_sev.c_str();
	package->head.inter.total_len = net_tle.c_str();
	package->head.inter.identification = net_ide.c_str();
	package->head.inter.flags = net_fag.c_str();
	package->head.inter.live = net_tim.c_str();
	package->head.inter.protocol = net_pro.c_str();
	package->head.inter.checksum = net_chk.c_str();
	package->head.inter.src_ip = net_src.c_str();
	package->head.inter.dst_ip = net_dst.c_str();
	////----------------------------------------------------------------------------------------------------------------------------------
	string usr_src = to_string(S2B_S(p_data + 34));
	string usr_dst = to_string(S2B_S(p_data + 36));
	string usr_len = to_string(S2B_S(p_data + 38));
	string usr_chk = "0x" + asciiTohex(_val->substr(40, 2), false);
	package->head.user.length = usr_len.c_str();
	package->head.user.checksum = usr_chk.c_str();
	package->head.user.src_port = usr_src.c_str();
	package->head.user.dst_port = usr_dst.c_str();
	////----------------------------------------------------------------------------------------------------------------------------------
	package->head.is_analysis = true;
}
void Wireshark::AnalysisPackages() {
	int table_height = ui->tableWidget_pack->height() - ui->tableWidget_pack->horizontalHeader()->height();
	int row_height = ui->tableWidget_pack->rowHeight(0);
	if (row_height <= 0)row_height = 20;
	pack_row_num = table_height / row_height;

	ui->verticalScrollBar->setMaximum(packets.size() / 3);
	on_verticalScrollBar_valueChanged(ui->verticalScrollBar->value());
}

void Wireshark::IpToNet(){
	ch_name = "";
	char buf[512];
	pcap_if_t* all;
	if (pcap_findalldevs(&all, buf) == -1)return;
	for (; all != nullptr && ch_name.empty(); all = all->next) {
		for (pcap_addr_t* a = all->addresses; a != nullptr && ch_name.empty(); a = a->next) {
			string _guid = all->name;
			if(!is_Linux){
				int f_ = _guid.find('{'), l_ = _guid.find('}');
				if (f_ == string::npos || l_ == string::npos)continue;
				if (_guid.substr(f_, l_ - f_ + 1) == ch_guid)ch_name = all->name;
			}
			else if (_guid == ch_guid)ch_name = all->name;
		}
	}
	pcap_freealldevs(all);
}

auto SetVal = [](QString msg, QString tips, QTreeWidgetItem* parten = nullptr)->QTreeWidgetItem* {
	QTreeWidgetItem* _t = new QTreeWidgetItem;
	_t->setText(0, msg); _t->setWhatsThis(0, tips);
	if (parten)parten->addChild(_t);
	return _t;
};
void Wireshark::AnalysisGigE(string msg) {
	ushort usr_src = S2B_S(msg.data() + 34);
	ushort dst_src = S2B_S(msg.data() + 36);
	QTreeWidgetItem* data;
	if (usr_src == 3956 || dst_src == 3956) {
		data = SetVal("GigE Vision Control Protocol", "42,0");
		if (usr_src == 3956 && (msg.size() >= 50)) {
			string status_ = "0x" + asciiTohex(msg.substr(42, 2), false);
			string ack = "0x" + asciiTohex(msg.substr(44, 2), false);
			string len = "0x" + asciiTohex(msg.substr(46, 2), false);
			string id = "0x" + asciiTohex(msg.substr(48, 2), false);
			string reg = "0x" + asciiTohex(msg.substr(50), false);
			SetVal(QString("Status: ") + status_.c_str(), "42,2", data);
			SetVal(QString("Acknowledge: ") + ack.c_str(), "44,2", data);
			SetVal(QString("Payload Length: ") + len.c_str(), "46,2", data);
			SetVal(QString("Request Id: ") + id.c_str(), "48,2", data);
			SetVal(QString("Register: ") + reg.c_str(), "50,0", data);
		}
		else {
			string key_ = "0x" + asciiTohex(msg.substr(42, 12), false);
			string flags = "0x" + asciiTohex(msg.substr(43, 1), false);
			string cmd = "0x" + asciiTohex(msg.substr(44, 2), false);
			string pay = "0x" + asciiTohex(msg.substr(46, 2), false);
			string id = "0x" + asciiTohex(msg.substr(48, 2), false);
			string reg = "0x" + asciiTohex(msg.substr(50, 4), false);
			string count = "NONE";
			if (msg.size() > 54) count = "0x" + asciiTohex(msg.substr(54, 2), false);
			SetVal(QString("Message Key Code: ") + key_.c_str(), "42,1", data);
			SetVal(QString("Flags: ") + flags.c_str(), "43,1", data);
			SetVal(QString("Command: ") + cmd.c_str(), "44,2", data);
			SetVal(QString("Payload Length: ") + pay.c_str(), "46,2", data);
			SetVal(QString("Request Id: ") + id.c_str(), "48,2", data);
			SetVal(QString("Memory Bootstrap Register: ") + reg.c_str(), "50,4", data);
			SetVal(QString("Register: ") + count.c_str(), "54,2", data);
		}
	}
	else if (((usr_src == 3959) || (dst_src == 3959)) && (msg.size() >= 93)) {
		data = SetVal("GigE Vision Streaming Protocol", "42,0");
		string status_ = "0x" + asciiTohex(msg.substr(42, 2), false);
		string flags_  = "0x" + asciiTohex(msg.substr(44, 2), false);
		string format_ = "0x" + asciiTohex(msg.substr(46, 1), false);
		uchar format = msg.data()[46];
		ulong block_id = S2B_L(msg.data() + 50);
		uint pack_id = S2B_I(msg.data() + 58);
		SetVal(QString("Status: ") + status_.c_str(), "42,2", data);
		SetVal(QString("Flags: ") + flags_.c_str(), "44,2", data);
		SetVal(QString("Format: ") + format_.c_str(), "46,1", data);
		SetVal(QString("Block Id: %1").arg(block_id), "50,8", data);
		SetVal(QString("Pack Id: %1").arg(pack_id), "58,4", data);
		if (format == 0x81) {
			string filed_ = "0x" + asciiTohex(msg.substr(62, 1), false);
			string gener_ = "0x" + asciiTohex(msg.substr(63, 1), false);
			string type_ = "0x" + asciiTohex(msg.substr(64, 2), false);
			string times_ = "0x" + asciiTohex(msg.substr(66, 8), false);
			SetVal(QString("Filed Info: ") + filed_.c_str(), "62,1", data);
			SetVal(QString("Generic Flags: ") + gener_.c_str(), "63,1", data);
			SetVal(QString("Payload Type: ") + type_.c_str(), "64,2", data);
			SetVal(QString("Timestamp: ") + times_.c_str(), "66,8", data);
			if (msg.data()[65] == 7) {
				ulong data_ = S2B_L(msg.data() + 74);
				string tim_ = "0x" + asciiTohex(msg.substr(82, 8), false);
				string val_ = "0x" + asciiTohex(msg.substr(90, 4), false);
				SetVal(QString("Payload Data Size: %1").arg(data_), "74,8", data);
				SetVal(QString("Timestamp Tick: ") + tim_.c_str(), "82,8", data);
				SetVal(QString("Data Format: ") + val_.c_str(), "90,4", data);
			}
			else if(msg.size() >= 98){
				string pixel_ = "0x" + asciiTohex(msg.substr(74, 4), false);
				uint x = S2B_I(msg.data() + 78);
				uint y = S2B_I(msg.data() + 82);
				uint off_x = S2B_I(msg.data() + 86);
				uint off_y = S2B_I(msg.data() + 90);
				ushort pad_x = S2B_S(msg.data() + 94);
				ushort pad_y = S2B_S(msg.data() + 96);
				SetVal(QString("Pixel Format: ") + pixel_.c_str(), "74,2", data);
				SetVal(QString("X: %1").arg(x), "78,4", data);
				SetVal(QString("Y: %1").arg(y), "82,4", data);
				SetVal(QString("Offset X: %1").arg(off_x), "86,4", data);
				SetVal(QString("Offset Y: %1").arg(off_y), "90,4", data);
				SetVal(QString("Padding X: %1").arg(pad_x), "94,2", data);
				SetVal(QString("Padding Y: %1").arg(pad_y), "96,2", data);
			}
		}
		else if (format == 0x82) {
			string filed_ = "0x" + asciiTohex(msg.substr(62, 2), false);
			string type_ = "0x" + asciiTohex(msg.substr(64, 2), false);
			uint size_y = S2B_I(msg.data() + 66);
			SetVal(QString("Filed Info: ") + filed_.c_str(), "62,2", data);
			SetVal(QString("Payload Type: ") + type_.c_str(), "64,2", data);
			SetVal(QString("Size Y: %1").arg(size_y), "66,4", data);
		}
		else SetVal(QString("Payload Data: (%1 bytes)").arg(msg.size() - 62), "62,0", data);
	}
	ui->treeWidget_protocol->addTopLevelItem(data);
}

void Wireshark::Pack_itemClicked(QTableWidgetItem* item) {
	if(item == NULL)return;
	auto number = ui->tableWidget_pack->item(item->row(), 0)->text().toULongLong();
	if (packets.size() <= number)return;
	auto it = packets.begin();
	std::advance(it, number);
	bool eth_expand = ui->treeWidget_protocol->topLevelItem(0) && ui->treeWidget_protocol->topLevelItem(0)->isExpanded();
	bool inter_expand = ui->treeWidget_protocol->topLevelItem(1) && ui->treeWidget_protocol->topLevelItem(1)->isExpanded();
	bool usr_expand = ui->treeWidget_protocol->topLevelItem(2) && ui->treeWidget_protocol->topLevelItem(2)->isExpanded();
	ui->treeWidget_protocol->clear();
	////----------------------------------------------------------------------------------------------------------------------------------
	QString ethernet = "Ethernet Ⅱ, Mac: " + it->head.eth.src_mac + " → " + it->head.eth.dst_mac;
	QString proc = it->head.eth.protocol + " (0x" + it->head.eth.protocol_hex + ")";

	QTreeWidgetItem* eth = SetVal(ethernet, "0,14");
	SetVal(QString("Destination: ") + it->head.eth.dst_mac, "0,6", eth);
	SetVal(QString("Source: ") + it->head.eth.src_mac, "6,6", eth);
	SetVal(proc, "12,2", eth);
	ui->treeWidget_protocol->addTopLevelItem(eth);
	if (eth_expand)eth->setExpanded(true);
	////----------------------------------------------------------------------------------------------------------------------------------
	QString net_ver = it->head.inter.version;
	QString net_src = it->head.inter.src_ip;
	QString net_dst = it->head.inter.dst_ip;
	QString internet = "Internet Protocol Version  " + net_ver + ", Ip: " + net_src + " → " + net_dst;

	QTreeWidgetItem* inter = SetVal(internet, "14,20");
	SetVal(QString("Version: ") + net_ver, "14,1", inter);
	SetVal(QString("Header Length: ") + it->head.inter.head_len, "14,1", inter);
	SetVal(QString("Services: ") + it->head.inter.services, "15,1", inter);
	SetVal(QString("Total Length: ") + it->head.inter.total_len, "16,2", inter);
	SetVal(QString("Identification: ") + it->head.inter.identification, "18,2", inter);
	SetVal(QString("Flags: ") + it->head.inter.flags, "20,2", inter);
	SetVal(QString("Time To Live: ") + it->head.inter.live, "22,1", inter);
	SetVal(QString("Protocol: ") + it->head.inter.protocol, "23,1", inter);
	SetVal(QString("Header Checksum: ") + it->head.inter.checksum, "24,2", inter);
	SetVal(QString("Source Address: ") + net_src, "26,4", inter);
	SetVal(QString("Destination Address: ") + net_dst, "30,4", inter);
	ui->treeWidget_protocol->addTopLevelItem(inter);
	if (inter_expand)inter->setExpanded(true);
	////----------------------------------------------------------------------------------------------------------------------------------
	QString usr_src = it->head.user.src_port;
	QString usr_dst = it->head.user.dst_port;
	QString user = "User Datagram, Port: " + usr_src + " → " + usr_dst;

	QTreeWidgetItem* usr = SetVal(user, "34,8");
	SetVal(QString("Source Port: ") + usr_src, "34,2", usr);
	SetVal(QString("Destination Port: ") + usr_dst, "36,2", usr);
	SetVal(QString("Length: ") + it->head.user.length, "38,2", usr);
	SetVal(QString("Checksum: ") + it->head.user.checksum, "40,2", usr);
	ui->treeWidget_protocol->addTopLevelItem(usr);
	if (usr_expand)usr->setExpanded(true);
	////----------------------------------------------------------------------------------------------------------------------------------
	string d_protocol = ui->tableWidget_pack->item(item->row(), 5)->text().toStdString();
	if (d_protocol == "GVSP" || d_protocol == "GVCP") AnalysisGigE(it->msg);
	else {
		QTreeWidgetItem* data_ = SetVal(QString("Data(%1 byte)").arg(it->msg.size() - 42), "42,0");
		ui->treeWidget_protocol->addTopLevelItem(data_);
	}
	////----------------------------------------------------------------------------------------------------------------------------------
	for (int i = ui->tableWidget_value->rowCount(); i; i--)ui->tableWidget_value->removeRow(0);
	uint row_c = it->msg.size() / 15 + ((it->msg.size() % 15) ? 1 : 0);
	ui->tableWidget_value->setRowCount(row_c);
	int row(0), col(0);
	std::string h_val;
	for (uchar i : it->msg) {
		std::string to_h = "";
		short ten = (short)i / 16;
		short ge = (short)i % 16;
		to_h.push_back(ten < 10 ? ten + '0' : (ten + 'A' - 10));
		to_h.push_back(ge < 10 ? ge + '0' : (ge + 'A' - 10));
		h_val.push_back((i > 33 && i < 127 ? i : '.'));
		h_val.push_back(' ');
		ui->tableWidget_value->setItem(row, col, new QTableWidgetItem(to_h.c_str()));
		ui->tableWidget_value->item(row, col++)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		if (col == 16) {
			ui->tableWidget_value->setItem(row, 16, new QTableWidgetItem(h_val.c_str()));
			ui->tableWidget_value->item(row, 16)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		if (col % 16 == 0) { h_val.clear(); col = 0; ++row; }
	}
	if (!h_val.empty()) {
		ui->tableWidget_value->setItem(row, 16, new QTableWidgetItem(h_val.c_str()));
		ui->tableWidget_value->item(row, 16)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	current_index = ui->tableWidget_pack->item(item->row(), 0)->text().toLongLong();
}

void Wireshark::Protocol_itemClicked(QTreeWidgetItem* item) {
	string wt = item->whatsThis(0).toStdString();
	uint start_ = atoi(wt.c_str());
	uint length_ = atoi(wt.substr(wt.find(',') + 1).c_str());

	ui->tableWidget_value->clearSelection();
	uint st_r = start_ / 16;
	uint st_c = start_ % 16;
	for (; length_; st_c++) {
		if (st_c && (st_c % 16 == 0)) { st_r++; st_c = -1; continue; }
		auto item = ui->tableWidget_value->item(st_r, st_c);
		ui->tableWidget_value->setCurrentItem(item);
		length_--;
	}
}

void Wireshark::Value_itemClicked(QTableWidgetItem* item) {
	ui->tableWidget_value->clearSelection();
	ui->tableWidget_value->setCurrentItem(item);
}

void Wireshark::on_lineEdit_cmd_editingFinished() {
	QString text = ui->lineEdit_cmd->text();
	for (auto& i : command) {
		i.second = "";
		int index = text.indexOf(i.first);
		int end = text.indexOf(',', index);

		if (index == -1)continue;
		index += i.first.length();
		if (end == -1)end = text.size();
		i.second = text.mid(index, end - index);
	}
}
void Wireshark::on_lineEdit_cmd_textChanged(QString text) {
	QStringList options;
	for (auto i : command) 
		if (i.first.indexOf(text) >= 0) options << i.first;
	qobject_cast<QStringListModel*>(completer->model())->setStringList(options);
}

void Wireshark::on_pushButton_net_refresh_clicked() {
	if (!GetNetCardList()) ErrExit(LU_STR138);
	else RefreshListView();
}
void Wireshark::on_toolButton_save_clicked() {
	QString filename = QFileDialog::getSaveFileName(this,tr("Save"),"",tr("*.pcapng")); //选择路径
	if (filename.isEmpty()) return;

	pcap_dumper_t* dumpfile;
	dumpfile = pcap_dump_open(pcap_handle, filename.toLocal8Bit().toStdString().c_str());
	if (dumpfile == nullptr) {
		QMessageBox::warning(nullptr, LU_STR139, LU_STR140);
		return;
	}

	ui->toolButton_save->setEnabled(false);
	for (auto i : packets) pcap_dump((uchar*)dumpfile, &i.header, (u_char*)i.msg.c_str());
	pcap_dump_close(dumpfile);
	QMessageBox::information(nullptr, LU_STR139, LU_STR141);
	ui->toolButton_save->setEnabled(true);
}
void Wireshark::on_toolButton_over_clicked() {
	auto result = QMessageBox::information(nullptr, LU_STR142, LU_STR143, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
	if (result == QMessageBox::Cancel)return;
	start_main_speed_get = false;

	on_toolButton_stop_clicked();
	if (result == QMessageBox::Yes) on_toolButton_save_clicked();

	pcap_close(pcap_handle);
	ui->toolButton_over->setEnabled(false);
	capturing = false;
	ch_guid = ch_name = "";
	packview.stop();
	on_toolButton_clear_clicked(true);
	ui->widget_6->setVisible(true);
	ui->widget_2->setVisible(false);
	ui->toolButton_over->setEnabled(true);
	on_pushButton_net_refresh_clicked();
}
void Wireshark::on_toolButton_stop_clicked() {
	stopping = true;
	ui->toolButton_stop->setEnabled(false);
	ui->toolButton_start->setEnabled(true);
	ui->toolButton_save->setEnabled(true);
}
void Wireshark::on_toolButton_start_clicked() {
	stopping = false;
	ui->toolButton_stop->setEnabled(true);
	ui->toolButton_save->setEnabled(false);
	ui->toolButton_start->setEnabled(false);
}
void Wireshark::on_toolButton_clear_clicked(bool clear) {
	if (!clear) {
		auto result = QMessageBox::information(nullptr, LU_STR144, LU_STR145, QMessageBox::Yes, QMessageBox::No);
		if (result == QMessageBox::No)return;
	}

	bool status = stopping;
	if (!status)stopping = true;
	refresh.stop();
	ui->toolButton_clear->setEnabled(false);
	speed_val.val.clear();
	speed_val.inout[0] = speed_val.inout[1] = 0;
	packets.clear();
	speed_val.first_ = true;
	for (ushort i = 0; i < pack_row_num; i++) 
		for (int j = 0; j < 8; j++)ui->tableWidget_pack->item(i, j)->setText("");
	ui->treeWidget_protocol->clear();
	ui->toolButton_clear->setEnabled(true);
	for (int i = ui->tableWidget_value->rowCount(); i; i--)ui->tableWidget_value->removeRow(0);
	refresh.start(1000);
	if (!status)stopping = false;
}

void Wireshark::ErrExit(QString msg) {
	QMessageBox::warning(nullptr, LU_STR1, msg);
	this->close();
}
bool Wireshark::eventFilter(QObject* obj, QEvent* event){
	if (obj != ui->tableWidget_pack->verticalScrollBar())
		return QMainWindow::eventFilter(obj, event);

	if (event->type() != QEvent::Wheel)
		return QMainWindow::eventFilter(obj, event);

	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	auto val_ = ui->verticalScrollBar->value();
	if (wheelEvent->delta() > 0)ui->verticalScrollBar->setValue(val_ - 1);
	else ui->verticalScrollBar->setValue(val_ + 1);

	return QMainWindow::eventFilter(obj, event);
}
