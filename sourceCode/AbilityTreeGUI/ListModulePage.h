#ifndef LIST_MODULE_PAGE_H
#define LIST_MODULE_PAGE_H

#include "opencv2/opencv.hpp"
#include "ModuleCard.h"
#include <QSpacerItem>
#include <QScrollArea>
#include <QGridLayout>
#include <qtoolbutton.h>
#include "SettingPage.h"
#include "define.hpp"
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QTimer>

class ListModulePage : public QWidget{
	Q_OBJECT
public:
	explicit ListModulePage();
	~ListModulePage();

	void clicked();
	void ReLayout();
	void releseFouce();
	QWidget* GetWidget() { return ground; }
	void SetModuleFunc(std::vector<ModuleFunc>& _func);
	void SetParam(QString icon, QString name, QString describe);

private slots:
	void CreateAnimation();
	void ModuleDelete(QString,void*);
	void ModuleModify(QString name, QString describe, bool is_delete);

signals:
	void DeleteSelf();
	void ModifySelf(QString name, QString describe);

private:
	bool language;
	QTimer refresh;
	int width = 150;
	float module_v = 170;
	int line_v, widget_v;
	std::vector<ModuleCard*>all_card;
	std::vector<ModuleFunc> module_func;

	QLabel* icon = nullptr;
	QLabel* line = nullptr;
	QLabel* titel = nullptr;
	QLabel* describe = nullptr;
	QWidget* module = nullptr;
	QScrollArea* ground = nullptr;
	QWidget* icon_widget = nullptr;
	QWidget* titel_widget = nullptr;
	QWidget *scrollContent = nullptr;
	QWidget* describe_widget = nullptr;
	QHBoxLayout* icon_layout = nullptr;
	QHBoxLayout* titel_layout = nullptr;
	QGridLayout* module_layout = nullptr;
	QGridLayout* ground_layout = nullptr;
	QHBoxLayout* describe_layout = nullptr;

	SettingPage* set_page = nullptr;
	QSpacerItem* hspacer = nullptr;
	QToolButton* setting = nullptr;
};


#endif // LIST_MODULE_PAGE_H