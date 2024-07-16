#include "Protocol.h"
#include <QCloseEvent>
#include <QScrollBar>
#include <QAction>
#include <QDebug>
#include <QMenu>
#include <fstream>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QFileIconProvider>
using namespace LxProtocol;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

Protocol::Protocol(std::string ip, int port, QWidget *parent) : QDialog(parent){
	ui.setupUi(this);

	ui.lineEdit_tcp_cli_ip->setText(ip.c_str());
	ui.lineEdit_ftp_ip->setText(ip.c_str());
	ui.lineEdit_mptcp_ip->setText(ip.c_str());
	ui.lineEdit_tcp_cli_port->setText(QString::number(port));

	lab_scr[ui.label_tcp_sev_msg] = ui.scrollArea;
	lab_scr[ui.label_tcp_cli_msg] = ui.scrollArea_2;
	lab_scr[ui.label_udp_msg] = ui.scrollArea_3;
	lab_scr[ui.label_ftp_msg] = ui.scrollArea_4;
	lab_scr[ui.label_can_msg] = ui.scrollArea_5;

	Qt::WindowFlags windowFlag = Qt::Dialog;
	windowFlag |= Qt::WindowMinimizeButtonHint;
	windowFlag |= Qt::WindowMaximizeButtonHint;
	windowFlag |= Qt::WindowCloseButtonHint;
	setWindowFlags(windowFlag);

	qRegisterMetaType<LxProtocol::DataStu>("LxProtocol::DataStu");
	qRegisterMetaType<LxProtocol::Cb_type>("LxProtocol::Cb_type");
	connect(&udp, &Udp::CallBack, this, &Protocol::UdpCallBack);
	connect(&ftp, &Ftp::CallBack, this, &Protocol::FtpCallBack);
	connect(&can, &CAN::CallBack, this, &Protocol::CanCallBack);
	connect(&ts, &TcpSever::CallBack, this, &Protocol::TcpSevCallBack);
	connect(&tc, &TcpClient::CallBack, this, &Protocol::TcpCliCallBack);
	connect(&mtp, &ModbusTcpPoll::CallBack, this, &Protocol::MtpCallBack);
	connect(&mts, &ModbusTcpSlave::CallBack, this, &Protocol::MtsCallBack);
	connect(&mrp, &ModbusRtuPoll::CallBack, this, &Protocol::MrpCallBack);
	connect(&mrs, &ModbusRtuSlave::CallBack, this, &Protocol::MrsCallBack);
	connect(&ftpt, &QTimer::timeout, this, &Protocol::DownloadOrUploadFiles);
	connect(&udpt, &QTimer::timeout, this, &Protocol::on_pushButton_udp_send_clicked);
	connect(&tcpsev, &QTimer::timeout, this, &Protocol::on_pushButton_tcp_sev_send_clicked);
	connect(&tcpcli, &QTimer::timeout, this, &Protocol::on_pushButton_tcp_cli_send_clicked);

	auto tab_set = [&](QTableWidget* p,bool t = false) {
		p->setShowGrid(false);
		p->setColumnWidth(0, 15);
		p->setColumnWidth(1, t ? 170 : 230);
		p->setColumnWidth(2, 90);
		p->setColumnWidth(3, 60);
		p->setColumnWidth(4, 150);
		if (t)p->setColumnWidth(5, 60);
		if (t)p->setColumnWidth(6, 80);
		p->verticalHeader()->setDefaultSectionSize(20);
		connect(p, &QTableWidget::customContextMenuRequested, this, &Protocol::ShowFtpMenu);
	};
	tab_set(ui.tableWidget);
	tab_set(ui.tableWidget_2, true);

	ui.tableWidget_mstcp->setRowCount(10);
	ui.tableWidget_mptcp->setRowCount(10);
	ui.tableWidget_mstcp->verticalHeader()->setDefaultSectionSize(28);
	ui.tableWidget_mptcp->verticalHeader()->setDefaultSectionSize(28);
	ui.tableWidget_mstcp->horizontalHeader()->setDefaultSectionSize(60);
	ui.tableWidget_mptcp->horizontalHeader()->setDefaultSectionSize(60);

	ui.tableWidget_msrtu->setRowCount(10);
	ui.tableWidget_mprtu->setRowCount(10);
	ui.tableWidget_msrtu->verticalHeader()->setDefaultSectionSize(28);
	ui.tableWidget_mprtu->verticalHeader()->setDefaultSectionSize(28);
	ui.tableWidget_msrtu->horizontalHeader()->setDefaultSectionSize(60);
	ui.tableWidget_mprtu->horizontalHeader()->setDefaultSectionSize(60);

	ui.comboBox_ftp_path->setItemText(0, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
	ui.comboBox_ftp_path->setItemText(1, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
	ui.comboBox_ftp_path->setItemText(2, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
	ui.comboBox_ftp_path->setItemText(3, QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
	ui.comboBox_ftp_path->setItemText(4, QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

	if (!is_Linux)ui.tabWidget->removeTab(5);
	SwitchLanguage();
}
void Protocol::SwitchLanguage() {
	this->setWindowTitle(LU_STR378);
	ui.groupBox->setTitle(LU_STR379);
	ui.groupBox_2->setTitle(LU_STR380);
	ui.label->setText(LU_STR381);
	ui.pushButton_tcp_sev_ok->setText(LU_STR382);
	ui.label_11->setText(LU_STR383);
	ui.checkBox_tcp_sev_auto->setText(LU_STR384);
	ui.pushButton_tcp_sev_clr->setText(LU_STR385);
	ui.label_5->setText(LU_STR386);
	ui.label_6->setText(LU_STR387);
	ui.pushButton_tcp_sev_send->setText(LU_STR388);
	ui.label_3->setText(LU_STR381);
	ui.pushButton_tcp_cli_ok->setText(LU_STR382);
	ui.label_15->setText(LU_STR389);
	ui.checkBox_tcp_cli_auto->setText(LU_STR384);
	ui.pushButton_tcp_cli_clr->setText(LU_STR385);
	ui.label_9->setText(LU_STR386);
	ui.label_10->setText(LU_STR387);
	ui.pushButton_tcp_cli_send->setText(LU_STR388);
	ui.label_21->setText(LU_STR381);
	ui.pushButton_udp_ok->setText(LU_STR382);
	ui.label_23->setText(LU_STR383);
	ui.checkBox_udp_auto->setText(LU_STR384);
	ui.pushButton_udp_clr->setText(LU_STR385);
	ui.label_17->setText(LU_STR386);
	ui.label_18->setText(LU_STR387);
	ui.pushButton_udp_send->setText(LU_STR388);
	ui.label_25->setText(LU_STR381);
	ui.pushButton_ftp_ok->setText(LU_STR382);
	ui.pushButton_ftp_refresh_2->setText(LU_STR390);
	QStringList heardList;
	if (is_zh_CN)heardList << "#" << "名称" << "大小" << "类型" << "修改时间";
	else heardList << "#" << "Name" << "Size" << "Type" << "Last modify time";
	ui.tableWidget->setHorizontalHeaderLabels(heardList);
	QStringList heardList2;
	if (is_zh_CN)heardList2 << "名称" << "大小" << "类型" << "修改时间" << "所有者" << "属性";
	else heardList2 << "Name" << "Size" << "Type" << "Last modify time" << "User" << "Param";
	ui.tableWidget_2->setHorizontalHeaderLabels(heardList2);
	ui.label_27->setText(LU_STR381);
	ui.label_29->setText(LU_STR381);
	ui.pushButton_mstcp_ok->setText(LU_STR382);
	ui.pushButton_mptcp_ok->setText(LU_STR382);
	ui.label_34->setText(LU_STR391); 
	ui.label_35->setText(LU_STR391);
	ui.label_31->setText(LU_STR392);
	ui.label_7->setText(LU_STR393);
	ui.label_8->setText(LU_STR393);
	ui.label_32->setText(LU_STR394);
	ui.label_33->setText(LU_STR394);
	ui.label_16->setText(LU_STR395);
	ui.label_12->setText(LU_STR395);
	ui.pushButton_mstcp_set->setText(LU_STR396);
	ui.pushButton_mptcp_set->setText(LU_STR396);
	ui.label_43->setText(LU_STR396);
	ui.label_36->setText(LU_STR396);
	ui.label_44->setText(LU_STR398);
	ui.label_37->setText(LU_STR398);
	ui.label_49->setText(LU_STR399);
	ui.label_53->setText(LU_STR399);
	ui.label_50->setText(LU_STR400);
	ui.label_52->setText(LU_STR400);
	ui.label_51->setText(LU_STR401);
	ui.label_54->setText(LU_STR401);
	ui.pushButton_msrtu_ok->setText(LU_STR382);
	ui.pushButton_mprtu_ok->setText(LU_STR382);
	ui.label_45->setText(LU_STR391);
	ui.label_38->setText(LU_STR391);
	ui.label_40->setText(LU_STR393);
	ui.label_46->setText(LU_STR393);
	ui.label_39->setText(LU_STR392);
	ui.label_41->setText(LU_STR394);
	ui.label_47->setText(LU_STR394);
	ui.label_48->setText(LU_STR395);
	ui.label_42->setText(LU_STR395);
	ui.pushButton_msrtu_set->setText(LU_STR396);
	ui.pushButton_mprtu_set->setText(LU_STR396);
}

void Protocol::TcpSevCallBack(Cb_type type, DataStu data) {
	if (type == MsG)AddMessageToLabel(ui.label_tcp_sev_msg, data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.pushButton_tcp_sev_ok->setText(state ? LU_STR224 : LU_STR225);
		ui.lineEdit_tcp_sev_ip->setEnabled(!state);
		ui.lineEdit_tcp_sev_port->setEnabled(!state);
		ui.pushButton_tcp_sev_send->setEnabled(state);
		ui.checkBox_tcp_sev_auto->setEnabled(state);
		if (!state) ui.comboBox_tcp_sev->clear(), ui.comboBox_tcp_sev->addItem("ALL CLIENT");
		AddMessageToLabel(ui.label_tcp_sev_msg, QString(LU_STR226).append(state ? LU_STR227 : LU_STR228));
	}
	else if (type == NewLINK || type == CloseLINK) {
		bool state = type == NewLINK;
		std::string ipp = data.first;
		if (state) ui.comboBox_tcp_sev->addItem(ipp.c_str());
		else ui.comboBox_tcp_sev->removeItem(ui.comboBox_tcp_sev->findText(ipp.c_str()));
		ipp = (state ? "new client connect ip: " : "client close connect ip: ") + ipp;
		AddMessageToLabel(ui.label_tcp_sev_msg, ipp.c_str());
	}
	else if (type == NewDATA) {
		auto msg = ui.radioButton_tcp_sev_hex_2->isChecked() ? asciiTohex(data.second) : data.second;
		std::string ipp = "RECV " + std::to_string(data.second.size()) + " BYTE FROM " + data.first + " >\r\n" + msg;
		AddMessageToLabel(ui.label_tcp_sev_msg, QString::fromLocal8Bit(ipp.c_str()));
	}
}
void Protocol::TcpCliCallBack(Cb_type type, DataStu data) {
	if (type == MsG)AddMessageToLabel(ui.label_tcp_cli_msg, data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		if (state && ui.comboBox_tcp_cli->findText(data.first.c_str()) == -1)
			ui.comboBox_tcp_cli->addItem(data.first.c_str());
		ui.pushButton_tcp_cli_ok->setText(state ? LU_STR224 : LU_STR225);
		ui.lineEdit_tcp_cli_ip->setEnabled(!state);
		ui.lineEdit_tcp_cli_port->setEnabled(!state);
		ui.comboBox_tcp_cli->setEnabled(!state);
		ui.pushButton_tcp_cli_send->setEnabled(state);
		ui.checkBox_tcp_cli_auto->setEnabled(state);
		if(type == CloseLINK) AddMessageToLabel(ui.label_tcp_cli_msg, data.first.c_str());
		AddMessageToLabel(ui.label_tcp_cli_msg, QString(LU_STR229).append(state ? LU_STR227 : LU_STR228));
	}
	else if (type == NewDATA) {
		auto msg = ui.radioButton_tcp_cli_hex_2->isChecked() ? asciiTohex(data.second) : data.second;
		std::string ipp = "RECV " + std::to_string(data.second.size()) + " BYTE >\r\n" + msg;
		AddMessageToLabel(ui.label_tcp_cli_msg, QString::fromLocal8Bit(ipp.c_str()));
	}
}
void Protocol::UdpCallBack(Cb_type type, DataStu data) {
	if (type == MsG)AddMessageToLabel(ui.label_udp_msg, data.second.c_str()); 
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.pushButton_udp_ok->setText(state ? LU_STR224 : LU_STR225);
		ui.lineEdit_udp_ip->setEnabled(!state);
		ui.lineEdit_udp_port->setEnabled(!state);
		ui.comboBox_udp->setEnabled(!state);
		ui.pushButton_udp_send->setEnabled(state);
		ui.checkBox_udp_auto->setEnabled(state);
		ui.comboBox_udp->setEnabled(state);
		AddMessageToLabel(ui.label_udp_msg, QString(LU_STR229).append(state ? LU_STR227 : LU_STR228));
	}
	else if (type == SendDONE) {
		AddMessageToLabel(ui.label_udp_msg, data.second.c_str());
		if (ui.comboBox_udp->findText(data.first.c_str()) == -1)
			ui.comboBox_udp->addItem(data.first.c_str());
	}
	else if (type == NewDATA) {
		auto msg = ui.radioButton_udp_hex_2->isChecked() ? asciiTohex(data.second) : data.second;
		std::string ipp = "RECV " + std::to_string(data.second.size()) + " BYTE >\r\n" + msg;
		AddMessageToLabel(ui.label_udp_msg, QString::fromLocal8Bit(ipp.c_str()));
	}
}
void Protocol::FtpCallBack(Cb_type type, DataStu data) {
	if (type == MsG)AddMessageToLabel(ui.label_ftp_msg, data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.lineEdit_ftp_ip->setEnabled(!state);
		ui.lineEdit_ftp_port->setEnabled(!state);
		ui.lineEdit_ftp_user->setEnabled(!state);
		ui.lineEdit_ftp_pw->setEnabled(!state);
		ui.lineEdit_ftp_path_2->setEnabled(state);
		ui.pushButton_ftp_refresh_2->setEnabled(state);
		ui.pushButton_ftp_ok->setText(state ? LU_STR224 : LU_STR225);
		if(state)ftpt.start(100);
		else ftpt.stop();
		for (; ui.tableWidget_2->rowCount() > 0;) ui.tableWidget_2->removeRow(0);
		for (; ui.tableWidget->rowCount() > 0;) ui.tableWidget->removeRow(0);
		AddMessageToLabel(ui.label_ftp_msg, QString(LU_STR230).append(state ? LU_STR227 : LU_STR228));
	}
	else if (type == FtpCMD) {
		bool show_msg = true;
		if (data.first.substr(0, 3) == "PWD") {
			auto ph = data.second.substr(data.second.find('"') + 1);
			ui.lineEdit_ftp_path_2->setText(ph.substr(0, ph.find('"')).c_str());
		}
		else if (data.first.substr(0, 5) == "LOCAL") ui.comboBox_ftp_path->setItemText(5, data.second.c_str()), ui.comboBox_ftp_path->setCurrentIndex(5);
		else if (data.first == "STOR")ui.progressBar->setValue(atoi(data.second.c_str())), show_msg = false;
		else if (data.first == "RETR")ui.progressBar_2->setValue(atoi(data.second.c_str())), show_msg = false;
		if (show_msg)AddMessageToLabel(ui.label_ftp_msg, QString::fromUtf8(data.second.c_str()).toStdString().c_str());
	}
	else if (type == NewDATA) {
		if (data.first == "LIST" || data.first == "LOCAL") {
			int row = 0;
			bool bc = data.first == "LIST";
			auto pt = bc ? &ftp.ftp_map : &ftp.local_map;
			auto up = bc ? ui.tableWidget_2 : ui.tableWidget;
			auto li = FtpTableSort(pt, ftp.sort_type);
			for (; up->rowCount() > 0;) up->removeRow(0);
			for (auto x : li) {
				auto i = (*pt)[x];
				up->insertRow(row);
				QTableWidgetItem* btItem = new QTableWidgetItem();
				btItem->setIcon(i.icon);
				up->setItem(row, 0, btItem);

				std::string kcu = " BKBMBGBTBEB";
				float si = i.size;
				while (si > 1024)si /= 1024.f, kcu = kcu.substr(2);
				auto sis = QString::number(si, 'f', 2) + QString(" %2").arg(kcu.substr(0, 2).c_str());
				QTableWidgetItem* btItem2 = new QTableWidgetItem();
				btItem2->setText(si ? sis : " ");
				btItem2->setTextAlignment(Qt::AlignRight);
				up->setItem(row, 2, btItem2);

				up->setItem(row, 1, new QTableWidgetItem(QString::fromLocal8Bit(x.c_str())));
				up->setItem(row, 3, new QTableWidgetItem(i.type.c_str()));
				up->setItem(row, 4, new QTableWidgetItem(i.time.c_str()));
				if (bc) up->setItem(row, 5, new QTableWidgetItem(i.user.c_str()));
				if (bc) up->setItem(row, 6, new QTableWidgetItem(i.parameter.c_str()));
			}
		}
		else AddMessageToLabel(ui.label_ftp_msg, data.second.c_str());
	}
}
void Protocol::MtsCallBack(Cb_type type, DataStu data) {
	if (type == MsG) ui.label_mstcp_msg->setText(data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.lineEdit_mstcp_ip->setEnabled(!state);
		ui.lineEdit_mstcp_port->setEnabled(!state);
		ui.pushButton_mstcp_ok->setText(state ? LU_STR224 : LU_STR225);
		if (state) on_pushButton_mstcp_set_clicked(false);
		else for (;ui.tableWidget_mstcp->columnCount();) ui.tableWidget_mstcp->removeColumn(0);
	}
	else if (type == NewDATA) {
		int type = ui.comboBox_mstcp->currentIndex() + 1;
		if (data.first[0] - 48 != type)on_pushButton_mstcp_set_clicked(true);
	}
}
void Protocol::MtpCallBack(Cb_type type, DataStu data) {
	if (type == MsG) ui.label_mptcp_msg->setText(data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT; 
		ui.lineEdit_mptcp_ip->setEnabled(!state);
		ui.lineEdit_mptcp_port->setEnabled(!state);
		ui.pushButton_mptcp_ok->setText(state ? LU_STR224 : LU_STR225);
		if (state) on_pushButton_mptcp_set_clicked(false);
		else for (; ui.tableWidget_mptcp->columnCount();) ui.tableWidget_mptcp->removeColumn(0);
	}
	else if (type == NewDATA) {
		int type = ui.comboBox_mptcp->currentIndex() + 1;
		if (data.first[0] - 48 == type)on_pushButton_mptcp_set_clicked(true);
	}
}
void Protocol::MrsCallBack(Cb_type type, DataStu data) {
	if (type == MsG) ui.label_msrtu_msg->setText(data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.comboBox_msrtu_com->setEnabled(!state);
		ui.comboBox_msrtu_baud->setEnabled(!state);
		ui.comboBox_msrtu_parity->setEnabled(!state);
		ui.comboBox_msrtu_bs->setEnabled(!state);
		ui.comboBox_msrtu_sts->setEnabled(!state);
		ui.pushButton_msrtu_ok->setText(state ? LU_STR224 : LU_STR225);
		if (state) on_pushButton_msrtu_set_clicked(false);
		else for (; ui.tableWidget_msrtu->columnCount();) ui.tableWidget_msrtu->removeColumn(0);
	}
	else if (type == NewDATA) {
		int type = ui.comboBox_msrtu->currentIndex() + 1;
		if (data.first[0] - 48 != type)on_pushButton_msrtu_set_clicked(true);
	}
}
void Protocol::MrpCallBack(Cb_type type, DataStu data) {
	if (type == MsG) ui.label_mprtu_msg->setText(data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.comboBox_mprtu_com->setEnabled(!state);
		ui.comboBox_mprtu_baud->setEnabled(!state);
		ui.comboBox_mprtu_parity->setEnabled(!state);
		ui.comboBox_mprtu_bs->setEnabled(!state);
		ui.comboBox_mprtu_sts->setEnabled(!state);
		ui.pushButton_mprtu_ok->setText(state ? LU_STR224 : LU_STR225);
		if (state) on_pushButton_mprtu_set_clicked(false);
		else for (; ui.tableWidget_mprtu->columnCount();) ui.tableWidget_mprtu->removeColumn(0);
	}
	else if (type == NewDATA) {
		int type = ui.comboBox_mprtu->currentIndex() + 1;
		if (data.first[0] - 48 == type)on_pushButton_mprtu_set_clicked(true);
	}
}
void Protocol::CanCallBack(Cb_type type, DataStu data) {
	if (type == MsG) ui.label_can_msg->setText(data.second.c_str());
	else if (type == DisCONNECT || type == ConnECT) {
		bool state = type == ConnECT;
		ui.pushButton_can_ok->setText(state ? LU_STR224 : LU_STR225);
	}
	else if (type == NewDATA) {
		ui.label_can_msg->setText(data.second.c_str());
	}
}

void Protocol::AddMessageToLabel(QLabel* label, QString msg) {
	if (msg.size() == 0)return;

	label->setText(label->text() + GetTime().c_str() + msg + "\r\n");
	auto scr = lab_scr[label]->verticalScrollBar();
	scr->setValue(scr->maximum());
}
void Protocol::closeEvent(QCloseEvent* e){
	this->setVisible(false);
	e->ignore();            //忽视关闭信号，不让QVTK收到此信号
}

void Protocol::on_pushButton_tcp_sev_ok_clicked() {
	if (ui.pushButton_tcp_sev_ok->text() == LU_STR225)
		ts.Connect(ui.lineEdit_tcp_sev_ip->text().toStdString(), ui.lineEdit_tcp_sev_port->text().toInt());
	else ts.Close();
}
void Protocol::on_pushButton_tcp_cli_ok_clicked() {
	if (ui.pushButton_tcp_cli_ok->text() == LU_STR225)
		tc.Connect(ui.lineEdit_tcp_cli_ip->text().toStdString(), ui.lineEdit_tcp_cli_port->text().toInt());
	else tc.Close();
}
void Protocol::on_pushButton_udp_ok_clicked() {
	if (ui.pushButton_udp_ok->text() == LU_STR225)
		udp.Connect(ui.lineEdit_udp_ip->text().toStdString(), ui.lineEdit_udp_port->text().toInt());
	else udp.Close();
}
void Protocol::on_pushButton_ftp_ok_clicked() {
	ui.pushButton_ftp_ok->setEnabled(false);
	if (ui.pushButton_ftp_ok->text() == LU_STR225) {
		std::string ip = ui.lineEdit_ftp_ip->text().toStdString();
		std::string user = ui.lineEdit_ftp_user->text().toStdString();
		std::string pwd = ui.lineEdit_ftp_pw->text().toStdString();
		int port = ui.lineEdit_ftp_port->text().toInt();
		if (ftp.Connect(ip, port, user, pwd)){
			ftp.GetFileList();
			QString document_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
			ftp.ReadLocalFile(document_path.toStdString());
		}
	}
	else ftp.Close();
	ui.pushButton_ftp_ok->setEnabled(true);
}
void Protocol::on_pushButton_mstcp_ok_clicked() {
	ui.pushButton_mstcp_ok->setEnabled(false);
	if (ui.pushButton_mstcp_ok->text() == LU_STR225) {
		std::string ip = ui.lineEdit_mstcp_ip->text().toStdString();
		int port = ui.lineEdit_mstcp_port->text().toInt();
		int id = ui.spinBox_mstcp_id->text().toInt();
		mts.SetSlaveId(id);
		mts.Connect(ip, port);
	}
	else mts.Close();
	ui.pushButton_mstcp_ok->setEnabled(true);
}
void Protocol::on_pushButton_mptcp_ok_clicked() {
	ui.pushButton_mptcp_ok->setEnabled(false);
	if (ui.pushButton_mptcp_ok->text() == LU_STR225) {
		std::string ip = ui.lineEdit_mptcp_ip->text().toStdString();
		int port = ui.lineEdit_mptcp_port->text().toInt();
		int id = ui.spinBox_mptcp_id->text().toInt();
		int sc = ui.spinBox_mptcp->value();
		mtp.SetSlaveId(id);
		mtp.SetScanTime(sc);
		mtp.Connect(ip, port);
	}
	else mtp.Close();
	ui.pushButton_mptcp_ok->setEnabled(true);
}
void Protocol::on_pushButton_msrtu_ok_clicked() {
	ui.pushButton_msrtu_ok->setEnabled(false);
	if (ui.pushButton_msrtu_ok->text() == LU_STR225) {
		std::string com = ui.comboBox_msrtu_com->currentText().toStdString();
		int baud = ui.comboBox_msrtu_baud->currentText().toInt();
		int parity = ui.comboBox_msrtu_parity->currentText().toInt();
		int bs = ui.comboBox_msrtu_bs->currentText().toInt() + 4;
		int sts = ui.comboBox_msrtu_sts->currentText().toInt();
		int id = ui.spinBox_mstcp_id->text().toInt();
		mrs.SetSlaveId(id);
		mrs.ConnectCom(com, baud, parity, bs, sts);
	}
	else mrs.Close();
	ui.pushButton_msrtu_ok->setEnabled(true);
}
void Protocol::on_pushButton_mprtu_ok_clicked() {
	ui.pushButton_mprtu_ok->setEnabled(false);
	if (ui.pushButton_mprtu_ok->text() == LU_STR225) {
		std::string com = ui.comboBox_mprtu_com->currentText().toStdString();
		int baud = ui.comboBox_mprtu_baud->currentText().toInt();
		int parity = ui.comboBox_mprtu_parity->currentText().toInt();
		int bs = ui.comboBox_mprtu_bs->currentText().toInt() + 4;
		int sts = ui.comboBox_mprtu_sts->currentText().toInt();
		int id = ui.spinBox_mptcp_id->text().toInt();
		int sc = ui.spinBox_mptcp->value();
		mrp.SetSlaveId(id);
		mrp.SetScanTime(sc);
		mrp.ConnectCom(com, baud, parity, bs, sts);
	}
	else mrp.Close();
	ui.pushButton_mprtu_ok->setEnabled(true);
}
void Protocol::on_pushButton_can_ok_clicked() {
	ui.pushButton_can_ok->setEnabled(false);
	if (ui.pushButton_can_ok->text() == LU_STR225) {
		can.Connect("", 0);
	}
	else can.Close();
	ui.pushButton_can_ok->setEnabled(true);
}

void Protocol::on_pushButton_tcp_sev_send_clicked() {
	auto msg = ui.textEdit_tcp_sev->toPlainText().toLocal8Bit().toStdString();
	if (!msg.size()) { AddMessageToLabel(ui.label_tcp_sev_msg, LU_STR231); return; }
	if (ui.radioButton_tcp_sev_hex->isChecked())msg = hexToascii(msg);
	auto cli = ui.comboBox_tcp_sev->currentText().toStdString();

	if (cli == "ALL CLIENT")
		for (int i = 1; i < ui.comboBox_tcp_sev->count(); i++) {
			cli = ui.comboBox_tcp_sev->itemData(i, 0).toString().toStdString();
			ts.Send(cli, (void*)msg.c_str(), msg.size());
		}
	else ts.Send(cli, (void*)msg.c_str(), msg.size());
}
void Protocol::on_pushButton_tcp_cli_send_clicked() {
	auto msg = ui.textEdit_tcp_cli->toPlainText().toLocal8Bit().toStdString();
	if (!msg.size()) { AddMessageToLabel(ui.label_tcp_cli_msg, LU_STR231); return; }
	if (ui.radioButton_tcp_cli_hex->isChecked())msg = hexToascii(msg);
	auto cli = ui.lineEdit_tcp_cli_ip->text().toStdString();
	tc.Send(cli, (void*)msg.c_str(), msg.size());
}
void Protocol::on_pushButton_udp_send_clicked() {
	auto msg = ui.textEdit_udp->toPlainText().toLocal8Bit().toStdString();
	if (!msg.size()) { AddMessageToLabel(ui.label_udp_msg, LU_STR231); return; }
	if (ui.radioButton_udp_hex->isChecked())msg = hexToascii(msg);
	auto cli = ui.comboBox_udp->currentText().toStdString();
	udp.Send(cli, (void*)msg.c_str(), msg.size());
}
void Protocol::on_pushButton_can_send_clicked() {
	auto msg = ui.lineEdit_can->text().toStdString();
	if (!msg.size()) { AddMessageToLabel(ui.label_can_msg, LU_STR231); return; }
	msg = hexToascii(msg);
	can.Send("", (void*)msg.data(), msg.size());
}

void Protocol::on_checkBox_tcp_sev_auto_clicked() {
	if (ui.checkBox_tcp_sev_auto->isChecked()) {
		int t = ui.spinBox_tcp_sev_auto->value();
		t = t < 100 ? 100 : t;
		ui.spinBox_tcp_sev_auto->setValue(t);
		tcpsev.start(t);
	}
	else tcpsev.stop();
}
void Protocol::on_checkBox_tcp_cli_auto_clicked() {
	if (ui.checkBox_tcp_cli_auto->isChecked()) {
		int t = ui.spinBox_tcp_cli_auto->value();
		t = t < 100 ? 100 : t;
		ui.spinBox_tcp_cli_auto->setValue(t);
		tcpcli.start(t);
	}
	else tcpcli.stop();
}
void Protocol::on_checkBox_udp_auto_clicked() {
	if (ui.checkBox_udp_auto->isChecked()) {
		int t = ui.spinBox_udp_auto->value();
		t = t < 100 ? 100 : t;
		ui.spinBox_udp_auto->setValue(t);
		udpt.start(t);
	}
	else udpt.stop();
}

void Protocol::on_comboBox_tcp_cli_currentIndexChanged(QString text) {
	std::string ip = text.toStdString();
	int port = stoi(ip.substr(ip.find(':') + 1));
	ip = ip.substr(0, ip.find(':'));
	ui.lineEdit_tcp_cli_ip->setText(ip.c_str());
	ui.lineEdit_tcp_cli_port->setText(QString::number(port));
}

void Protocol::on_radioButton_tcp_sev_hex_toggled(bool state) {
	ts.send_encoding = state;
	auto msg = ui.textEdit_tcp_sev->toPlainText().toLocal8Bit().toStdString();
	msg = state ? asciiTohex(msg) : hexToascii(msg);
	ui.textEdit_tcp_sev->setText(QString::fromLocal8Bit(msg.c_str()));
}
void Protocol::on_radioButton_tcp_cli_hex_toggled(bool state) {
	tc.send_encoding = state;
	auto msg = ui.textEdit_tcp_cli->toPlainText().toLocal8Bit().toStdString();
	msg = state ? asciiTohex(msg) : hexToascii(msg);
	ui.textEdit_tcp_cli->setText(QString::fromLocal8Bit(msg.c_str()));
}
void Protocol::on_radioButton_udp_hex_toggled(bool state) {
	udp.send_encoding = state;
	auto msg = ui.textEdit_udp->toPlainText().toLocal8Bit().toStdString();
	msg = state ? asciiTohex(msg) : hexToascii(msg);
	ui.textEdit_udp->setText(QString::fromLocal8Bit(msg.c_str()));
}

void Protocol::on_tableWidget_itemDoubleClicked(QTableWidgetItem* item){
	auto st = ui.tableWidget->item(item->row(),1)->text().toStdString();
	auto st1 = ui.tableWidget->item(item->row(), 1)->text().toLocal8Bit().toStdString();
	std::string path = ui.comboBox_ftp_path->currentText().toStdString();
	auto pt = ftp.local_map[st1];
	if (pt.type == LU_STR233) {
		if (path[path.size() - 1] == '/')path.pop_back();
		if (st == ".." && path.rfind('/') == std::string::npos)return;
		path = ((st == "..") ? path.substr(0, path.rfind('/') + 1) : path + '/' + st);
		ftp.ReadLocalFile(path);
	}
	else {
		if (ftp.ftp_map.find(st1) != ftp.ftp_map.end()) {
			auto cig = QMessageBox::information(this, LU_STR44, LU_STR232, QMessageBox::Yes, QMessageBox::No);
			if (cig == QMessageBox::No)return;
		}
		FtpDUStu du{ path + '/' + st, st, false };
		ftpdu.push_back(du);
	}
}
void Protocol::on_tableWidget_2_itemDoubleClicked(QTableWidgetItem* item) {
	auto st = ui.tableWidget_2->item(item->row(), 1)->text().toStdString();
	auto st1 = ui.tableWidget_2->item(item->row(), 1)->text().toLocal8Bit().toStdString();
	std::string path = ui.lineEdit_ftp_path_2->text().toStdString();
	auto pt = ftp.ftp_map[st1];
	if (pt.type == LU_STR233) {
		path = ((st == "..") ? path.substr(0, path.rfind('/') + 1) : path + '/' + st);
		ftp.ToFtpPath(path);
		ftp.GetFileList();
	}
	else {
		std::string dst = ui.comboBox_ftp_path->currentText().toStdString() + "/" + st;
		if (QFile::exists(dst.c_str())) {
			auto cig = QMessageBox::information(this, LU_STR44, LU_STR232, QMessageBox::Yes, QMessageBox::No);
			if (cig == QMessageBox::No)return;
		}
		FtpDUStu du{ st, dst, true };
		ftpdu.push_back(du);
	}
}

void Protocol::on_pushButton_ftp_refresh_2_clicked() {
	ui.pushButton_ftp_refresh_2->setEnabled(false);
	ftp.GetFileList();
	ui.pushButton_ftp_refresh_2->setEnabled(true);
}

void Protocol::on_tableWidget_itemChanged(QTableWidgetItem* item) {
	if (item->column() != 1)return;
	if (ftp.local_map.find(item->text().toLocal8Bit().toStdString()) != ftp.local_map.end())return;
	auto cheitem = ftp.local_map.begin();
	for (; cheitem != ftp.local_map.end(); cheitem++)
		if (ui.tableWidget->findItems(QString::fromLocal8Bit(cheitem->first.c_str()), 0).empty())break;

	if (cheitem == ftp.local_map.end())return;
	std::string path = ui.comboBox_ftp_path->currentText().toStdString();
	std::string src = path + "/" + cheitem->first;
	std::string dst = path + "/" + item->text().toStdString();
	if (ftp.RenameFile(QString::fromLocal8Bit(src.c_str()).toStdString(), dst, true))RefreshFtpTable(1);
}
void Protocol::on_tableWidget_2_itemChanged(QTableWidgetItem* item){
	if (item->column() != 1)return;
	if (ftp.ftp_map.find(item->text().toLocal8Bit().toStdString()) != ftp.ftp_map.end())return;
	auto cheitem = ftp.ftp_map.begin();
	for (; cheitem != ftp.ftp_map.end(); cheitem++)
		if (ui.tableWidget_2->findItems(QString::fromLocal8Bit(cheitem->first.c_str()), 0).empty())break;

	if (cheitem == ftp.ftp_map.end())return;
	std::string src = cheitem->first;
	std::string dst = item->text().toStdString();
	if (ftp.RenameFile(QString::fromLocal8Bit(src.c_str()).toStdString(), dst, false))RefreshFtpTable(2);
}

void Protocol::on_tableWidget_mstcp_itemChanged(QTableWidgetItem* item) {
	auto row = item->row();
	auto col = item->column();
	int type = ui.comboBox_mstcp->currentIndex() + 1;
	mts.WriteDate(type, (ushort)item->text().toInt(), col * 10 + row);
}
void Protocol::on_tableWidget_mptcp_itemDoubleClicked(QTableWidgetItem* item) {
	int type = ui.comboBox_mptcp->currentIndex() + 1;
	auto row = item->row();
	auto col = item->column();
	if (type == 2 || type == 4)return;
	ushort val; bool ok;
	QString text = QInputDialog::getText(this, tr(LU_STR234), tr(LU_STR235), QLineEdit::Normal, 0, &ok);
	if (ok && !text.isEmpty()) {
		val = text.toInt();
		mtp.WriteDate(type, val, col * 10 + row);
	}
}
void Protocol::on_pushButton_mstcp_set_clicked(bool cur) {
	static std::mutex lc_;
	if (!lc_.try_lock())return;
	if (!cur) {
		int st = ui.spinBox_mstcp_start->text().toInt();
		int le = ui.spinBox_mstcp_len->text().toInt();
		int ty = ui.comboBox_mstcp->currentIndex();
		if (!mts.SetAddr(st, le, ty))return;
		for (; ui.tableWidget_mstcp->columnCount();) ui.tableWidget_mstcp->removeColumn(0);
		auto val = ui.spinBox_mstcp_len->value();
		auto num = ui.spinBox_mstcp_start->value();
		auto col = val / 10 + (val % 10 ? 1 : 0);
		ui.tableWidget_mstcp->setColumnCount(col);
		QStringList head;
		for (int i = num; i < num + val; i += 10)head << QString("%1").arg(i, 4, 10, QLatin1Char('0'));
		ui.tableWidget_mstcp->setHorizontalHeaderLabels(head);
	}

	int type = ui.comboBox_mstcp->currentIndex() + 1;
	ui.tableWidget_mstcp->blockSignals(true);
	if (type <= 2) {
		auto val = mts.GetBitPtr(type == 1);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mstcp->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	else {
		auto val = mts.GetwordPtr(type == 3);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mstcp->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	ui.tableWidget_mstcp->blockSignals(false);
	lc_.unlock();
}
void Protocol::on_pushButton_mptcp_set_clicked(bool cur){
	static std::mutex lc_;
	if (!lc_.try_lock())return;
	if (!cur) {
		int st = ui.spinBox_mptcp_start->text().toInt();
		int le = ui.spinBox_mptcp_len->text().toInt();
		int ty = ui.comboBox_mptcp->currentIndex();
		if (!mtp.SetAddr(st, le, ty))return;
		for (; ui.tableWidget_mptcp->columnCount();) ui.tableWidget_mptcp->removeColumn(0);
		auto val = ui.spinBox_mptcp_len->value();
		auto num = ui.spinBox_mptcp_start->value();
		auto col = val / 10 + (val % 10 ? 1 : 0);
		ui.tableWidget_mptcp->setColumnCount(col);
		QStringList head;
		for (int i = num; i < num + val; i += 10)head << QString("%1").arg(i, 4, 10, QLatin1Char('0'));
		ui.tableWidget_mptcp->setHorizontalHeaderLabels(head);
	}

	int type = ui.comboBox_mptcp->currentIndex() + 1;
	ui.tableWidget_mptcp->blockSignals(true);
	if (type <= 2) {
		auto val = mtp.GetBitPtr(type == 1);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mptcp->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	else {
		auto val = mtp.GetwordPtr(type == 3);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mptcp->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	ui.tableWidget_mptcp->blockSignals(false);
	lc_.unlock();
}
void Protocol::on_spinBox_mstcp_id_valueChanged(int val) { mts.SetSlaveId(val); }
void Protocol::on_spinBox_mptcp_id_valueChanged(int val) { mtp.SetSlaveId(val); }
void Protocol::on_spinBox_mptcp_valueChanged(int val) { mtp.SetScanTime(val); }

void Protocol::on_tableWidget_msrtu_itemChanged(QTableWidgetItem* item) {
	auto row = item->row();
	auto col = item->column();
	int type = ui.comboBox_msrtu->currentIndex() + 1;
	mrs.WriteDate(type, (ushort)item->text().toInt(), col * 10 + row);
}
void Protocol::on_tableWidget_mprtu_itemDoubleClicked(QTableWidgetItem* item) {
	int type = ui.comboBox_mprtu->currentIndex() + 1;
	auto row = item->row();
	auto col = item->column();
	if (type == 2 || type == 4)return;
	ushort val; bool ok;
	QString text = QInputDialog::getText(this, tr(LU_STR234), tr(LU_STR235), QLineEdit::Normal, 0, &ok);
	if (ok && !text.isEmpty()) {
		val = text.toInt();
		mrp.WriteDate(type, val, col * 10 + row);
	}
}
void Protocol::on_pushButton_msrtu_set_clicked(bool cur) {
	static std::mutex lc_;
	if (!lc_.try_lock())return;
	if (!cur) {
		int st = ui.spinBox_msrtu_start->text().toInt();
		int le = ui.spinBox_msrtu_len->text().toInt();
		int ty = ui.comboBox_msrtu->currentIndex();
		if (!mrs.SetAddr(st, le, ty))return;
		for (; ui.tableWidget_msrtu->columnCount();) ui.tableWidget_msrtu->removeColumn(0);
		auto val = ui.spinBox_msrtu_len->value();
		auto num = ui.spinBox_msrtu_start->value();
		auto col = val / 10 + (val % 10 ? 1 : 0);
		ui.tableWidget_msrtu->setColumnCount(col);
		QStringList head;
		for (int i = num; i < num + val; i += 10)head << QString("%1").arg(i, 4, 10, QLatin1Char('0'));
		ui.tableWidget_msrtu->setHorizontalHeaderLabels(head);
	}

	int type = ui.comboBox_msrtu->currentIndex() + 1;
	ui.tableWidget_msrtu->blockSignals(true);
	if (type <= 2) {
		auto val = mrs.GetBitPtr(type == 1);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_msrtu->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	else {
		auto val = mrs.GetwordPtr(type == 3);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_msrtu->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	ui.tableWidget_msrtu->blockSignals(false);
	lc_.unlock();
}
void Protocol::on_pushButton_mprtu_set_clicked(bool cur) {
	static std::mutex lc_;
	if (!lc_.try_lock())return;
	if (!cur) {
		int st = ui.spinBox_mprtu_start->text().toInt();
		int le = ui.spinBox_mprtu_len->text().toInt();
		int ty = ui.comboBox_mprtu->currentIndex();
		if (!mrp.SetAddr(st, le, ty))return;
		for (; ui.tableWidget_mprtu->columnCount();) ui.tableWidget_mprtu->removeColumn(0);
		auto val = ui.spinBox_mprtu_len->value();
		auto num = ui.spinBox_mprtu_start->value();
		auto col = val / 10 + (val % 10 ? 1 : 0);
		ui.tableWidget_mprtu->setColumnCount(col);
		QStringList head;
		for (int i = num; i < num + val; i += 10)head << QString("%1").arg(i, 4, 10, QLatin1Char('0'));
		ui.tableWidget_mprtu->setHorizontalHeaderLabels(head);
	}

	int type = ui.comboBox_mprtu->currentIndex() + 1;
	ui.tableWidget_mprtu->blockSignals(true);
	if (type <= 2) {
		auto val = mrp.GetBitPtr(type == 1);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mprtu->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	else {
		auto val = mrp.GetwordPtr(type == 3);
		for (Ulong i = 0; i < val->size(); i++)
			ui.tableWidget_mprtu->setItem(i % 10, i / 10, new QTableWidgetItem(std::to_string((*val)[i]).c_str()));
	}
	ui.tableWidget_mprtu->blockSignals(false);
	lc_.unlock();
}
void Protocol::on_spinBox_msrtu_id_valueChanged(int val) { mrs.SetSlaveId(val); }
void Protocol::on_spinBox_mprtu_id_valueChanged(int val) { mrp.SetSlaveId(val); }
void Protocol::on_spinBox_mprtu_valueChanged(int val) { mrp.SetScanTime(val); }

void Protocol::DownloadOrUploadFiles() {
	if (!ftpdu.size())return;
	ftpt.stop();
	bool ret = false;
	if (ftpdu[0].is_download) ret = ftp.DownloadFile(ftpdu[0].src, ftpdu[0].dst);
	else ret = ftp.UploadFile(ftpdu[0].src, ftpdu[0].dst);
	if (ret && ftpdu[0].is_download) RefreshFtpTable(1);
	if (ret && !ftpdu[0].is_download) RefreshFtpTable(2);
	ftpdu.pop_back();
	ftpt.start(100);
}
void Protocol::RefreshFtpTable(int mode) {
	if (mode == 1 || mode == 3) ftp.ReadLocalFile(ui.comboBox_ftp_path->currentText().toStdString());
	else if (mode == 2 || mode == 3)ftp.GetFileList();
}
void Protocol::ShowFtpMenu(const QPoint& pt) {
	QTableWidget* tw = (QTableWidget*)QObject::sender();
	bool is_local = (tw == ui.tableWidget);
	if (tw->currentRow() < 0)return;
	QString path = (is_local ? ui.comboBox_ftp_path->currentText() : ui.lineEdit_ftp_path_2->text());
	QString sele = tw->item(tw->currentRow(), 1)->text();
	QString golb = path + "/" + sele;
	if (tw->item(tw->currentRow(), 3)->text() == LU_STR233) return;

	QMenu menu;
	menu.addAction(LU_STR236, this, [&]() {if(ftp.RemoveFile((is_local ? golb : sele).toStdString(), is_local)) RefreshFtpTable(is_local ? 1 : 2); });
	menu.addAction(LU_STR237, this, [&]() {tw->editItem(tw->item(tw->currentRow(), 1)); });
	menu.exec(QCursor::pos());
}
std::list<std::string> Protocol::FtpTableSort(FtpMap* ptr, int sort_type) {
	//排序分三块：至于顶层不变的一块，文件一块，文件夹一块
	std::list<std::string> top_map, dir_map, file_map;
	auto lef = ptr->find("..");
	if (lef != ptr->end())top_map.push_back(lef->first);

	auto sort_f = [&](std::list<std::string>* _fm, FtpMap::iterator fs) {
		if (fs->first == "." || fs->first == "..")return;
		auto res = _fm->begin();
		for (auto i = _fm->begin(); i != _fm->end(); i++) {
			if (sort_type == 0 && *i > fs->first)res = i;
			else if (sort_type == 1 && *i < fs->first)res = i;
			else if (sort_type == 2 &&  *i < fs->second.time)res = i;
		}
		_fm->insert(res, fs->first);
	};

	for (auto i = ptr->begin(); i != ptr->end(); i++)
		sort_f(i->second.type == LU_STR233 ? &dir_map : &file_map, i);

	for (auto i : dir_map)top_map.insert(top_map.begin(), i);
	for (auto i : file_map)top_map.insert(top_map.begin(), i);

	return top_map;
}

void Protocol::on_tabWidget_currentChanged(int index) {
	if (index == 4) {
		auto res = com.GetComList();
		ui.comboBox_msrtu_com->clear();
		ui.comboBox_mprtu_com->clear();
		for (auto i : res)ui.comboBox_msrtu_com->addItem(i.c_str());
		for (auto i : res)ui.comboBox_mprtu_com->addItem(i.c_str());
	}
	else if (index == 2) {
		QString document_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
		ftp.ReadLocalFile(document_path.toStdString());
	}
}

void Protocol::on_comboBox_ftp_path_currentIndexChanged() {
	std::string path = ui.comboBox_ftp_path->currentText().toLocal8Bit().toStdString();
	ftp.ReadLocalFile(path);
}



//========================================================== RAW TCP SEVER ==========================================================
void RawTcpSever::Send(std::string cli, void* data, unsigned long long size) {
	int fd = client_map[cli];
	int ret = send(fd, (char*)data, size, 0);
	if (ret == -1 && errno == 32) Close();
	else if (ret == -1) son->CallBack(MsG, { cli,LU_STR238 });
	else son->CallBack(MsG, { cli,QString("SEND %1 BETY TO %2").arg(size).arg(cli.c_str()).toStdString() });
}
bool RawTcpSever::Connect(std::string ip, int port) {
	l_s = DISCONNECT;
	struct sockaddr_in _addr;
	soc_fd = socket(AF_INET, SOCK_STREAM, 0);
	inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
	_addr.sin_port = htons(port);
	_addr.sin_family = AF_INET;

	if (bind(soc_fd, (sockaddr*)&_addr, sizeof(sockaddr)) == -1) {
		son->CallBack(MsG, { "", LU_STR239 + std::to_string(errno) });
		return false;
	}
	else son->CallBack(MsG, { "", LU_STR240 });
	if (listen(soc_fd, 256) < 0) {
		son->CallBack(MsG, { "", LU_STR241 + std::to_string(errno) });
		return false;
	}
	else son->CallBack(MsG, { "", LU_STR242 });

	son->CallBack(ConnECT, { "","" });

	std::thread _t(&RawTcpSever::Run, this);
	_t.detach();
	return true;
}
void RawTcpSever::Close() {
	if (l_s == RUNNING) {
		l_s = EXECUTE;
		while (l_s == EXECUTE) std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	closesocket(soc_fd);
	son->CallBack(DisCONNECT, { "","" });
}
void RawTcpSever::Run() {
	l_s = RUNNING;
	int max_fd = soc_fd;
	fd_set rd, gol;
	FD_ZERO(&gol);FD_ZERO(&rd);
	FD_SET(soc_fd, &gol);
	char* buff = new char[PACK_LENGTH];
	
	while (l_s == RUNNING) {
		timeval tv{ 0,500 };
		rd = gol;
		if (select(max_fd + 1, &rd, NULL,NULL, &tv) == -1) {
			son->CallBack(MsG, { "","Select failed!" });
			continue;
		}

		if (FD_ISSET(soc_fd, &rd)) {
			socklen_t s;
			sockaddr_in _addr;
			memset(&_addr, 0, s = sizeof(sockaddr_in));
			int new_fd = accept(soc_fd, (sockaddr*)&_addr, &s);
			if (new_fd == -1) son->CallBack(MsG, { "",LU_STR243 });
			else {
				FD_SET(new_fd, &gol);
				max_fd = new_fd > max_fd ? new_fd : max_fd;
				setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(100));
				setsockopt(new_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(100));
				char _ip_[24];
				inet_ntop(AF_INET, &_addr.sin_addr, _ip_, 24);
				std::string ipp = std::string(_ip_) + ":" + std::to_string(_addr.sin_port);
				client_map[ipp] = new_fd;
				son->CallBack(NewLINK, { ipp,""});
			}
			continue;
		}

		for (int i = 0; i <= max_fd; i++) {
			if (!FD_ISSET(i, &rd))continue;

			memset(buff, '\0', PACK_LENGTH);
			std::string mes, ipp;
			for (auto x : client_map) ipp = x.second == i ? x.first : ipp;
			long long _ret = recv(i, buff, PACK_LENGTH, 0);
			mes.append(buff, _ret < 0 ? 0 : _ret);
			if (mes.size() == 0){
				closesocket(i);
				FD_CLR(i, &gol);
				son->CallBack(CloseLINK, { ipp,"" });
				client_map.erase(ipp);
				continue;
			}

			while (_ret == PACK_LENGTH) {
				_ret = recv(i, buff, PACK_LENGTH, 0);
				mes.append(buff, _ret < 0 ? 0 : _ret);
			}
			DataStu ds{ ipp ,mes };
			son->CallBack(NewDATA, ds);
		}
	};
	delete[] buff;
	for (auto i : client_map) closesocket(i.second);
	client_map.clear();
	l_s = DISCONNECT;
}

//========================================================== RAW TCP CLIENT ==========================================================
void RawTcpClient::Send(std::string cli, void* data, unsigned long long size) {
	int ret = send(soc_fd, (char*)data, size, 0);
	if (ret == -1 && errno == 32) Close();
	else if (ret == -1) son->CallBack(MsG, { cli,LU_STR238 });
	else son->CallBack(MsG, { cli,QString("SEND %1 BETY TO %2").arg(size).arg(cli.c_str()).toStdString() });
}
bool RawTcpClient::Connect(std::string ip, int port) {
	l_s = DISCONNECT;
	soc_fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in _addr;
	inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
	_addr.sin_port = htons(port);
	_addr.sin_family = AF_INET;

	if (::connect(soc_fd, (sockaddr*)&_addr, sizeof(sockaddr)) == -1) {
		son->CallBack(MsG, { "",LU_STR244 + std::to_string(errno) });
		return false;
	}

	std::string ipp = ip + ":" + std::to_string(port);
	son->CallBack(ConnECT, { ipp,"" });

	std::thread _t(&RawTcpClient::Run, this);
	_t.detach();
	return true;
}
void RawTcpClient::Close() { 
	l_s = DISCONNECT; 
	closesocket(soc_fd);
	son->CallBack(DisCONNECT, { "","" });
}
void RawTcpClient::Run() {
	l_s = RUNNING;
	char* buff = new char[PACK_LENGTH];
	while (l_s == RUNNING) {
		memset(buff, '\0', PACK_LENGTH);
		std::string mes;
		long long _ret = recv(soc_fd, buff, PACK_LENGTH, 0);
		mes.append(buff, _ret < 0 ? 0 : _ret);
		if (mes.size() == 0) break;

		while (_ret == PACK_LENGTH) {
			_ret = recv(soc_fd, buff, PACK_LENGTH, 0);
			mes.append(buff, _ret < 0 ? 0 : _ret);
		}
		DataStu ds{ "" ,mes };
		son->CallBack(NewDATA, ds);
	};
	delete[] buff;
	if (l_s == RUNNING) Close();
}

//==========================================================    RAW  UDP    ==========================================================
void RawUdp::Send(std::string cli, void* data, unsigned long long size) {
	std::string ip = cli;
	int port = stoi(ip.substr(ip.find(':') + 1));
	ip = ip.substr(0, ip.find(':'));

	sockaddr_in _addr;
	inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
	_addr.sin_port = htons(port);
	_addr.sin_family = AF_INET;
	int ret = sendto(soc_fd, (char*)data, size, 0, (sockaddr*)&_addr, sizeof(_addr));

	if (ret == -1 && errno == 32) Close();
	else if (ret == -1) son->CallBack(MsG, { cli,LU_STR238 });
	else son->CallBack(SendDONE, { cli,QString("SEND %1 BETY TO %2").arg(size).arg(cli.c_str()).toStdString() });
}
bool RawUdp::Connect(std::string ip, int port) {
	l_s = DISCONNECT;
	soc_fd = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in _addr;
	inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
	_addr.sin_port = htons(port);
	_addr.sin_family = AF_INET;

	if (bind(soc_fd, (sockaddr*)&_addr, sizeof(sockaddr)) == -1) {
		son->CallBack(MsG, { "", LU_STR239 + std::to_string(errno) });
		return false;
	}
	else son->CallBack(MsG, { "", LU_STR240 });

	son->CallBack(ConnECT, { "",""});

	std::thread _t(&RawUdp::Run, this);
	_t.detach();
	return true;
}
void RawUdp::Close() {
	l_s = DISCONNECT;
	closesocket(soc_fd);
	son->CallBack(DisCONNECT, { "","" });
}
void RawUdp::Run() {
	l_s = RUNNING;
	char* buff = new char[PACK_LENGTH];
	while (l_s == RUNNING) {
		memset(buff, '\0', PACK_LENGTH);
		std::string mes, ipp;
		socklen_t s;
		sockaddr_in _addr;
		memset(&_addr, 0, s = sizeof(sockaddr_in));
		long long _ret = recvfrom(soc_fd, buff, PACK_LENGTH, 0, (sockaddr*)&_addr, &s);
		mes.append(buff, _ret < 0 ? 0 : _ret);
		if (mes.size() == 0) break;

		while (_ret == PACK_LENGTH) {
			_ret = recvfrom(soc_fd, buff, PACK_LENGTH, 0, (sockaddr*)&_addr, &s);
			mes.append(buff, _ret < 0 ? 0 : _ret);
		}
		DataStu ds{ ipp ,mes };
		son->CallBack(NewDATA, ds);
	};
	delete[] buff;
}

//==========================================================  Modbus Slave  ==========================================================
std::string ModbusSlave::ReplyMessage(uchar* num, uchar id, uchar _type, std::deque<uchar> _dat) {
	bool real_q = false;
	std::string val, reply;
	reply.push_back(num[0]); reply.push_back(num[1]);
	uchar _t[4]{ _dat[1],_dat[0],_dat[3],_dat[2] };
	ushort start = *(ushort*)_t, length = *(ushort*)(_t + 2);

	auto wk_bit = [&](std::vector<bool>* vl, Ulong st, uchar code) {
		if (vl->size() < start + length || start < st)_type = code, val.push_back(2);
		else if (!length || length > 0x7D0)_type = code, val.push_back(3);
		else {
			auto add = start - st;
			uchar ch_val = 0, fvb = 7;
			for (; length; length--) {
				ch_val >>= 1, ch_val |= (*vl)[add++] ? 0x80 : 0x00;
				if (length == 1 && fvb) while (fvb--)ch_val >>= 1;
				if (!fvb-- || length == 1) val.push_back(ch_val), ch_val = 0, fvb = 7;
			}
			real_q = true;
		}
	};
	auto wk_ushort = [&](std::vector<ushort>* vl, Ulong st, uchar code) {
		if (vl->size() < start + length || start < st)_type = code, val.push_back(2);
		else if (!length || length > 0x7D)_type = code, val.push_back(3);
		else {
			for (auto add = start - st; length; length--) {
				auto v = (*vl)[add++];
				val.push_back(((uchar*)&v)[1]);
				val.push_back(((uchar*)&v)[0]);
			}
			real_q = true;
		}
	};

	if (_type == 0x01) wk_bit(&coil, coil_start, 0x81);
	else if (_type == 0x02) wk_bit(&input, input_start, 0x82);
	else if (_type == 0x03) wk_ushort(&holding, holding_start, 0x83);
	else if (_type == 0x04) wk_ushort(&input_R, input_R_start, 0x84);
	else if (_type == 0x05) {
		if (start < coil_start || start>coil_start + coil.size())_type = 0x85, val.push_back(2);
		else if (_t[3] != 0 && _t[3] != 255)_type = 0x85, val.push_back(3);
		else { coil[start - coil_start] = length ? true : false; return ""; }
	}
	else if (_type == 0x06) {
		if (start < holding_start || start>holding_start + holding.size())_type = 0x86, val.push_back(2);
		else if (!_t[2] && _t[2] != 255)_type = 0x86, val.push_back(3);
		else { holding[start - holding_start] = length; return ""; }
	}
	else if (_type == 0x0F) {
		auto add = start - coil_start;
		uchar num = _dat[4];
		if (start < coil_start || start>coil_start + coil.size())_type = 0x8F, val.push_back(2);
		if (num * 8 < length)_type = 0x8F, val.push_back(3);
		else {
			for (int i(0); length;) {
				auto vl = _dat[i + 5];
				for (int n = length > 8 ? 8 : length; n > 0; n--, length--)
					coil[add++] = vl & 0x01, vl >>= 1;
			}
			return "";
		}
	}
	else if (_type == 0x10) {
		auto add = start - holding_start;
		uchar num = _dat[4];
		if (start < holding_start || start>holding_start + holding.size())_type = 0x90, val.push_back(2);
		if (num * 2 < length)_type = 0x90, val.push_back(3);
		else {
			for (int i = 0; length; length -= 2, i += 2) {
				uchar v[2]{ _dat[i + 6],_dat[i + 5] };
				holding[add++] = *(ushort*)v;
			}
			return "";
		}
	}
	else _type += 0x80, val.push_back(1);

	Ulong fsz = _type < 0x80 ? val.size() + 3 : 3;
	for (int i = 3; i >= 0; i--)reply.push_back(((uchar*)&fsz)[i]);
	reply.push_back(id);
	reply.push_back(_type);
	if (real_q) reply.push_back((uchar)val.size());
	reply = reply + val;
	return reply;
}
bool ModbusSlave::SetAddr(Ulong _start, Ulong _len, ushort type) {
	if (_start < 0 || _len <= 0)return false;

	if (type == 0) {
		for (Ulong i = coil_start; i > _start; i--)coil.insert(coil.begin(), 0);
		for (Ulong i = coil_start; i < _start; i++)coil.erase(coil.begin());
		for (Ulong i = coil.size(); i < _len; i++)coil.push_back(0);
		for (Ulong i = coil.size(); i > _len; i--)coil.pop_back();
		coil_start = _start;
	}
	else if (type == 1) {
		for (Ulong i = input_start; i > _start; i--)input.insert(input.begin(), 0);
		for (Ulong i = input_start; i < _start; i++)input.erase(input.begin());
		for (Ulong i = input.size(); i < _len; i++)input.push_back(0);
		for (Ulong i = input.size(); i > _len; i--)input.pop_back();
		input_start = _start;
	}
	else if (type == 2) {
		for (Ulong i = holding_start; i > _start; i--)holding.insert(holding.begin(), 0);
		for (Ulong i = holding_start; i < _start; i++)holding.erase(holding.begin());
		for (Ulong i = holding.size(); i < _len; i++)holding.push_back(0);
		for (Ulong i = holding.size(); i > _len; i--)holding.pop_back();
		holding_start = _start;
	}
	else if (type == 3) {
		for (Ulong i = input_R_start; i > _start; i--)input_R.insert(input_R.begin(), 0);
		for (Ulong i = input_R_start; i < _start; i++)input_R.erase(input_R.begin());
		for (Ulong i = input_R.size(); i < _len; i++)input_R.push_back(0);
		for (Ulong i = input_R.size(); i > _len; i--)input_R.pop_back();
		input_R_start = _start;
	}
	CallBack(NewDATA, { std::to_string(type), "" });
	return true;
};
void ModbusSlave::WriteDate(int type, ushort val, Ulong addr) {
	if (type == 1) coil[addr] = val;
	else if (type == 2) input[addr] = val;
	else if (type == 3) holding[addr] = val;
	else if (type == 4) input_R[addr] = val;
}
std::string ModbusSlave::AnalysisMessage(std::string m_g) {
	char _tm[6]{ m_g[1],m_g[0],m_g[5],m_g[4],m_g[3],m_g[2] };
	uint len = *(ushort*)(_tm + 2);
	uchar id = m_g[6];
	if (id != slave_id)return "";
	uchar _type = m_g[7];
	if (len + 6 != m_g.length())return "";
	std::deque<uchar> _dat;
	for (len -= 2; len > 0; len--) _dat.push_front(m_g[len + 7]);
	auto reply = ReplyMessage((uchar*)m_g.c_str(), slave_id, _type, _dat);
	return reply.empty() ? m_g : reply;
}
void ModbusSlave::SetSlaveId(uchar id) { slave_id = id; }

//==========================================================  Modbus Poll   ==========================================================
void ModbusPoll::ReplyMessage(uchar* num, uchar id, uchar _type, std::deque<uchar> _dat) {
	std::string val, reply;
	reply.push_back(num[0]); reply.push_back(num[1]);
	uchar len = _dat[0];

	auto wk_bit = [&](std::vector<bool>* vl) {
		if (len * 8 < vl->size())return;
		int pt = 1; uchar val = 0;
		for (Ulong i = 0; i < vl->size(); i++) {
			if (i % 8 == 0)val = _dat[pt++];
			(*vl)[i] = val & 0x01;
			val >>= 1;
		}
	};
	auto wk_ushort = [&](std::vector<ushort>* vl) {
		if (len != vl->size() * 2)return;
		int pt = 1;
		for (Ulong i = 0; i < vl->size(); i++, pt += 2) {
			uchar v[2]{ _dat[pt + 1],_dat[pt] };
			(*vl)[i] = *(ushort*)v;
		}
	};

	if (_type == 0x01) wk_bit(&coil);
	else if (_type == 0x02) wk_bit(&input);
	else if (_type == 0x03) wk_ushort(&holding);
	else if (_type == 0x04) wk_ushort(&input_R);
	auto smg = QString("Coil Tx: %1,  Input Tx: %2,  Holding Tx: %3,  Input_R Tx: %4")
		.arg(_type - 0x80 == 1 ? -1 : scan_num[0])
		.arg(_type - 0x80 == 2 ? -1 : scan_num[1])
		.arg(_type - 0x80 == 3 ? -1 : scan_num[2])
		.arg(_type - 0x80 == 4 ? -1 : scan_num[3]).toStdString();
	CallBack(MsG, { "", smg });
	CallBack(NewDATA, { std::to_string(_type), "" });
}
std::string ModbusPoll::CreateMessage(uchar type, ushort add, ushort len, ushort num) {
	std::string msg;
	msg.push_back(slave_id);
	msg.push_back(type);
	for (int i = 2; --i >= 0;)msg.push_back(((uchar*)&add)[i]);
	for (int i = 2; --i >= 0;)msg.push_back(((uchar*)&len)[i]);

	Ulong msglen = msg.size();
	std::string head;
	for (int i = 2; --i >= 0;)head.push_back(((uchar*)&num)[i]);
	for (int i = 4; --i >= 0;)head.push_back(((uchar*)&msglen)[i]);

	head += msg;
	return head;
}
bool ModbusPoll::SetAddr(Ulong _start, Ulong _len, ushort type) {
	if (_start < 0 || _len <= 0)return false;

	if (type == 0) {
		for (Ulong i = coil_start; i > _start; i--)coil.insert(coil.begin(), 0);
		for (Ulong i = coil_start; i < _start; i++)coil.erase(coil.begin());
		for (Ulong i = coil.size(); i < _len; i++)coil.push_back(0);
		for (Ulong i = coil.size(); i > _len; i--)coil.pop_back();
		coil_start = _start;
	}
	else if (type == 1) {
		for (Ulong i = input_start; i > _start; i--)input.insert(input.begin(), 0);
		for (Ulong i = input_start; i < _start; i++)input.erase(input.begin());
		for (Ulong i = input.size(); i < _len; i++)input.push_back(0);
		for (Ulong i = input.size(); i > _len; i--)input.pop_back();
		input_start = _start;
	}
	else if (type == 2) {
		for (Ulong i = holding_start; i > _start; i--)holding.insert(holding.begin(), 0);
		for (Ulong i = holding_start; i < _start; i++)holding.erase(holding.begin());
		for (Ulong i = holding.size(); i < _len; i++)holding.push_back(0);
		for (Ulong i = holding.size(); i > _len; i--)holding.pop_back();
		holding_start = _start;
	}
	else if (type == 3) {
		for (Ulong i = input_R_start; i > _start; i--)input_R.insert(input_R.begin(), 0);
		for (Ulong i = input_R_start; i < _start; i++)input_R.erase(input_R.begin());
		for (Ulong i = input_R.size(); i < _len; i++)input_R.push_back(0);
		for (Ulong i = input_R.size(); i > _len; i--)input_R.pop_back();
		input_R_start = _start;
	}
	CallBack(NewDATA, { std::to_string(type), "" });
	return true;
};
void ModbusPoll::AnalysisMessage(std::string m_g) {
	char _tm[6]{ m_g[1],m_g[0],m_g[5],m_g[4],m_g[3],m_g[2] };
	uint len = *(ushort*)(_tm + 2);
	uchar id = m_g[6];
	if (id != slave_id)return;
	uchar _type = m_g[7];
	if (len + 6 != m_g.length()) { CallBack(MsG, { "", "error" }); return; }
	std::deque<uchar> _dat;
	for (len -= 2; len > 0; len--) _dat.push_front(m_g[len + 7]);
	ReplyMessage((uchar*)m_g.c_str(), slave_id, _type, _dat);
}

//==========================================================       COM      ==========================================================
bool RawCOM::ConnectCom(std::string com, int baud, int parity, int byte_size, int stop_bits) {
	if (!Connect(com, baud))return false;

#ifdef _WIN32
	DCB dcb;
	if (!GetCommState(hCom, &dcb)) {
		son->CallBack(MsG, { "", LU_STR245 });
		Close();
		return false;
	}
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = baud;
	dcb.Parity = parity;
	dcb.ByteSize = byte_size;
	dcb.StopBits = stop_bits;
	if (!SetCommState(hCom, &dcb)) {
		son->CallBack(MsG, { "", LU_STR246 });
		Close();
		return false;
	}
	COMMTIMEOUTS TimeOuts{ MAXDWORD ,0,0,500,1000 };
	if (false == SetCommTimeouts(hCom, &TimeOuts)) {
		son->CallBack(MsG, { "", LU_STR247 });
		Close();
		return false;
	}
	if (false == PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT)) {
		son->CallBack(MsG, { "", LU_STR248 });
		Close();
		return false;
	}
#else
	if (fcntl(hCom, F_SETFL, O_NONBLOCK) < 0){
		son->CallBack(MsG, { "", LU_STR249 });
		Close();
		return false;
	}
	struct  termios options;
	tcgetattr(hCom, &options);
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);
	options.c_cflag &= ~CSIZE;
	switch (byte_size){
	case 8:options.c_cflag |= CS8;break;
	default:options.c_cflag |= CS7;
	}
	switch (parity){
	case 0:options.c_cflag &= ~PARENB;options.c_iflag &= ~INPCK;break;
	case 1:options.c_cflag |= (PARODD | PARENB);options.c_iflag |= INPCK;break;
	case 2:options.c_cflag |= PARENB;options.c_cflag &= ~PARODD;options.c_iflag |= INPCK;break;
	case 3:options.c_cflag &= ~PARENB;options.c_cflag &= ~CSTOPB; break;
	}
	switch (stop_bits){
	case 0:options.c_cflag &= ~CSTOPB;break;
	case 2:options.c_cflag |= CSTOPB;break;
	}

	if (parity != 0)options.c_iflag |= INPCK;
	tcsetattr(hCom, TCSANOW, &options);
#endif

	son->CallBack(ConnECT, { "","" });

	std::thread _t(&COM::Run, this);
	_t.detach();
	return true;
}
void RawCOM::Send(std::string cli, void* data, unsigned long long size) {
	DWORD writeSize = size;
	if (!writeCom(hCom, data, size, &writeSize)) son->CallBack(MsG, { cli,LU_STR238 });
	else son->CallBack(SendDONE, { cli,QString("SEND %1 BETY").arg(writeSize).toStdString() });
}
bool RawCOM::Connect(std::string com, int baud) {
	hCom = openCom(com.c_str());

	if (hCom == INVALID_HANDLE_VALUE){
		hCom = 0;
		son->CallBack(MsG, { "", LU_STR250 });
		return false;
	}
	return hCom;
}
std::list<std::string> RawCOM::GetComList() {
	std::list<std::string> res;
#ifdef _WIN32
	HKEY hkey;
	int result, i = 0;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hkey))
	{
		TCHAR portName[0x100], commName[0x100];
		DWORD dwLong, dwSize;
		do {
			dwSize = sizeof(portName) / sizeof(TCHAR);
			dwLong = dwSize;
			result = RegEnumValue(hkey, i, portName, &dwLong, NULL, NULL, (LPBYTE)commName, &dwSize);
			if (ERROR_NO_MORE_ITEMS == result) break;

			res.push_back(commName);
			i++;
		} while (1);

		RegCloseKey(hkey);
	}
#else
	struct dirent** namelist;
	const char* sysdir = "/dev/";

	int n = scandir(sysdir, &namelist, NULL, NULL);
	if (n < 0)return res;

	while (n--) {
		if (strcmp(namelist[n]->d_name, "..") && strcmp(namelist[n]->d_name, ".")) {
			std::string devicedir = sysdir;
			devicedir += namelist[n]->d_name;
			if (devicedir.find("tty") != std::string::npos)res.push_back(devicedir);
		}
		free(namelist[n]);
	}
	free(namelist);
#endif
	return res;
}
void RawCOM::Close() {
	if (l_s == RUNNING) {
		l_s = EXECUTE;
		while (l_s == EXECUTE) std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	if (hCom) closeCom(hCom);
	hCom = 0;
	son->CallBack(DisCONNECT, { "","" });
}
void RawCOM::Run() {
	l_s = RUNNING;
	char* buff = new char[PACK_LENGTH];
	while (l_s == RUNNING) {
		memset(buff, '\0', PACK_LENGTH);
		DWORD readSize = PACK_LENGTH;
		int ret = readCom(hCom, buff, readSize, &readSize);
		if (ret < 0 || !readSize)continue;

		std::string msg;
		msg.append(buff, is_Linux ? ret : readSize);
		DataStu ds{ "" ,msg };
		son->CallBack(NewDATA, ds);
	};
	delete[] buff;
	l_s = DISCONNECT;
}





//==========================================================       FTP      ==========================================================
void Ftp::Send(std::string cli, void* data, unsigned long long size) {cmdsoc.Send(ipp, data, size);}
bool Ftp::Connect(std::string ip, int port) {
	cmdsoc_b = cmdsoc.Connect(ip, port);
	if (!cmdsoc_b) return false;

	ipp = ip + ":" + std::to_string(port);
	std::thread _t(&Ftp::Run, this);
	_t.detach();

	return true;
}
void Ftp::Close() {
	cmdsoc.Close();
	l_s = DISCONNECT;
	CallBack(DisCONNECT, { "","" });
}
Ftp::Ftp() {
	connect(&cmdsoc, &TcpClient::CallBack, this, &Ftp::CmdCallBack);
	connect(&valsoc, &TcpClient::CallBack, this, &Ftp::DataCallBack);
}
int Ftp::SetCmd(std::string cmd, bool is_data_cmd, bool is_no_recv) {
	std::string _cmd = cmd + "\r\n";
	if (is_data_cmd) {
		val_msg.clear();
		SetCmd("PASV");
		std::string ipp = cmd_msg.substr(cmd_msg.find('(') + 1);
		int v[6];for (int i = 0; i < 6; i++)v[i] = atoi(ipp.c_str()), ipp = ipp.substr(ipp.find(',') + 1);
		valsoc_b = valsoc.Connect(QString("%1.%2.%3.%4").arg(v[0]).arg(v[1]).arg(v[2]).arg(v[3]).toStdString(), 256 * v[4] + v[5]);
	}

	cmd_msg.clear();
	Send("", (void*)_cmd.c_str(), _cmd.size());
	if (is_no_recv)return true;

	for (int i = 0; i < 500 && !cmd_msg.size(); i++) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		QApplication::processEvents(QEventLoop::AllEvents);
	}
	int code = atoi(cmd_msg.c_str());
	CallBack(FtpCMD, { cmd,cmd_msg });

	return code;
}
bool Ftp::Connect(std::string ip, int port, std::string user, std::string pwd) {
	if (!lock.try_lock())return false;
	bool cnnt = Connect(ip, port);
	SetCmd("USER " + user);
	if (cnnt) cnnt = (SetCmd("PASS " + pwd) / 100) <= 2 ? true : false;
	if (cnnt) cnnt = (SetCmd("CWD /") / 100) <= 2 ? true : false;
	if (cnnt) cnnt = (SetCmd("PWD") / 100) <= 2 ? true : false;
	if (cnnt) cnnt = (SetCmd("TYPE A") / 100) <= 2 ? true : false;
	if (cnnt) CallBack(ConnECT, { "","" });
	l_s = cnnt ? CONNECT : DISCONNECT;
	lock.unlock();
	return cnnt;
}
void Ftp::CmdCallBack(Cb_type type, DataStu data) {
	if (type == DisCONNECT)CallBack(type, { "","" });
	else if (type == NewDATA) cmd_msg = data.second;
}
void Ftp::DataCallBack(Cb_type type, DataStu data) {
	if (type == DisCONNECT || type == ConnECT) valsoc_b = (type == ConnECT);
	else if (type == NewDATA) val_msg.push_back(data.second);
}
void Ftp::GetFileList() {
	if (!lock.try_lock())return;
	SetCmd("PWD");
	val_msg.clear();
	int os = SetCmd("LIST", true);
	CallBack(MsG, { "LIST",LU_STR251 + std::to_string(os) });
	std::string _msg = "";
	for (auto i : val_msg)_msg += i, val_msg.pop_front();
	if (_msg.empty()) CallBack(MsG, { "LIST",LU_STR252 });

	ftp_map.clear();
	FtpStu fss;
	fss.size = 0;
	fss.type = LU_STR233;
	QFileIconProvider icon_provider;
	fss.icon = icon_provider.icon(QFileIconProvider::Folder);
	ftp_map[".."] = fss;
	while (_msg.size()) {
		auto ps = _msg.find("\r\n"); ps = ps == std::string::npos ? _msg.size() : ps;
		std::string line_ = QString(_msg.substr(0, ps).c_str()).toLocal8Bit().toStdString();
		if (ps != _msg.size())_msg = _msg.substr(ps + 2);
		else _msg.clear();
		if (line_.size() < 57)continue;
		std::string nm = line_[56] == ' ' ? line_.substr(57).c_str() : line_.substr(56).c_str();
		FtpStu fs;
		fs.size = atoi(line_.substr(31, 11).c_str());
		fs.time = line_.substr(43, 12);
		fs.type = line_[0] == 'd' ? LU_STR233 : (line_.rfind('.') == std::string::npos) ? "None" : line_.substr(line_.rfind('.') + 1);
		fs.user = line_.substr(15, 9);
		fs.parameter = line_.substr(0, 11);
		QFileIconProvider icon_provider;
		if (line_[0] == 'd') fs.icon = icon_provider.icon(QFileIconProvider::Folder);
		else {
			std::string dx = "./templllltemplsja" + nm;
			QFile fi(dx.c_str());
			fi.open(QIODevice::WriteOnly);
			fs.icon = icon_provider.icon(QFileInfo(dx.c_str()));
			fi.remove();
		}
		ftp_map[nm] = fs;
	}
	CallBack(NewDATA, { "LIST","" });
	lock.unlock();
}
void Ftp::ToFtpPath(std::string path) {
	if (!lock.try_lock())return;
	SetCmd("CWD " + path);
	CallBack(MsG, { "",path });
	lock.unlock();
}
void Ftp::ReadLocalFile(std::string path) {
	QDir dir(path.c_str());
	if (!dir.exists()) return;
	QFileInfoList list = dir.entryInfoList();

	local_map.clear();
	for (int i = 0; i < list.count(); i++){
		QFileInfo fi = list.at(i);
		std::string nm = fi.fileName().toLocal8Bit().toStdString();
		if (nm == ".")nm = "..";
		FtpStu fs;
		fs.size = fi.size();
		fs.time = fi.lastModified().toString("yyyy/MM/dd hh:mm:ss").toStdString();
		fs.type = fi.isDir() ? LU_STR233 : nm.substr(nm.rfind(".") + 1);
		QFileIconProvider icon_provider;
		fs.icon = fi.isDir() ? icon_provider.icon(QFileIconProvider::Folder) : icon_provider.icon(QFileInfo(fi.filePath()));
		local_map[nm] = fs;
	}
	CallBack(FtpCMD, { "LOCAL",path });
	CallBack(NewDATA, { "LOCAL",""});
}
bool Ftp::RemoveFile(std::string path, bool is_local) {
	if (!lock.try_lock())return false;
	bool ret = false;
	if (is_local) {
		QFile fi(QString(path.c_str()));
		if (fi.exists()) ret = fi.remove();
		else CallBack(MsG, { "Remov",LU_STR253 }), ret = false;
	}
	else {
		int os = SetCmd("DELE " + path);
		CallBack(MsG, { "DELE",LU_STR254 + std::to_string(os) });
		ret = os / 100 == 2;
	}
	lock.unlock();
	CallBack(MsG, { "Remov",QString(LU_STR255).arg(path.c_str()).arg(ret ? LU_STR194 : LU_STR256).toStdString() });
	return ret;
}
bool Ftp::UploadFile(std::string src, std::string dst) {
	if (!lock.try_lock())return false;
	QFileInfo files(QString(src.c_str()));
	if(!files.exists()){
		CallBack(MsG, { "",LU_STR257 + src });
		lock.unlock();
		return false;
	}
	unsigned long long size = files.size();
	unsigned long long size_raw = size;

	std::fstream file(dst, std::ios::in | std::ios::binary);
	val_msg.clear();
	int os = SetCmd("STOR " + dst, true);
	CallBack(MsG, { "STOR",LU_STR258 + std::to_string(os) });
	while (valsoc_b && size) {
		char* buf = new char[8192];
		int si = size > 8192 ? 8192 : size;
		file.get(buf, si);
		valsoc.Send("", buf, si);
		size -= si;
		delete buf;
		CallBack(FtpCMD, { "STOR", std::to_string(size ? (100 - (size * 100 / size_raw)) : 100) });
	}
	valsoc.Close();
	file.close();

	if (!size)CallBack(MsG, { "RETR",LU_STR259 + src + LU_STR260 + dst });
	lock.unlock();
	return size == 0;
}
bool Ftp::DownloadFile(std::string src, std::string dst) {
	if (!lock.try_lock())return false;
	if (SetCmd(std::string("SIZE " + src).c_str()) != 213) {
		CallBack(MsG, { "SIZE",LU_STR261 + src });
		lock.unlock();
		return false;
	}
	unsigned long long size = atoi(cmd_msg.substr(4).c_str());
	unsigned long long size_raw = size;

	dst = QString(dst.c_str()).toLocal8Bit().toStdString();
	std::fstream file(dst, std::ios::out | std::ios::trunc | std::ios::in | std::ios::binary);
	val_msg.clear();
	int os = SetCmd("RETR " + src, true);
	CallBack(MsG, { "",LU_STR258 + std::to_string(os) });
	while (cmdsoc_b && size) {
		if (val_msg.empty())QApplication::processEvents(QEventLoop::AllEvents);
		else {
			file << val_msg[0];
			size -= val_msg[0].size();
			val_msg.pop_front();
			CallBack(FtpCMD, { "RETR", std::to_string(size ? (100 - (size * 100 / size_raw)) : 100) });
		}
	}
	file.flush();
	file.close();

	if(!size)CallBack(MsG, { "",LU_STR259 + src + LU_STR262 + dst });
	lock.unlock();
	return size == 0;
}
bool Ftp::RenameFile(std::string path, std::string name, bool is_local) {
	if (!lock.try_lock())return false;
	bool ret = false;
	if (is_local) {
		QFile fi(QString(path.c_str()));
		if (fi.exists()) ret = fi.rename(name.c_str());
		else CallBack(MsG, { "Rename",LU_STR253 }), ret = false;
	}
	else {
		SetCmd("RNFR " + path, false, true);
		int os = SetCmd("RNTO " + name);
		CallBack(MsG, { "RNTO",LU_STR263 + std::to_string(os) });
		ret = os / 100 == 2;
	}
	lock.unlock();
	CallBack(MsG, { "Remov",QString(LU_STR264).arg(path.c_str()).arg(ret ? LU_STR194 : LU_STR256).toStdString() });
	return ret;
}

//==========================================================   Modbus Tcp   ==========================================================
void ModbusTcpSlave::SevCallBack(Cb_type type, DataStu data) {
	if (type == DisCONNECT || type == ConnECT) {
		l_s = type == DisCONNECT ? DISCONNECT : CONNECT;
		if (type == DisCONNECT) CallBack(type, { "",LU_STR265 });
		else CallBack(type, { "",LU_STR266 });
	}
	else if (type == NewDATA) {
		auto msg = AnalysisMessage(data.second);
		soc.Send(data.first, (void*)msg.data(), msg.size());
		if (data.second[7] > 4)CallBack(NewDATA, { std::to_string(data.second[7]), "" });
	}
}
bool ModbusTcpSlave::Connect(std::string ip, int port) {
	if (!soc.Connect(ip, port)) {
		CallBack(MsG, { "",LU_STR267 });
		return false;
	}
	CallBack(MsG, { "",LU_STR266 });

	return true;
}
void ModbusTcpSlave::Close() {soc.Close();}

void ModbusTcpPoll::CliCallBack(LxProtocol::Cb_type type, LxProtocol::DataStu data) {
	if (type == DisCONNECT || type == ConnECT) {
		if (type == DisCONNECT) CallBack(type, { "",LU_STR268 });
		else CallBack(type, { "",LU_STR269 });
		l_s = type == DisCONNECT ? DISCONNECT : CONNECT;
	}
	else if (type == NewDATA) AnalysisMessage(data.second);
}
void ModbusTcpPoll::WriteDate(int type, ushort val, Ulong addr) {
	if (type == 1) {
		auto msg = CreateMessage(5, addr, val ? 0xff00 : 0x0000, 0);
		soc.Send("", (void*)msg.data(), msg.size());
	}
	else if (type == 3) {
		auto msg = CreateMessage(6, addr, val, 0);
		soc.Send("", (void*)msg.data(), msg.size());
	}
}
bool ModbusTcpPoll::Connect(std::string ip, int port) {
	if (!soc.Connect(ip, port)) {
		CallBack(MsG, { "LINK",LU_STR270 });
		return false;
	}

	std::thread _t(&ModbusTcpPoll::Run, this);
	_t.detach();
	CallBack(MsG, { "",LU_STR271 });
	return true;
}
void ModbusTcpPoll::Close() { soc.Close(); }
void ModbusTcpPoll::Run() {
	while (l_s == CONNECT) {
		if (coil.size()) {
			auto head = CreateMessage(1, coil_start, (ushort)coil.size(), scan_num[0]++);
			soc.Send("", (void*)head.data(), head.size());
		}
		if (input.size()) {
			auto head = CreateMessage(2, input_start, (ushort)input.size(), scan_num[1]++);
		soc.Send("", (void*)head.data(), head.size());
	}
		if (holding.size()) {
			auto head = CreateMessage(3, holding_start, (ushort)holding.size(), scan_num[2]++);
		soc.Send("", (void*)head.data(), head.size());
}
		if (input_R.size()) {
			auto head = CreateMessage(4, input_R_start, (ushort)input_R.size(), scan_num[3]++);
		soc.Send("", (void*)head.data(), head.size());
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(scan_time));
	}
}

//==========================================================   Modbus Rtu   ==========================================================
bool ModbusRtuSlave::ConnectCom(std::string _com, int buad, int parity, int byte_size, int stop_size) {
	if (!com.ConnectCom(_com, buad, parity, byte_size, stop_size)) {
		CallBack(MsG, { "",LU_STR250 });
		return false;
	}
	CallBack(MsG, { "",LU_STR266 });

	return true;
}
void ModbusRtuSlave::ComCallBack(LxProtocol::Cb_type type, LxProtocol::DataStu data) {
	if (type == DisCONNECT || type == ConnECT) {
		if (type == DisCONNECT) CallBack(type, { "",LU_STR268 });
		else CallBack(type, { "",LU_STR269 });
		l_s = type == DisCONNECT ? DISCONNECT : CONNECT;
	}
	else if (type == NewDATA) {
		auto msg = AnalysisMessage(data.second);
		com.Send(data.first, (void*)msg.data(), msg.size());
		if (data.second[7] > 4)CallBack(NewDATA, { std::to_string(data.second[7]), "" });
	}
}
void ModbusRtuSlave::Close() { com.Close(); }

bool ModbusRtuPoll::ConnectCom(std::string _com, int buad, int parity, int byte_size, int stop_size) {
	if (!com.ConnectCom(_com, buad, parity, byte_size, stop_size)) {
		CallBack(MsG, { "LINK",LU_STR250 });
		return false;
	}

	std::thread _t(&ModbusRtuPoll::Run, this);
	_t.detach();
	CallBack(MsG, { "",LU_STR271 });
	return true;
}
void ModbusRtuPoll::ComCallBack(LxProtocol::Cb_type type, LxProtocol::DataStu data) {
	if (type == DisCONNECT || type == ConnECT) {
		if (type == DisCONNECT) CallBack(type, { "",LU_STR268 });
		else CallBack(type, { "",LU_STR269 });
		l_s = type == DisCONNECT ? DISCONNECT : CONNECT;
	}
	else if (type == NewDATA) AnalysisMessage(data.second);
}
void ModbusRtuPoll::WriteDate(int type, ushort val, Ulong addr) {
	if (type == 1) {
		auto msg = CreateMessage(5, addr, val ? 0xff00 : 0x0000, 0);
		com.Send("", (void*)msg.data(), msg.size());
	}
	else if (type == 3) {
		auto msg = CreateMessage(6, addr, val, 0);
		com.Send("", (void*)msg.data(), msg.size());
	}
}
void ModbusRtuPoll::Close() { com.Close(); }
void ModbusRtuPoll::Run() {
	while (l_s == CONNECT) {
		if (coil.size()) {
			auto head = CreateMessage(1, coil_start, (ushort)coil.size(), scan_num[0]++);
			com.Send("", (void*)head.data(), head.size());
		}
		if (input.size()) {
			auto head = CreateMessage(2, input_start, (ushort)input.size(), scan_num[1]++);
			com.Send("", (void*)head.data(), head.size());
		}
		if (holding.size()) {
			auto head = CreateMessage(3, holding_start, (ushort)holding.size(), scan_num[2]++);
			com.Send("", (void*)head.data(), head.size());
		}
		if (input_R.size()) {
			auto head = CreateMessage(4, input_R_start, (ushort)input_R.size(), scan_num[3]++);
			com.Send("", (void*)head.data(), head.size());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(scan_time));
	}
}

//==========================================================       CAN      ==========================================================
void CAN::Send(std::string cli, void* data, unsigned long long size) {
#ifdef __linux__
	struct can_frame frame;
	frame.data[0] = ((uchar*)data)[0];
	frame.data[1] = ((uchar*)data)[1];
	frame.data[2] = ((uchar*)data)[2];
	frame.data[3] = ((uchar*)data)[3];
	frame.data[4] = ((uchar*)data)[4];
	frame.data[5] = ((uchar*)data)[5];
	frame.data[6] = ((uchar*)data)[6];
	frame.data[7] = ((uchar*)data)[7];
	frame.can_dlc = 8;
	frame.can_id = 1;

	int res = write(soc_fd, &frame, sizeof(frame));
	CallBack(MsG, { "","Send: " + std::to_string(res) });
#endif
}
bool CAN::Connect(std::string ip, int port) {
#ifdef __linux__
#define ip_cmd_set_can0_params "ip link set can0 type can bitrate 1000000 triple-sampling on"
#define ip_cmd_can0_up         "ifconfig can0 up"
#define ip_cmd_can0_down       "ifconfig can0 down"
	int ret = system(ip_cmd_set_can0_params); // 设置参数
	ret = system(ip_cmd_can0_up);  // 开启can0接口
	if (ret <= 0) {
		CallBack(MsG, { "",LU_STR273 });
		return false;
	}

	soc_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
	if (soc_fd < 0){
		CallBack(MsG, { "",LU_STR274 });
		return false;
	}

	struct ifreq ifr;
	strcpy(ifr.ifr_name, "can0");
	ioctl(soc_fd, SIOCGIFINDEX, &ifr);

	struct sockaddr_can addr;
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	int bind_res = bind(soc_fd, (struct sockaddr*)&addr, sizeof(addr));
	if (bind_res < 0){
		CallBack(MsG, { "",LU_STR275 });
		return false;
	}
	struct can_filter rfilter[3];
	rfilter[0].can_id = 1;
	rfilter[0].can_mask = CAN_SFF_MASK; // 标准帧 (SFF: standard frame format)
	rfilter[1].can_id = 2;
	rfilter[1].can_mask = CAN_SFF_MASK; // 标准帧
	rfilter[2].can_id = 3;
	rfilter[2].can_mask = CAN_SFF_MASK; // 标准帧
	setsockopt(soc_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	CallBack(MsG, { "",LU_STR266 });
	std::thread _t(&CAN::Run, this);
	_t.detach();

	return true;
#endif
	return true;
}
void CAN::Close() {
	if (l_s == RUNNING) {
		l_s = EXECUTE;
		while (l_s == EXECUTE) std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	closesocket(soc_fd);
	CallBack(DisCONNECT, { "","" });
}
void CAN::Run() {
#ifdef __linux__
	l_s = RUNNING;

	while (l_s == RUNNING) {
		struct can_frame status;   // can_frame 结构体定义在头文件 can.h 中
		 int ret = read(soc_fd, &status, sizeof(status));  // 读取数据，读取到的有效数据保存在 status.data[] 数组中;
		 if (!ret)continue;
		std::string msg;
		for (int i = 0; i < 8; i++)msg.push_back(status.data[i]);
		CallBack(NewDATA, { "", msg });
	}
	l_s = DISCONNECT;
#endif
}