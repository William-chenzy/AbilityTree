#ifndef MAINWINDOW_H_LW
#define MAINWINDOW_H_LW

#include <QMainWindow>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <deque>
#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QCompleter>
#include <map>
#define HAVE_REMOTE
#include <pcap.h>
#include "ui_Wireshark.h"

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <net/if.h>
#include <linux/if_arp.h>
//#include <armadillo>
typedef unsigned long       DWORD;
#define localtime_s(A,B) {A = localtime(B);}
#define pcap_open(a,b,c,d,e,f) pcap_open_live(a,b,c,d,f)
#else
#include <WinSock2.h>
#include <Iphlpapi.h>
#include <windows.h>
#include <winuser.h>
#include <ws2tcpip.h>
#include <Mstcpip.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")
#endif // __linux__

namespace Ui {
	class Wireshark;
}

typedef unsigned long ulong;
struct NIpINFO { std::string ip, mask, getway; };
struct NetINFO {
	std::string name, description, mac;
	std::list<NIpINFO>ipd;
};
struct PACKAGE {
	struct ETHHead {
		QString dst_mac;
		QString src_mac;
		QString protocol;
		QString protocol_hex;
	};
	struct INTERHead {
		QString version;
		QString head_len;
		QString services;
		QString total_len;
		QString identification;
		QString flags;
		QString live;
		QString protocol;
		QString checksum;
		QString src_ip;
		QString dst_ip;
	};
	struct USRHead {
		QString length;
		QString checksum;
		QString src_port;
		QString dst_port;
	};
	struct Head {
		ETHHead eth;
		USRHead user;
		INTERHead inter;
		bool is_analysis = false;
	}head;
	std::string msg;
	pcap_pkthdr header;
};
struct SPeeD {
	DWORD inout[2]{ 0,0 };
	bool first_ = true;
	std::list<float>val;
	QLabel* p11 = new QLabel();
	QWidget* widget1 = new QWidget();
	QHBoxLayout* pLayout1 = new QHBoxLayout();
	SPeeD() {
		pLayout1->setMargin(0);
		pLayout1->addWidget(p11);
		widget1->setLayout(pLayout1);
	}
	~SPeeD() {
		pLayout1->deleteLater();
		widget1->deleteLater();
		p11->deleteLater();
	}
};

class Wireshark : public QMainWindow
{
	Q_OBJECT

public:
	Wireshark(QWidget *parent = 0);
	~Wireshark() = default;
	void SwitchLanguage();

protected:
	bool eventFilter(QObject* obj, QEvent* event);

private:
	void IpToNet();
	bool GetNetCardList();
	void RefreshListView();
	void AnalysisPackages();
	void ErrExit(QString msg);
	void AnalysisGigE(std::string msg);
	void AnalysisPackageHead(std::list<PACKAGE>::iterator);
	std::string ProtocolInfo(std::string& raw, std::string val);
	bool GetSpeedAndByte(std::string guid, ulong& speed, ulong& in, ulong& out);

private slots:
	void RefreshSpeed();
	void on_toolButton_stop_clicked();
	void on_toolButton_over_clicked();
	void on_toolButton_save_clicked();
	void on_toolButton_start_clicked();
	void on_pushButton_net_start_cicked();
	void on_lineEdit_cmd_editingFinished();
	void on_pushButton_net_refresh_clicked();
	void on_lineEdit_cmd_textChanged(QString);
	void Pack_itemClicked(QTableWidgetItem* item);
	void Value_itemClicked(QTableWidgetItem* item);
	void netCard_itemClicked(QTreeWidgetItem* item);
	void on_verticalScrollBar_valueChanged(int val);
	void Protocol_itemClicked(QTreeWidgetItem* item);
	void netCard_itemdoubleClicked(QTreeWidgetItem* item);
	void on_toolButton_clear_clicked(bool clear = false);

public:
	bool stopping = false;
	std::list<PACKAGE> packets;

private:
	long long current_index = -1;
	bool is_execute = false;
	pcap_t* pcap_handle;
	int pack_row_num;
	bool capturing = false;
	std::map<std::string, SPeeD>all_speed;
	std::string val_mask = "";
	SPeeD speed_val;
	std::string ch_guid, ch_name;
	QTimer refresh;
	QTimer packview;
	std::list<NetINFO> nifl;
	QCompleter *completer = nullptr;

	std::map<QString, QString>command{
		{"src.ip=",""},
		{"dst.ip=",""},
		{"src.mac=",""},
		{"dst.mac=",""},
		{"src.port=",""},
		{"dst.port=",""},
		{"data.length=",""},
	};

	Ui::Wireshark* ui;
};

#endif // MAINWINDOW_H_LW
