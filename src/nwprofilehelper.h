//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

nwprofilehelper.h

Abstract:  
    This provides the helper functions of constructing/loading a native wifi profile.

Functions:

Notes: 

--*/
#include "wifiwork.hpp"

#ifdef __cplusplus
extern "C"
{
#endif


void InitProfileHelper();

DWORD
WlanStringToSsid(
    __in LPCWSTR strSsid,
    __out PDOT11_SSID pDot11Ssid
    );

DWORD
WlanSsidToDisplayName(
    __in PDOT11_SSID pDot11Ssid,
    __out_ecount_opt(*pcchDisplayName) LPWSTR strDisplayName,
    __inout DWORD *pcchDisplayName
    );

DWORD SyncProfile(PNW_WLAN_CONFIG pConfig, BOOL fUpdateXmlDirection, BSTR* pstrXml);
DWORD GetWiFiConfigByConnectionName(WCHAR* pConnectionName, PNW_WLAN_CONFIG pWiFiConfig);
void GetWiFiConfigFromAvailableNetwork(PWLAN_AVAILABLE_NETWORK pNetwork, PNW_WLAN_CONFIG pWiFiConfig);

#ifdef __cplusplus
}
#endif
