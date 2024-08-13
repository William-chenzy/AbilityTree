#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GlobalDefine.h"
#include "define.hpp"
#include <QDebug>
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

void MainWindow::RefreshViewer(){
	static int c_height = ui->widget_center->width();
	static int c_width = ui->widget_center->width();
	if (c_height != ui->widget_center->width() || c_width != ui->widget_center->width()) {
		//if (current_widget == user_widget)SwitchStartPage();

		if (current_m)current_m->widget->GetPagePtr()->ReLayout();

		c_height = ui->widget_center->width();
		c_width = ui->widget_center->width();
	}

	if (refresh_module_list) {
		int insert_index = 0;
		auto layout = ui->scrollAreaWidgetContents->layout();
		for (auto i : ui->scrollAreaWidgetContents->findChildren<QWidget*>()) layout->removeWidget(i);
		for (auto& i : moduleList) {
			if (!i.widget){
				i.widget = new ModuleList();
				i.widget->SetMaxListShow(ui->spinBox_list_show->value());
				connect(i.widget, &ModuleList::clicked, this, &MainWindow::ModuleListClicked);
			}
			QString name = !language ? i.name_ch : i.name_en;
			QString describe = !language ? i.describe_ch : i.describe_en;
			module_layout->insertWidget(insert_index++, i.widget);
			i.widget->SetModuleInfo(i);
		}
		refresh_module_list = false;
	}

	g += 0.01;
	if (g >= 0.99)g = -0.99;
	if (g >= -0.01 && g <= 0.01)g = 0.01;
	QString color_val = QString::number(abs(g), 'f', 2);
	QString color = QString("stop:0 #f4716e, stop:%1 #42f875, stop:1 #1edffc").arg(color_val);
	ui->widget_line->setStyleSheet(QString("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,%1);").arg(color));
	return;
}

void MainWindow::ModuleListClicked(void* sender) {
	ModuleList* curr = static_cast<ModuleList*>(sender);
	if (current_m) {
		if (current_m->widget == curr)return;
		for (auto i : moduleList) {
			if (i.widget != current_m->widget) continue;
			disconnect(i.widget, &ModuleList::DeleteModule, this, &MainWindow::DeleteModule);
			i.widget->GetPage()->setVisible(false);
			i.widget->ClearFouce();
		}
	}
	for (auto& i : moduleList) {
		if (i.widget != curr)continue;
		main_layout->addWidget(i.widget->GetPage(), 0, 0);
		connect(i.widget, &ModuleList::DeleteModule, this, &MainWindow::DeleteModule);
		i.widget->GetPage()->setVisible(true);
		i.widget->GetFouce();
		current_m = &i;
	}
}
void MainWindow::DeleteModule() {
	ui->scrollAreaWidgetContents->layout()->removeWidget(current_m->widget);
	disconnect(current_m->widget, &ModuleList::DeleteModule, this, &MainWindow::DeleteModule);
	for (auto i = moduleList.begin(); i != moduleList.end(); i++) {
		auto n_name = !language ? i->name_ch : i->name_en;
		auto r_name = !language ? current_m->name_ch : current_m->name_en;
		if (n_name != r_name)continue;
		moduleList.erase(i);
		break;
	}
	WriteModuleNumbers();
	current_m = nullptr;
	refresh_module_list = true;
}

void MainWindow::SwitchStartPage() {
	//todo::show welcome page
	//todo:choose module show mode
	//使用专注视图选择视图模式
}