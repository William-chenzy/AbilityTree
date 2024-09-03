#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <GlobalPerformance.h>
#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QPushButton>
#include <QProcess>
#include <Qtimer>

namespace Ui {
    class InjectionTool;
}

enum SORT{NAME,CPU,MEM,IO,PID,DES,TREE};
struct ProcessInfo {
	QString name;
	int64_t memory_byte;
	int64_t disk_byte;
	int64_t last_io;
	int64_t p_pid;
	int64_t pid;
	float cpu;
	QPixmap icon;
	QString description;

	bool is_active;
	bool is_expand = true;
	int64_t lastTime = 0;
	int64_t lastSystemTime = 0;
};
struct HookInfo {
	QProcess* dialog = nullptr;
	QProcess* debug = nullptr;
	QProcess* mouse = nullptr;
	QProcess* key = nullptr;
	QProcess* all = nullptr;

	bool sendb = false;
	bool recvb = false;
	bool install = false;

	int count = 0;
};

class InjectionTool : public QMainWindow
{
	Q_OBJECT

public:
    InjectionTool(QWidget *parent = 0);
	~InjectionTool();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	void DllInjectionExecute();
	QProcess* HookInjection(QPushButton* but, QProcess* proc, int* val, int num);

private slots:
	void on_treeWidget_all_process_itemClicked(QTreeWidgetItem*, int);
	void on_treeWidget_all_process_itemExpanded(QTreeWidgetItem*);
	void on_lineEdit_filter_editingFinished();
	void on_pushButton_refresh_clicked();
	void on_pushButton_kill_clicked();
	void on_pushButton_tree_clicked();
	void TreeWidgetHeaderClicked(int);

	void on_pushButton_dll_file_clicked();
	void on_pushButton_kill_self_clicked();
	void on_pushButton_dll_injection_clicked();
	void on_pushButton_dll_uninjection_clicked();
	void on_pushButton_dll_injection_apc_clicked();
	void on_pushButton_dialog_hook_clicked();
	void on_pushButton_mouse_hook_clicked();
	void on_pushButton_debug_hook_clicked();
	void on_pushButton_key_hook_clicked();
	void on_pushButton_dll_hook_clicked();

private:
	bool up_down;
	QString filter;
	QTimer refresh;
	SORT sort = TREE;
	QString dll_file;
	int64_t current_pid;
    Ui::InjectionTool* ui;
	std::map<int64_t, HookInfo>hook_map;
};


#endif // MAINWINDOW_H
