// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <vector>
#include <string>
#include <fstream>
#include <dll/dll.h>
#pragma comment(lib,"ws2_32.lib") 

int recv1(_In_ SOCKET s, _Out_ char* buf, _In_ int len, _In_ int flags) {
	//g_MsgHook.UnHook();
	//recv(s, buf, 255, 0);
	//g_MsgHook.ReHook();
	//getchar();
	return 0;
}
int send1(_In_ SOCKET s, _Out_ const char* buf, _In_ int len, _In_ int flags) {
	return 0;
}

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	int flag = ReadFromIniFile(hModule);
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH: {
		if (flag & 0x01) recv_h.Hook("WS2_32.dll", "recv", (PROC)recv1);
		if (flag & 0x02) send_h.Hook("WS2_32.dll", "send", (PROC)send1);
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH: {
		recv_h.UnHook();
		send_h.UnHook();
		break;
	}
	}
	return TRUE;
}
