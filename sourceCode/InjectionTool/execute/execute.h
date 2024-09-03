#include <CppWindowsDefine.h>
#include <iostream>

enum TYPE { KILL, MOUSE, KEY, DEBUG, DIALOG, ALL };
int thread_injection(DWORD pid, TYPE type);

void HookThread(DWORD pid, TYPE type);