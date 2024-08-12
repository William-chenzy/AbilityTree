#include "execute.h"

void abilitytree_cn(const httplib::Request &, httplib::Response &res) {
	res.set_content(ServerExecute::ReadHtml("./AbilityTree.html"), "text/plain");
}

int main(int argc, char *argv[]) {
	ServerExecute server(true);
	if (!server.UdpBind("121.40.55.218.", 46784))return 0;
	if (!server.TcpBind("121.40.55.218", 46785))return 0;
	if (!server.HttpBind("121.40.55.218", 80))return 0;

	server.SetHttplUrl("www.abilitytree.cn", abilitytree_cn);

	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		server.OutputLog("server is running!\r");
	}
	return true;
}

ServerExecute::ServerExecute(bool flag) {
	log_flag = flag;
	if (log_flag)log = new std::fstream("./server.log", std::ios::out);
}
ServerExecute::~ServerExecute() {}

bool ServerExecute::UdpBind(std::string ip, int port) {
	return true;
}
bool ServerExecute::TcpBind(std::string ip, int port) {
	return true;
}
bool ServerExecute::HttpBind(std::string ip, int port) {
	return http_svr.listen(ip, port);
}
bool ServerExecute::SetHttplUrl(std::string url, http_callback cb) {
	http_svr.Get(url, cb);
	return true;
}

std::string ServerExecute::ReadHtml(std::string path) {
	std::string str = "";
	std::fstream _file(path, std::ios::in);
	if (!_file.is_open())return str;

	while (!_file.eof())str.push_back(_file.get());
	str.pop_back();
	return str;
}