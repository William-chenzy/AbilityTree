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
#include <map>
#define HAVE_REMOTE
#include <pcap.h>
#include "ui_LxWireshark.h"

#ifdef __linux__
static const bool is_Linux = true;
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <net/if.h>
#include <linux/if_arp.h>
#include <armadillo>
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
static const bool is_Linux = false;
#endif // __linux__

namespace Ui {
	class LxWireshark;
}

typedef unsigned long ulong;
struct NIpINFO { std::string ip, mask, getway; };
struct NetINFO {
	std::string name, description, mac;
	std::list<NIpINFO>ipd;
};
struct PAKETS {
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

class LxWireshark : public QMainWindow
{
	Q_OBJECT

public:
	LxWireshark(QWidget *parent = 0);
	~LxWireshark() = default;
	void SwitchLanguage();

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:
	void IpToNet();
	bool GetNetCardList();
	void AnalysisPackets();
	void RefreshListView();
	void ErrExit(QString msg);
	void AnalysisGigE(std::string msg);
	std::string ProtocolInfo(std::string& raw, std::string val);
	bool GetSpeedAndByte(std::string guid, ulong& speed, ulong& in, ulong& out);

private slots:
	void RefreshSpeed();
	void on_pushButton_top_clicked();
	void on_toolButton_stop_clicked();
	void on_toolButton_over_clicked();
	void on_toolButton_save_clicked();
	void on_toolButton_start_clicked();
	void on_toolButton_close_clicked();
	void on_lineEdit_cmd_editingFinished();
	void on_pushButton_net_refresh_clicked();
	bool eventFilter(QObject* obj, QEvent* event);
	void Pack_itemClicked(QTableWidgetItem* item);
	void Value_itemClicked(QTableWidgetItem* item);
	void netCard_itemClicked(QTreeWidgetItem* item);
	void on_verticalScrollBar_valueChanged(int val);
	void Protocol_itemClicked(QTreeWidgetItem* item);
	void on_comboBox_currentIndexChanged(QString text);
	void netCard_itemdoubleClicked(QTreeWidgetItem* item);
	void on_toolButton_clear_clicked(bool clear = false);

public:
	bool stopping = false;
	std::deque<PAKETS> packets;

private:
	std::string udp_protocol;
	pcap_t* pcap_handle;
	bool capturing = false;
	std::map<std::string, SPeeD>all_speed;
	std::string val_mask = "";
	SPeeD speed_val;
	std::string ch_guid, ch_ip, ch_name;
	QTimer refresh;
	QTimer packview;
	std::list<NetINFO> nifl;
	Ui::LxWireshark* ui;
};
static bool is_zh_CN;
#define LU_STR1 (is_zh_CN?"错误":"Error")
#define LU_STR123 (is_zh_CN?"WSA 初始化失败":"WSA Init failed")
#define LU_STR124 (is_zh_CN?"获取网卡流量失败":"Failed to obtain network card traffic")
#define LU_STR125 (is_zh_CN?"申请内存失败":"Failed to apply for memory")
#define LU_STR126 (is_zh_CN?"抓包失败: ":"Packet capture failed: ")
#define LU_STR127 (is_zh_CN?"回显应答":"Echo Reply")
#define LU_STR128 (is_zh_CN?"回显请求":"Echo request")
#define LU_STR129 (is_zh_CN?"网络不可达":"Network is unreachable")
#define LU_STR130 (is_zh_CN?"主机不可达":"Host Unreachable")
#define LU_STR131 (is_zh_CN?"协议不可达":"Protocol unreachable")
#define LU_STR132 (is_zh_CN?"端口不可达":"Port unreachable")
#define LU_STR133 (is_zh_CN?"需要进行分片但设置不分片比特":"Need to perform sharding but set non sharding bits")
#define LU_STR134 (is_zh_CN?"由于过滤，通信被强制禁止":"Communication is forcibly prohibited due to filtering")
#define LU_STR135 (is_zh_CN?"对主机重定向":"Redirect to host")
#define LU_STR136 (is_zh_CN?"传输期间生存时间为0":"Survival time during transmission is 0")
#define LU_STR137 (is_zh_CN?"坏的IP首部（包括各种差错）":"Bad IP header (including various errors)")
#define LU_STR138 (is_zh_CN?"获取网卡列表失败!":"Failed to obtain network card list!")
#define LU_STR139 (is_zh_CN?"保存到文件":"Save to file")
#define LU_STR140 (is_zh_CN?"保存失败，文件创建失败":"Save failed, file creation failed")
#define LU_STR141 (is_zh_CN?"保存成功":"Save success")
#define LU_STR142 (is_zh_CN?"停止捕获":"Stop capturing")
#define LU_STR143 (is_zh_CN?"是否保存到文件?":"Save to file?")
#define LU_STR144 (is_zh_CN?"清空历史":"Clear History")
#define LU_STR145 (is_zh_CN?"确认清空历史?":"Are you sure to clear history?")
#define LU_STR390 (is_zh_CN?"刷新":"Refresh")
#define LU_STR413 (is_zh_CN?"置顶":"Top")
#define LU_STR414 (is_zh_CN?"开始":"Start")
#define LU_STR415 (is_zh_CN?"暂停捕获":"Pause capturing")
#define LU_STR416 (is_zh_CN?"停止捕获":"Stop capturing")
#define LU_STR417 (is_zh_CN?"保存到文件":"Save to file")
#define LU_STR418 (is_zh_CN?"清空":"Clear")
#define LU_STR419 (is_zh_CN?"UDP解析为: ":"UDP Resolve as")
#define LU_STR420 (is_zh_CN?"网卡流量: ":"Network card traffic")
#define LU_STR421 (is_zh_CN?"显示过滤器，输入命令以过滤数据":"Display filters, enter commands to filter data")


#endif // MAINWINDOW_H_LW
