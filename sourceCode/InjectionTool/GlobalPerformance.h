#ifndef GLOBALPERFORMANCE_H
#define GLOBALPERFORMANCE_H

#include <opencv2/opencv.hpp>
#include "GlobalDefine.h"
#include <QMainWindow>
#include <Qtimer>
#include <QLabel>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "version.lib")
#pragma  comment(lib,"Psapi.lib")
#pragma  comment(lib,"ntdll.lib")
#pragma  comment(lib,"Pdh.lib")

class GlobalPerformance : public QObject
{
	Q_OBJECT

public:
	GlobalPerformance();
	~GlobalPerformance() = default;

	void InstallCpuLabel(QLabel* label);
	void UnInstallCpuLabel(QLabel* label);

	void InstallMemLabel(QLabel* label);
	void UnInstallMemLabel(QLabel* label);

	void InstallIoLabel(QLabel* label);
	void UnInstallIoLabel(QLabel* label);

	void InstallNetLabel(QLabel* label);
	void UnInstallNetLabel(QLabel* label);

	void InstallGpuLabel(QLabel* label);
	void UnInstallGpuLabel(QLabel* label);

	void InstallDiskLabel(QLabel* label);
	void UnInstallDiskLabel(QLabel* label);

	QWidget* GetMainWidget() { return widget; }

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:

private slots:
	void RefreshViewer();

private:
	QTimer refresh;
	QWidget* widget;
	std::map<uchar, cv::Mat>cpu_core;
	std::map <QLabel*, bool>io_label;
	std::map <QLabel*, bool>cpu_label;
	std::map <QLabel*, bool>mem_label;
	std::map <QLabel*, bool>net_label;
	std::map <QLabel*, bool>gpu_label;
	std::map <QLabel*, bool>disk_label;
	cv::Mat io, cpu, mem, net, gpu, disk;
};
QString b_to_tb(unsigned long long val);


#endif // GLOBALPERFORMANCE_H
