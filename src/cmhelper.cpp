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

cmhelper.cpp

Abstract:  

    This provides the helper functions of Connection Manager.

Functions:

Notes: 

--*/


#include "cmhelper.h"


DWORD CreateConnectionConfigFromXml(
    const WCHAR *szName, const WCHAR *szXml,
    GUID * pInterfaceGuid,
    CM_CONNECTION_CONFIG **ppConfig,
    DWORD *pcbConfig)
{
    DWORD dwError = ERROR_SUCCESS;
    CM_CONNECTION_CONFIG *pConfig = NULL;
    UINT ulConfig = 0;
    DWORD cchXmlString = 0;
    UINT ulXmlBuffer = 0; // Length of the XML in bytes including NULL termination

    if (szName == NULL || szXml == NULL || ppConfig == NULL || pcbConfig == NULL || pInterfaceGuid == NULL)
        dwError = ERROR_INVALID_PARAMETER;
    BAIL_ON_WIN32_ERROR(dwError);

    // Calculate the size needed for CmConfig struct
    cchXmlString = wcslen(szXml);

    ulXmlBuffer = (cchXmlString + 1) * sizeof(WCHAR);
    ulConfig = offsetof(CM_CONNECTION_CONFIG, TypeSpecificInfo[0]) + 
               offsetof(CspWiFiSpecificInfo, DevConfigXml[0]) + 
               ulXmlBuffer;

    pConfig = (CM_CONNECTION_CONFIG*)LocalAlloc(LPTR, ulConfig);
    if (pConfig == NULL)
    {
        dwError = GetLastError();
        goto exit;
    }

    pConfig->Version = CM_CURRENT_VERSION;
    pConfig->fEnabled = TRUE;
    pConfig->ConnectBehavior = CMCB_ALWAYS_CONNECTED;
    pConfig->Type = CM_CSP_WIFI_TYPE;
    pConfig->cbTypeSpecificInfo = 
        offsetof(CspWiFiSpecificInfo, DevConfigXml[0]) + ulXmlBuffer;

    // Fill WiFi specific info.
    CspWiFiSpecificInfo *pCspWiFiInfo = 
        (CspWiFiSpecificInfo*)&pConfig->TypeSpecificInfo[0];

    pCspWiFiInfo->Version = CSP_WIFI_SPECIFIC_INFO_VERSION;
    pCspWiFiInfo->InterfaceSpecific = TRUE;
    memcpy(&pCspWiFiInfo->InterfaceGuid, pInterfaceGuid, sizeof(GUID));

    pCspWiFiInfo->cbUserDataOffset = ulXmlBuffer;
    pCspWiFiInfo->cbDevConfig = ulXmlBuffer;
    memcpy(&pCspWiFiInfo->DevConfigXml[0], szXml, pCspWiFiInfo->cbDevConfig);

    *ppConfig = pConfig;
    *pcbConfig = ulConfig;

exit:
    if (dwError != ERROR_SUCCESS && pConfig != NULL)
        LocalFree(pConfig);

    return dwError;
}

DWORD GetConnectionNameByHandle(CM_CONNECTION_HANDLE hConnection, WCHAR* pBuffer, DWORD size)
{
    DWORD dwError = ERROR_SUCCESS;
    CM_CONNECTION_DETAILS* pDetails = NULL;
    *pBuffer = 0;
    dwError = GetConnectionDetailsByHandle(hConnection, &pDetails);
    BAIL_ON_WIN32_ERROR(dwError);
    StringCchCopy(pBuffer, size, pDetails->szName);

exit:
    LocalFree(pDetails);
    return dwError;
}

DWORD GetConnectionDetailsByHandle(CM_CONNECTION_HANDLE hConnection, CM_CONNECTION_DETAILS** ppDetails)
{
    DWORD dwError = ERROR_SUCCESS;
    CM_RESULT result = CMRE_INSUFFICIENT_BUFFER;
    CM_CONNECTION_DETAILS* pDetails = NULL;
    DWORD dwDetailSize = 255;
    // Get the connection details.
    for (int i = 0; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
    {
        LocalFree(pDetails);
        pDetails = (CM_CONNECTION_DETAILS *)LocalAlloc(0, dwDetailSize);
        if (pDetails == NULL)
        {
            dwError = GetLastError();
            goto exit;
        }

        result = CmGetConnectionDetailsByHandle(hConnection,
            pDetails,
            &dwDetailSize);
    }

    if (NO_ERROR != result)
    {
        dwError = GetLastError();
        goto exit;
    }

    *ppDetails = pDetails;
    pDetails = NULL;

exit:
    LocalFree(pDetails);
    return dwError;
}

DWORD GetConnectionDetailsByName(WCHAR* szConnection, CM_CONNECTION_DETAILS** ppDetails)
{
    DWORD dwError = ERROR_SUCCESS;
    CM_RESULT result = CMRE_INSUFFICIENT_BUFFER;
    CM_CONNECTION_DETAILS* pDetails = NULL;
    DWORD dwDetailSize = 255;
    // Get the connection details.
    for (int i = 0; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
    {
        LocalFree(pDetails);
        pDetails = (CM_CONNECTION_DETAILS *)LocalAlloc(0, dwDetailSize);
        if (pDetails == NULL)
        {
            dwError = GetLastError();
            goto exit;
        }

        result = CmGetConnectionDetailsByName(szConnection,
            pDetails,
            &dwDetailSize);
    }

    if (NO_ERROR != result)
    {
        dwError = GetLastError();
        goto exit;
    }

    *ppDetails = pDetails;
    pDetails = NULL;

exit:
    LocalFree(pDetails);
    return dwError;
}

DWORD AllocAndGetConnectionConfig(
    const WCHAR *szName,
    CM_CONNECTION_CONFIG **ppConfig,
    DWORD *pcbConfig)
{
    CM_RESULT             result = CMRE_INSUFFICIENT_BUFFER;
    CM_CONNECTION_CONFIG* pConfig = NULL;
    DWORD                 cbConfig = sizeof(CM_CONNECTION_CONFIG);

    for (int i = 0; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
    {
        LocalFree(pConfig);
        pConfig = (CM_CONNECTION_CONFIG*)LocalAlloc(LPTR, cbConfig);
        if (NULL == pConfig)
        {
            result = CMRE_OUT_OF_MEMORY;
            break;
        }
        pConfig->Version = CM_CURRENT_VERSION;
        result = CmGetConnectionConfig(szName, pConfig, &cbConfig);
        if (CMRE_INSUFFICIENT_BUFFER != result)
        {
            break;
        }

        ASSERT(cbConfig >= sizeof(CM_CONNECTION_CONFIG));
    }

    if (CMRE_SUCCESS != result)
    {
        LocalFree(pConfig);
        pConfig = NULL;
        cbConfig = 0;
    }

    *ppConfig = pConfig;
    *pcbConfig = cbConfig;

    return (result == CMRE_SUCCESS) ? ERROR_SUCCESS : ERROR_NOT_FOUND;
}

DWORD AllocAndGetXmlFromConnectionConfig(
    const CM_CONNECTION_CONFIG *pConfig,
    LPWSTR *pszXml)
{
    DWORD dwError = ERROR_SUCCESS;
    WCHAR *szXml = NULL;

    if (pConfig == NULL)
        return ERROR_INVALID_PARAMETER;

    CspWiFiSpecificInfo *pCspWiFiInfo = (CspWiFiSpecificInfo*)&pConfig->TypeSpecificInfo[0];

    // Extract the profile XML and make a copy
    // We only get the connection configuration xml, ignore eap user credential xml.
    // cbUserDataOffset should be always the connection config xml length, even if there is no user data in the config.
    // Terminate with NULL.
    //

    // Make sure no overflow, prevent prefast warning.
    if (pCspWiFiInfo->cbUserDataOffset + sizeof(WCHAR) < pCspWiFiInfo->cbUserDataOffset)
    {
        dwError = ERROR_OUTOFMEMORY;
        goto exit;
    }

    szXml = (LPWSTR)LocalAlloc(0, pCspWiFiInfo->cbUserDataOffset + sizeof(WCHAR));
    if (szXml == NULL)
    {
        dwError = GetLastError();
        goto exit;
    }

    // DevConfigXml should be NULL terminated
    memcpy(szXml, &pCspWiFiInfo->DevConfigXml[0], pCspWiFiInfo->cbUserDataOffset);
    // when there is eap user data, the xml is not NULL terminated, appending the NULL.
    szXml[pCspWiFiInfo->cbUserDataOffset/sizeof(WCHAR)] = 0;

    *pszXml = szXml;

exit:
    if (dwError != ERROR_SUCCESS)
    {
        if (szXml != NULL)
            LocalFree(szXml);
    }
        
    return dwError;
}

DWORD DeleteConnection(const WCHAR *szName)
{
    DWORD dwError = ERROR_SUCCESS;
    CM_RESULT CmResult;
    CM_CONNECTION_CONFIG *pConfig = NULL;

    if (szName == NULL)
        return ERROR_INVALID_PARAMETER;

    CmResult = CmDeleteConnectionConfig(szName);
    if (CMRE_SUCCESS != CmResult)
        dwError = ERROR_INVALID_PARAMETER;

    return dwError;
}

int MultiToUnicode(const char* str,unsigned int code,wchar_t* result,unsigned int len){
	unsigned int n = MultiByteToWideChar(code,0,str,-1,0,0);
	n>len?n=len:n;
	MultiByteToWideChar(code,0,str,-1,result,n);
	return n;
}
int UnicodeToMulti(wchar_t* str,unsigned int code,char* result,unsigned int len){
	unsigned int n = WideCharToMultiByte(code,0,str,-1,0,0,0,0);    
	n>len?n=len:n;
	WideCharToMultiByte(code,0,str,-1,result,n,0,0);
	return n;
}

int ssid_2_connect_string(const char* src,LPTSTR dst,int dstlen){
	char buf[64];
	strcpy(buf,src);
	strcat(buf,"\xe5\x88\x98");
	int len=MultiToUnicode((const char*)buf,CP_ACP,dst,strlen(src));
	if (dstlen<len)
	{
		return -1;
	}else{
		if(dst[len-2]==0x9352){
			dst[len-2]=0;
		}else{
			dst[len-3]&=0x00FF;
			dst[len-2]=0;
		}
		return 0;
	}
}