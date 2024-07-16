#pragma once
#include <objbase.h>
#include <Netcfgn.h>
#include <atlbase.h>
#include <netcon.h>
#include <atlcom.h>
#include <icftypes.h>
#include <netfw.h>
#include <QUuid>
#include <CFGMGR32.h>
#include <iphlpapi.h>
#include <initguid.h>
#include <Devpkey.h>
#include <qDebug>	
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Cfgmgr32.lib")

class CNetConnectionPropertyUi :
	public CComCoClass<CNetConnectionPropertyUi>,
	public CComObjectRootEx<CComMultiThreadModelNoCS>,
	public INetLanConnectionUiInfo
{
public:
	CNetConnectionPropertyUi() {};
	~CNetConnectionPropertyUi() {};

	virtual HRESULT  GetDeviceGuid(GUID* pGuid);

private:
	QString GetNetcardGuid(GUID* pGuid);
public:
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CNetConnectionPropertyUi)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CNetConnectionPropertyUi)
		COM_INTERFACE_ENTRY_IID(IID_INetLanConnectionUiInfo, INetLanConnectionUiInfo)
	END_COM_MAP()
};

QString strGuid = "{8D3EBF0E-1435-4293-B9D5-9B16826B9447}";
HRESULT CNetConnectionPropertyUi::GetDeviceGuid(GUID* pGuid)
{
	GUID temp = GUID(QUuid(strGuid));
	CopyMemory(pGuid, &temp, sizeof(GUID));
	return S_OK;
}

CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule = &_Module;
bool InvokeTCPIPProperties(HWND hWndParent, std::string Guid)
{
	strGuid = Guid.c_str();
	CComPtr<INetCfg> NetCfg;
	CComQIPtr<INetCfgLock> NetCfgLock;
	CComPtr<INetCfgComponent> NetCfgComponent;
	LPWSTR ppszwrClient = NULL;

	CComObject<CNetConnectionPropertyUi>* pNetLanConnectionUiObj = NULL;
	HRESULT hr = CComObject<CNetConnectionPropertyUi>::CreateInstance(&pNetLanConnectionUiObj);
	CComPtr<IUnknown> spNetConnectionPropertyUi((INetConnectionConnectUi*)pNetLanConnectionUiObj);

	if (hr == S_OK)hr = CoCreateInstance(CLSID_CNetCfg, NULL, CLSCTX_SERVER, IID_INetCfg, (LPVOID*)&NetCfg);

	WCHAR wsz[64];
	swprintf(wsz, L"%S", "MY CLIENT");
	LPCWSTR lpcs = wsz;
	WCHAR wsz2[64];
	swprintf(wsz2, L"%S", NETCFG_TRANS_CID_MS_TCPIP);
	LPCWSTR lpcs2 = wsz2;

	NetCfgLock = NetCfg;
	if (hr == S_OK)hr = NetCfgLock->AcquireWriteLock(5, lpcs, &ppszwrClient);
	if (hr == S_OK)hr = NetCfg->Initialize(NULL);

	if (hr == S_OK)hr = NetCfg->FindComponent(lpcs2, &NetCfgComponent);
	if (hr == S_OK)hr = NetCfgComponent->RaisePropertyUi(hWndParent, NCRP_SHOW_PROPERTY_UI, spNetConnectionPropertyUi);

	if (hr == S_OK)hr = NetCfg->Apply();
	if (hr == S_OK)hr = NetCfg->Uninitialize();
	if (hr == S_OK)hr = NetCfgLock->ReleaseWriteLock();

	return hr == S_OK;
}

#define DeviceProperties_RunDLL  "DeviceProperties_RunDLLA"
typedef void(_stdcall* PDEVICEPROPERTIES)(
	HWND hwndStub,
	HINSTANCE hAppInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	);
PDEVICEPROPERTIES pDeviceProperties;

std::string GetDeviceID(std::string name)
{
	std::string dev_id = "";
	ULONG ulLen;
	if (CM_Get_Device_ID_List_SizeW(&ulLen, 0, CM_GETIDLIST_FILTER_PRESENT) != 0 || ulLen <= 1)return dev_id;
	PZZWSTR pDeviceID = new WCHAR[ulLen];
	CONFIGRET cr = CM_Get_Device_ID_ListW(0, pDeviceID, ulLen, CM_GETIDLIST_FILTER_PRESENT);
	if (cr == CR_SUCCESS && *pDeviceID) {
		do{
			DEVINST dnDevInst;
			if (CM_Locate_DevNodeW(&dnDevInst, pDeviceID, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) continue;
			ULONG len = 1024;
			char szProperty[1024] = { 0 };
			cr = CM_Get_DevNode_Registry_Property(dnDevInst, 1, NULL, szProperty, &len, 0);
			if (cr == CR_SUCCESS && name == szProperty) {
				std::string nam = "";
				while (pDeviceID[dev_id.size()])dev_id.push_back(pDeviceID[dev_id.size()]);
				break;
			}
		} while (*(pDeviceID += wcslen(pDeviceID) + 1));
	}
	dev_id = "/DeviceID " + dev_id;
	return dev_id;
}