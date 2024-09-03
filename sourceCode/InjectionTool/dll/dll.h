#include <CppWindowsDefine.h>

class MainHook {
public:
	MainHook() {
		m_pfnOld = nullptr;
		ZeroMemory(m_bNewBytes, 5);
		ZeroMemory(m_bOldBytes, 5);
	}
	~MainHook() { UnHook(); }

	BOOL Hook(char* szModuleName, char* szFuncName, PROC pHookFunc) {
		m_pfnOld = GetProcAddress(GetModuleHandleA(szModuleName), szFuncName);
		if (!m_pfnOld)return FALSE;

		SIZE_T dwNum = 0;
		ReadProcessMemory(GetCurrentProcess(), m_pfnOld, m_bOldBytes, 5, &dwNum);

		m_bNewBytes[0] = '\xe9';
		*(DWORD*)(m_bNewBytes + 1) = (DWORD)pHookFunc - (DWORD)m_pfnOld - 5;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOld, m_bNewBytes, 5, &dwNum);
		return TRUE;
	}

	void UnHook() {
		if (m_pfnOld == nullptr)return;
		SIZE_T dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOld, m_bOldBytes, 5, &dwNum);
	}

	bool ReHook() {
		if (m_pfnOld == nullptr)return false;

		SIZE_T dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOld, m_bNewBytes, 5, &dwNum);
		return TRUE;
	}
private:
	PROC m_pfnOld;
	BYTE m_bOldBytes[5];
	BYTE m_bNewBytes[5];
};
MainHook recv_h, send_h;


int ReadFromIniFile(HMODULE hModule) {
	static int val = 0;
	if (val)return val;

	std::vector<char> path(1024);
	std::vector<char> buffer(24);
	DWORD processId = GetCurrentProcessId();
	std::string pid_str = std::to_string(processId);
	DWORD length = GetModuleFileName(hModule, path.data(), path.size());
	std::string path_str = std::string(path.data(), length).c_str();
	while (path_str.find('\\') != std::string::npos) path_str.replace(path_str.find('\\'), 1, "/");
	path_str = path_str.substr(0, path_str.find_last_of('/'));
	path_str = path_str + "/conf/InjectionTool.param";

	GetPrivateProfileString("Main", pid_str.c_str(), "0", buffer.data(), buffer.size(), path_str.c_str());
	val = atoi(std::string(buffer.data()).c_str());
	return val;
}