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

cmhelper.h

Abstract:
    This provides the helper functions of Connection Manager.

Functions:

Notes: 

--*/

#include <rmnet.h>
#include <windows.h>
#include "string.h"
#include "memory.h"
#include "commctrl.h"
#include <wlanapi.h>
#include <cmnet.h>
#include "cmcspwifi.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER       5
#define WIN32_FROM_HRESULT(hr)           \
	(SUCCEEDED(hr) ? ERROR_SUCCESS :    \
	(HRESULT_FACILITY(hr) == FACILITY_WIN32 ? HRESULT_CODE(hr) : (hr)))
#define BAIL_ON_WIN32_ERROR(dwError) \
	if (dwError != ERROR_SUCCESS) goto exit;

#define BAIL_ON_HRESULT_ERROR(hr) \
	if (FAILED(hr)) \
	{\
	dwError = WIN32_FROM_HRESULT(hr);\
	BAIL_ON_WIN32_ERROR(dwError);\
}


DWORD DeleteConnection(const WCHAR *szName);

DWORD GetConnectionNameByHandle(CM_CONNECTION_HANDLE hConnection, WCHAR* pBuffer, DWORD size);

DWORD GetConnectionDetailsByHandle(CM_CONNECTION_HANDLE hConnection, CM_CONNECTION_DETAILS** ppDetails);
DWORD GetConnectionDetailsByName(WCHAR* szConnection, CM_CONNECTION_DETAILS** ppDetails);

DWORD 
CreateConnectionConfigFromXml(
                              const WCHAR *szName, const WCHAR *szXml,
                              GUID* pInterfaceGuid,
                              CM_CONNECTION_CONFIG **ppConfig,
                              DWORD *pcbConfig);

DWORD 
AllocAndGetConnectionConfig(
                            const WCHAR *szName,
                            CM_CONNECTION_CONFIG **ppConfig,
                            DWORD *pcbConfig);

DWORD 
AllocAndGetXmlFromConnectionConfig(
                                   const CM_CONNECTION_CONFIG *pConfig,
                                   LPWSTR *pszXml);
int MultiToUnicode(const char* str,unsigned int code,wchar_t* result,unsigned int len);
int UnicodeToMulti(wchar_t* str,unsigned int code,char* result,unsigned int len);
int ssid_2_connect_string(const char* src,LPTSTR dst,int dstlen);
#ifdef __cplusplus
}
#endif
