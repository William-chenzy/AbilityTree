#ifndef PEFILEANALYZER_H
#define PEFILEANALYZER_H

#pragma once
#include <QMainWindow>
#include <windows.h>
#include <QTreeWidgetItem>
namespace Ui {
    class PeFileAnalyzer;
}

struct PEHead {
    std::string raw_val;
	bool is_x32 = false;
    _IMAGE_DOS_HEADER* dos = nullptr;
    std::string dos_info;
    _IMAGE_NT_HEADERS* nt = nullptr;
	_IMAGE_NT_HEADERS64* nt64 = nullptr;
    _IMAGE_SECTION_HEADER* section = nullptr;

	_IMAGE_IMPORT_DESCRIPTOR* imp = nullptr;
};
class PeFileAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    PeFileAnalyzer(QWidget *parent = 0);
    ~PeFileAnalyzer() = default;

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private:
    bool ReadFile();			//读取文件
    void ShowValue();			//在界面上显示
	ULONG RvaToFoa(ULONG val);	//RVA转FOV

private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_lineEdit_rva_textChanged(QString msg);
	void on_pushButton_foa_clicked();

private:
	ULONG foa = 0;
    PEHead pehead;
    QString file_name;
    Ui::PeFileAnalyzer* ui;
};

static WORD wb_to_s(WORD val) {
	char _p[2]{ ((char*)&val)[1],((char*)&val)[0] };
	return *(WORD*)_p;
}
static DWORD dwb_to_s(DWORD val) {
	char _p[4]{ ((char*)&val)[3],((char*)&val)[2],((char*)&val)[1],((char*)&val)[0] };
	return *(DWORD*)_p;
}
static ULONGLONG ldwb_to_s(ULONGLONG val) {
	char _p[8]{ ((char*)&val)[7],((char*)&val)[6],((char*)&val)[5],((char*)&val)[4],((char*)&val)[3],((char*)&val)[2],((char*)&val)[1],((char*)&val)[0] };
	return *(ULONGLONG*)_p;
}
static std::string w_to_str(std::string msg_, WORD val) {
	msg_.append((char*)&val, 2);
	return msg_;
}
static std::string dw_to_str(std::string msg_, DWORD val) {
	msg_.append((char*)&val, 4);
	return msg_;
}
static std::string w_to_int(std::string msg_, WORD val) {
	msg_ += std::to_string(val);
	return msg_;
}
static std::string dw_to_int(std::string msg_, DWORD val) {
	msg_ += std::to_string(val);
	return msg_;
}
static std::string ldw_to_int(std::string msg_, ULONGLONG val) {
	msg_ += std::to_string(val);
	return msg_;
}
static std::string l_to_int(std::string msg_, LONG val) {
	msg_ += std::to_string(val);
	return msg_;
}
static std::string c_to_hex(uchar i) {
	std::string msg;
	short ten = (short)i / 16;
	short ge = (short)i % 16;
	msg.push_back(ten < 10 ? ten + '0' : (ten + 'A' - 10));
	msg.push_back(ge < 10 ? ge + '0' : (ge + 'A' - 10));
	return msg;
}
static std::string s_to_hex(std::string msg_, std::string msg, int size = 1) {
	for (uchar i : msg) msg_ += c_to_hex(i);
	return msg_;
}
static std::string ldw_to_hex(std::string msg_, ULONGLONG* val, int size = 1) {
	std::string msg;
	while (size--) {
		ULONGLONG _val = ldwb_to_s(val[size]);
		msg.append((char*)&_val, 8);
	}
	return s_to_hex(msg_, msg);
}
static std::string dw_to_hex(std::string msg_, DWORD* val, int size = 1) {
	std::string msg;
	while (size--) {
		DWORD _val = dwb_to_s(val[size]);
		msg.append((char*)&_val, 4);
	}
	return s_to_hex(msg_, msg);
}
static std::string w_to_hex(std::string msg_, WORD* val, int size = 1) {
	std::string msg;
	while (size--) {
		WORD _val = wb_to_s(val[size]);
		msg.append((char*)&_val, 2);
	}
	return s_to_hex(msg_, msg);
}
static std::string s_to_fos(std::string bm, std::string msg_) {
	for (int i = 0; i < msg_.length(); i++)
		if (msg_[i] < 32 || msg_[i] > 126)msg_[i] = '.';
	return bm + msg_;
}
#define MSG_RET(A) {QMessageBox::warning(this, "提示", A);return false;}

#endif // PEFILEANALYZER_H
