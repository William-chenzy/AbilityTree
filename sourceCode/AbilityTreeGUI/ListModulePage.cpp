#include <ModuleList.h>
#include <QDebug>
#include <random>
#include <QPixmap>
#include <QPainter>
#include "GlobalDefine.h"
#include <QFileIconProvider>

ListModulePage::ListModulePage() {
	icon = new QLabel();
	line = new QLabel();
	titel = new QLabel();
	describe = new QLabel();
	module = new QWidget();
	ground = new QScrollArea();
	icon_widget = new QWidget();
	titel_widget = new QWidget();
	describe_widget = new QWidget();
	icon_layout = new QHBoxLayout();
	titel_layout = new QHBoxLayout();
	module_layout = new QGridLayout();
	describe_layout = new QHBoxLayout();
	scrollContent = new QWidget(ground);
	ground_layout = new QGridLayout(scrollContent);
	hspacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
	set_page = new SettingPage(Module_Set);
	setting = new QToolButton();

	describe_widget->setLayout(describe_layout);
	scrollContent->setLayout(ground_layout);
	titel_widget->setLayout(titel_layout);
	icon_widget->setLayout(icon_layout);
	module->setLayout(module_layout);

	describe_layout->setMargin(0);
	ground->setWidgetResizable(true);
	titel_widget->setObjectName("titel_widget");
	ground->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ground->setWidget(scrollContent);
	describe_layout->addWidget(describe);
	icon_layout->addWidget(icon);

	titel_layout->addWidget(icon_widget);
	titel_layout->addWidget(titel);
	titel_layout->addSpacerItem(hspacer);
	titel_layout->addWidget(setting);

	ground_layout->addWidget(titel_widget, 0, 0);
	ground_layout->addWidget(describe_widget, 1, 0);
	ground_layout->addWidget(line, 2, 0);
	ground_layout->addWidget(module, 3, 0);

	line->setFixedHeight(2);
	icon_layout->setMargin(0);
	module_layout->setSpacing(6);
	setting->setFixedWidth(48), setting->setFixedHeight(48);
	icon_widget->setFixedWidth(48), icon_widget->setFixedHeight(48);
	icon_widget->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Expanding);
	titel_widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Maximum);
	describe_widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Maximum);

	ground->setStyleSheet(QString(SCROLL_AREA_STYLE));
	icon_widget->setStyleSheet("border-radius: 6px;border:2px solid #eeeeee;");
	module->setStyleSheet("border-radius: 6px;background-color: rgba(170,170,170,24);");
	describe->setStyleSheet("font-family: \"华文楷体\";color: #eeeeee;background-color: transparent;font-size: 18px;");
	titel->setStyleSheet("font-family: \"华文楷体\";font-size: 36px;font-weight: bold;background-color: transparent;color: #eeeeee;");
	setting->setStyleSheet("QToolButton{border-image:url(./res/setting.png);}QToolButton:hover{background-color: rgba(170,170,170,66);}");

	connect(&refresh, &QTimer::timeout, this, &ListModulePage::CreateAnimation);
	connect(set_page, &SettingPage::ModuleModify, this, &ListModulePage::ModuleModify);
	connect(setting, &QToolButton::clicked, this, [&]() {
		set_page->showNormal(); 
		set_page->SetModuleParam(titel->text(), describe->text());
		});
	language = LoadConfigure::getInstance()->IS_EN();
}
ListModulePage::~ListModulePage() {
	disconnect(&refresh, &QTimer::timeout, this, &ListModulePage::CreateAnimation);
	for (auto i : all_card)delete i;
	delete set_page;
	refresh.stop();
}

void ListModulePage::SetModuleFunc(std::vector<ModuleFunc>& _func) {
	if (_func.size() == module_func.size())return;
	for (auto i : all_card)delete i;
	all_card.clear();

	module_func = _func;
	static unsigned int index = 0;
	for (auto i : module_func) {
		QString path = "./res/BrokenIcon.png";
		QString check_str = i.path + (is_Linux?"":".exe");
		if (CheckFile(check_str)) {
			QFileIconProvider icon_provider;
			path = QString("./temp/%1.png").arg(index++);
			icon_provider.icon(i.path).pixmap(12, 12).save(path, "png");
		}
		ModuleCard* card = new ModuleCard();
		connect(card, &ModuleCard::deleteModule, this, &ListModulePage::ModuleDelete);
		card->SetParam(i.name, path, width, width * 2);
		card->SetProgramPath(i.path);
		all_card.push_back(card);
	}
}
void ListModulePage::SetParam(QString icon_, QString name_, QString describe_v) {
	titel->setText(name_);
	describe->setText(describe_v);
	icon->setStyleSheet(QString("border-image:url(%1);").arg(icon_));
}

void ListModulePage::ReLayout() {
	scrollContent->setFixedWidth(ground->width());

	int cols = 0, rows = 0;
	int now_width = module->width();
	int max_cols = now_width / width;
	if (now_width % width < (max_cols - 1) * 30)max_cols--;
	for (int i = 0; i < module_func.size(); i++) {
		module_layout->addWidget(all_card[i], rows, cols++);
		if (cols == max_cols)rows++, cols = 0;
	}
}
void ListModulePage::clicked() {
	SetModuleFunc(module_func);
	for (int i = 0; i < module_func.size(); i++) all_card[i]->BeginAnimation();
	ReLayout();

	line_v = 0;
	widget_v = 99;
	if (LoadConfigure::getInstance()->GetAnimation())refresh.start(25);
}
void ListModulePage::releseFouce() {
	refresh.stop();
}

void ListModulePage::CreateAnimation() {
	static QString style = "background: qlineargradient(x1:0,y1:1,x2:1,y2:1";
	if (line_v >= 50 && widget_v <= 10) refresh.stop();

	if (line_v < 50) {
		int l_val = 50 - line_v, r_val = 50 + line_v;
		if (l_val < 10)l_val = 0; if (r_val >= 100)r_val = 99;
		auto st = QString(style + ",stop:0.%1 transparent, stop:0.5 #1edffc, stop:0.%2 transparent);").arg(l_val).arg(r_val);
		line->setStyleSheet(st);
		line_v += 4;
	}
	else line->setStyleSheet(style + ",stop:0 transparent, stop:0.5 #1edffc, stop:1 transparent);");

	if (widget_v > 0) {
		int val = widget_v < 10 ? 1 : widget_v;
		auto st = QString("#titel_widget{" + style + ",stop:0.%1 transparent, stop:1 rgba(170,170,170,36));border-radius: 6px;}").arg(val);
		titel_widget->setStyleSheet(st);
		widget_v -= 7;
	}
	else titel_widget->setStyleSheet("#titel_widget{" + style + ",stop:0 transparent, stop:1 rgba(170,170,170,36));border-radius: 6px;}");
}
void ListModulePage::ModuleDelete(QString name, void* ptr) {
	for (auto f = module_func.begin(); f != module_func.end(); f++) {
		if (f->name != name)continue;
		module_func.erase(f);
		break;
	}

	for (auto c = all_card.begin(); c != all_card.end(); c++) {
		if (*c != ptr)continue;
		(*c)->deleteLater();
		all_card.erase(c);
		break;
	}

	for (auto& i : moduleList) {
		auto n_name = !language ? i.name_ch : i.name_en;
		if (n_name != titel->text())continue;
		for (auto f = i.func.begin(); f != i.func.end(); f++) {
			if (f->name != name)continue;
			i.func.erase(f);
			break;
		}
	}
	WriteModuleNumbers();
	clicked();
}

void ListModulePage::ModuleModify(QString _name, QString _describe, bool is_delete) {
	if (is_delete) {
		auto res = QMessageBox::information(this, "提示", "确认删除此模组吗?", QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::No)return;
		for (int i = 0; i < all_card.size(); i++) delete all_card[i];
		all_card.clear();
		emit DeleteSelf();
	}
	else {
		for (auto& i : moduleList) {
			auto n_name = !language ? i.name_ch : i.name_en;
			if (n_name != titel->text())continue;
			if (!language)i.name_ch = _name, i.describe_ch = _describe;
			else i.name_en = _name, i.describe_en = _describe;
		}
		titel->setText(_name);
		describe->setText(_describe);
		emit ModifySelf(_name, _describe);
		WriteModuleNumbers();
	}
	set_page->hide();
}
