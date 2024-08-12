#ifndef Server_H
#define Server_H

#include <QMainWindow>
#include <Qtimer>
namespace Ui {
    class Server;
}

class Server : public QMainWindow
{
	Q_OBJECT

public:
    Server(QWidget *parent = 0);
	~Server() = default;

protected:

private:

private slots:

private:
    Ui::Server* ui;
};


#endif // Server_H
