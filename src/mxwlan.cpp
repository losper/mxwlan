// mxwlan.cpp : Defines the entry point for the DLL application.
//
#include "windows.h"
#include "wlanapi.h"
#include "cmnet.h"
#include "cmhelper.h"
#include "xstream.hpp"
#include "nwprofilehelper.h"
#include <mmsystem.h>
#include "quicksort.hpp"
#include "cmcsp.h"

#pragma comment(lib, "Mmtimer.lib")
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "cmnet.lib")

template<typename T>
class mxwifi{
public:
	static mxwifi& get_inst(){
		static mxwifi inst;
		return inst;
	}
	int disconnect(){
		
		WCHAR szConnectionName[WLAN_MAX_NAME_LENGTH];
		MultiToUnicode((const char*)curNetwork.config.Ssid.ucSSID,CP_ACP,szConnectionName,WLAN_MAX_NAME_LENGTH);
		DeleteAllConnectionConfigs(szConnectionName);
		/*
		if (ERROR_SUCCESS==DeleteConnection(szConnectionName))
		{
			MultiToUnicode((const char*)curNetwork.config.Ssid.ucSSID,65001,szConnectionName,WLAN_MAX_NAME_LENGTH);
			for (DWORD i=0;i<listsize;i++)
			{
				if(!strcmp((const char*)wlist[i].config.Ssid.ucSSID,(const char*)curNetwork.config.Ssid.ucSSID)){
					wlist[i].bConnected=0;
					break;
				}
			}
			curNetwork.bConnected=0;
			return 0;
		}*/
		return -1;
	}
	/*int disconnect(LPCTSTR szConnectionName){
		if (ERROR_SUCCESS==DeleteConnection(szConnectionName))
		{
			for (DWORD i=0;i<listsize;i++)
			{
				if(!strcmp((const char*)wlist[i].config.Ssid.ucSSID,(const char*)curNetwork.config.Ssid.ucSSID)){
					wlist[i].bConnected=0;
					break;
				}
			}
			return 0;
		}
		return -1;
		
	}*/
	int scan(){
		return PostThreadMessage(g_itrd,WM_USER+1,NULL,NULL);
	}
	int connect(LPTSTR ssid,LPTSTR pwd){
		EnterCriticalSection(&cs);
		_tcscpy_s(ucSSID,DOT11_SSID_MAX_LENGTH+1,ssid);
		_tcscpy_s(KeyMaterial,NWCTL_MAX_WEPK_MATERIAL,pwd);
		LeaveCriticalSection(&cs);
		if (WAIT_OBJECT_0==WaitForSingleObject(hpost,0))
		{
			PostThreadMessage(g_itrd,WM_USER+3,NULL,NULL);	
		}
		return 0;
	}
	
	
	int getWnetwork(tag_NetWork &tag,DWORD idx){
		if(idx<listsize){
			tag=wlist[idx];
			return 0;
		}
		return -1;
	}
	int getWlistSize(){
		return listsize;
	}
	tag_NetWork& getCurNetwork(){
		return curNetwork;
	}
	int getQualityBySsid(LPCSTR name){
		TCHAR buf[40];
		for (int i=0;i<listsize;i++)
		{
			MultiToUnicode(name,65001,buf,40);
			if(!_tcscpy(wlist[i].ucSSID,buf)){
				return wlist[i].ulSingleQuality;
			}
		}
		return 0;
	}
	
private:
	static int compare(WLAN_AVAILABLE_NETWORK& arg1,WLAN_AVAILABLE_NETWORK& arg2){
		return arg1.wlanSignalQuality>arg2.wlanSignalQuality;
	}
	int findConfig(LPTSTR szConnectName){
		CM_CONNECTION_NAME_LIST* pCmConnections;
		DWORD dwError = -1;
		CM_RESULT result = CMRE_INSUFFICIENT_BUFFER;
		DWORD dwSize = sizeof(CM_CONNECTION_NAME_LIST) * DEFAULT_CONNECTION_CONFIG_COUNT;
		UINT i;

		for (i = 0; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
		{
			pCmConnections = (CM_CONNECTION_NAME_LIST*)LocalAlloc(0, dwSize);
			result = CmEnumConnectionsConfigByType(CM_CSP_WIFI_TYPE, pCmConnections, &dwSize);
		}
		if (result != CMRE_SUCCESS)
		{
			dwError = -1;
			printf("[mxwlan]: findConfig failed 0x%X\r\n",GetLastError());
		}else{
			for (i=0;i<pCmConnections->cConnection;i++)
			{
				if (!_tcscmp(szConnectName,pCmConnections->Connection[i].szName))
				{
					dwError=0;
				}
			}
		}
		if (pCmConnections)
		{
			LocalFree(pCmConnections);
		}
		return dwError;
	}
	DWORD updateConfig(LPTSTR szConnectionName,CM_CONNECTION_CONFIG* pConfig,DWORD cbConfig)
	{
		DWORD dwError = ERROR_SUCCESS;
		CM_RESULT result;
		int fAdd=findConfig(szConnectionName);
		if (fAdd)
		{
			result = CmAddConnectionConfig(szConnectionName, pConfig, cbConfig);
		}
		else
		{
			CM_CONFIG_CHANGE_HANDLE pUpdateHandle = NULL;
			CM_CONNECTION_CONFIG* pOldConfig = NULL;
			DWORD cbOldSize = 255;
			UINT i;
			for (i = 0, result = CMRE_INSUFFICIENT_BUFFER; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
			{
				LocalFree(pOldConfig);

				pOldConfig = (CM_CONNECTION_CONFIG*)LocalAlloc(0, cbOldSize);
				if (!pOldConfig)
				{
					dwError = GetLastError();
					BAIL_ON_WIN32_ERROR(dwError);
				}

				result = CmGetToUpdateConnectionConfig(szConnectionName, pOldConfig, &cbOldSize, &pUpdateHandle);
			}

			// We don't care about old config, just free it.
			LocalFree(pOldConfig);

			if (result != CMRE_SUCCESS)
			{
				dwError = GetLastError();
				BAIL_ON_WIN32_ERROR(dwError);
			}

			result = CmUpdateConnectionConfig(pUpdateHandle, CMCO_IMMEDIATE_APPLY, pConfig, cbConfig);
		}

		if (result != CMRE_SUCCESS)
		{
			dwError = GetLastError();
			BAIL_ON_WIN32_ERROR(dwError);
		}
exit:
		return dwError;
	}

	void scanlist(){
		int iRet=0,iRSSI=0;
		DWORD dwResult=0;
		PWLAN_INTERFACE_INFO_LIST pIfList = NULL;  
		PWLAN_INTERFACE_INFO pIfInfo = NULL;  

		PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;  
		PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;  
		WCHAR GuidString[39] = {0}; 
		
		
		dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList); 
		printf("scaning\r\n");
		if (!pIfList)
		{
			return ;
		}
		for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++)   
		{  
			pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i]; 
			g_guid=pIfInfo->InterfaceGuid;
			dwResult = WlanGetAvailableNetworkList(hClient,  
				&pIfInfo->InterfaceGuid,  
				0,   
				NULL,   
				&pBssList);  
			if (dwResult != ERROR_SUCCESS)   
			{  
				printf("WlanGetAvailableNetworkList failed with error: %u\r\n",dwResult);  
				HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
				PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_SCAN_AVALIABLE_NETWORK,EN_WIFI_CONN_RESULT_TIMEOUT);					
				// You can use FormatMessage to find out why the function failed   
			}else{  
				printf("WLAN_AVAILABLE_NETWORK_LIST for this interface\r\n");  
				printf("Num Entries: %d\r\n", pBssList->dwNumberOfItems);
				listsize=0;
				dpz::sort(pBssList->Network,0,pBssList->dwNumberOfItems,compare);
				if (pBssList->dwNumberOfItems>30)
				{
					pBssList->dwNumberOfItems=30;
				}
				for (DWORD j = 0; j < pBssList->dwNumberOfItems; j++)   
				{  
					pBssEntry = (WLAN_AVAILABLE_NETWORK *)&pBssList->Network[j];  
					if(!pBssEntry->dot11Ssid.uSSIDLength || IsExist((LPSTR)(pBssEntry->dot11Ssid.ucSSID))){
						continue;
					}
					/*int iUniTest ;
					iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;
					if (!IsTextUnicode(pBssEntry->dot11Ssid.ucSSID,pBssEntry->dot11Ssid.uSSIDLength,&iUniTest))
					{*/
						MultiToUnicode((const char*)(pBssEntry->dot11Ssid.ucSSID),65001,wlist[listsize].ucSSID,32);
					//}
					GetWiFiConfigFromAvailableNetwork(pBssEntry,&(wlist[listsize].config));
					wlist[listsize].bNetworkConnectable=pBssEntry->bNetworkConnectable;
					wlist[listsize].ulSingleQuality = pBssEntry->wlanSignalQuality;
					wlist[listsize].bSecurityEnabled = pBssEntry->bSecurityEnabled;
					wlist[listsize].uSSIDLength=pBssEntry->dot11Ssid.uSSIDLength;
					wlist[listsize].nIndex=listsize;
					if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED){
						wlist[listsize].bConnected=1;
						curNetwork=wlist[listsize];
						HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI"));
						PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),TRUE,curNetwork.ulSingleQuality); 

					}else{
						wlist[listsize].bConnected=0;
					}
					listsize++;
				}
				HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
				PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_SCAN_AVALIABLE_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);					
			} 
		}
		if (pBssList != NULL)   
		{  
			WlanFreeMemory(pBssList);  
			pBssList = NULL;  
		}  

		if (pIfList != NULL)   
		{  
			WlanFreeMemory(pIfList);  
			pIfList = NULL;  
		} 
	};

	int connect(DWORD flag){
		static WCHAR	ssid[DOT11_SSID_MAX_LENGTH+1];
		static WCHAR   pwd[NWCTL_MAX_WEPK_MATERIAL+1];
		int ret=0;
		ResetEvent(hpost);
		EnterCriticalSection(&cs);
		ret=(!_tcscmp(ssid,ucSSID))&&(!_tcscmp(pwd,KeyMaterial))&&flag;

		_tcscpy_s(ssid,DOT11_SSID_MAX_LENGTH+1,ucSSID);	//add by mmm at2016-07-15 18:48:43 for DOT11_SSID_MAX_LENGTH+1 to resolve the long name question 
		_tcscpy_s(pwd,NWCTL_MAX_WEPK_MATERIAL,KeyMaterial);
		LeaveCriticalSection(&cs);
		if (!ret)
		{
			for (DWORD idx=0;idx<listsize;idx++)
			{
				if (!_tcscmp(wlist[idx].ucSSID,ssid))
				{
					
					wprintf(L"[wifi] start connect %s!!!\r\n",ssid);
					connect(wlist[idx],pwd); 
					break;
				}
			}
			if (!flag)
			{
				PostThreadMessage(g_itrd,WM_USER+3,NULL,1);
			}
		}
		SetEvent(hpost);
		return 0;
	}
	CM_RESULT AcquireConnection(CM_CONNECTION_HANDLE hConnection){
		CM_CONNECTION_CONFIG* pConfigTmp=NULL;
		DWORD cbConfigTmp=0;
		WCHAR szConnectionName[WLAN_MAX_NAME_LENGTH];
		CM_RESULT result=CMRE_SUCCESS;
		MultiToUnicode((const char*)curNetwork.config.Ssid.ucSSID,CP_ACP,szConnectionName,WLAN_MAX_NAME_LENGTH);

		AllocAndGetConnectionConfig(szConnectionName,&pConfigTmp,&cbConfigTmp);
		DeleteAllConnectionConfigs(NULL);
		result = CmAcquireConnection(hConnection);
		if (pConfigTmp)
		{
			CmAddConnectionConfig(szConnectionName,pConfigTmp,cbConfigTmp);
			LocalFree(pConfigTmp);
		}
		return result;
	}
	int connect(tag_NetWork& tag,LPCTSTR password){
		CM_CONNECTION_DETAILS *pDetails = NULL;
		
		CM_CONNECTION_HANDLE hConnection = NULL;
		CM_RESULT result;
		CM_CONNECTION_CONFIG* pConfig = NULL;

		WCHAR szConnectionName[WLAN_MAX_NAME_LENGTH];
		DWORD cbNameSize = WLAN_MAX_NAME_LENGTH;
		DWORD cbConfigSize = 10,dwError;
		BSTR bstrXml=NULL;

		PNW_WLAN_CONFIG pWiFiConfig=&tag.config;
		LPCTSTR ssid=tag.ucSSID;

		dwError = WlanSsidToDisplayName(&pWiFiConfig->Ssid, szConnectionName, &cbNameSize);
		BAIL_ON_WIN32_ERROR(dwError);


		if (tag.bSecurityEnabled)
		{
			_tcscpy_s(pWiFiConfig->KeyMaterial,NWCTL_MAX_WEPK_MATERIAL,password);
			pWiFiConfig->dwCtlFlags |= (NWCTL_BROADCAST_SSID|NWCTL_WEPK_PASSPHRASE);
		}else{
			pWiFiConfig->dwCtlFlags |= (NWCTL_BROADCAST_SSID);
			pWiFiConfig->Privacy=DOT11_CIPHER_ALGO_NONE;
		}

		dwError=SyncProfile(pWiFiConfig, TRUE/*updateXmlDirection*/, &bstrXml);
		BAIL_ON_WIN32_ERROR(dwError);

		//disconnect();
		//disconnect(szConnectionName);
		DeleteAllConnectionConfigs(NULL);

		CreateConnectionConfigFromXml(szConnectionName,bstrXml,&g_guid,&pConfig,&cbConfigSize);

		if (pConfig) {
			
			result = CmAddConnectionConfig(szConnectionName,pConfig,cbConfigSize);
			//updateConfig(szConnectionName,pConfig,cbConfigSize);
			LocalFree(pConfig);
		}

			//Sleep(5000);
		//}
		
		for (result = CmGetFirstCandidateConnection(myhandle, NULL, CMSO_DEFAULT, &hConnection);result==CMRE_SUCCESS;
			result = CmGetNextCandidateConnection(myhandle, &hConnection)) {
				DWORD dwError = GetConnectionDetailsByHandle(hConnection, &pDetails);
				if (dwError==0) {
					if (memcmp(&pDetails->Type, &CM_CSP_WIFI_TYPE, sizeof(CM_CONNECTION_TYPE)) == 0) {
						if (wcscmp(pDetails->szName,szConnectionName) == 0) {
							result = CmAcquireConnection(hConnection);
						}
					}
				}
		}
exit:
		mxwifi<void>::get_inst().notify(2);
		printf("[wifi] connect over!!!\r\n");
		return -1;
	}

	static DWORD wifiproc(LPVOID lp){
		MSG msg;
		int bexit=1;
		DWORD timescan=0,timenow=0/*,timeconnect=0*/;
		/*int bConnect=0;*/
		CoInitialize(NULL);
		DeleteAllConnectionConfigs(NULL);
		while (bexit)
		{
			PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE);
			switch (msg.message)
			{
			case WM_USER+1:
				mxwifi<void>::get_inst().scanlist();
				break;
			case WM_USER+2:
				/*switch (msg.lParam)
				{
				case 0:
					bConnect=1;
					timeconnect=GetTickCount();
					break;
				case 1:
					bConnect=0;
					break;
				case 2:
					if (bConnect==1)
					{
						timenow=GetTickCount();
						printf("%d\r\n",timenow);
						timeconnect=GetTickCount()-25000;
						printf("%d\r\n",timeconnect);
					}
					break;
				}*/
				break;
			case WM_USER+3:
				mxwifi<void>::get_inst().connect(msg.lParam);
				break;
			case WM_QUIT:
				bexit=0;
				break;
			default:
				break;
			}
			timenow=GetTickCount();
			if((timenow-timescan)>10000){
				timescan=timenow;
				xwifi<void>::get_inst().scan();
			}

			msg.message=0;
			Sleep(100);

		}
		CoUninitialize();
		return 0;
	}
	
	int IsExist(LPSTR name){
		/*TCHAR buf[200];
		MultiToUnicode(name,65001,buf,200);*/
		for (DWORD i=0;i<listsize;i++)
		{
			if(!strncmp((const char*)wlist[i].config.Ssid.ucSSID,name,wlist[i].config.Ssid.uSSIDLength)){
				return 1;
			}
		}
		return 0;
	}
	void setCurNetwork(LPCTSTR name){
		for (DWORD i=0;i<listsize;i++)
		{
			if(!_tcscmp(wlist[i].ucSSID,name)){
				wlist[i].bConnected=1;
				curNetwork=wlist[i];
				return ;
			}
		}
	}
	int getStatus(){
		return status;
	}
	static void TimerCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2){
		if (!mxwifi<void>::get_inst().getStatus()/* || _tcscmp(mxwifi<void>::get_inst().ucSSID,mxwifi<void>::get_inst().curNetwork.ucSSID)*/)
		{
			printf("mxwifi<void>::get_inst().getStatus():%d\r\n",mxwifi<void>::get_inst().getStatus());
			HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
			HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI")); 
			PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_PASSWORD_ERROR);					
			PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),FALSE,NULL); 
		}
	}
	void notify(int flag){
		static int cls=0;
		switch(flag){
			case 0:
				status=1;
				cls=0;
				timeKillEvent(g_timer);
				break;
			case 1:
				status=0;
				cls++;
				timeKillEvent(g_timer);
				if (cls<3)
				{
					g_timer=timeSetEvent(10000,1,TimerCallBack,0,TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
				}else{
					TimerCallBack(NULL,NULL,NULL,NULL,NULL);
				}
				break;
			case 2:
				cls=0;
				timeKillEvent(g_timer);
				g_timer=timeSetEvent(10000,1,TimerCallBack,0,TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
				break;
			default:
				status=0;
				break;
		}
		printf("mxwifi<void>::get_inst().getStatus():%d-%d\r\n",flag,mxwifi<void>::get_inst().getStatus());
		
	}
	static void WINAPI NotificationCallBack(PWLAN_NOTIFICATION_DATA pNotifData, PVOID pContent){
		RETAILMSG(1, (_T("[WIFI]NotificationSource:%d NotifDataCode:%d\n"),pNotifData->NotificationSource,pNotifData->NotificationCode));
		PWLAN_CONNECTION_NOTIFICATION_DATA pConectNotifData = NULL;
		switch(pNotifData->NotificationSource)
		{
		case WLAN_NOTIFICATION_SOURCE_NONE:
			break;
		case WLAN_NOTIFICATION_SOURCE_ALL:
		case WLAN_NOTIFICATION_SOURCE_ACM:
			{
				switch(pNotifData->NotificationCode)
				{
				case wlan_notification_acm_connection_start :
					{
						break;
					}
				case  wlan_notification_acm_autoconf_enabled:
					{
						break;
					}
				case wlan_notification_acm_autoconf_disabled:
					{
						break;
					}
				case wlan_notification_acm_background_scan_enabled:
					{
						break;
					}
				case wlan_notification_acm_background_scan_disabled:
					{
						break;
					}
				case wlan_notification_acm_bss_type_change:
					{
						break;
					}
				case wlan_notification_acm_power_setting_change:
					{
						break;
					}
				case wlan_notification_acm_scan_complete:
					{		
						//扫描完成
						break;
					}
				case wlan_notification_acm_scan_fail:					 //8
					{
						//扫描失败
						break;
					}
				case wlan_notification_acm_connection_complete:		 //10
					{
						//连接完成
						pConectNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)pNotifData->pData;

						
						
						
						if (pConectNotifData->wlanReasonCode == WLAN_REASON_CODE_SUCCESS)
						{	
							mxwifi<void>::get_inst().notify(0);
							HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
							HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI")); 

							TCHAR buf[200];
							MultiToUnicode((const char*)(pConectNotifData->dot11Ssid.ucSSID),65001,buf,200);
							mxwifi<void>::get_inst().setCurNetwork(buf);
							PostThreadMessage(mxwifi<void>::get_inst().g_itrd,WM_USER+2,0,1);
							PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);
							PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),TRUE,xwifi<void>::get_inst().getCurNetwork().ulSingleQuality);  
						}
						else
						{
							mxwifi<void>::get_inst().notify(1);
						}
						
						break;
					}
				case wlan_notification_acm_connection_attempt_fail:	 //11
					{
						break;
					}
				case wlan_notification_acm_filter_list_change:	  //12
					{
						break;
					}
				case wlan_notification_acm_interface_arrival:
					{
						break;
					}
				case wlan_notification_acm_interface_removal:
					{
						RETAILMSG(1, (_T("[WIFI]connection manager interface is removed!\n")));
						break;
					}
				case wlan_notification_acm_profile_change:
					{
						RETAILMSG(1, (_T("[WIFI]connection manager profile is changed!\n")));
						break;
					}
				case wlan_notification_acm_profile_name_change:
					{
						break;
					}
				case wlan_notification_acm_profiles_exhausted:
					{
						break;
					}
				case wlan_notification_acm_network_not_available:
					{
						break;
					}
				case wlan_notification_acm_network_available: //19
					{
						
						break;
					}
				case wlan_notification_acm_disconnecting:
					{
						break;
					}
				case wlan_notification_acm_disconnected:
					{
						mxwifi<void>::get_inst().notify(-1);
						pConectNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)pNotifData->pData;
						xwifi<void>::get_inst().getCurNetwork().bConnected=0;
						HWND hwnd_setting = ::FindWindow(NULL,_T("Front_Setting_UI"));
						PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_DISCONN_NETWORK,TRUE);
						Sleep(5);//5ms延迟
						HWND hwnd_main_process = FindWindow(NULL,_T("Front_main_menu_UI"));
						PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),FALSE,NULL);
						break;
					}
				case wlan_notification_acm_adhoc_network_state_change:
					{
						break;
					}
				case wlan_notification_acm_end:
					{
						break;
					}
				default:
					printf("[wifi]unknow code:%d\r\n",pNotifData->NotificationCode);	
				}
				break;
			}
		default:
			printf("[wifi]unknow MSG code:%d\r\n",pNotifData->NotificationCode);	
		}
	}
	void register_wlan(){
		DWORD dwRet,dwPrevNotif;
		if(!(dwRet = WlanRegisterNotification(hClient, WLAN_NOTIFICATION_SOURCE_ACM, NULL, NotificationCallBack, this, NULL, &dwPrevNotif)) == ERROR_SUCCESS)
		{
			printf("[wifi]error!!!\r\n");
		}
	}
	mxwifi(){
		//CoInitialize(NULL, COINIT_MULTITHREADED);
		InitializeCriticalSection(&cs);
		myhandle = CmCreateSession(); 
		hpost=CreateEvent(NULL,FALSE,TRUE,NULL);
		InitProfileHelper();
		DWORD dwMaxClient=2,dwCurVersion=0;
		DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
		register_wlan();
		CloseHandle(CreateThread(NULL,NULL,wifiproc,NULL,NULL,&g_itrd));
		status=0;
	}
	~mxwifi(){
		PostThreadMessage(g_itrd,WM_QUIT,0,0);
		CmCloseSession(myhandle);
	}
private:
	HANDLE hClient;
	GUID g_guid;
	DWORD listsize;
	DWORD g_timer;
	tag_NetWork wlist[30];
	tag_NetWork curNetwork;
	DWORD	g_itrd;
	WCHAR	ucSSID[DOT11_SSID_MAX_LENGTH+1];
	WCHAR   KeyMaterial[NWCTL_MAX_WEPK_MATERIAL+1];
	CRITICAL_SECTION cs;
	HANDLE hpost;
	CM_SESSION_HANDLE myhandle;
	BYTE status;
};

int mxwConnect(LPTSTR ssid,LPTSTR pwd){
	return mxwifi<void>::get_inst().connect(ssid,pwd);
}
int mxwDisconnect(){
	return mxwifi<void>::get_inst().disconnect();
}
int mxwGetListSize(){
	return mxwifi<void>::get_inst().getWlistSize();
}
int mxwScan(){
	mxwifi<void>::get_inst().scan();
	return 0;
}
int mxwGetWnetwork(tag_NetWork& tag,DWORD idx){
	return mxwifi<void>::get_inst().getWnetwork(tag,idx);
}
tag_NetWork& mxwGetCurNetwork(){
	return mxwifi<void>::get_inst().getCurNetwork();
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
