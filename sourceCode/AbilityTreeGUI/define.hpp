#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <cmath>
#include <QFile>
#include <QString>
#include <QJsonArray>
#include <QMessageBox>
#include <QScrollArea>
#include <QJsonObject>
#include <QJsonDocument>
#include "opencv2/opencv.hpp"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define STYLE_COROR_R "#f4716e"
#define STYLE_COROR_G "#42f875"
#define STYLE_COROR_B "#1edffc"
#define STYLE_COROR_HRGB "qlineargradient(x1:0,y1:1,x2:1,y2:1,stop:0.1 #f4716e, stop:0.5 #42f875, stop:0.9 #1edffc);"
#define STYLE_COROR_VRGB "qlineargradient(x1:1,y1:1,x2:1,y2:0,stop:0.1 #f4716e, stop:0.5 #42f875, stop:0.9 #1edffc);"

class ModuleList;
struct ModuleFunc {
	QString name;
	QString path;
};
struct ModuleMember {
	QString icon_path;
	QString ch_icon_path;
	QString name_ch, name_en;
	QString describe_ch, describe_en;
	ModuleList* widget = nullptr;
	std::vector<ModuleFunc>func;
};

extern std::vector<ModuleMember>moduleList;
static void WriteModuleNumbers() {
	QFile file("./conf/ModuleNumbers.json");
	file.open(QIODevice::ReadWrite | QIODevice::Text);

	QFile file_old("./conf/ModuleNumbers_old.json");
	file_old.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	file_old.write(file.readAll());
	file_old.close();
	file.close();

	QJsonDocument doc;
	QJsonArray main_arr;
	for (auto i : moduleList) {
		QJsonObject _obj;
		_obj["name_ch"] = i.name_ch;
		_obj["name_en"] = i.name_en;
		_obj["icon_path"] = i.icon_path;
		_obj["describe_ch"] = i.describe_ch;
		_obj["describe_en"] = i.describe_en;
		_obj["choosed_icon_path"] = i.ch_icon_path;

		QJsonArray _arr;
		for (auto f : i.func) {
			QJsonObject f_obj;
			f_obj["name"] = f.name;
			f_obj["path"] = f.path;
			if (f.name.isEmpty() || f.path.isEmpty())continue;
			_arr.push_back(f_obj);
		}
		_obj["Func"] = _arr;
		main_arr.push_back(_obj);
	}
	file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
	doc.setArray(main_arr);
	file.write(doc.toJson());
	file.close();
}
static bool LoadModuleNumbers() {
	QFile file("./conf/ModuleNumbers.json");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString str = file.readAll();
	file.close();
	QJsonArray moudle_json = QJsonDocument::fromJson(str.toUtf8()).array();
	if (moudle_json.isEmpty()) {
		auto choose = QMessageBox::warning(nullptr, "错误", "资源文件损坏!是否回退到最后一次正确配置?", QMessageBox::Yes | QMessageBox::No);
		if (choose == QMessageBox::No) {
			QMessageBox::warning(nullptr, "错误", "资源文件损坏!请重新安装程序!");
			return false;
		}
		QFile file("./conf/ModuleNumbers_old.json");
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		QString str = file.readAll();
		file.close();
		moudle_json = QJsonDocument::fromJson(str.toUtf8()).array();
		if (moudle_json.isEmpty()) {
			QMessageBox::warning(nullptr, "错误", "回溯失败，文件损坏或不存在!请重新安装程序!");
			return false;
		}
	}

	std::map<QString, bool>ch_name_check, en_name_check;
	for (auto i : moudle_json) {
		auto _obj = i.toObject();
		ModuleMember _temp;
		_temp.name_ch = _obj["name_ch"].toString();
		_temp.name_en = _obj["name_en"].toString();
		_temp.icon_path = _obj["icon_path"].toString();
		_temp.describe_ch = _obj["describe_ch"].toString();
		_temp.describe_en = _obj["describe_en"].toString();
		_temp.ch_icon_path = _obj["choosed_icon_path"].toString();
		auto func_arr = _obj["Func"].toArray();
		for (auto f : func_arr) {
			ModuleFunc func;
			auto f_obj = f.toObject();
			func.name = f_obj["name"].toString();
			func.path = f_obj["path"].toString();
			if (func.name.isEmpty() || func.path.isEmpty())continue;
			_temp.func.push_back(func);
		}
		if (en_name_check.count(_temp.name_en))continue;
		if (en_name_check.count(_temp.name_ch))continue;
		if (_temp.name_ch.isEmpty() || _temp.name_en.isEmpty())continue;
		en_name_check[_temp.name_ch] = true;
		ch_name_check[_temp.name_ch] = true;
		moduleList.push_back(_temp);
	}

	return true;
}

static std::vector<uchar> hexTorgb(std::string msg) {
	std::vector<uchar>rgb;
	std::string temp = "";
	for (uchar i : msg) {
		bool is_cap = (i >= 'A'&&i <= 'F'), is_wod = (i >= 'a'&&i <= 'f');
		if ((i >= '0' && i <= '9') || is_cap || is_wod)temp.push_back(i);
		if (temp.size() < 2) continue;
		int ten = temp[0] >= 'a' ? temp[0] - 'a' + 10 : (temp[0] >= 'A' ? temp[0] - 'A' + 10 : temp[0] - '0');
		ten = ten * 16 + (temp[0] >= 'a' ? temp[1] - 'a' + 10 : (temp[0] >= 'A' ? temp[1] - 'A' + 10 : temp[1] - '0'));
		rgb.push_back(ten);
		temp.clear();
	}
	return rgb;
}
static QString rgbTohex(std::vector<uchar> msg) {
	QString to_h = "";
	for (auto i : msg) {
		short ten = (short)i / 16;
		short ge = (short)i % 16;
		to_h.push_back(ten < 10 ? ten + '0' : (ten + 'A' - 10));
		to_h.push_back(ge < 10 ? ge + '0' : (ge + 'A' - 10));
	}
	return to_h;
}

#endif // DEFINE_HPP