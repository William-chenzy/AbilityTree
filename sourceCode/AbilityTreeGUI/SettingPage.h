#ifndef SETTING_PAGE_H
#define SETTING_PAGE_H

#include <map>
#include <qtimer.h>
#include <QDialog>
#include <GlobalDefine.h>

namespace Ui {
	class SettingPage;
}
enum Method{ Module_Set, Main_Set };
struct MainConfig {
	bool language;
	bool animation;
};
class SettingPage : public QDialog
{
	Q_OBJECT
public:
	explicit SettingPage(Method _method, QWidget* parent = nullptr);
	~SettingPage();

	void SetModuleParam(MainConfig val);
	void SetModuleParam(QString name, QString describe);

signals:
	void MainConfigChange(MainConfig val);
	void ModuleModify(QString name, QString describe, bool is_delete);

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:
	void on_pushButton_main_confirm_clicked();
	void on_pushButton_module_delete_clicked();
	void on_pushButton_module_confirm_clicked();

private:
	Ui::SettingPage* ui;
};

#endif // SETTING_PAGE_H
