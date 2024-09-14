#include <ModuleList.h>
#include <QDebug>
#include <random>
#include <QPixmap>
#include <QPainter>
#include "GlobalDefine.h"
#include <QFileIconProvider>

#define RAW_STYLE QString("#%1{border-radius: 5px;border:0px solid #272e2e;%2}").arg(obj_name)

static int obj_name_index = 10001;
static std::vector<uchar>begin_color = hexTorgb("#AAAAAA");
static std::vector<uchar>end_color = hexTorgb("#272e3e");
static std::vector<uchar>r_color = hexTorgb("#f4716e");
static std::vector<uchar>g_color = hexTorgb("#42f875");
static std::vector<uchar>b_color = hexTorgb("#1edffc");

ModuleList::ModuleList(QWidget* parent) : QWidget(parent) {
	obj_name = QString("ModuleList_%1").arg(obj_name_index++);
	describe_lab = new QLabel();
	icon_lab = new QLabel();
	name_lab = new QLabel();
	color_line = new QLabel();
	ground_widget = new QWidget();
	icon_widget = new QWidget();
	layout = new QGridLayout();
	w_lay = new QVBoxLayout();
	b_lay = new QVBoxLayout();

	icon_lab->installEventFilter(this);
	name_lab->installEventFilter(this);
	icon_widget->installEventFilter(this);
	describe_lab->installEventFilter(this);
	ground_widget->installEventFilter(this);

	this->setLayout(b_lay);
	this->setMinimumHeight(48);
	color_line->setMaximumHeight(2);

	icon_widget->setLayout(w_lay);
	ground_widget->setLayout(layout);
	ground_widget->setObjectName(obj_name);

	name_lab->setWordWrap(true);
	icon_lab->setFixedWidth(36);
	icon_lab->setFixedHeight(36);
	describe_lab->setWordWrap(true);
	describe_lab->setAlignment(Qt::AlignTop);
	name_lab->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Preferred);
	icon_widget->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Expanding);
	describe_lab->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::MinimumExpanding);

	b_lay->setMargin(0);
	b_lay->setSpacing(0);
	b_lay->addWidget(ground_widget);
	b_lay->setAlignment(Qt::AlignTop);

	w_lay->setMargin(0);
	w_lay->setSpacing(0);
	w_lay->addWidget(icon_lab, 0, 0);
	w_lay->setAlignment(Qt::AlignTop);

	layout->setMargin(6);
	layout->setVerticalSpacing(6);
	layout->setHorizontalSpacing(6);
	layout->addWidget(name_lab, 0, 1);
	layout->addWidget(describe_lab, 1, 1);
	layout->addWidget(color_line, 2, 0, 1, 3);
	layout->addWidget(icon_widget, 0, 0, 2, 1);
	layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #AAAAAA;background-color: transparent;font-size: 14px;");
	name_lab->setStyleSheet("font-family: \"华文楷体\";font-size:20px;font-weight: bold;background-color: transparent;");

	connect(&animation, &QTimer::timeout, this, &ModuleList::CreateAnimation);
	has_fouce = animation_val = animation_rand = 0;
	transfrormation = cv::Mat::zeros(2, 3, CV_32FC1);
	transfrormation.at<float>(0, 0) = 1;
	transfrormation.at<float>(0, 2) = 1;
	transfrormation.at<float>(1, 1) = 1;
	transfrormation.at<float>(1, 2) = 0;

	lm_page = new ListModulePage();
	connect(lm_page, &ListModulePage::ModifySelf, this, &ModuleList::ModifySelf);
	connect(lm_page, &ListModulePage::DeleteSelf, this, &ModuleList::DeleteSelf);
	language = LoadConfigure::getInstance()->IS_EN();
	max_show = 5;
}
ModuleList::~ModuleList() {
	disconnect(&animation, &QTimer::timeout, this, &ModuleList::CreateAnimation);
	lm_page->setVisible(false); 
	animation.stop();
	delete lm_page;
}

void ModuleList::SetModuleInfo(ModuleMember& module) {
	icon = module.icon_path;
	module_func = module.func;
	ch_icon = module.ch_icon_path;
	name = !language ? module.name_ch : module.name_en;
	describe = !language ? module.describe_ch : module.describe_en;

	RefreshDescribe();
	name_lab->setText(name);
	icon_lab->setStyleSheet(QString("border-image:url(%1) 4 4 4 4 stretch stretch;border-radius: 4px;").arg(icon));

	lm_page->SetModuleFunc(module_func);
	lm_page->SetParam(icon, name, describe);
}
void ModuleList::RefreshDescribe(int expand_number) {
	int step[3]{ 0,0,0 };
	bool all_expand = expand_number == max_show;
	auto change_color = all_expand ? b_color : begin_color;
	QString final_str = expand_number < module_func.size() ? "..." : "-";
	if (!all_expand)for (int i = 0; i < 3; i++)step[i] = (change_color[i] - end_color[i]) / (expand_number + 2);

	QString show_str = describe;
	for (auto i : module_func) {
		for (int i = 0; i < 3; i++)change_color[i] -= step[i];
		show_str += QObject::tr("<br><font color =#%1> > %2</font>").arg(rgbTohex(change_color)).arg(i.name);
		if (!expand_number--)break;
	}
	show_str += QObject::tr("<br><font color =#%1>%2</font>").arg(rgbTohex(change_color)).arg(final_str);
	describe_lab->setText(show_str);
	update();
}

void ModuleList::CreateAnimation() {
	if (!this->isActiveWindow() || !this->isVisible())return;

	if (fx_func.empty()) {
		std::default_random_engine e;
		e.seed(GetTimeM);
		srand(e());
		int use_height = this->height() - 48;
		int temp_val = (rand() % (use_height - use_height / 2)) + use_height / 2;
		animation_rand = temp_val == animation_rand ? temp_val / 2 : temp_val;

		if (animation_val == 0)animation_val = rand() % temp_val;
		int all_len = abs(animation_rand - animation_val), x = 0;
		for (int val = animation_val; val != animation_rand;) {
			if (val < animation_rand)val++;
			if (val > animation_rand)val--;
			fx_func.push_back(val * cos(0.1) + x * sin(0.1));
		}
		animation_val = animation_rand;
	}
	
	if (background.empty()) background = cv::Mat(this->height(), this->width(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
	while (background.rows < this->height()) {
		cv::Mat ins = cv::Mat(1, this->width(), CV_8UC4, cv::Scalar(0,0,0,0));
		cv::vconcat(ins, background, background);
	}
	warpAffine(background, background, transfrormation, background.size());

	float val = 120;
	float step = val / *fx_func.begin();
	for (int i = background.rows - 2; i; i--) {
		background.at<cv::Vec4b>(i, 0) = cv::Vec4b{ b_color[0],b_color[1],b_color[2],(uchar)(int)val };
		val = (val && (val - step) >= 0) ? val - step : 0;
	}
	ground_widget->setStyleSheet(QString("#%1{background: rgba(127,134,150,0.5);border-radius: 2px;}").arg(obj_name));
	fx_func.pop_front();
	update();
}

void ModuleList::GetFouce() {
	icon_lab->setStyleSheet(QString("border-image:url(%1) 4 4 4 4 stretch stretch;border-radius: 4px;").arg(ch_icon));
	describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #eeeeee;background-color: transparent;font-size: 14px;");
	name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 20px;font-weight: bold;background-color: transparent;color: #eeeeee;");
	if (LoadConfigure::getInstance()->GetAnimation())animation.start(25);
	RefreshDescribe(max_show);
	lm_page->clicked();
	has_fouce = true;
}
void ModuleList::ClearFouce() {
	icon_lab->setStyleSheet(QString("border-image:url(%1) 4 4 4 4 stretch stretch;border-radius: 4px;").arg(icon));
	name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 20px;font-weight: bold;background-color: transparent;");
	describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #AAAAAA;background-color: transparent;font-size: 14px;");
	ground_widget->setStyleSheet(RAW_STYLE.arg(""));
	lm_page->releseFouce();
	RefreshDescribe(2);
	has_fouce = false;
	animation.stop();
}

void ModuleList::DeleteSelf() {
	disconnect(&animation, &QTimer::timeout, this, &ModuleList::CreateAnimation);
	animation.stop();
	emit DeleteModule();
	lm_page->setVisible(false);
	ClearFouce();
	this->deleteLater();
}
void ModuleList::ModifySelf(QString name_, QString describe_) {
	name = name_;
	describe = describe_;
	name_lab->setText(name_);
	RefreshDescribe(max_show);
}

void ModuleList::paintEvent(QPaintEvent*){
	if (!has_fouce) return;
	QPainter painter(this);
	QImage img_(background.data, background.cols, background.rows, background.cols * background.channels(), QImage::Format_RGBA8888);
	painter.drawImage(0, 0, img_);
}
bool ModuleList::eventFilter(QObject* watched, QEvent* event) {
	if (event->type() != QEvent::MouseButtonRelease) return false;
	emit clicked(this);
	return true;
}
void ModuleList::enterEvent(QEvent* event) {
	ground_widget->setStyleSheet(RAW_STYLE.arg("background: qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0 #575e6e, stop:1 transparent);"));
	color_line->setStyleSheet("background: qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0 transparent, stop:0.5 #1edffc, stop:1 transparent);");

	describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #1edffc;background-color: transparent;font-size: 14px;");
	name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 20px;font-weight: bold;background-color: transparent;color: #1edffc;");
}
void ModuleList::leaveEvent(QEvent* event) {
	color_line->setStyleSheet("");
	ground_widget->setStyleSheet(RAW_STYLE.arg(""));

	if (has_fouce) {
		describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #eeeeee;background-color: transparent;font-size: 14px;");
		name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 20px;font-weight: bold;background-color: transparent;color: #eeeeee;");
	}
	else {
		describe_lab->setStyleSheet("font-family: \"华文楷体\";color: #AAAAAA;background-color: transparent;font-size: 14px;");
		name_lab->setStyleSheet("font-family: \"华文楷体\";font-size: 20px;font-weight: bold;background-color: transparent;");
	}
}