// reg.cpp
//
//////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include <windows.h>
#include <afx.h>
#include "reg.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../prssr/debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
HKEY RegOpenConfigKey() {
	DWORD dwDisposition;
	HKEY hApp;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_PRSSR, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hApp, &dwDisposition) == ERROR_SUCCESS)
		return hApp;
	else
		return NULL;
}

HKEY RegOpenItemsKey() {
	DWORD dwDisposition;
	HKEY hItems;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_PRSSR _T("\\") REG_ITEMS, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hItems, &dwDisposition) == ERROR_SUCCESS)
		return hItems;
	else
		return NULL;
}

HKEY RegOpenSiteListCacheKey() {
	DWORD dwDisposition;
	HKEY hItems;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_PRSSR _T("\\") REG_SITE_LIST_CACHE, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hItems, &dwDisposition) == ERROR_SUCCESS)
		return hItems;
	else
		return NULL;
}
*/

void RegWriteDword(HKEY hKey, LPCTSTR valueName, DWORD value) {
	RegSetValueEx(hKey, valueName, 0, REG_DWORD, (BYTE *) &value, sizeof(value));
}

void RegWriteString(HKEY hKey, LPCTSTR valueName, LPCTSTR value) {
	if (value != NULL)
		RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE *) value, sizeof(TCHAR) * (lstrlen(value) + 1));
	else {
		TCHAR v = 0;
		RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE *) v, sizeof(TCHAR));
	}
}

DWORD RegReadDword(HKEY hKey, LPCTSTR valueName, DWORD defaultValue) {
	DWORD dwType, dwValue, dwSize = sizeof(DWORD);

	if (RegQueryValueEx(hKey, valueName, NULL, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS)
		return dwValue;
	else
		return defaultValue;
}

CString RegReadString(HKEY hKey, LPCTSTR valueName, LPCTSTR defaultValue) {
	DWORD dwType, dwSize;
	CString strValue;

	if (RegQueryValueEx(hKey, valueName, NULL, &dwType, NULL, &dwSize) == ERROR_SUCCESS) {
		LPTSTR lpValue = strValue.GetBufferSetLength((dwSize / sizeof(TCHAR)) - 1);
		if (RegQueryValueEx(hKey, valueName, NULL, &dwType, (BYTE *) lpValue, &dwSize) == ERROR_SUCCESS) {
			return strValue;
		}
		else {
			return CString(defaultValue);
		}
	}
	else {
		return CString(defaultValue);
	}
}

// CRegistry //////////////

CRegistry::CRegistry(HKEY hRoot, const CString &regPath) {
	DWORD dwDisposition;
	RegCreateKeyEx(hRoot, regPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &Key, &dwDisposition);
}

CRegistry::~CRegistry() {
	Close();
}

void CRegistry::DeleteKey(HKEY hRoot, const CString &regPath) {
	RegDeleteKey(hRoot, regPath);
}

void CRegistry::Write(LPCTSTR valueName, DWORD value) {
	RegWriteDword(Key, valueName, value);
}

void CRegistry::Write(LPCTSTR valueName, UINT value) {
	RegWriteDword(Key, valueName, value);
}

void CRegistry::Write(LPCTSTR valueName, BOOL value) {
	RegWriteDword(Key, valueName, value);
}

void CRegistry::Write(LPCTSTR valueName, LPCTSTR value) {
	RegWriteString(Key, valueName, value);
}

void CRegistry::Write(LPCTSTR valueName, void *value, DWORD size) {
	if (value != NULL)
		RegSetValueEx(Key, valueName, 0, REG_BINARY, (BYTE *) value, size);
	else {
		TCHAR v = 0;
		RegSetValueEx(Key, valueName, 0, REG_BINARY, (BYTE *) v, sizeof(TCHAR));
	}
}

DWORD CRegistry::Read(LPCTSTR valueName, DWORD defaultValue) {
	return RegReadDword(Key, valueName, defaultValue);
}

BOOL CRegistry::Read(LPCTSTR valueName, BOOL defaultValue) {
	return RegReadDword(Key, valueName, defaultValue);
}

UINT CRegistry::Read(LPCTSTR valueName, UINT defaultValue) {
	return RegReadDword(Key, valueName, defaultValue);
}

CString CRegistry::Read(LPCTSTR valueName, const CString &defaultValue) {
	return RegReadString(Key, valueName, defaultValue);
}

BOOL CRegistry::Read(LPCTSTR valueName, void *value, DWORD size) {
	DWORD dwType = REG_BINARY, dwSize = size;

	if (RegQueryValueEx(Key, valueName, NULL, &dwType, (BYTE *) value, &dwSize) == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

void CRegistry::DeleteValue(LPCTSTR valueName) {
	RegDeleteValue(Key, valueName);
}

void CRegistry::QuerySubKeyNumber(LPDWORD lpcSubKeys) {
	RegQueryInfoKey(Key, NULL, NULL, NULL, lpcSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

void CRegistry::QueryValueNumber(LPDWORD lpcValues) {
	RegQueryInfoKey(Key, NULL, NULL, NULL, NULL, NULL, NULL, lpcValues, NULL, NULL, NULL, NULL);
}

void CRegistry::Close() {
	if (Key != NULL) {
		RegCloseKey(Key);
		Key = NULL;
	}
}
