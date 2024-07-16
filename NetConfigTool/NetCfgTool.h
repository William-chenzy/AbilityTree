#ifndef MAINWINDOW_H_LN
#define MAINWINDOW_H_LN

#include <QMainWindow>
#include "ui_LxNetCfgTool.h"
namespace Ui {
    class LxNetcfgTool;
}

class LxNetcfgTool : public QMainWindow
{
    Q_OBJECT

public:
    LxNetcfgTool(QWidget *parent = 0);
    ~LxNetcfgTool() = default;
    void SwitchLanguage();

protected:
	void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event); 

private:
    void FindAllNetHard();

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_attr_clicked();
    void on_toolButton_close_clicked();
    void on_pushButton_task__clicked();
    void on_pushButton_net_cen_clicked();
    void on_pushButton_ip_attr_clicked();
    void on_pushButton_fire_wall_clicked();
    void on_pushButton_set_firewall_clicked();

private:
    typedef struct { std::string guid; bool attr; } NETAttr;
    std::map<std::string, NETAttr>net_hard; ///first: name, second: CLSID
    Ui::LxNetcfgTool* ui;
};

#ifdef __linux__
static const bool is_Linux = true;
#else
static const bool is_Linux = false;
#endif

static bool is_zh_CN;
#define LU_STR44 (is_zh_CN?"提示":"Tips")
#define LU_STR122 (is_zh_CN?"Linux 不支持此功能":"Linux does not support this feature")
#define LU_STR156 (is_zh_CN?"初始化":"Init")
#define LU_STR157 (is_zh_CN?"COM初始化失败":"COM init failed")
#define LU_STR158 (is_zh_CN?"devmgr初始化失败":"devmgr init failed")
#define LU_STR159 (is_zh_CN?"防火墙设置成功!":"Successfully set firewall!")
#define LU_STR160 (is_zh_CN?"防火墙设置失败，COM初始化失败: 0x%08lx\n":"Firewall setup failed, COM initialization failed: 0x%08lx\n")
#define LU_STR161 (is_zh_CN?"域网设置失败: 0x%08lx\n":"Domain network setup failed: 0x%08lx\n")
#define LU_STR162 (is_zh_CN?"专网设置失败: 0x%08lx\n":"Private network setup failed: 0x%08lx\n")
#define LU_STR163 (is_zh_CN?"公网设置失败: 0x%08lx\n":"Public network setup failed: 0x%08lx\n")
#define LU_STR164 (is_zh_CN?"防火墙设置成功: %s\n":"Successfully set firewall: %s\n")
#define LU_STR165 (is_zh_CN?"已关闭":"CLOSED")
#define LU_STR166 (is_zh_CN?"已打开":"OPENED")
#define LU_STR167 (is_zh_CN?"设置防火墙":"Set firewall")

#define LU_STR422 (is_zh_CN?"网卡配置":"Network card configuration")
#define LU_STR423 (is_zh_CN?"网卡":"Network card")
#define LU_STR424 (is_zh_CN?"网卡属性":"Network Card Properties")
#define LU_STR425 (is_zh_CN?"打开":"Open")
#define LU_STR426 (is_zh_CN?"网卡协议属性":"Network Card Protocol Properties")
#define LU_STR427 (is_zh_CN?"防火墙配置页":"Firewall Configuration Page")
#define LU_STR428 (is_zh_CN?"任务管理器":"Task manager")
#define LU_STR429 (is_zh_CN?"网络连接中心":"Network Connection Center")
#define LU_STR430 (is_zh_CN?"完成":"Complete")
#define LU_STR431 (is_zh_CN?"打开/关闭":"Open/Close")

#endif // MAINWINDOW_H_LN
