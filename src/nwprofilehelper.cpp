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

nwprofilehelper.cpp

Abstract:  

    This provides the helper functions of constructing/loading a native wifi profile.

Functions:

Notes: 

--*/


#include <msxml2.h>
#include <hash_map.hxx>
#include <string.hxx>
#include "cmhelper.h"
#include <CmCspWiFi.h>
#include "nwprofilehelper.h"

using namespace ce;

// The naming is consistent with Wlan Profile xml Schema
enum NodeType
{
    Node_WLANProfile = 0,
    Node_name = 1,
    Node_SSIDConfig = 2,
    Node_SSID = 3,
    Node_nonBroadcast = 4,
    Node_connectionType = 5,
    Node_connectionMode = 6,
    Node_MSM = 7,
    Node_security = 8,
    Node_authEncryption = 9,
    Node_authentication = 10,
    Node_encryption = 11,
    Node_useOneX = 12,
    Node_sharedKey = 13,
    Node_keyType = 14,
    Node_protected = 15,
    Node_keyMaterial = 16,
    Node_documentRoot = 17,
    Node_OneX = 18,
    Node_EapHostConfig = 19,
    Node_EapMethod = 20,
    Node_eapCommonType = 21,
    Node_eapCommonAuthorId = 22,
    Node_ConfigBlob = 23,
    Node_EAPConfig = 24,
    Node_keyIndex = 25,
	Node_hex=26,

    Node_Last
};

static ce::hash_map<wstring, NodeType> s_nodeMap;

template<typename Value>
struct NameValuePair
{
    WCHAR szName[255];
    Value value;
};

static NameValuePair<NodeType> s_profileNodePairs[] = 
{
    {L"WLANProfile",    Node_WLANProfile},
    {L"name",           Node_name},
    {L"SSIDConfig",     Node_SSIDConfig},
    {L"SSID",           Node_SSID},
    {L"nonBroadcast",   Node_nonBroadcast},
    {L"connectionType", Node_connectionType},
    {L"connectionMode", Node_connectionMode},
    {L"MSM",            Node_MSM},
    {L"security",       Node_security},
    {L"authEncryption", Node_authEncryption},
    {L"authentication", Node_authentication},
    {L"encryption",     Node_encryption},
    {L"useOneX",        Node_useOneX},
    {L"sharedKey",      Node_sharedKey},
    {L"keyType",        Node_keyType},
    {L"protected",      Node_protected},
    {L"keyMaterial",    Node_keyMaterial},
    {L"#document",      Node_documentRoot},
    {L"OneX",           Node_OneX},
    {L"EapHostConfig",  Node_EapHostConfig},
    {L"EapMethod",      Node_EapMethod},
    {L"ConfigBlob",     Node_ConfigBlob},
    {L"Type",           Node_eapCommonType},
    {L"AuthorId",       Node_eapCommonAuthorId},
    {L"EAPConfig",      Node_EAPConfig},
    {L"keyIndex",       Node_keyIndex},
	{L"hex",			Node_hex}
};

#define NODE_COUNT _countof(s_profileNodePairs)

static NameValuePair<DWORD> s_authAlgorithmPairs[] =
{
    {L"open",           DOT11_AUTH_ALGO_80211_OPEN},
    {L"shared",         DOT11_AUTH_ALGO_80211_SHARED_KEY},
    {L"WPA",            DOT11_AUTH_ALGO_WPA},
    {L"WPAPSK",         DOT11_AUTH_ALGO_WPA_PSK},
    {L"WPA2",           DOT11_AUTH_ALGO_RSNA},
    {L"WPA2PSK",        DOT11_AUTH_ALGO_RSNA_PSK},
};

static NameValuePair<DWORD> s_cipherAlgorithmPairs[] =
{
    {L"none",           DOT11_CIPHER_ALGO_NONE},
    {L"WEP",            DOT11_CIPHER_ALGO_WEP},
    {L"TKIP",           DOT11_CIPHER_ALGO_TKIP},
    {L"AES",            DOT11_CIPHER_ALGO_CCMP},
};

static NameValuePair<DWORD> s_useOneXPairs[] =
{
    {L"true",           TRUE},
    {L"false",          FALSE},
};

static NameValuePair<DWORD> s_connectionTypePairs[] =
{
    {L"ESS",            dot11_BSS_type_infrastructure},
    {L"IBSS",           dot11_BSS_type_independent},
};

static NameValuePair<DWORD> s_keyProtectPairs[] =
{
    {L"true",           NWCTL_WEPK_HEXTEXT},
    {L"false",          0},
};

static NameValuePair<DWORD> s_keyTypePairs[] =
{
    {L"passPhrase",     NWCTL_WEPK_PASSPHRASE},
    {L"networkKey",     0},
};

static NameValuePair<DWORD> s_nonBroadcastPairs[] =
{
    {L"false",          NWCTL_BROADCAST_SSID},
    {L"true",           0},
};

WCHAR g_szProfileTemplate[] = L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\
<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">\
    <name>SampleProfile</name>\
    <SSIDConfig>\
        <SSID>\
			<hex>Samplehex</hex>\
            <name>SampleProfile</name>\
        </SSID>\
        <nonBroadcast>false</nonBroadcast>\
    </SSIDConfig>\
    <connectionType>ESS</connectionType>\
    <connectionMode>manual</connectionMode>\
    <MSM>\
        <security>\
            <authEncryption>\
                <authentication>WPAPSK</authentication>\
                <encryption>TKIP</encryption>\
                <useOneX>false</useOneX>\
            </authEncryption>\
            <sharedKey>\
                <keyType>passPhrase</keyType>\
                <protected>false</protected>\
                <keyMaterial>SampleKey</keyMaterial>\
            </sharedKey>\
            <keyIndex>0</keyIndex> \
            <OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\">\
                <authMode>user</authMode>\
                <EAPConfig>\
                  <EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\" \
                    xmlns:eapCommon=\"http://www.microsoft.com/provisioning/EapCommon\">\
                    <EapMethod>\
                      <eapCommon:Type>13</eapCommon:Type> \
                      <eapCommon:AuthorId>0</eapCommon:AuthorId> \
                    </EapMethod>\
                    <ConfigBlob>0201</ConfigBlob>\
                  </EapHostConfig>\
                </EAPConfig> \
            </OneX> \
         </security>\
    </MSM>\
</WLANProfile>";

#define INVALID_SIZE ((size_t)~0)
// Converts a buffer to formatted octets. 'dst' must point to sufficient
// storage as determined by BufferToBinHexLength. And the hexadecimal
// digits A-F must be uppercase.
void ConvertBufferToBinHex(
        __in_bcount(srcLen) const void* src,
        size_t srcLen,
        __inout_ecount(dstLen) wchar_t* dst,
        size_t dstLen
        ) throw ();

// Returns the number of bytes required to hold the converted string or
// invalidConversion if src isn't a valid bin.hex string.
size_t BinHexToBufferLength(
          const wchar_t* src,
          size_t srcLen
          ) throw ();

// Converts a bin.hex string to a buffer. 'src' must point to a valid bin.hex
// string. 'dst' must point to sufficient storage as determined by
// BinHexToBufferLength.
void ConvertBinHexToBuffer(
        const wchar_t* src,
        size_t srcLen,
        void* dst
        ) throw ();

// Converts a hexadecimal digit to an integer. src must be a valid hexadecimal
// digit.
unsigned char ConvertHexWCharToInteger(wchar_t src) throw ();

// Converts an integer to a hexadecimal digit. src must be less than 0x10.
wchar_t ConvertIntegerToHexWChar(unsigned int src) throw ();

inline size_t BufferToBinHexLength(
                 const void*,
                 size_t length
                 ) throw ()
{
   // Two characters per octet + terminator.
   return (length * 2) + 1;
}


inline bool IsHexDigit(wchar_t src) throw ()
{
   return (src >= L'0' && src <= L'9') ||
          (src >= L'A' && src <= L'F') ||
          (src >= L'a' && src <= L'f');
}

// Converts a buffer to formatted octets. 'dst' must point to sufficient
// storage as determined by BufferToBinHexLength.
void ConvertBufferToBinHex(
        __in_bcount(srcLen) const void* src,
        size_t srcLen,
        __inout_ecount(dstLen) wchar_t* dst,
        size_t dstLen
        ) throw ()
{
   assert((src != 0) || (srcLen == 0));
   assert((dst != 0) && (dstLen != 0));

   const unsigned char* next = static_cast<const unsigned char*>(src);
   const unsigned char* end = next + srcLen;
   wchar_t* dstEnd = dst + dstLen;

   for ( ; (next < end) && (dst < dstEnd - 1); ++next)
   {
      *dst = ConvertIntegerToHexWChar((*next) >> 4);
      ++dst;
      *dst = ConvertIntegerToHexWChar((*next) & 0xF);
      ++dst;
   }

   // Add the null terminator.
   if (dst < dstEnd)
   {
      *dst = L'\0';
   }
   else
   {
      *(dstEnd - 1) = L'\0';
   }
}

size_t BinHexToBufferLength(
          const wchar_t* src,
          size_t srcLen
          ) throw ()
{
   assert((src != 0) || (srcLen == 0));

   const wchar_t* end = src + srcLen;

   for ( ; src < end; ++src)
   {
      if (!IsHexDigit(*src))
      {
         return INVALID_SIZE;
      }
   }

   // Divide by two rounding up in case we have an odd number of nibbles.
   return (srcLen + 1) / 2;
}



void ConvertBinHexToBuffer(
        const wchar_t* src,
        size_t srcLen,
        void* dst
        ) throw ()
{
   assert((src != 0) || (srcLen == 0));
   assert((dst != 0) || (srcLen == 0));
   assert(BinHexToBufferLength(src, srcLen) != INVALID_SIZE);

   unsigned char* nextDst = static_cast<unsigned char*>(dst);

   const wchar_t* nextSrc = src;
   const wchar_t* endSrc = src + srcLen;

   // If the given string is a odd length string, then special case the first
   // nibble.
   if ((srcLen % 2) != 0)
   {
      *nextDst = ConvertHexWCharToInteger(*nextSrc);
      ++nextSrc;
      ++nextDst;
   }

   for ( ; nextSrc < endSrc; ++nextSrc, ++nextDst)
   {
      *nextDst = ConvertHexWCharToInteger(*nextSrc) << 4;
      ++nextSrc;
      *nextDst |= ConvertHexWCharToInteger(*nextSrc);
   }
}


unsigned char ConvertHexWCharToInteger(wchar_t src) throw ()
{
   assert(IsHexDigit(src));

   if ((src >= L'0') && (src <= L'9'))
   {
      return static_cast<unsigned char>(src - L'0');
   }

   if ((src >= L'A') && (src <= L'F'))
   {
      return static_cast<unsigned char>(src - (L'A' - 10));
   }

   return static_cast<unsigned char>(src - (L'a' - 10));
}


wchar_t ConvertIntegerToHexWChar(unsigned int src) throw ()
{
   assert(src < 0x10);

   static const wchar_t conversionMap[] =
   {
      L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
      L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F'
   };

   return conversionMap[src];
}

void InitProfileHelper()
{
    for (int i = 0; i < NODE_COUNT; i++)
    {
        *s_nodeMap[s_profileNodePairs[i].szName] = s_profileNodePairs[i].value;
    }
}


DWORD
WlanStringToSsid(
    __in LPCWSTR strSsid,
    __out PDOT11_SSID pDot11Ssid
    )
{
    DWORD dwError = NO_ERROR;
    BYTE  pbSsid[DOT11_SSID_MAX_LENGTH + 1];
    DWORD dwBytes;
 
    if (strSsid == NULL || pDot11Ssid == NULL || wcslen(strSsid) == 0)
    {
        dwError = ERROR_INVALID_PARAMETER;
        goto exit;
    }

    dwBytes = WideCharToMultiByte(CP_ACP, 0,
        strSsid, -1, (LPSTR)pbSsid, sizeof(pbSsid), NULL, NULL);

    if (dwBytes == 0)
    {
        // Conversion failed.
        dwError = GetLastError();
        
        if (dwError == ERROR_INSUFFICIENT_BUFFER)
        {
            dwError = ERROR_BAD_LENGTH;
        }

        goto exit;
    }
    else if (dwBytes == 1)
    {
        // Zero-length SSID.
        dwError = ERROR_BAD_LENGTH;
        goto exit;
    }
    else
    {
        // Conversion succeeded and length valid.
        pDot11Ssid->uSSIDLength = dwBytes - 1;
        memcpy(pDot11Ssid->ucSSID, pbSsid, pDot11Ssid->uSSIDLength);
    }
    
exit:

    return dwError;
}

DWORD
WlanSsidToDisplayName(
    __in PDOT11_SSID pDot11Ssid,
    __out_ecount_opt(*pcchDisplayName) LPWSTR strDisplayName,
    __inout DWORD *pcchDisplayName
    )
{
    DWORD dwError = NO_ERROR;
    DWORD cchDisplayName;
    WCHAR strSsid[8];
    LPWSTR pszArgs[1] = {strSsid};

    if (pDot11Ssid == NULL || 
        pcchDisplayName == NULL ||
        (strDisplayName == NULL && *pcchDisplayName != 0) ||
        pDot11Ssid->uSSIDLength > DOT11_SSID_MAX_LENGTH)
    {
        dwError = ERROR_INVALID_PARAMETER;
        goto exit;
    }

    if (pDot11Ssid->uSSIDLength == 0)
    {
        cchDisplayName = 0;
    }
    else
    {
        // Convert to get length, not including null.
        cchDisplayName = MultiByteToWideChar(CP_ACP, 0,
            (LPCSTR)pDot11Ssid->ucSSID, pDot11Ssid->uSSIDLength, 
            NULL, 0);
    }

    if (pDot11Ssid->uSSIDLength == 0 || cchDisplayName > 0)
    {
        // Length including null.
        cchDisplayName ++;
        
        // Conversion succeeded.
        if (*pcchDisplayName < cchDisplayName)
        {
            // Insufficient buffer.
            *pcchDisplayName = cchDisplayName;
            dwError = ERROR_INSUFFICIENT_BUFFER;
        }
        else
        {
            // Sufficient buffer.
            if (pDot11Ssid->uSSIDLength > 0)
            {
                // Convert. Leave out space for null.
                cchDisplayName = MultiByteToWideChar(CP_ACP, 0,
                    (LPCSTR)pDot11Ssid->ucSSID, pDot11Ssid->uSSIDLength, 
                    strDisplayName, *pcchDisplayName - 1);

                if (cchDisplayName == 0)
                {                
                    dwError = GetLastError();
                    goto exit;
                }

                cchDisplayName ++;
            }

            // Succeeded.
            strDisplayName[cchDisplayName - 1] = L'\0';
            *pcchDisplayName = cchDisplayName;
        }
    }

exit:

    return dwError;        
}          

DWORD SyncXmlNode(PNW_WLAN_CONFIG pConfig, BOOL fUpdateXmlDirection, IXMLDOMNode *pNode, NodeType parentNodeType, BOOL *pfShouldRemove);

/// <summary>
/// Expand a xml node, sync value for each child.
/// </summary>
/// <param name="pConfig">The PNW_WLAN_CONFIG profile.</param>
/// <param name="fUpdateXmlDirection">The Sync direction.</param>
/// <param name="pNode">The xml node to expand.</param>
/// <returns>ERROR_SUCCESS if the operation is successful. Otherwise error code returned.</returns>
DWORD ExpandNode(PNW_WLAN_CONFIG pConfig, BOOL fUpdateXmlDirection, IXMLDOMNode *pNode, NodeType nodeType)
{
    DWORD dwError = ERROR_SUCCESS;
    IXMLDOMNodeList* pChildren = NULL;
    LONG lCount = 0;
    HRESULT hr = S_OK;
    if (pNode == NULL)
        return ERROR_INVALID_PARAMETER;
    if (FAILED(hr = pNode->get_childNodes(&pChildren)))
    {
        dwError = WIN32_FROM_HRESULT(hr);
        BAIL_ON_WIN32_ERROR(dwError);
    }

    if (FAILED(hr = pChildren->get_length(&lCount)))
    {
        dwError = WIN32_FROM_HRESULT(hr);
        BAIL_ON_WIN32_ERROR(dwError);
    }

    for (int i = 0; i < lCount;)
    {
        IXMLDOMNode *pChild = NULL;
        BOOL fShouldRemove = FALSE;
        if (FAILED(hr = pChildren->get_item(i, &pChild)))
        {
            dwError = WIN32_FROM_HRESULT(hr);
            BAIL_ON_WIN32_ERROR(dwError);
        }

        dwError = SyncXmlNode(pConfig, fUpdateXmlDirection, pChild, nodeType, &fShouldRemove);
        BAIL_ON_WIN32_ERROR(dwError);
        if (fShouldRemove)
        {
            IXMLDOMNode *pRemovedChild = NULL;
            if (SUCCEEDED(hr = pNode->removeChild(pChild, &pRemovedChild)))
            {
                pRemovedChild->Release();
            }

            dwError = WIN32_FROM_HRESULT(hr);
        }

        pChild->Release();
        BAIL_ON_WIN32_ERROR(dwError);

            if (fShouldRemove)
        {
            lCount--;
        }
        else
        {
            i++;
        }
    }

exit:

    if (pChildren)
    {
        pChildren->Release();
    }
    return dwError;
}

DWORD SyncXmlEnumValue(IXMLDOMNode* pNode, NameValuePair<DWORD>* pEnums, DWORD dwCount, DWORD* pdwValue, BOOL fUpdateXmlDirection)
{
    DWORD dwError = ERROR_SUCCESS;
    HRESULT hr = S_OK;
    BSTR bstrInnerText = NULL;
    if (fUpdateXmlDirection)
    {
        for (DWORD i = 0; i < dwCount; i++)
        {
            if (*pdwValue == pEnums[i].value)
            {
                bstrInnerText = SysAllocString(pEnums[i].szName);
                if (bstrInnerText == NULL)
                {
                    dwError = GetLastError();
                    BAIL_ON_WIN32_ERROR(dwError);
                }

                hr = pNode->put_text(bstrInnerText);
                BAIL_ON_HRESULT_ERROR(hr);
                break;
            }
        }
    }
    else
    {
        hr = pNode->get_text(&bstrInnerText);
        BAIL_ON_HRESULT_ERROR(hr);

        for (DWORD i = 0; i < dwCount; i++)
        {
            if (!wcscmp(pEnums[i].szName, bstrInnerText))
            {
                *pdwValue = pEnums[i].value;
                break;
            }
        }
    }
exit:
    SysFreeString(bstrInnerText);
    return dwError;
}

DWORD SyncXmlFlagValue(IXMLDOMNode* pNode, NameValuePair<DWORD>* pEnums, DWORD* pdwValue, BOOL fUpdateXmlDirection)
{
    DWORD dwError = ERROR_SUCCESS;
    HRESULT hr = S_OK;
    BSTR bstrInnerText = NULL;
    DWORD dwMask = pEnums[0].value;
    if (fUpdateXmlDirection)
    {
        bstrInnerText = (*pdwValue & dwMask) ? SysAllocString(pEnums[0].szName) : SysAllocString(pEnums[1].szName);
        if (bstrInnerText == NULL)
        {
            dwError = GetLastError();
            BAIL_ON_WIN32_ERROR(dwError);
        }
        hr = pNode->put_text(bstrInnerText);
        BAIL_ON_HRESULT_ERROR(hr);
    }
    else
    {
        hr = pNode->get_text(&bstrInnerText);
        BAIL_ON_HRESULT_ERROR(hr);

        if (!wcscmp(pEnums[0].szName, bstrInnerText))
        {
            *pdwValue |= dwMask;
        }
        else
        {
            *pdwValue &= ~dwMask;
        }
    }
exit:
    SysFreeString(bstrInnerText);
    return dwError;
}

/// <summary>
/// Sync a xml node value with the corresponding field of PNW_WLAN_CONFIG.
/// </summary>
/// <param name="pConfig">The PNW_WLAN_CONFIG profile.</param>
/// <param name="fUpdateXmlDirection">The Sync direction.</param>
/// <param name="pNode">The xml node to sync.</param>
/// <param name="pfShouldRemove">The BOOL indicates whether the node should be removed from tree.</param>
/// <returns>ERROR_SUCCESS if the operation is successful. Otherwise error code returned.</returns>
DWORD SyncXmlNode(PNW_WLAN_CONFIG pConfig, BOOL fUpdateXmlDirection, IXMLDOMNode *pNode, NodeType parentNodeType, BOOL *pfShouldRemove = NULL)
{
    DWORD dwError = ERROR_SUCCESS;
    HRESULT hr = S_OK;
    NodeType nodeType;
    BSTR bstrName = NULL;
    BSTR bstrInnerText = NULL;
    ce::hash_map<wstring, NodeType>::iterator it;
    if (pNode == NULL)
        return ERROR_INVALID_PARAMETER;

    if (pfShouldRemove)
        *pfShouldRemove = FALSE;

    pNode->get_nodeName(&bstrName);
    wstring nodeName(bstrName);
    size_t pos = nodeName.find(L":");
    if (pos != wstring::npos)
    {
        nodeName.erase(0, pos + 1);
        nodeName.trim(L" ");
    }

    it = s_nodeMap.find(nodeName);
    if (it == s_nodeMap.end())
    {
        goto exit;
    }

    nodeType = it->second;
    switch(nodeType)
    {
    case Node_WLANProfile:
    case Node_MSM:
    case Node_authEncryption:
    case Node_SSIDConfig:
    case Node_SSID:
    case Node_security:
    case Node_documentRoot:
    case Node_EapMethod:
    case Node_EAPConfig:
    case Node_EapHostConfig:
        dwError = ExpandNode(pConfig, fUpdateXmlDirection, pNode, nodeType);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_connectionMode:
        // Do nothing since we use default value.
        break;
    case Node_sharedKey:
        // Cut this node
        if (fUpdateXmlDirection &&
            (((pConfig->AuthenticationMode == DOT11_AUTH_ALGO_80211_OPEN || pConfig->AuthenticationMode == DOT11_AUTH_ALGO_80211_SHARED_KEY) && 
             (pConfig->Privacy != DOT11_CIPHER_ALGO_WEP || (pConfig->dwCtlFlags & NWCTL_WEPK_PRESENT) == 0)) ||
            pConfig->AuthenticationMode == DOT11_AUTH_ALGO_WPA ||
            pConfig->AuthenticationMode == DOT11_AUTH_ALGO_RSNA))
        {
            if (pfShouldRemove)
            {
                *pfShouldRemove = TRUE;
            }
            else
            {
                dwError = ERROR_INVALID_PARAMETER;
                BAIL_ON_WIN32_ERROR(dwError);
            }

            break;
        }

        dwError = ExpandNode(pConfig, fUpdateXmlDirection, pNode, nodeType);
        BAIL_ON_WIN32_ERROR(dwError);
        break;

    case Node_OneX:
        // Cut this node.
        if (fUpdateXmlDirection && !pConfig->EapHostParams.bEnable8021x)
        {
            if (pfShouldRemove)
            {
                *pfShouldRemove = TRUE;
            }
            else
            {
                dwError = ERROR_INVALID_PARAMETER;
                BAIL_ON_WIN32_ERROR(dwError);
            }

            break;
        }

        dwError = ExpandNode(pConfig, fUpdateXmlDirection, pNode, nodeType);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_keyIndex:
        // Cut this node if the keyType is not networkKey but passphrase
        if (fUpdateXmlDirection && (pConfig->dwCtlFlags & NWCTL_WEPK_PASSPHRASE))
        {
            if (pfShouldRemove)
            {
                *pfShouldRemove = TRUE;
            }
            else
            {
                dwError = ERROR_INVALID_PARAMETER;
                BAIL_ON_WIN32_ERROR(dwError);
            }

            break;
        }

        if (fUpdateXmlDirection)
        {
            WCHAR szKeyIndex[2] = {0}; // keyIndex range is 1 to 4, 2 chars are enough
            _itow_s(pConfig->KeyIndex, szKeyIndex, 2/*size*/, 10/*radix*/);
            // Will be freed on return
            bstrInnerText = SysAllocString(szKeyIndex);
            if (!bstrInnerText)
            {
                dwError = GetLastError();
                BAIL_ON_WIN32_ERROR(dwError);
            }
            hr = pNode->put_text(bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);
        }
        else
        {
            hr = pNode->get_text(&bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);

            // The range of the value has already been checked by Native WLan. We don't check this.
            pConfig->KeyIndex = _wtoi(bstrInnerText);
        }
        break;
    case Node_eapCommonType:
        if (fUpdateXmlDirection)
        {
            WCHAR szType[5] = {0}; // 5 is enough for a byte string.
            _itow_s(pConfig->EapHostParams.EapMethodType.eapType.type, szType, 5/*size*/, 10/*radix*/);
            bstrInnerText = SysAllocString(szType);
            if (!bstrInnerText)
            {
                dwError = GetLastError();
                BAIL_ON_WIN32_ERROR(dwError);
            }
            hr = pNode->put_text(bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);
        }
        else
        {
            hr = pNode->get_text(&bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);

            // The range of the value has already been checked by Native WLan. We don't check this.
            pConfig->EapHostParams.EapMethodType.eapType.type = (BYTE)_wtoi(bstrInnerText);
        }
        break;
    case Node_eapCommonAuthorId:
        if (fUpdateXmlDirection)
        {
            WCHAR szType[32] = {0}; // 32 is enough for a DWORD string.
            _itow_s(pConfig->EapHostParams.EapMethodType.dwAuthorId, szType, 32/*size*/, 10/*radix*/);
            bstrInnerText = SysAllocString(szType);
            if (!bstrInnerText)
            {
                dwError = GetLastError();
                BAIL_ON_WIN32_ERROR(dwError);
            }
            hr = pNode->put_text(bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);
        }
        else
        {
            hr = pNode->get_text(&bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);

            // The range of the value has already been checked by Native WLan. We don't check this.
            pConfig->EapHostParams.EapMethodType.dwAuthorId = (DWORD)_wtoi(bstrInnerText);
        }
        break;
    case Node_ConfigBlob:
        if (fUpdateXmlDirection)
        {
            if (pConfig->EapHostParams.pbAuthData && pConfig->EapHostParams.dwAuthDataLen)
            {
                DWORD dwHexLen = (DWORD)BufferToBinHexLength(pConfig->EapHostParams.pbAuthData, pConfig->EapHostParams.dwAuthDataLen);
                bstrInnerText = SysAllocStringLen(L"", dwHexLen + 1);
                if (!bstrInnerText)
                {
                    dwError = GetLastError();
                    BAIL_ON_WIN32_ERROR(dwError);
                }
                ConvertBufferToBinHex(pConfig->EapHostParams.pbAuthData, pConfig->EapHostParams.dwAuthDataLen, bstrInnerText, dwHexLen + 1);
            }
            else
            {
                bstrInnerText = SysAllocString(L"");
                if (!bstrInnerText)
                {
                    dwError = GetLastError();
                    BAIL_ON_WIN32_ERROR(dwError);
                }
            }

            hr = pNode->put_text(bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);
        }
        else
        {
            hr = pNode->get_text(&bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);

            DWORD dwBufferLen = BinHexToBufferLength(bstrInnerText, SysStringLen(bstrInnerText));
            if (dwBufferLen != INVALID_SIZE && dwBufferLen)
            {
                if (pConfig->EapHostParams.pbAuthData)
                {
                    LocalFree(pConfig->EapHostParams.pbAuthData);
                }

                pConfig->EapHostParams.pbAuthData = (BYTE*)LocalAlloc(0, dwBufferLen);
                pConfig->EapHostParams.dwAuthDataLen = dwBufferLen;
                if (pConfig->EapHostParams.pbAuthData == NULL)
                {
                    dwError = GetLastError();
                    BAIL_ON_WIN32_ERROR(dwError);
                }

                ConvertBinHexToBuffer(bstrInnerText, SysStringLen(bstrInnerText), pConfig->EapHostParams.pbAuthData);
            }
            else
            {
                pConfig->EapHostParams.pbAuthData = NULL;
                pConfig->EapHostParams.dwAuthDataLen = 0;
            }
        }
        break;
	case Node_hex:
		if (parentNodeType != Node_SSID)
		{
			break;
		}
		if (fUpdateXmlDirection)
		{
			WCHAR szSsidName[DOT11_SSID_MAX_LENGTH*2+2];
			/*char szSid2[DOT11_SSID_MAX_LENGTH+1];
			for(int i=0;i<pConfig->Ssid.uSSIDLength;i++){
				szSid2[i]=ConvertIntegerToHexWChar(pConfig->Ssid.ucSSID[i]);
			}*/
			ConvertBufferToBinHex(pConfig->Ssid.ucSSID,pConfig->Ssid.uSSIDLength,szSsidName,DOT11_SSID_MAX_LENGTH*2);
			bstrInnerText = SysAllocString(szSsidName);
			if (!bstrInnerText)
			{
				dwError = GetLastError();
				BAIL_ON_WIN32_ERROR(dwError);
			}
			hr = pNode->put_text(bstrInnerText);
			BAIL_ON_HRESULT_ERROR(hr);
		}
		break;
	case Node_name:
        {
            // We only sync the SSID name, the profile name will be auto generated by Connection Manager. We don't care about it. It's not equal to SSID name now.
            // Hence skip profile name.
            if (parentNodeType != Node_SSID)
            {
                break;
            }

            if (fUpdateXmlDirection)
            {
                DWORD cchSize = DOT11_SSID_MAX_LENGTH+1;
                WCHAR szSsidName[DOT11_SSID_MAX_LENGTH+1];
                dwError = WlanSsidToDisplayName(&pConfig->Ssid, szSsidName, &cchSize);
				BAIL_ON_WIN32_ERROR(dwError);
                bstrInnerText = SysAllocString(szSsidName);
                if (!bstrInnerText)
                {
                    dwError = GetLastError();
                    BAIL_ON_WIN32_ERROR(dwError);
                }
                hr = pNode->put_text(bstrInnerText);
                BAIL_ON_HRESULT_ERROR(hr);
            }
            else
            {
                hr = pNode->get_text(&bstrInnerText);
                BAIL_ON_HRESULT_ERROR(hr);
                dwError = WlanStringToSsid(bstrInnerText, &pConfig->Ssid);
                BAIL_ON_WIN32_ERROR(dwError);
            }

            break;
        }
    case Node_connectionType:
        dwError = SyncXmlEnumValue(pNode, s_connectionTypePairs, _countof(s_connectionTypePairs), (DWORD*)&pConfig->InfrastructureMode, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_nonBroadcast:
        dwError = SyncXmlFlagValue(pNode, s_nonBroadcastPairs, &pConfig->dwCtlFlags, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_authentication:
        dwError = SyncXmlEnumValue(pNode, s_authAlgorithmPairs, _countof(s_authAlgorithmPairs), (DWORD*)&pConfig->AuthenticationMode, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_encryption:
        dwError = SyncXmlEnumValue(pNode, s_cipherAlgorithmPairs, _countof(s_cipherAlgorithmPairs), (DWORD*)&pConfig->Privacy, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_useOneX:
        dwError = SyncXmlEnumValue(pNode, s_useOneXPairs, _countof(s_useOneXPairs), (DWORD*)&pConfig->EapHostParams.bEnable8021x, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_keyMaterial:
        if (fUpdateXmlDirection)
        {
            bstrInnerText = SysAllocString(pConfig->KeyMaterial);
            if (!bstrInnerText)
            {
                dwError = GetLastError();
                BAIL_ON_WIN32_ERROR(dwError);
            }
            hr = pNode->put_text(bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);
        }
        else
        {
            hr = pNode->get_text(&bstrInnerText);
            BAIL_ON_HRESULT_ERROR(hr);

            StringCchCopy(pConfig->KeyMaterial, NWCTL_MAX_WEPK_MATERIAL + 1, bstrInnerText);
        }
        break;
    case Node_protected:
        dwError = SyncXmlFlagValue(pNode, s_keyProtectPairs, &pConfig->dwCtlFlags, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    case Node_keyType:
        dwError = SyncXmlFlagValue(pNode, s_keyTypePairs, &pConfig->dwCtlFlags, fUpdateXmlDirection);
        BAIL_ON_WIN32_ERROR(dwError);
        break;
    }

exit:
    SysFreeString(bstrName);
    SysFreeString(bstrInnerText);
    return dwError;
}

/// <summary>
/// Sync the xml profile with the PNW_WLAN_CONFIG profile.
/// </summary>
/// <param name="pConfig">The PNW_WLAN_CONFIG profile.</param>
/// <param name="fUpdateXmlDirection">The Sync direction.</param>
/// <param name="pstrXml">The xml profile.</param>
/// <returns>ERROR_SUCCESS if the operation is successful. Otherwise error code returned.</returns>
DWORD SyncProfile(PNW_WLAN_CONFIG pConfig, BOOL fUpdateXmlDirection, BSTR* pstrXml)
{
    DWORD dwError = ERROR_SUCCESS;
    BSTR bstrXml = NULL;
    IUnknown *pUnk = NULL;
    IXMLDOMDocument* pDOMDoc = NULL;
    VARIANT_BOOL fSuccess = 0;
    HRESULT hr = S_OK;
    if (!pConfig || !pstrXml)
        return ERROR_INVALID_PARAMETER;

    // If we are syncing from xml to pConfig, zero the output memory first.
    if (!fUpdateXmlDirection)
    {
        memset(pConfig, 0, sizeof(NW_WLAN_CONFIG));
    }

    bstrXml = fUpdateXmlDirection ? SysAllocString(g_szProfileTemplate) : SysAllocString(*pstrXml);
    if (!bstrXml)
    {
        dwError = GetLastError();
        BAIL_ON_WIN32_ERROR(dwError);
    }

    hr = CoCreateInstance(__uuidof(DOMDocument), NULL, CLSCTX_INPROC_SERVER, 
                          __uuidof(IUnknown), (LPVOID*)&pUnk );
    BAIL_ON_HRESULT_ERROR(hr);

    hr = pUnk->QueryInterface(__uuidof(IXMLDOMDocument), (LPVOID*)&pDOMDoc);
    BAIL_ON_HRESULT_ERROR(hr);

    hr = pDOMDoc->loadXML(bstrXml, &fSuccess);
    BAIL_ON_HRESULT_ERROR(hr);
    if (!fSuccess)
    {
        dwError = ERROR_INVALID_OPERATION;
        BAIL_ON_WIN32_ERROR(dwError);
    }

    dwError = SyncXmlNode(pConfig, fUpdateXmlDirection, pDOMDoc, Node_Last);
    BAIL_ON_WIN32_ERROR(dwError);

    if (fUpdateXmlDirection)
    {
        hr = pDOMDoc->get_xml(pstrXml);
        BAIL_ON_HRESULT_ERROR(hr);
    }

exit:
    SysFreeString(bstrXml);
    if (pDOMDoc)
        pDOMDoc->Release();
    return dwError;
}


DWORD GetWiFiConfigByConnectionName(WCHAR* pConnectionName, PNW_WLAN_CONFIG pWiFiConfig)
{
	DWORD dwError = ERROR_SUCCESS;
	DWORD cbSize = 255;
	CM_CONNECTION_CONFIG* pConfig = NULL;
	CM_RESULT result = CMRE_INSUFFICIENT_BUFFER;
	WCHAR* szXml = NULL;
	UINT i;
	for (i = 0; i < MAX_TRY_COUNT_FOR_INSUFFICIENT_BUFFER && result == CMRE_INSUFFICIENT_BUFFER; i++)
	{
		LocalFree(pConfig);

		pConfig = (CM_CONNECTION_CONFIG*)LocalAlloc(0, cbSize);
		if (!pConfig)
		{
			dwError = GetLastError();
			BAIL_ON_WIN32_ERROR(dwError);
		}

		result = CmGetConnectionConfig(pConnectionName, pConfig, &cbSize);
	}

	if (result != CMRE_SUCCESS)
	{
		dwError = GetLastError();
		BAIL_ON_WIN32_ERROR(dwError);
	}

	dwError = AllocAndGetXmlFromConnectionConfig(pConfig, &szXml);
	BAIL_ON_WIN32_ERROR(dwError);

	dwError = SyncProfile(pWiFiConfig, FALSE/*updateXmlDirection*/, &szXml);
	BAIL_ON_WIN32_ERROR(dwError);

exit:
	LocalFree(pConfig);
	LocalFree(szXml);
	return dwError;
}

void GetWiFiConfigFromAvailableNetwork(PWLAN_AVAILABLE_NETWORK pNetwork, PNW_WLAN_CONFIG pWiFiConfig)
{
	pWiFiConfig->Ssid = pNetwork->dot11Ssid;
	pWiFiConfig->AuthenticationMode = pNetwork->dot11DefaultAuthAlgorithm;
	pWiFiConfig->InfrastructureMode = pNetwork->dot11BssType;
	pWiFiConfig->Privacy = (DOT11_CIPHER_ALGORITHM)(pWiFiConfig->Privacy | pNetwork->dot11DefaultCipherAlgorithm);
}