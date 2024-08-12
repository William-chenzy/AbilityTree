#ifndef ServerExecute_H
#define ServerExecute_H

#include "CppGlobalDefine.h"
#include "httplib.h"

typedef void(*http_callback)(const httplib::Request &, httplib::Response &);

class ServerExecute{
public:
	ServerExecute(bool flag);
	~ServerExecute();

	void OutputLog(std::string str) { *log << str; }

	bool UdpBind(std::string ip, int port);
	bool TcpBind(std::string ip, int port);
	bool HttpBind(std::string ip, int port);

	bool SetHttplUrl(std::string url, http_callback cb);

public:
	static std::string ReadHtml(std::string path);

private:
	std::fstream* log;
	bool log_flag = false;
	httplib::Server http_svr;
};

#endif // ServerExecute_H