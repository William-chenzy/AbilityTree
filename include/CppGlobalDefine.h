#ifndef __CPP_GLOBAL_DEFINE__H__
#define __CPP_GLOBAL_DEFINE__H__
#pragma once
#include <vector>
#include <chrono>
#include <fstream>

typedef unsigned char uchar;
typedef unsigned long Socket_fd;
#ifdef __linux__
#include <CppLinuxDefine.h>
#else
#include <CppWindowsDefine.h>
#endif

#define SCH std::chrono
#define PI_U 3.14159265358979323846
#define GetTimeM SCH::duration_cast<SCH::microseconds>(SCH::system_clock::now().time_since_epoch()).count()

#define is_CH !LoadConfigure::getInstance()->IS_EN()
class LoadConfigure{
public:
	static LoadConfigure* getInstance() {
		static LoadConfigure* ptr = new LoadConfigure();
		return ptr;
	}
	LoadConfigure(const LoadConfigure&) = delete;
	LoadConfigure& operator=(const LoadConfigure&) = delete;

public:
	bool IS_EN() { return val[0]; }
	bool GetAnimation() { return val[1]; }
	bool GetConfigStatus() { return config_status; }

public:
	void SetEN(char s) { val[0] = s; WriteConfigFile(); }
	void SetAnimation(char s) { val[1] = s; WriteConfigFile();}

private:
	LoadConfigure(){
		std::fstream conf_file("./conf/AbilityTree.pr", std::ios::in | std::ios::binary);
		if (conf_file.is_open()){
			while (!conf_file.eof())val.push_back(conf_file.get());
			val.pop_back();
			conf_file.close();
		}

		if (val.size() < 1)val.push_back(0);//语言
		if (val.size() < 2)val.push_back(0);//动画效果
		
		ts.open("./conf/AbilityTree.pr", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
		config_status = ts.is_open();
		WriteConfigFile();
	}
	~LoadConfigure() { WriteConfigFile(); ts.close(); };
private:
	void WriteConfigFile() {
		if (!config_status) return;
		ts.seekg(0, std::ios::beg);
		ts.write(val.c_str(), val.length());
	}
	std::fstream ts;
	std::string val;
	bool config_status = false;
};

static std::string asciiTohex(std::string msg, bool add_space = true) {
	std::string to_h = "";
	for (uchar i : msg) {
		short ten = (short)i / 16;
		short ge = (short)i % 16;
		to_h.push_back(ten < 10 ? ten + '0' : (ten + 'A' - 10));
		to_h.push_back(ge < 10 ? ge + '0' : (ge + 'A' - 10));
		if (add_space) to_h.push_back(' ');
	}
	return to_h;
}
static std::string hexToascii(std::string msg) {
	std::string to_h = "";
	std::string temp = "";
	for (uchar i : msg) {
		bool is_cap = (i >= 'A'&&i <= 'F'), is_wod = (i >= 'a'&&i <= 'f');
		if ((i >= '0' && i <= '9') || is_cap || is_wod)temp.push_back(i);
		if (temp.size() < 2) continue;
		int ten = temp[0] >= 'a' ? temp[0] - 'a' + 10 : (temp[0] >= 'A' ? temp[0] - 'A' + 10 : temp[0] - '0');
		ten = ten * 16 + (temp[0] >= 'a' ? temp[1] - 'a' + 10 : (temp[0] >= 'A' ? temp[1] - 'A' + 10 : temp[1] - '0'));
		to_h.push_back(ten);
		temp.clear();
	}
	return to_h;
}

static tm GetTimeT() {
	time_t nows = time(NULL);
	static tm* _t = new tm();
	localtime_s(_t, &nows);
	return *_t;
}


#define IS_CH !LoadConfigure::getInstance()->IS_EN()
#define LU_STR1 (IS_CH?"错误":"Error")
#define LU_STR44  (IS_CH?"提示":"Tips")
#define LU_STR122 (IS_CH?"Linux 不支持此功能":"Linux does not support this feature")
#define LU_STR123 (IS_CH?"WSA 初始化失败":"WSA Init failed")
#define LU_STR124 (IS_CH?"获取网卡流量失败":"Failed to obtain network card traffic")
#define LU_STR125 (IS_CH?"申请内存失败":"Failed to apply for memory")
#define LU_STR126 (IS_CH?"抓包失败: ":"Packet capture failed: ")
#define LU_STR127 (IS_CH?"回显应答":"Echo Reply")
#define LU_STR128 (IS_CH?"回显请求":"Echo request")
#define LU_STR129 (IS_CH?"网络不可达":"Network is unreachable")
#define LU_STR130 (IS_CH?"主机不可达":"Host Unreachable")
#define LU_STR131 (IS_CH?"协议不可达":"Protocol unreachable")
#define LU_STR132 (IS_CH?"端口不可达":"Port unreachable")
#define LU_STR133 (IS_CH?"需要进行分片但设置不分片比特":"Need to perform sharding but set non sharding bits")
#define LU_STR134 (IS_CH?"由于过滤，通信被强制禁止":"Communication is forcibly prohibited due to filtering")
#define LU_STR135 (IS_CH?"对主机重定向":"Redirect to host")
#define LU_STR136 (IS_CH?"传输期间生存时间为0":"Survival time during transmission is 0")
#define LU_STR137 (IS_CH?"坏的IP首部（包括各种差错）":"Bad IP header (including various errors)")
#define LU_STR138 (IS_CH?"获取网卡列表失败!":"Failed to obtain network card list!")
#define LU_STR139 (IS_CH?"保存到文件":"Save to file")
#define LU_STR140 (IS_CH?"保存失败，文件创建失败":"Save failed, file creation failed")
#define LU_STR141 (IS_CH?"保存成功":"Save success")
#define LU_STR142 (IS_CH?"停止捕获":"Stop capturing")
#define LU_STR143 (IS_CH?"是否保存到文件?":"Save to file?")
#define LU_STR144 (IS_CH?"清空历史":"Clear History")
#define LU_STR145 (IS_CH?"确认清空历史?":"Are you sure to clear history?")
//#define LU_STR156 (IS_CH?"初始化":"Init")
#define LU_STR157 (IS_CH?"COM初始化失败":"COM init failed")
#define LU_STR158 (IS_CH?"devmgr初始化失败":"devmgr init failed")
#define LU_STR159 (IS_CH?"防火墙设置成功!":"Successfully set firewall!")
#define LU_STR160 (IS_CH?"防火墙设置失败，COM初始化失败: 0x%08lx\n":"Firewall setup failed, COM initialization failed: 0x%08lx\n")
#define LU_STR161 (IS_CH?"域网设置失败: 0x%08lx\n":"Domain network setup failed: 0x%08lx\n")
#define LU_STR162 (IS_CH?"专网设置失败: 0x%08lx\n":"Private network setup failed: 0x%08lx\n")
#define LU_STR163 (IS_CH?"公网设置失败: 0x%08lx\n":"Public network setup failed: 0x%08lx\n")
#define LU_STR164 (IS_CH?"防火墙设置成功: %s\n":"Successfully set firewall: %s\n")
#define LU_STR165 (IS_CH?"已关闭":"CLOSED")
#define LU_STR166 (IS_CH?"已打开":"OPENED")
#define LU_STR167 (IS_CH?"设置防火墙":"Set firewall")
#define LU_STR194 (IS_CH?" 成功":" Success")
#define LU_STR232 (IS_CH?"此文件已存在，确认替换吗?":"This file already exists. Are you sure to replace it?")
#define LU_STR233 (IS_CH?"文件夹":"Folder")
#define LU_STR224 (IS_CH?"断开":"Disconnect")
#define LU_STR225 (IS_CH?"连接":"Connect")
#define LU_STR226 (IS_CH?"TCP 服务器状态: ":"TCP Server Status: ")
#define LU_STR227 (IS_CH?"上线.":"Online.")
#define LU_STR228 (IS_CH?"离线.":"Offline.")
#define LU_STR229 (IS_CH?"连接状态: ":"Connection status: ")
#define LU_STR230 (IS_CH?"FTP 连接状态: ":"FTP Connection status: ")
#define LU_STR231 (IS_CH?"发送内容为空!":"The sent content is empty!")
#define LU_STR232 (IS_CH?"此文件已存在，确认替换吗?":"This file already exists. Are you sure to replace it?")
#define LU_STR233 (IS_CH?"文件夹":"Folder")
#define LU_STR234 (IS_CH?"修改":"Modify")
#define LU_STR235 (IS_CH?"请输入数据: ":"Please enter data: ")
#define LU_STR236 (IS_CH?"删除":"Delete")
#define LU_STR237 (IS_CH?"重命名":"Rename")
#define LU_STR238 (IS_CH?"消息发送失败":"Message sending failed")
#define LU_STR239 (IS_CH?"绑定套接字失败，错误码: ":"Binding socket failed with error code: ")
#define LU_STR240 (IS_CH?"绑定套接字完成":"Binding socket completed")
#define LU_STR241 (IS_CH?"监听套接字失败，错误码: ":"Listening socket failed with error code: ")
#define LU_STR242 (IS_CH?"监听套接字完成，最大连接数: 256":"Listening socket completion, maximum number of connections: 256")
#define LU_STR243 (IS_CH?"客户端尝试连接失败":"Client attempt to connect failed")
#define LU_STR244 (IS_CH?"连接服务器失败，错误码: ":"Connection to server failed with error code: ")
#define LU_STR245 (IS_CH?"获取串口信息失败":"Failed to obtain serial port information")
#define LU_STR246 (IS_CH?"设置串口信息失败":"Failed to set serial port information")
#define LU_STR247 (IS_CH?"设置超时时间失败":"Failed to set timeout time")
#define LU_STR248 (IS_CH?"清空缓冲区失败":"Empty buffer failed")
#define LU_STR249 (IS_CH?"设置非阻塞失败":"Setting non blocking failed")
#define LU_STR250 (IS_CH?"绑定串口失败":"Binding serial port failed")
#define LU_STR251 (IS_CH?"获取列表返回码: ":"Get List Return Code: ")
#define LU_STR252 (IS_CH?"列表为空!":"List is empty!")
#define LU_STR253 (IS_CH?"文件不存在":"File does not exist")
#define LU_STR254 (IS_CH?"文件删除请求返回码: ":"File deletion request return code: ")
#define LU_STR255 (IS_CH?"文件 %1 删除 %2":"File %1 deleted %2")
#define LU_STR256 (IS_CH?"失败":"Failed")
#define LU_STR257 (IS_CH?"文件上传失败：文件不存在: ":"File upload failed: the file does not exist: ")
#define LU_STR258 (IS_CH?"传输请求返回码: ":"Transmission Request Return Code: ")
#define LU_STR259 (IS_CH?"文件 ":"File ")
#define LU_STR260 (IS_CH?" 上传完成: ":" Upload completed: ")
#define LU_STR261 (IS_CH?"获取文件大小失败: ":"Failed to obtain file size: ")
#define LU_STR262 (IS_CH?" 下载完成: ":" Download completed: ")
#define LU_STR263 (IS_CH?"RNTO 请求返回码: ":"RNTO Request return code: ")
#define LU_STR264 (IS_CH?"文件 %1 重命名%2":"File %1 rename %2")
#define LU_STR265 (IS_CH?"服务器已关闭":"Server shutdown")
#define LU_STR266 (IS_CH?"服务器已开启":"Server turned on")
#define LU_STR267 (IS_CH?"绑定失败：IP与端口已被绑定":"Binding failure: IP and port have already been bound")
#define LU_STR268 (IS_CH?"连接已断开":"Connection dropped")
#define LU_STR269 (IS_CH?"连接成功":"Successfully connected")
#define LU_STR270 (IS_CH?"连接失败：服务器未在线":"Connection failure: server not online")
#define LU_STR271 (IS_CH?"服务器已连接":"Server connected")
#define LU_STR272 (IS_CH?"连接失败":"Connect failed")
#define LU_STR273 (IS_CH?"设置CAN失败":"Failed to set CAN")
#define LU_STR378 (IS_CH?"LxProtocol - 通讯工具包":"LxProtocol - Communication tool")
#define LU_STR379 (IS_CH?"TCP 服务器":"TCP Server")
#define LU_STR380 (IS_CH?"TCP 客户端":"TCP Client")
#define LU_STR381 (IS_CH?"Ip地址: ":"IP Address")
#define LU_STR382 (IS_CH?"连接":"Connect")
#define LU_STR383 (IS_CH?"接收对端:":"Client:")
#define LU_STR384 (IS_CH?"循环发送周期:":"Circular send")
#define LU_STR385 (IS_CH?"清空历史消息":"Clear history")
#define LU_STR386 (IS_CH?"发送编码: ":"Send code")
#define LU_STR387 (IS_CH?"接收编码: ":"Recv code")
#define LU_STR388 (IS_CH?"发送":"Send")
#define LU_STR389 (IS_CH?"历史连接:":"Connected:")
#define LU_STR390 (IS_CH?"刷新":"Refresh")
#define LU_STR391 (IS_CH?"主机ID: ":"Slave ID:")
#define LU_STR392 (IS_CH?"扫描间隔(ms)： ":"Scan time(ms): ")
#define LU_STR393 (IS_CH?"起始地址: ":"Start address: ")
#define LU_STR394 (IS_CH?"地址长度:":"Address length: ")
#define LU_STR395 (IS_CH?"模式: ":"Mode: ")
#define LU_STR396 (IS_CH?"设置完成":"Configure done")
#define LU_STR397 (IS_CH?"端口: ":"COM: ")
#define LU_STR398 (IS_CH?"波特率: ":"Baud: ")
#define LU_STR399 (IS_CH?"校验位: ":"Check bit: ")
#define LU_STR400 (IS_CH?"数据位: ":"Data bit")
#define LU_STR401 (IS_CH?"停止位: ":"Stop bit")
#define LU_STR413 (IS_CH?"置顶":"Top")
#define LU_STR414 (IS_CH?"开始":"Start")
#define LU_STR415 (IS_CH?"暂停捕获":"Pause capturing")
#define LU_STR416 (IS_CH?"停止捕获":"Stop capturing")
#define LU_STR417 (IS_CH?"保存到文件":"Save to file")
#define LU_STR418 (IS_CH?"清空":"Clear")
//#define LU_STR419 (IS_CH?"":"")
#define LU_STR420 (IS_CH?"网卡流量: ":"Network card traffic")
//#define LU_STR421 (IS_CH?"":"")
#define LU_STR422 (IS_CH?"网卡配置":"Network card configuration")
#define LU_STR423 (IS_CH?"网卡":"Network card")
#define LU_STR424 (IS_CH?"网卡属性":"Network Card Properties")
#define LU_STR425 (IS_CH?"打开":"Open")
#define LU_STR426 (IS_CH?"网卡协议属性":"Network Card Protocol Properties")
#define LU_STR427 (IS_CH?"防火墙配置页":"Firewall Configuration Page")
#define LU_STR428 (IS_CH?"任务管理器":"Task manager")
#define LU_STR429 (IS_CH?"网络连接中心":"Network Connection Center")
#define LU_STR430 (IS_CH?"完成":"Complete")
#define LU_STR431 (IS_CH?"打开/关闭 防火墙":"Turn on/off firewall")

#endif //__CPP_GLOBAL_DEFINE__H__
