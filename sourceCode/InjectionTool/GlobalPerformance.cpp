#include <GlobalPerformance.h>
#include <QTextCodec>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

GlobalPerformance::GlobalPerformance() {

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	connect(&refresh, &QTimer::timeout, this, &GlobalPerformance::RefreshViewer);
}

QString b_to_tb(unsigned long long val) {
	QString byte = " B";
	long double f_val = val;
	if (f_val >= 1000)f_val /= 1000.f, byte = " KB";
	if (f_val >= 1000)f_val /= 1000.f, byte = " MB";
	if (f_val >= 1000)f_val /= 1000.f, byte = " GB";
	if (f_val >= 1000)f_val /= 1000.f, byte = " TB";
	if (f_val >= 1000)f_val /= 1000.f, byte = " PB";
	if (f_val >= 1000)f_val /= 1000.f, byte = " EB";
	return QString::number(f_val, 'f', 2) + byte;
}
bool getCpuLoadInfo() {
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		qInfo() << "Failed to initialize COM library. Error code = 0x" << QString::number(hres, 16);
		return 0;
	}

	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
		qInfo() << "Failed to initialize security. Error code = 0x" << QString::number(hres, 16);
		CoUninitialize();
		return 0;
	}

	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres)) {
		qInfo() << "Failed to create IWbemLocator object. Err code = 0x" << hres;
		CoUninitialize();
		return 0;
	}

	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres)) {
		qInfo() << "Could not connect. Error code = 0x" << hres;
		pLoc->Release();
		CoUninitialize();
		return 0;
	}

	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) {
		qInfo() << "Could not set proxy blanket. Error code = 0x" << hres;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 0;
	}

	int i = 0;
	IWbemClassObject *pclsObj;
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_PerfFormattedData_PerfOS_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL, &pEnumerator);
	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 0;
	}

	ULONG uReturn = 0;
	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (!uReturn)break;

		VARIANT vtProp;
		hr = pclsObj->Get(L"PercentProcessorTime", 0, &vtProp, 0, 0);
		qInfo() << " CPU Usage of CPU " << i << " : " << QString::fromStdWString(vtProp.bstrVal);
		VariantClear(&vtProp);
		pclsObj->Release();
		i++;
	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	return 0;
}
bool GetNetIoBytes() {
	PDH_FMT_COUNTERVALUE cv;
	HQUERY  hQuery = nullptr;
	HCOUNTER hcReceived, hcSent, counter, memCounter;

	if (PdhOpenQuery(NULL, NULL, &hQuery) != ERROR_SUCCESS)return false;

	PdhAddCounter(hQuery, "\\Network Interface(*)\\Bytes Sent/sec", NULL, &hcSent);
	PdhAddCounter(hQuery, "\\Network Interface(*)\\Bytes Received/sec", NULL, &hcReceived);
	PdhAddCounter(hQuery, "\\Processor(_Total)\\% Processor Time", NULL, &counter);
	PdhAddCounter(hQuery, "\\Memory\\Available MBytes", NULL, &memCounter);

	if (PdhCollectQueryData(hQuery) != ERROR_SUCCESS)return false;
	Sleep(1000);
	if (PdhCollectQueryData(hQuery) != ERROR_SUCCESS)return false;

	if (PdhGetFormattedCounterValue(hcSent, PDH_FMT_LONG, NULL, &cv) == ERROR_SUCCESS)qInfo() << cv.longValue;
	if (PdhGetFormattedCounterValue(hcReceived, PDH_FMT_LONG, NULL, &cv) == ERROR_SUCCESS)qInfo() << cv.longValue;
	if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &cv) == ERROR_SUCCESS)qInfo() << cv.doubleValue;
	if (PdhGetFormattedCounterValue(memCounter, PDH_FMT_LONG, NULL, &cv) == ERROR_SUCCESS)qInfo() << b_to_tb(cv.longValue * 10000000);

	PdhRemoveCounter(hcSent);
	PdhRemoveCounter(hcReceived);
	PdhCloseQuery(hQuery);
	return true;
}

void global_data() {
	GetNetIoBytes();
	getCpuLoadInfo();

	//i/o

	//gpu
}

void GlobalPerformance::RefreshViewer() {
	std::thread* _t = new std::thread(global_data);
}





void GlobalPerformance::InstallCpuLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallCpuLabel(QLabel* label) {

}

void GlobalPerformance::InstallMemLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallMemLabel(QLabel* label) {

}

void GlobalPerformance::InstallIoLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallIoLabel(QLabel* label) {

}

void GlobalPerformance::InstallNetLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallNetLabel(QLabel* label) {

}

void GlobalPerformance::InstallGpuLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallGpuLabel(QLabel* label) {

}

void GlobalPerformance::InstallDiskLabel(QLabel* label) {

}
void GlobalPerformance::UnInstallDiskLabel(QLabel* label) {

}

bool GlobalPerformance::eventFilter(QObject* obj, QEvent* event) {
	return false;
}