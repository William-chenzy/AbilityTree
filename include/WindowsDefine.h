#ifndef __WINDOWS_DEFINE__H__
#define __WINDOWS_DEFINE__H__

#include "CppWindowsDefine.h"

static const char* QStringToLPCSTR(const QString& qstr) {
	return qstr.toUtf8().constData();
}
static std::wstring QStringToStdWstring(const QString& qstr) {
	std::wstring wstr(reinterpret_cast<const wchar_t*>(qstr.utf16()), qstr.length());
	return wstr;
}
static QString ByteArrayToQString(const LPBYTE byteArray, size_t length) {
	QString result = QString::fromUtf8(reinterpret_cast<const char*>(byteArray), length);
	return result;
}

static bool GetRegistryValue(QString& key_val) {
	HKEY key;
	DWORD type = REG_SZ;
	DWORD dwBytes = 1024;
	LPBYTE path_Get = new BYTE[1024];
	auto res = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &key);
	if (res == ERROR_SUCCESS){
		res = RegQueryValueEx(key, key_val.toStdString().c_str(), 0, &type, path_Get, &dwBytes);
		if (res == ERROR_SUCCESS)key_val = ByteArrayToQString(path_Get, dwBytes);
		else key_val = "";
	}
	RegCloseKey(key);
	return res == ERROR_SUCCESS;
}
static bool SetRegistryValue(QString& key_val, QString val) {
	HKEY hKey;
	LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
	if (result != ERROR_SUCCESS) return false;

	auto t_val = reinterpret_cast<const BYTE*>(val.toStdString().c_str());
	result = RegSetValueEx(hKey, key_val.toStdString().c_str(), 0, REG_SZ, t_val, (val.size() + 1) * sizeof(wchar_t));

	RegCloseKey(hKey);
	return result == ERROR_SUCCESS;
}
static bool RemoveRegistryValue(QString key_val) {
	HKEY hKey;
	LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);
	if (result != ERROR_SUCCESS) return false;

	result = RegDeleteValue(hKey, key_val.toStdString().c_str());

	RegCloseKey(hKey);
	return result == ERROR_SUCCESS;
}

static bool CreateShortcut(const std::wstring shortcutPath, const LPCSTR targetPath) {
	HRESULT hResult;
	IShellLink* pShellLink = nullptr;
	hResult = CoInitialize(nullptr);
	if (FAILED(hResult)) return false;

	hResult = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
	if (FAILED(hResult)) {
		CoUninitialize();
		return false;
	}

	IPersistFile* pPersistFile = nullptr;
	pShellLink->SetPath(targetPath);
	pShellLink->SetArguments("");

	hResult = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
	if (SUCCEEDED(hResult)) {
		hResult = pPersistFile->Save(shortcutPath.c_str(), TRUE);
		pPersistFile->Release();
	}

	pShellLink->Release();
	CoUninitialize();

	if (FAILED(hResult)) return false;
	return true;
}

#endif //__GLOBAL_DEFINE__H__