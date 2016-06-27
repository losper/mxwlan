#pragma once

#define MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER       5
#define NWCTL_MAX_WEPK_MATERIAL   1024

#define WM_USER                          0x0400
#define MSGID_MIN                        WM_USER+0xFFF  //ID = 0x13FF

#define	PROID_F_WIFI_LINK_UI              (MSGID_MIN + 0x4300) //0x56FF
#define	WM_WIFI_UI_OPERATOR_RESULT			 PROID_F_WIFI_LINK_UI + 0x0001

#define NWCTL_MAX_WEPK_MATERIAL   1024
#define NWCTL_WEPK_PRESENT        0x0001  // specifies whether the configuration includes or not a WEP key
#define NWCTL_WEPK_PASSPHRASE     0x0002  // the Key material is passPhrase type(Not networkKey type).
#define NWCTL_WEPK_HEXTEXT        0x0004  // the Key material is in hex format, stored as text such as a string L"ac72f"
#define NWCTL_ONEX_ENABLED        0x0010  // for this configuration 802.1X should be enabled
#define NWCTL_BROADCAST_SSID      0x4000

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
	NW_WLAN_CONFIG config;
}tag_NetWork,*ptag_Network;
