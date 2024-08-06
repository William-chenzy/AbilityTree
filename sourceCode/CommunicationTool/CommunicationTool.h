#pragma once

#include <QTableWidgetItem>
#include <QScrollArea>
#include <QDialog>
#include <thread>
#include <QTimer>
#include <mutex>
#include <QIcon>
#include <deque>
#include "ui_CommunicationTool.h"
#include "GlobalDefine.h"
class CommunicationTool;
class TcpSever;
class TcpClient;

namespace Communication {
	typedef struct FtpDUStu { std::string src, dst; bool is_download; }FtpDUStu;
	typedef struct FtpStu { int size; std::string type, time, parameter, user; QIcon icon; }FtpStu;
	typedef std::map<std::string, Communication::FtpStu> FtpMap;  //file name, file info
	typedef std::pair<std::string, std::string> DataStu;      //ip&port, message
	enum Cb_type { MsG, ConnECT, NewLINK, CloseLINK, DisCONNECT, NewDATA, SendDONE,FtpCMD };
	enum L_Status { DISCONNECT, CONNECT, RUNNING, EXECUTE };
	class Link : public QObject {
	protected:
		Socket_fd soc_fd = 0;
	public:
		L_Status l_s = DISCONNECT;
		virtual void Send(std::string cli, void* data, unsigned long long size) = 0;
		virtual bool Connect(std::string ip, int port) = 0;
		virtual void Close() = 0;
		virtual void Run() = 0;
	};

	class RawTcpSever : public Link {
	protected:
		void Run() override;
		RawTcpSever* son;
		std::map<std::string, int>client_map;
	public:
		void Close() override;
		bool Connect(std::string ip, int port) override;
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;
		void Send(std::string cli, void* data, unsigned long long size) override;
	};

	class RawTcpClient : public Link {
	protected:
		void Run() override;
		RawTcpClient* son;
	public:
		void Close() override;
		bool Connect(std::string ip, int port) override;
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;
		void Send(std::string cli, void* data, unsigned long long size) override;
	};

	class RawUdp : public Link {
	protected:
		void Run() override;
		RawUdp* son;
	public:
		void Close() override;
		bool Connect(std::string ip, int port) override;
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;
		void Send(std::string cli, void* data, unsigned long long size) override;
	};

	class ModbusSlave {
	public:
		void SetSlaveId(uchar id);
		void WriteDate(int type, ushort val, Ulong addr);
		bool SetAddr(Ulong _start, Ulong _len, ushort type);
		const std::vector<bool>* const GetBitPtr(bool is_coil) { return is_coil ? &coil : &input; }
		const std::vector<ushort>* const GetwordPtr(bool is_holding) { return is_holding ? &holding : &input_R; }
		Ulong GetStartAddr(int type) { return type <= 2 ? (type == 1 ? coil_start : input_start) : (type == 3 ? holding_start : input_R_start); }

	protected:
		std::string AnalysisMessage(std::string msg);
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;
		std::string ReplyMessage(uchar* num, uchar id, uchar _type, std::deque<uchar> _dat);

		std::vector<bool> coil;
		std::vector<bool> input;
		std::vector<ushort> holding;
		std::vector<ushort> input_R;

		uchar slave_id = 1;
		Ulong coil_start = 0;
		Ulong input_start = 0;
		Ulong holding_start = 0;
		Ulong input_R_start = 0;
	};

	class ModbusPoll {
	public:
		void SetSlaveId(uchar id) { slave_id = id; }
		void SetScanTime(Ulong time) { scan_time = time; }
		bool SetAddr(Ulong _start, Ulong _len, ushort type);
		const std::vector<bool>* const GetBitPtr(bool is_coil) { return is_coil ? &coil : &input; }
		const std::vector<ushort>* const GetwordPtr(bool is_holding) { return is_holding ? &holding : &input_R; }
		Ulong GetStartAddr(int type) { return type <= 2 ? (type == 1 ? coil_start : input_start) : (type == 3 ? holding_start : input_R_start); }

	protected:
		void AnalysisMessage(std::string msg);
		std::string CreateMessage(uchar type, ushort add, ushort len, ushort num);
		void ReplyMessage(uchar* num, uchar id, uchar _type, std::deque<uchar> _dat);
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;

		std::vector<bool> coil;
		std::vector<bool> input;
		std::vector<ushort> holding;
		std::vector<ushort> input_R;

		uchar slave_id = 1;
		Ulong coil_start = 0;
		Ulong input_start = 0;
		Ulong holding_start = 0;
		Ulong input_R_start = 0;
		Ulong scan_time = 1000;
		Ulong scan_num[4]{ 0,0,0,0 };
	};

	class RawCOM : public Link {
	private:
		bool Connect(std::string com, int baud) override;
		virtual void CallBack(Communication::Cb_type type, Communication::DataStu data) = 0;
	protected:
		void Run() override;
		RawCOM* son;
		HANDLE hCom;
	public:
		void Close() override;
		std::list<std::string> GetComList();
		//parity     ：校验方式 EVENPARITY  偶校验 MARKPARITY  标号校验 NOPARITY    无校验 ODDPARITY   奇校验 SPACEPARITY 空格校验 
		//byte_size  ：数据位大小 4，5，6，7，8
		//stop_bits  ：停止位 ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
		bool ConnectCom(std::string com, int baud,int parity, int byte_size, int stop_bits);
		void Send(std::string cli, void* data, unsigned long long size) override;
	};
};

class TcpSever : public Communication::RawTcpSever {
	Q_OBJECT
public:
	TcpSever() { son = this; }
	int recv_encoding = 0;   //0:ascii,  1:hex
	int send_encoding = 0;   //0:ascii,  1:hex

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data) override;
};

class TcpClient : public Communication::RawTcpClient {
	Q_OBJECT
public:
	TcpClient() { son = this; }
	int recv_encoding = 0;   //0:ascii,  1:hex
	int send_encoding = 0;   //0:ascii,  1:hex

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data) override;
};

class Udp : public Communication::RawUdp {
	Q_OBJECT
public:
	Udp() { son = this; }
	int recv_encoding = 0;   //0:ascii,  1:hex
	int send_encoding = 0;   //0:ascii,  1:hex

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data) override;
};

class COM : public Communication::RawCOM {
	Q_OBJECT
public:
	COM() { son = this; }

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data) override;
};

class Ftp : public Communication::Link {
	Q_OBJECT
public:
	Ftp();
	void Close();
	void GetFileList();
	void ToFtpPath(std::string path);
	void ReadLocalFile(std::string path);
	bool RemoveFile(std::string path, bool is_local);
	bool UploadFile(std::string src, std::string dst);
	bool DownloadFile(std::string src, std::string dst);
	bool RenameFile(std::string path, std::string name, bool is_local);
	bool Connect(std::string ip, int port,std::string user, std::string pwd);

protected:
	void Run() {};
	bool Connect(std::string ip, int port) override;
	void Send(std::string cli, void* data, unsigned long long size) override;
	int SetCmd(std::string cmd, bool is_data_cmd = false, bool is_no_recv = false);

private slots:
	void CmdCallBack(Communication::Cb_type type, Communication::DataStu data);
	void DataCallBack(Communication::Cb_type type, Communication::DataStu data);

public:
	Communication::FtpMap local_map;
	Communication::FtpMap ftp_map;
	int sort_type = 2;

private:
	std::mutex lock;
	std::string ipp;
	TcpClient cmdsoc;
	TcpClient valsoc;
	std::string cmd_msg;
	std::deque<std::string> val_msg;
	bool cmdsoc_b, valsoc_b;

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data);
};

class ModbusTcpSlave : public Communication::Link, public Communication::ModbusSlave {
	Q_OBJECT
public:
	void Close();
	bool Connect(std::string ip, int port) override;
	ModbusTcpSlave() { connect(&soc, &TcpSever::CallBack, this, &ModbusTcpSlave::SevCallBack); };

protected:
	void Run() {};
	void Send(std::string cli, void* data, unsigned long long size) {};

private slots:
	void SevCallBack(Communication::Cb_type type, Communication::DataStu data);

private:
	TcpSever soc;

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data);
};

class ModbusTcpPoll : public Communication::Link, public Communication::ModbusPoll {
	Q_OBJECT
public:
	void Close();
	bool Connect(std::string ip, int port) override;
	void WriteDate(int type, ushort val, Ulong addr);
	ModbusTcpPoll() { connect(&soc, &TcpClient::CallBack, this, &ModbusTcpPoll::CliCallBack); };

protected:
	void Run() override;
	void Send(std::string cli, void* data, unsigned long long size) {};

private slots:
	void CliCallBack(Communication::Cb_type type, Communication::DataStu data);

private:
	TcpClient soc;

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data);
};

class ModbusRtuSlave : public Communication::Link, public Communication::ModbusSlave {
	Q_OBJECT
public:
	void Close();
	bool ConnectCom(std::string _com, int buad, int parity, int byte_size, int stop_size);
	ModbusRtuSlave() { connect(&com, &COM::CallBack, this, &ModbusRtuSlave::ComCallBack); };

protected:
	void Send(std::string cli, void* data, unsigned long long size) {};
	bool Connect(std::string ip, int port) { return true; };
	void Run() {};

private:
	COM com;

private slots:
	void ComCallBack(Communication::Cb_type type, Communication::DataStu data);

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data);
};

class ModbusRtuPoll : public Communication::Link, public Communication::ModbusPoll {
	Q_OBJECT
public:
	void Close();
	void WriteDate(int type, ushort val, Ulong addr);
	bool ConnectCom(std::string _com, int buad, int parity, int byte_size, int stop_size);
	ModbusRtuPoll() { connect(&com, &COM::CallBack, this, &ModbusRtuPoll::ComCallBack); };

protected:
	void Send(std::string cli, void* data, unsigned long long size) {};
	bool Connect(std::string ip, int port) { return true; };
	void Run() override;

private slots:
	void ComCallBack(Communication::Cb_type type, Communication::DataStu data);

private:
	COM com;

signals:
	void CallBack(Communication::Cb_type type, Communication::DataStu data);
};


class CommunicationTool : public QDialog {
	Q_OBJECT
public:
	CommunicationTool(QWidget* parent = nullptr);
	~CommunicationTool() = default;
	void SwitchLanguage();

private:
	void AddMessageToLabel(QLabel* label, QString msg);
	std::list<std::string> FtpTableSort(Communication::FtpMap* ptr, int sort_type);

private slots:
	void on_tabWidget_currentChanged(int index);
	//=================================================== TCP ===================================================
	void on_pushButton_tcp_sev_ok_clicked();
	void on_pushButton_tcp_cli_ok_clicked();
	void on_checkBox_tcp_sev_auto_clicked();
	void on_checkBox_tcp_cli_auto_clicked();
	void on_pushButton_tcp_sev_send_clicked();
	void on_pushButton_tcp_cli_send_clicked();
	void on_radioButton_tcp_sev_hex_toggled(bool state);
	void on_radioButton_tcp_cli_hex_toggled(bool state);
	void on_comboBox_tcp_cli_currentIndexChanged(QString text);
	void TcpSevCallBack(Communication::Cb_type type, Communication::DataStu data);
	void TcpCliCallBack(Communication::Cb_type type, Communication::DataStu data);
	void on_pushButton_tcp_sev_clr_clicked() { ui.label_tcp_sev_msg->setText(""); };
	void on_pushButton_tcp_cli_clr_clicked() { ui.label_tcp_cli_msg->setText(""); };
	void on_radioButton_tcp_sev_hex_2_toggled(bool state) { ts.recv_encoding = state; };
	void on_radioButton_tcp_cli_hex_2_toggled(bool state) { tc.recv_encoding = state; };

	//=================================================== UDP ===================================================
	void on_pushButton_udp_ok_clicked();
	void on_checkBox_udp_auto_clicked();
	void on_pushButton_udp_send_clicked();
	void on_radioButton_udp_hex_toggled(bool state);
	void UdpCallBack(Communication::Cb_type type, Communication::DataStu data);
	void on_pushButton_udp_clr_clicked() { ui.label_udp_msg->setText(""); };
	void on_radioButton_udp_hex_2_toggled(bool state) { udp.recv_encoding = state; };

	//=================================================== FTP ===================================================
	void DownloadOrUploadFiles();
	void RefreshFtpTable(int mode);
	void ShowFtpMenu(const QPoint& pt);
	void on_pushButton_ftp_ok_clicked();
	void on_pushButton_ftp_refresh_2_clicked();
	void on_comboBox_ftp_path_currentIndexChanged();
	void on_tableWidget_itemChanged(QTableWidgetItem*);
	void on_tableWidget_2_itemChanged(QTableWidgetItem*);
	void on_tableWidget_itemDoubleClicked(QTableWidgetItem*);
	void on_tableWidget_2_itemDoubleClicked(QTableWidgetItem*);
	void FtpCallBack(Communication::Cb_type type, Communication::DataStu data); 

	//=============================================== Modbus Tcp ================================================
	void on_pushButton_mptcp_ok_clicked();
	void on_pushButton_mstcp_ok_clicked();
	void on_spinBox_mptcp_valueChanged(int val);
	void on_spinBox_mstcp_id_valueChanged(int val);
	void on_spinBox_mptcp_id_valueChanged(int val);
	void on_pushButton_mstcp_set_clicked(bool type);
	void on_pushButton_mptcp_set_clicked(bool type);
	void on_tableWidget_mstcp_itemChanged(QTableWidgetItem* item);
	void on_tableWidget_mptcp_itemDoubleClicked(QTableWidgetItem* item);
	void MtsCallBack(Communication::Cb_type type, Communication::DataStu data);
	void MtpCallBack(Communication::Cb_type type, Communication::DataStu data);

	//=============================================== Modbus Rtu ================================================
	void on_pushButton_mprtu_ok_clicked();
	void on_pushButton_msrtu_ok_clicked();
	void on_spinBox_mprtu_valueChanged(int val);
	void on_spinBox_msrtu_id_valueChanged(int val);
	void on_spinBox_mprtu_id_valueChanged(int val);
	void on_pushButton_msrtu_set_clicked(bool type);
	void on_pushButton_mprtu_set_clicked(bool type);
	void on_tableWidget_msrtu_itemChanged(QTableWidgetItem* item);
	void on_tableWidget_mprtu_itemDoubleClicked(QTableWidgetItem* item);
	void MrsCallBack(Communication::Cb_type type, Communication::DataStu data);
	void MrpCallBack(Communication::Cb_type type, Communication::DataStu data);

private:
	QTimer tcpsev, tcpcli,udpt, ftpt;
	TcpSever ts;
	TcpClient tc;
	Udp udp;
	Ftp ftp;
	COM com;
	ModbusTcpSlave  mts;
	ModbusTcpPoll   mtp;
	ModbusRtuSlave  mrs;
	ModbusRtuPoll   mrp;
	std::deque<Communication::FtpDUStu> ftpdu;
	std::map<QLabel*, QScrollArea*>lab_scr;
	Ui::CommunicationTool ui;
};

#define PACK_LENGTH 2097152