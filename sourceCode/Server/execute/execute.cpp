#include "execute.h"
#include <iostream>
using namespace std;

string GetTime() {
	long long mil = GetTimeM % 1000000;
	time_t nows = time(NULL);
	static tm* _t = new tm();
	localtime_s(_t, &nows);
	string tim_ = to_string(_t->tm_year + 1900) + "_"
		+ to_string(_t->tm_mon + 1) + "_"
		+ to_string(_t->tm_mday) + "_"
		+ to_string(_t->tm_hour) + "_"
		+ to_string(_t->tm_min) + "_"
		+ to_string(_t->tm_sec) + "_"
		+ to_string(mil);
	return tim_;
}

map<string, string>get_url, post_url;
void LoadHttpConfigure() {
	std::fstream _file("http_configure", std::ios::in);
	if (!_file.is_open())return;

	char buf[1024];
	while (!_file.eof()) {
		memset(buf, 0, 1024);
		_file.getline(buf, 1024);
		std::string str = buf;
		int sed_pos = str.find('=');
		if (sed_pos == std::string::npos)continue;
		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
		string url = str.substr(0, sed_pos), val = str.substr(sed_pos + 1);
		cout << "url: " << url << " : " << val << endl;
		if (val != "#post") get_url[url] = val;
		else  post_url[url] = val;
	}
	_file.close();
}

string get_mime_type(const string& path) {
	auto ext_pos = path.find_last_of('.');
	if (ext_pos == string::npos) return "application/octet-stream";

	std::string extension = path.substr(ext_pos);
	if (extension == ".html") return "text/html";
	if (extension == ".css") return "text/css";
	if (extension == ".js") return "application/javascript";
	if (extension == ".png") return "image/png";
	if (extension == ".jpg") return "image/jpeg";
	if (extension == ".gif") return "image/gif";
	return "application/octet-stream";
}

void serve_file(const string& path, httplib::Response& res) {
	cout << "visit path: " << path << endl;
	ifstream file(path, ios::binary);
	if (file) {
		stringstream buffer;
		buffer << file.rdbuf();
		res.set_content(buffer.str(), get_mime_type(path));
	}
	else {
		ifstream file_404("html/404NotFind.html", ios::binary);
		stringstream buffer;
		buffer << file_404.rdbuf();
		res.set_content(buffer.str(), "text/html");
	}
}

int main(int argc, char *argv[]) {
	system("sh ./config_server.sh");
	cout << "config_server init done!" << endl;

	ServerExecute server(true);
	cout << "ServerExecute init done!" << endl;

	string http_ip = "172.21.134.109";
	if (argc >= 2)http_ip = argv[1];

	if (!server.UdpBind("121.40.55.218.", 46784))return 0;
	cout << "UdpBind init done!" << endl;

	if (!server.TcpBind("121.40.55.218", 46785))return 0;
	cout << "TcpBind init done!" << endl;

	LoadHttpConfigure();
	cout << "HttpBind init: " << http_ip << endl;
	if (!server.HttpBind(http_ip, 8080))return 0;

	while (1) {
		this_thread::sleep_for(chrono::milliseconds(5000));
		server.OutputLog("server is running!\r");
		cout << "running..." << endl;
	}
	return true;
}

ServerExecute::ServerExecute(bool flag) {
	log_flag = flag;
	if (log_flag)log = new fstream("./server.log", ios::out);
}
ServerExecute::~ServerExecute() {}

bool ServerExecute::UdpBind(std::string ip, int port) {
	return true;
}
bool ServerExecute::TcpBind(std::string ip, int port) {
	return true;
}
bool ServerExecute::HttpBind(std::string ip, int port) {
	http_svr.Get(R"(/.*)", [](const httplib::Request& req, httplib::Response& res) {
		std::string path = req.path;
		if (path.find("/../") != string::npos)path = "404NotFind.html";
		if (get_url.count(path))path = get_url[path];
		serve_file("html" + path, res);
		});
	http_svr.Post(R"(/.*)", [&](const httplib::Request& req, httplib::Response& res) {
		std::string path = req.path;
		if (!post_url.count(path))return;
		string dir_path = path.substr(1) + "_" + GetTime();
		system(("mkdir " + dir_path).c_str());

		fstream _allfile(dir_path + "/all_param", ios::out | ios::trunc);
		auto add_to_file = [&](httplib::MultipartFormData data, string field_name = "") {
			if (!data.filename.empty()) {
				fstream _tempfile(dir_path + "/" + field_name + "_" + data.filename, ios::out | ios::trunc);
				_tempfile << data.content;
				_tempfile.flush();
				_tempfile.close();
			}
			else _allfile << data.name << "          =          " << data.content << " ::::::::: " << field_name << "\r\n";

		};
		for (const auto& file_pair : req.files)add_to_file(file_pair.second, file_pair.first);
		res.set_content("Data received", "text/plain");
		_allfile.flush();
		_allfile.close();

		});
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