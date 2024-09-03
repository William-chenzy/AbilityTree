#include "execute.h"
using namespace std;

bool EnableDebugPriv(){
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return false;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))return false;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))return false;

	return true;
}

int main(int argc, char *argv[]){
	if (!EnableDebugPriv())return -1;
	if (argc < 3) return -1;

	DWORD PID = atol(argv[1]);
	int type = atoi(argv[2]);
	if (type == KILL) thread_injection(PID, (TYPE)type);
	else HookThread(PID, (TYPE)type);
	return 0;
}