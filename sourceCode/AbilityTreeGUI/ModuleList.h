#ifndef MODULE_LIST_H
#define MODULE_LIST_H

#include "opencv2/opencv.hpp"
#include "ListModulePage.h"
#include "ModuleCard.h"
#include "define.hpp"
#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QTimer>

class ModuleList : public QWidget{
	Q_OBJECT
public:
	explicit ModuleList(QWidget* parent = nullptr);
	~ModuleList();

	void GetFouce();
	void ClearFouce();
	void SetModuleInfo(ModuleMember& module);
	void SetMaxListShow(int val) { max_show = val; }
	ListModulePage* GetPagePtr() { return lm_page; }
	QWidget* GetPage() { return lm_page->GetWidget(); }

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void paintEvent(QPaintEvent*) override;

signals:
	void DeleteModule();
	void clicked(void* sender);
	
private slots:
	void DeleteSelf();
	void CreateAnimation();
	void ModifySelf(QString name, QString describe);

private:
	void RefreshDescribe(int expand_number = 2);

private:
	std::vector<ModuleFunc> module_func;
	QString name, icon, describe, ch_icon;

	int max_show;
	bool language;
	bool has_fouce;
	QString obj_name;

	QTimer animation;
	int animation_val;
	int animation_rand;
	cv::Mat background;
	cv::Mat transfrormation;
	std::list<float>fx_func;

	QLabel* icon_lab = nullptr;
	QLabel* name_lab = nullptr;
	QLabel* color_line = nullptr;
	QVBoxLayout* b_lay = nullptr;
	QVBoxLayout* w_lay = nullptr;
	QGridLayout* layout = nullptr;
	QLabel* describe_lab = nullptr;
	QWidget* icon_widget = nullptr;
	QWidget* ground_widget = nullptr;

	ListModulePage* lm_page = nullptr;
};


#endif // MODULE_LIST_H