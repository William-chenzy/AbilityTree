#ifndef MAINWINDOW_H_LN
#define MAINWINDOW_H_LN

#include <QMainWindow>
#include "CppGlobalDefine.h"
#include "ui_NetConfigTool.h"
namespace Ui {
    class NetConfigTool;
}

class NetConfigTool : public QMainWindow
{
    Q_OBJECT

public:
    NetConfigTool(QWidget *parent = 0);
    ~NetConfigTool() = default;
    void SwitchLanguage();

private:
    void EnumNetCard();

private slots:
	void NetCardRule();
	void NetCardRuleWord();
    void on_pushButton_attr_clicked();
    void on_pushButton_task__clicked();
    void on_pushButton_net_cen_clicked();
    void on_pushButton_ip_attr_clicked();
    void on_pushButton_fire_wall_clicked();
    void on_pushButton_set_firewall_clicked();

private:
    std::map<std::string, std::string>net_hard;
	std::vector<QString>netcard_word;
	std::map<int, bool>netcard_rule;
    Ui::NetConfigTool* ui;
};

#endif // MAINWINDOW_H_LN
