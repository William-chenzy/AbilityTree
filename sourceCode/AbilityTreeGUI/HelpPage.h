#ifndef HELP_PAGE_H
#define HELP_PAGE_H

#include <map>
#include <qtimer.h>
#include <QDialog>
#include <GlobalDefine.h>

namespace Ui {
	class HelpPage;
}
class HelpPage : public QDialog
{
	Q_OBJECT
public:
	explicit HelpPage(QWidget* parent = nullptr);
	~HelpPage();

protected:
	void closeEvent(QCloseEvent *event) override;

private slots:

private:
	Ui::HelpPage* ui;
};

#endif // HELP_PAGE_H
