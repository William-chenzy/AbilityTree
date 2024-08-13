#include "InstallTool.h"
#include <QApplication>

#ifdef _WIN32
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "Wsock32.lib") 
#pragma comment(lib, "Imm32.lib") 
#pragma comment(lib, "Winmm.lib") 
#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    InstallTool w(0);
    w.show();

    return a.exec();
}
