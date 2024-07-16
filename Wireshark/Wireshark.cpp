#include "LxWireshark.h"
#include <iostream>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMouseEvent>
#include <thread>
#include <fstream>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileDialog>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace chrono;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

string asciiTohex(std::string msg, bool add_space = true) {
	std::string to_h = "";
	for (uchar i : msg) {
		short ten = (short)i / 16;
		short ge = (short)i % 16;
		to_h.push_back(ten < 10 ? ten + '0' : (ten + 'A' - 10));
		to_h.push_back(ge < 10 ? ge + '0' : (ge + 'A' - 10));
		if (add_space) to_h.push_back(' ');
	}
	return to_h;
}

string hexToascii(std::string msg) {
	std::string to_h = "";
	std::string temp = "";
	for (uchar i : msg) {
		if (i != ' ')temp.push_back(i);
		if (temp.size() < 2) continue;
		uchar ten = temp[0] > 'a' ? temp[0] - 'a' + 10 : (temp[0] > 'A' ? temp[0] - 'A' + 10 : temp[0] - '0') * 16;
		ten += temp[1] > 'a' ? temp[1] - 'a' + 10 : (temp[1] > 'A' ? temp[1] - 'A' + 10 : temp[1] - '0');
		to_h.push_back(ten);
		temp.clear();
	}
	return to_h;
}

string GetTime(timeval tv) {
	time_t nows = tv.tv_sec;
	tm* _t = new tm();
	localtime_s(_t, &nows);
	QString tim_ = QString("%1-%2-%3 %4:%5:%6 %7").arg(_t->tm_year + 1900).arg(_t->tm_mon + 1).arg(_t->tm_mday).arg(_t->tm_hour, 2, 10, QLatin1Char('0'))
		.arg(_t->tm_min, 2, 10, QLatin1Char('0')).arg(_t->tm_sec, 2, 10, QLatin1Char('0')).arg(tv.tv_usec, 6, 10, QLatin1Char('0'));
	return tim_.toStdString();
}

string GetEtProtocol(ushort et) {
	if (et == 0x0008)return "IPV4";
	else if (et == 0x0608)return "ARP";
	else if (et == 0x6488)return "PPPoE";
	else if (et == 0x0081)return "802.1Q tag";
	else if (et == 0xDD86)return "IPV6";
	else if (et == 0x4788)return "MPLS Label";
	else return to_string((short)et);
}

string GetIntProtocol(uchar pt) {
	if (pt == 1)return "ICMP";
	else if (pt == 2)return "IGMP";
	else if (pt == 6)return "TCP";
	else if (pt == 17)return "UDP";
	else if (pt == 88)return "EIGRP";
	else if (pt == 89)return "OSPF";
	else if (pt == 103)return "PIM";
	else if (pt == 128)return "SSCOPMCE";
	else if (pt == 132)return "SCTP";
	else return to_string((int)pt);
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
	auto this_ = (LxWireshark*)pt;
	if (this_->stopping)return;

	PAKETS pak;
	pak.header = *header;
	pak.msg.append((char*)pkt_data, header->caplen);
	this_->packets.push_back(pak);
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

LxWireshark::LxWireshark(QWidget* parent) :
	QMainWindow(parent), ui(new Ui::LxWireshark)
{
	ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	QRect deskRect = QApplication::desktop()->availableGeometry();//获取可用桌面大小
	this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);//让窗口居中显示
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	this->setWindowIcon(QIcon(QPixmap("./res/title.png")));

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
	udp_protocol = ui->comboBox->currentText().toStdString();

	connect(ui->tableWidget_pack, &QTableWidget::currentItemChanged, this, &LxWireshark::Pack_itemClicked);
	connect(ui->tableWidget_value, &QTableWidget::itemClicked, this, &LxWireshark::Value_itemClicked);
	
	connect(ui->treeWidget_netCard, &QTreeWidget::itemDoubleClicked, this, &LxWireshark::netCard_itemdoubleClicked);
	connect(ui->treeWidget_netCard, &QTreeWidget::itemClicked, this, &LxWireshark::netCard_itemClicked);
	connect(&refresh, &QTimer::timeout, this, &LxWireshark::RefreshSpeed);
	refresh.start(1000);
	connect(&packview, &QTimer::timeout, this, &LxWireshark::AnalysisPackets);

	connect(ui->treeWidget_protocol, &QTreeWidget::itemClicked, this, &LxWireshark::Protocol_itemClicked);
	ui->tableWidget_pack->verticalScrollBar()->installEventFilter(this);

	ui->tableWidget_pack->setRowCount(12);
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

	ui->tableWidget_value->setShowGrid(false);
	for(int i=0;i<16;i++)
		ui->tableWidget_value->setColumnWidth(i, 20);
	ui->tableWidget_value->setColumnWidth(16, 220);
	ui->tableWidget_value->verticalHeader()->hide();
	ui->tableWidget_value->verticalHeader()->setDefaultSectionSize(20);

	std::fstream* conf_file = new std::fstream;
	conf_file->open("./conf/LxViewerParam.lvp", std::ios::in | std::ios::binary);
	if (!conf_file->is_open()) is_zh_CN = 1;
	else is_zh_CN = (bool)conf_file->get();

	SwitchLanguage();
}
void LxWireshark::SwitchLanguage() {
	ui->pushButton_top->setText(LU_STR413);
	ui->pushButton_net_refresh->setText(LU_STR390);
	QStringList heardList;
	if (is_zh_CN)heardList << "网卡" << "流量" << "峰值(Mbps)" << "MAC" << "描述";
	else heardList << "Network card" << "Flow" << "Peak value(Mbps)" << "MAC" << "Describe";
	ui->treeWidget_netCard->setHeaderLabels(heardList);

	ui->toolButton_start->setText(LU_STR414);
	ui->toolButton_stop->setText(LU_STR415);
	ui->toolButton_over->setText(LU_STR416);
	ui->toolButton_save->setText(LU_STR417);
	ui->toolButton_clear->setText(LU_STR418);
	ui->label_4->setText(LU_STR419);
	ui->label_2->setText(LU_STR420);
	ui->lineEdit_cmd->setPlaceholderText(LU_STR421);
}

void LxWireshark::RefreshSpeed() {
	if (!ch_guid.empty()) {
		DWORD val[3]{ 0,0,0 };
		if (!GetSpeedAndByte(ch_guid, val[0], val[1], val[2]))return;
		float fval = ((val[1] - speed_val.inout[0]) + (val[2] - speed_val.inout[1])) / 1000000.f;
		speed_val.inout[0] = val[1], speed_val.inout[1] = val[2];
		if (speed_val.first_) { speed_val.first_ = false; return; }
		else if (speed_val.val.size() == 100) speed_val.val.pop_front();
		speed_val.val.push_back(fval);

		float top = 0;
		for (auto x : speed_val.val)top = x > top ? x : top;
		cv::Mat img = cv::Mat(20, 100, CV_8UC3, cv::Scalar(33, 29, 27));
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

bool LxWireshark::GetNetCardList() {
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
		do{
			NIpINFO nip{ IpA->IpAddress.String,IpA->IpMask.String,pA->GatewayList.IpAddress.String };
			nif.ipd.push_back(nip);
		} while (IpA = IpA->Next);
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

void LxWireshark::RefreshListView() {
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
}

bool LxWireshark::GetSpeedAndByte(string guid, DWORD& speed, DWORD& in, DWORD& out) {
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

void LxWireshark::netCard_itemClicked(QTreeWidgetItem* item){
	if (item->childCount()) item->setExpanded(!item->isExpanded());
}

void LxWireshark::netCard_itemdoubleClicked(QTreeWidgetItem* item) {
	if (!item->parent()) return;

	socket(AF_INET, SOCK_RAW, 0);
	ch_ip = item->text(0).toStdString();
	if (!is_Linux) ch_guid = item->parent()->text(4).toStdString();
	else ch_guid = item->parent()->text(0).toStdString();
	IpToNet();
	char errorBuffer[PCAP_ERRBUF_SIZE];
	pcap_handle = pcap_open(ch_name.c_str(), 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errorBuffer);
	if (pcap_handle == NULL) {
		QMessageBox::warning(this, LU_STR1, QString(LU_STR126) + errorBuffer);
		ch_ip = ch_guid = ch_name = "";
		return;
	}
	on_toolButton_start_clicked();
	ui->widget_6->setVisible(false);
	ui->widget_2->setVisible(true);
	speed_val.val = all_speed[ch_guid].val;
	ui->verticalScrollBar->setMaximum(0);

	std::thread* _t = new std::thread(pcap_loop, pcap_handle, 0, packet_back, (unsigned char*)this);
	_t->detach();
	packview.start(100);
}

void LxWireshark::on_comboBox_currentIndexChanged(QString text) {
	udp_protocol = text.toStdString();
	on_verticalScrollBar_valueChanged(ui->verticalScrollBar->value());
}

string LxWireshark::ProtocolInfo(string& raw, string val) {
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
	else if (raw == "UDP" && udp_protocol == "GigE") {
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

			info += " [Block ID: " + to_string(block_id) + "   Packet ID: " + to_string(pack_ic) + "]";
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

void LxWireshark::on_verticalScrollBar_valueChanged(int val) {
	static bool execute_ = false;
	if (execute_)return;
	execute_ = true;
	ui->tableWidget_pack->clear();
	static QStringList strs = { "No.", "Time", "Souce","Destination","EtherType","Protocol","Length","Info" };
	ui->tableWidget_pack->setHorizontalHeaderLabels(strs);
	for (ushort i = 0; i < 12; i++) {
		unsigned long number = val * 12 + i;
		if (number >= packets.size())continue;
		auto pac = &packets[number];
		ushort et = pac->msg.data()[12];
		string type, protocol, dst, time_, src;
		type = GetEtProtocol(et);
		uchar pt = pac->msg.data()[23];
		protocol = GetIntProtocol(pt);

		uchar* _p = (uchar*)pac->msg.c_str();
		src = to_string((int)_p[26]) + "." + to_string((int)_p[27]) + "." + to_string((int)_p[28]) + "." + to_string((int)_p[29]);
		dst = to_string((int)_p[30]) + "." + to_string((int)_p[31]) + "." + to_string((int)_p[32]) + "." + to_string((int)_p[33]);
		time_ = GetTime(pac->header.ts);
		auto proto = ProtocolInfo(protocol, pac->msg);

		ui->tableWidget_pack->setItem(i, 0, new QTableWidgetItem(QString::number(number)));
		ui->tableWidget_pack->setItem(i, 1, new QTableWidgetItem(time_.c_str()));
		ui->tableWidget_pack->setItem(i, 2, new QTableWidgetItem(src.c_str()));
		ui->tableWidget_pack->setItem(i, 3, new QTableWidgetItem(dst.c_str()));
		ui->tableWidget_pack->setItem(i, 4, new QTableWidgetItem(type.c_str()));
		ui->tableWidget_pack->setItem(i, 5, new QTableWidgetItem(protocol.c_str()));
		ui->tableWidget_pack->setItem(i, 6, new QTableWidgetItem(QString::number(pac->header.caplen)));
		ui->tableWidget_pack->setItem(i, 7, new QTableWidgetItem(proto.c_str()));
	}
	execute_ = false;
}

void LxWireshark::AnalysisPackets() {
	auto val = ui->verticalScrollBar->value();
	auto last = ui->verticalScrollBar->maximum();
	unsigned long long max = packets.size() / 12 + (packets.size() % 12 ? 1 : 0);
	ui->verticalScrollBar->setMaximum(max);
	if (max == 1)on_verticalScrollBar_valueChanged(0);
	else if (abs(last - val) <= 1)ui->verticalScrollBar->setValue(max - 1);
}

void LxWireshark::IpToNet(){
	char buf[512];
	pcap_if_t* all;
	if (pcap_findalldevs(&all, buf) == -1)return;
	for (; all != nullptr; all = all->next){
		for (pcap_addr_t* a = all->addresses; a != nullptr; a = a->next)
			if (((sockaddr_in*)a->addr)->sin_addr.s_addr != 0){
				uchar* _p = (uchar*)a->addr->sa_data + 2;
				string _ip_ = to_string((int)_p[0]) + "." + to_string((int)_p[1]) + "." + to_string((int)_p[2]) + "." + to_string((int)_p[3]);
				if (_ip_ == ch_ip)ch_name = all->name;
			}
	}
	pcap_freealldevs(all);
}

auto SetVal = [&](QString msg, QString tips, QTreeWidgetItem* parten = nullptr)->QTreeWidgetItem* {
	QTreeWidgetItem* _t = new QTreeWidgetItem;
	_t->setText(0, msg); _t->setWhatsThis(0, tips);
	if (parten)parten->addChild(_t);
	return _t;
};
void LxWireshark::AnalysisGigE(string msg) {
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

void LxWireshark::Pack_itemClicked(QTableWidgetItem* item) {
	if(item == NULL)return;
	auto number = ui->tableWidget_pack->item(item->row(), 0)->text().toULongLong();
	if (packets.size() <= number)return;
	auto p_val = &packets[number];
	ui->treeWidget_protocol->clear();
	////----------------------------------------------------------------------------------------------------------------------------------
	ushort et = p_val->msg.data()[12];
	string src_mac = asciiTohex(p_val->msg.substr(0, 6), false);
	for (int i = 10; i > 1; i -= 2)src_mac.insert(i, 1, ':');
	string dst_mac = asciiTohex(p_val->msg.substr(6, 6), false);
	for (int i = 10; i > 1; i -= 2)dst_mac.insert(i, 1, ':');
	string ethernet = "Ethernet Ⅱ, Mac: " + src_mac + " → " + dst_mac;
	string proc = GetEtProtocol(et) + " (0x" + asciiTohex(p_val->msg.substr(12, 2), false) + ")";

	QTreeWidgetItem* eth = SetVal(ethernet.c_str(), "0,14");
	SetVal(QString("Destination: ") + dst_mac.c_str(), "0,6", eth);
	SetVal(QString("Source: ") + src_mac.c_str(), "6,6", eth);
	SetVal(proc.c_str(), "12,2", eth);
	ui->treeWidget_protocol->addTopLevelItem(eth);
	////----------------------------------------------------------------------------------------------------------------------------------
	char net_head = p_val->msg.data()[14];
	string net_ver = to_string((int)net_head >> 4);
	string net_len = to_string((int)net_head & 0x0f) + " * 4 (bytes)";
	string net_src = ui->tableWidget_pack->item(item->row(), 2)->text().toStdString();
	string net_dst = ui->tableWidget_pack->item(item->row(), 3)->text().toStdString();
	string net_sev = "0x" + asciiTohex(p_val->msg.substr(15, 1), false);
	string net_tle = to_string(S2B_S(p_val->msg.data() + 16));
	string net_ide = "0x" + asciiTohex(p_val->msg.substr(18, 2), false) + "(" + to_string(S2B_S(p_val->msg.data() + 18)) + ")";
	string net_fag; 
	for (uchar i = 0x80; i; i >>= 1)net_fag += p_val->msg.data()[20] & i ? "1" : "0";
	for (uchar i = 0x80; i; i >>= 1)net_fag += p_val->msg.data()[21] & i ? "1" : "0";
	string net_tim = to_string((int)p_val->msg.data()[22]);
	string net_pro = GetIntProtocol(p_val->msg.data()[23]) + "(" + to_string((int)p_val->msg.data()[23]) + ")";
	string net_chk = "0x" + asciiTohex(p_val->msg.substr(24, 2), false);
	string internet = "Internet Protocol Version  " + net_ver + ", Ip: " + net_src + " → " + net_dst;

	QTreeWidgetItem* inter = SetVal(internet.c_str(), "14,20");
	SetVal(QString("Version: ") + net_ver.c_str(), "14,1", inter);
	SetVal(QString("Header Length: ") + net_len.c_str(), "14,1", inter);
	SetVal(QString("Services: ") + net_sev.c_str(), "15,1", inter);
	SetVal(QString("Total Length: ") + net_tle.c_str(), "16,2", inter);
	SetVal(QString("Identification: ") + net_ide.c_str(), "18,2", inter);
	SetVal(QString("Flags: ") + net_fag.c_str(), "20,2", inter);
	SetVal(QString("Time To Live: ") + net_tim.c_str(), "22,1", inter);
	SetVal(QString("Protocol: ") + net_pro.c_str(), "23,1", inter);
	SetVal(QString("Header Checksum: ") + net_chk.c_str(), "24,2", inter);
	SetVal(QString("Source Address: ") + net_src.c_str(), "26,4", inter);
	SetVal(QString("Destination Address: ") + net_dst.c_str(), "30,4", inter);
	ui->treeWidget_protocol->addTopLevelItem(inter);
	////----------------------------------------------------------------------------------------------------------------------------------
	string usr_src = to_string(S2B_S(p_val->msg.data() + 34));
	string usr_dst = to_string(S2B_S(p_val->msg.data() + 36));
	string usr_len = to_string(S2B_S(p_val->msg.data() + 38));
	string usr_chk = "0x" + asciiTohex(p_val->msg.substr(40, 2), false);
	string user = "User Datagram, Port: " + usr_src + " → " + usr_dst;

	QTreeWidgetItem* usr = SetVal(user.c_str(), "34,8");
	SetVal(QString("Source Port: ") + usr_src.c_str(), "34,2", usr);
	SetVal(QString("Destination Port: ") + usr_dst.c_str(), "36,2", usr);
	SetVal(QString("Length: ") + usr_len.c_str(), "38,2", usr);
	SetVal(QString("Checksum: ") + usr_chk.c_str(), "40,2", usr);
	ui->treeWidget_protocol->addTopLevelItem(usr);
	////----------------------------------------------------------------------------------------------------------------------------------
	string d_protocol = ui->tableWidget_pack->item(item->row(), 5)->text().toStdString();
	if (d_protocol == "GVSP" || d_protocol == "GVCP") AnalysisGigE(p_val->msg);
	else {
		QTreeWidgetItem* data = SetVal(QString("Data(%1 byte)").arg(p_val->msg.size() - 42), "42,0");
		ui->treeWidget_protocol->addTopLevelItem(data);
	}
	////----------------------------------------------------------------------------------------------------------------------------------
	for (int i = ui->tableWidget_value->rowCount(); i; i--)ui->tableWidget_value->removeRow(0);
	uint row_c = p_val->msg.size() / 15 + ((p_val->msg.size() % 15) ? 1 : 0);
	ui->tableWidget_value->setRowCount(row_c);
	int row(0), col(0);
	std::string h_val;
	for (uchar i : p_val->msg) {
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
}

void LxWireshark::Protocol_itemClicked(QTreeWidgetItem* item) {
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

void LxWireshark::Value_itemClicked(QTableWidgetItem* item) {
	ui->tableWidget_value->clearSelection();
	ui->tableWidget_value->setCurrentItem(item);
}

void LxWireshark::on_lineEdit_cmd_editingFinished() {
	auto mask_ = ui->lineEdit_cmd->text();
	if (mask_ == "") {}
}

void LxWireshark::on_pushButton_net_refresh_clicked() {
	if (!GetNetCardList()) ErrExit(LU_STR138);
	else RefreshListView();
}
void LxWireshark::on_toolButton_save_clicked() {
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
void LxWireshark::on_toolButton_over_clicked() {
	auto result = QMessageBox::information(nullptr, LU_STR142, LU_STR143, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
	if (result == QMessageBox::Cancel)return;
	else if (result == QMessageBox::Yes) {
		on_toolButton_stop_clicked();
		on_toolButton_save_clicked();
	}

	pcap_close(pcap_handle);
	ui->toolButton_over->setEnabled(false);
	capturing = false;
	ch_guid = ch_ip = ch_name = "";
	packview.stop();
	on_toolButton_clear_clicked(true);
	ui->widget_6->setVisible(true);
	ui->widget_2->setVisible(false);
	ui->toolButton_over->setEnabled(true);
	on_pushButton_net_refresh_clicked();
}
void LxWireshark::on_toolButton_stop_clicked() {
	stopping = true;
	ui->toolButton_stop->setEnabled(false);
	ui->toolButton_start->setEnabled(true);
	ui->toolButton_save->setEnabled(true);
}
void LxWireshark::on_toolButton_start_clicked() {
	stopping = false;
	ui->toolButton_stop->setEnabled(true);
	ui->toolButton_save->setEnabled(false);
	ui->toolButton_start->setEnabled(false);
}
void LxWireshark::on_toolButton_clear_clicked(bool clear) {
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
	ui->tableWidget_pack->clear();
	ui->treeWidget_protocol->clear();
	ui->toolButton_clear->setEnabled(true);
	for (int i = ui->tableWidget_value->rowCount(); i; i--)ui->tableWidget_value->removeRow(0);
	refresh.start(1000);
	if (!status)stopping = false;
}






void LxWireshark::ErrExit(QString msg) {
	QMessageBox::warning(this, LU_STR1, msg);
	this->close();
}

void LxWireshark::on_toolButton_close_clicked() { exit(0); }

void LxWireshark::on_pushButton_top_clicked() {
#ifdef _WIN32
	static bool is_top = false;
	::SetWindowPos((HWND)this->winId(), is_top ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
	is_top = !is_top;
#else
	QMessageBox::warning(this, LU_STR1, LU_STR122);
#endif
}

static QPoint mouse_pos;
static bool is_move_mouse;
void LxWireshark::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		is_move_mouse = true;
		mouse_pos = event->screenPos().toPoint();
	}
	QWidget::mousePressEvent(event);
}

void LxWireshark::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)is_move_mouse = false;
	QWidget::mouseReleaseEvent(event);
}

void LxWireshark::mouseMoveEvent(QMouseEvent* event) {
	if (is_move_mouse) {
		QPoint newPos = this->pos() + (event->screenPos().toPoint() - mouse_pos);
		move(newPos);
		mouse_pos = event->screenPos().toPoint();
	}

	QWidget::mouseMoveEvent(event);
}

bool LxWireshark::eventFilter(QObject* obj, QEvent* event){
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