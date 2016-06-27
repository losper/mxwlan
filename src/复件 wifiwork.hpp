#pragma once
#include "windows.h"
#include "commctrl.h"
#include "wlanapi.h"
#pragma comment(lib, "wlanapi.lib")
#define MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER       5
#define NWCTL_MAX_WEPK_MATERIAL   1024

#define WM_USER                          0x0400
#define MSGID_MIN                        WM_USER+0xFFF  //ID = 0x13FF

#define	PROID_F_WIFI_LINK_UI              (MSGID_MIN + 0x4300) //0x56FF
#define	WM_WIFI_UI_OPERATOR_RESULT			 PROID_F_WIFI_LINK_UI + 0x0001

enum enActionType
{
	EN_ACTION_SWITCH_ON = 0x001,
	EN_ACTION_SWITCH_OFF = 0x002,
	EN_ACTION_SCAN_AVALIABLE_NETWORK = 0x003,
	EN_ACTION_GET_AVALIABLE_NETWORK = 0x004,
	EN_ACTION_CONN_NETWORK	= 0x005,
	EN_ACTION_DISCONN_NETWORK	= 0x006,
};

enum enWifi_Operation_FeedBack
{
	EN_WIFI_CONN_RESULT_SUCCESS,
	EN_WIFI_CONN_RESULT_PASSWORD_ERROR,
	EN_WIFI_CONN_RESULT_RESOURCE_INVALID,
	EN_WIFI_CONN_RESULT_NAME_ERROR,
	EN_WIFI_CONN_RESULT_TIMEOUT
};
typedef struct
{
	BOOL	bEnable8021x;
	EAP_METHOD_TYPE   EapMethodType;    
	DWORD   dwAuthDataLen;				
	BYTE    *pbAuthData;				   // Pointer to provider specific config blob

}NW_EAPHOST_PARAMS, *PNW_EAPHOST_PARAMS;
typedef struct
{
	ULONG                               Length;             // Length of this structure
	DWORD                               dwCtlFlags;         // control flags (NON-NDIS) see NW_WEPK* constants
	// fields from the NDIS_WLAN_BSSID structure
	DOT11_MAC_ADDRESS                   MacAddress;         // BSSID
	DOT11_SSID                          Ssid;               // SSID
	DOT11_CIPHER_ALGORITHM              Privacy;            // Encryption requirement
	WLAN_SIGNAL_QUALITY                 Rssi;               // receive signal strength in dBm
	DOT11_BSS_TYPE                      InfrastructureMode;
	// fields from NDIS_802_11_WEP structure
	ULONG   KeyIndex;                               // 0 is the per-client key, 1-N are the global keys
	ULONG   KeyLength;                              // length of key in bytes
	WCHAR   KeyMaterial[NWCTL_MAX_WEPK_MATERIAL+1];  // variable length depending on above field
	// aditional field for the Authentication mode
	DOT11_AUTH_ALGORITHM                AuthenticationMode;

	NW_EAPHOST_PARAMS                    EapHostParams;		// 802.1x parameters

} NW_WLAN_CONFIG, *PNW_WLAN_CONFIG;

typedef struct _WLAN_CONFIG_LISTENT {
	struct _WLAN_CONFIG_LISTENT *pPrev, *pNext;
	INT                 nListIndex;           // index of the entry in the list
	DWORD               dwFlags;              // NW_DESCR* flags
	NW_WLAN_CONFIG     nwConfig;            // all NW configuration
} WLAN_CONFIG_LISTENT, *PWLAN_CONFIG_LISTENT;

typedef struct _network 
{
	BOOL bConnected;
	int nIndex;
	ULONG uSSIDLength;
	WCHAR ucSSID[DOT11_SSID_MAX_LENGTH+1];
	BOOL bNetworkConnectable;
	ULONG ulSingleQuality;
	BOOL bSecurityEnabled;
}tag_NetWork,*ptag_Network;


template<typename T>
class xwifi{
public:
	int notify(HWND hwnd){
		//hwnd_=hwnd;
		return 0;
	}
	static xwifi& get_inst(){
		static xwifi inst;
		return inst;
	}
	wchar_t* MultiToUnicode(const char* str,unsigned int code,wchar_t* result,unsigned int len){
		unsigned int n = MultiByteToWideChar(code,0,str,-1,0,0);
		n>len?n=len:n;
		MultiByteToWideChar(code,0,str,-1,result,n);
		return result;
	}
	char* UnicodeToMulti(wchar_t* str,unsigned int code,char* result,unsigned int len){
		unsigned int n = WideCharToMultiByte(code,0,str,-1,0,0,0,0);    
		n>len?n=len:n;
		WideCharToMultiByte(code,0,str,-1,result,n,0,0);
		return result;
	}

	HWND FindChildWindow(HWND hWnd,LPTSTR strchild){
		HWND hp=GetWindow(hWnd,GW_CHILD);
		TCHAR buf[250];
		while (hp)
		{	
			GetWindowText(hp,buf,250);
			if(!_tcscmp(buf,strchild)){
				GetClassName(hp,buf,250);
				return hp;
			}
			hp=GetWindow(hp,GW_HWNDNEXT);
		}
		return NULL;
	}
	void ListView_ItemSelect(HWND hWindow,int idx){
		LV_ITEM lvItem;
		if(hProcess){ 
			if(plvItem){
				lvItem.state = LVIS_SELECTED | LVIS_FOCUSED ;
				lvItem.stateMask = LVIS_SELECTED;
				WriteProcessMemory(hProcess, plvItem, &lvItem, sizeof(lvItem), 0);
				SendMessage(hWindow, LVM_SETITEMSTATE, idx, (LPARAM)plvItem);
			}
		}
	}
	int ListView_ItemFind(HWND hWindow,LPTSTR name){
		DWORD nLVItemCount=0,dwBytes=0;
		DWORD bWriteOK=0;
		LV_ITEM lvItemLocal;
		TCHAR szBuf[255];
		char comp[32];
		UnicodeToMulti(name,65001,comp,32);
		WLAN_CONFIG_LISTENT wlaninfo;

		if((!plvItem) || (!lpWlanInfo) ) //不能在指定进程内分配存储空间
			return -1;

		nLVItemCount=ListView_GetItemCount(hlist);
		for (DWORD i=0;i<nLVItemCount;i++) {
			bWriteOK= WriteProcessMemory(hProcess,lpText,(LPVOID)szBuf,255,(LPDWORD)&dwBytes);
			bWriteOK= WriteProcessMemory(hProcess,lpWlanInfo,(LPVOID)&wlaninfo,sizeof(WLAN_CONFIG_LISTENT),(LPDWORD)&dwBytes);
			if(!bWriteOK) //写内存错误
				return -1;
			lvItemLocal.iItem=i;
			lvItemLocal.iSubItem=0;
			lvItemLocal.mask=LVIF_TEXT | LVIF_PARAM;
			lvItemLocal.pszText=(LPTSTR)lpText;
			lvItemLocal.cchTextMax=255;
			lvItemLocal.lParam=(LPARAM)lpWlanInfo;
			bWriteOK=WriteProcessMemory(hProcess,plvItem,(LPVOID)&lvItemLocal,sizeof(LVITEM),(LPDWORD)&dwBytes);
			if(!bWriteOK) //写内存错误
				return -1;
			//SendMessage(hlist,LVM_GETITEMTEXT,(WPARAM)i,(LPARAM)plvItem);
			SendMessage(hlist,LVM_GETITEM,(WPARAM)0,(LPARAM)plvItem);
			bWriteOK=ReadProcessMemory(hProcess,lpText,&szBuf,255,&dwBytes);
			bWriteOK=ReadProcessMemory(hProcess,plvItem,&lvItemLocal,sizeof(lvItemLocal),&dwBytes);
			dwBytes=ReadProcessMemory(hProcess,(LPCVOID)(lvItemLocal.lParam),&wlaninfo,sizeof(WLAN_CONFIG_LISTENT),&dwBytes);
			//从指定进程存储空间读取文本
			if (dwBytes)
			{
				int ret=strcmp((LPSTR)(wlaninfo.nwConfig.Ssid.ucSSID),comp);
				if (!ret/*!_tcscmp((LPTSTR)(wlaninfo.nwConfig.Ssid.ucSSID),name)*/)
				{
					return i;
				}
			}
			if(!bWriteOK) //不能在指定进程内读取文本
				return -1;
		}
		return -1;
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
				for (DWORD j = 0; j < pBssList->dwNumberOfItems; j++)   
				{  
					pBssEntry = (WLAN_AVAILABLE_NETWORK *)&pBssList->Network[j];  
					if(IsExist((LPSTR)(pBssEntry->dot11Ssid.ucSSID))){
						continue;
					}
					/*int iUniTest ;
					iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;
					if (!IsTextUnicode(pBssEntry->dot11Ssid.ucSSID,pBssEntry->dot11Ssid.uSSIDLength,&iUniTest))
					{*/
						MultiToUnicode((const char*)(pBssEntry->dot11Ssid.ucSSID),65001,wlist[listsize].ucSSID,32);
					//}
					wlist[listsize].bNetworkConnectable=pBssEntry->bNetworkConnectable;
					wlist[listsize].ulSingleQuality = pBssEntry->wlanSignalQuality;
					wlist[listsize].bSecurityEnabled = pBssEntry->bSecurityEnabled;
					wlist[listsize].uSSIDLength=pBssEntry->dot11Ssid.uSSIDLength;
					wlist[listsize].nIndex=listsize;
					if (pBssEntry->dwFlags)   
					{  
						if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED){
							curNetwork=wlist[listsize];
						} 
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
	void scan(){
		PostThreadMessage(g_itrd,WM_USER+1,0,0);
	}
	void scan1(){
		DWORD nLVItemCount=0,dwBytes=0;
		DWORD bWriteOK=0;
		LV_ITEM lvItemLocal;
		TCHAR szBuf[255];
		WLAN_CONFIG_LISTENT wlaninfo;
		if((!plvItem) || (!lpWlanInfo) ) //不能在指定进程内分配存储空间
			return ;
		listsize=0;
		nLVItemCount=ListView_GetItemCount(hlist);
		for (DWORD i=0;i<nLVItemCount;i++) {
			bWriteOK= WriteProcessMemory(hProcess,lpText,(LPVOID)szBuf,255,(LPDWORD)&dwBytes);
			bWriteOK= WriteProcessMemory(hProcess,lpWlanInfo,(LPVOID)&wlaninfo,sizeof(WLAN_CONFIG_LISTENT),(LPDWORD)&dwBytes);
			if(!bWriteOK) //写内存错误
				return ;
			lvItemLocal.iItem=i;
			lvItemLocal.iSubItem=0;
			lvItemLocal.mask=LVIF_TEXT | LVIF_PARAM;
			lvItemLocal.pszText=(LPTSTR)lpText;
			lvItemLocal.cchTextMax=255;
			lvItemLocal.lParam=(LPARAM)lpWlanInfo;
			bWriteOK=WriteProcessMemory(hProcess,plvItem,(LPVOID)&lvItemLocal,sizeof(LVITEM),(LPDWORD)&dwBytes);
			if(!bWriteOK) //写内存错误
				return ;
			//SendMessage(hlist,LVM_GETITEMTEXT,(WPARAM)i,(LPARAM)plvItem);
			SendMessage(hlist,LVM_GETITEM,(WPARAM)0,(LPARAM)plvItem);
			bWriteOK=ReadProcessMemory(hProcess,lpText,&szBuf,255,&dwBytes);
			bWriteOK=ReadProcessMemory(hProcess,plvItem,&lvItemLocal,sizeof(lvItemLocal),&dwBytes);
			dwBytes=ReadProcessMemory(hProcess,(LPCVOID)(lvItemLocal.lParam),&wlaninfo,sizeof(WLAN_CONFIG_LISTENT),&dwBytes);
			//从指定进程存储空间读取文本
			if (dwBytes)
			{
				wlist[listsize].bNetworkConnectable=1;
				MultiToUnicode((const char*)(wlaninfo.nwConfig.Ssid.ucSSID),65001,wlist[listsize].ucSSID,32);
				//_tcscpy(wlist[i].ucSSID,szBuf);
				memcpy((void*)(&(wlist[listsize].confg)),(void*)(&(wlaninfo.nwConfig)),sizeof(WLAN_CONFIG_LISTENT));
				listsize++;
			}
			if(!bWriteOK) //不能在指定进程内读取文本
				return ;
		}
		return ;
	}
	int connect(LPTSTR name,LPTSTR password){
		int idx=ListView_ItemFind(hwifi,name);
		HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
		HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI"));
		if(!_tcscmp(curNetwork.ucSSID,name)){
			PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);					
			PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),TRUE,xwifi<void>::get_inst().getCurNetwork().ulSingleQuality); 
			return 0;
		}
		if (idx<0)
		{
			PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);					
			return -1;
		}
		ListView_ItemSelect(hlist,idx);
		PostMessage(hwifi,WM_COMMAND,6126,0);
		Sleep(100);
		HWND hpass, hwnd;
		hpass= FindWindow(NULL,L"Wireless Network Properties");
		hwnd= GetDlgItem(hpass,1254);
		if (hwnd && hProcess)
		{
			SetWindowText(hwnd,password);
			PostMessage(hpass,WM_COMMAND,1,0);
			PostThreadMessage(g_itrd,WM_USER+2,0,0);
		}
		return 0;
	}
	void disconnect(){
		PostMessage(hwifi,WM_COMMAND,6127,0);
	}
	int getWnetwork(tag_NetWork &tag,int idx){
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
private:
	int IsExist(LPSTR name){
		TCHAR buf[200];
		MultiToUnicode(name,65001,buf,200);
		for (int i=0;i<listsize;i++)
		{
			if(!_tcscmp(wlist[i].ucSSID,buf)){
				return 1;
			}
		}
		return 0;
	}
	static DWORD wifiproc(LPVOID lp){
		MSG msg;
		int bexit=1;
		DWORD timescan=0,timenow=0,timeconnect=0;
		int bConnect=0;
		while (bexit)
		{
			PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE);
			switch (msg.message)
			{
			case WM_USER+1:
				xwifi<void>::get_inst().scanlist();
				break;
			case WM_USER+2:
				switch (msg.lParam)
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
				}
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
			
			if(bConnect && ((timenow-timeconnect)>30000)){
				timeconnect=timenow;
				HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
				HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI"));
				PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_PASSWORD_ERROR);					
				PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),FALSE,NULL);
			}
			msg.message=0;
			Sleep(100);

		}
		return 0;
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
						HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
						PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_SCAN_AVALIABLE_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);					
						//扫描完成
						break;
					}
				case wlan_notification_acm_scan_fail:					 //8
					{
						HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
						PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_SCAN_AVALIABLE_NETWORK,EN_WIFI_CONN_RESULT_TIMEOUT);
						//扫描失败
						break;
					}
				case wlan_notification_acm_connection_complete:		 //10
					{
						//连接完成
						pConectNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)pNotifData->pData;

						HWND hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
						HWND hwnd_main_process = ::FindWindow(NULL,_T("Front_main_menu_UI"));  //add for navione by mmm 2015-12-7 17:24:56

						xwifi<void>::get_inst().setCurNetwork((const char*)(pConectNotifData->dot11Ssid.ucSSID));
						if (pConectNotifData->wlanReasonCode == WLAN_REASON_CODE_SUCCESS)
						{	
							PostThreadMessage(xwifi<void>::get_inst().g_itrd,WM_USER+2,0,1);
							PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_SUCCESS);
							PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),TRUE,xwifi<void>::get_inst().getCurNetwork().ulSingleQuality);  
						}
						else
						{
							PostThreadMessage(xwifi<void>::get_inst().g_itrd,WM_USER+2,0,2);
							/*PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_PASSWORD_ERROR);					
							PostMessage(hwnd_main_process,RegisterWindowMessage(_T("WIFI_STATUS_NOTIFY_NAVI")),FALSE,NULL); */
						}
						
						break;
					}
				case wlan_notification_acm_connection_attempt_fail:	 //11
					{
						HWND	hwnd_setting = FindWindow(NULL,_T("Front_Setting_UI"));
						PostMessage(hwnd_setting,WM_WIFI_UI_OPERATOR_RESULT,EN_ACTION_CONN_NETWORK,EN_WIFI_CONN_RESULT_PASSWORD_ERROR);		
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
						RETAILMSG(1, (_T("[WIFI]scan network not available!\n")));
						break;
					}
				case wlan_notification_acm_network_available: //19
					{
						
						RETAILMSG(1, (_T("[WIFI]scan network available!\n")))	;
						break;
					}
				case wlan_notification_acm_disconnecting:
					{
						break;
					}
				case wlan_notification_acm_disconnected:
					{
						pConectNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)pNotifData->pData;
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
	
	xwifi(){
		listsize=0;
		HWND hwnd=FindWindow(NULL,L"AR6K_SD1");
		hwifi=FindChildWindow(hwnd,L"Wireless Information");
		hlist=FindChildWindow(hwifi,L"List1");
		printf("[!!!!] find window%d,%d,%d\r\n",hwnd,hwifi,hlist);
		GetWindowThreadProcessId(hwifi,&dwProcessID);
		hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
		plvItem = VirtualAllocEx(hProcess, NULL, sizeof(LV_ITEM), MEM_COMMIT, PAGE_READWRITE);
		lpWlanInfo=VirtualAllocEx(hProcess,NULL,sizeof(WLAN_CONFIG_LISTENT),MEM_COMMIT,PAGE_READWRITE);
		lpText=VirtualAllocEx(hProcess,NULL,255,MEM_COMMIT,PAGE_READWRITE);
		CloseHandle(CreateThread(NULL,NULL,wifiproc,NULL,NULL,&g_itrd));
		DWORD dwMaxClient=2,dwCurVersion;
		DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
		register_wlan();
	}
	~xwifi(){
		VirtualFreeEx(hProcess,plvItem,0,MEM_RELEASE);
		VirtualFreeEx(hProcess,lpWlanInfo,0,MEM_RELEASE);
		VirtualFreeEx(hProcess,lpText,0,MEM_RELEASE);
		PostThreadMessage(g_itrd,WM_QUIT,0,0);
	}
	ULONG getQualityBySsid(LPCSTR name){
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
	void setCurNetwork(LPCSTR name){
		TCHAR buf[40];
		for (int i=0;i<listsize;i++)
		{
			MultiToUnicode(name,65001,buf,40);
			if(!_tcscmp(wlist[i].ucSSID,buf)){
				curNetwork=wlist[i];
				return ;
			}
		}
	}
	
private:
	DWORD	dwProcessID;
	HANDLE	hProcess;
	HWND	hwifi;
	HWND	hlist;
	LPVOID	plvItem;
	LPVOID lpWlanInfo;
	LPVOID lpText;
	tag_NetWork wlist[30];
	int listsize;
	DWORD	g_itrd;
	HANDLE hClient;
	tag_NetWork curNetwork;
};
typedef xwifi<void> wifihelper;