/**
 *  reg.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _REG_H_
#define _REG_H_

#define REG_KEY_COMPANY				L"Software\\DaProfik"
#define REG_KEY_APP					REG_KEY_COMPANY L"\\pRSSreader"
#define REG_KEY_CONFIGURATION		REG_KEY_APP L"\\Configuration"
#define REG_KEY_APPEARANCE			REG_KEY_APP L"\\Appearance"
#define REG_KEY_UI					REG_KEY_APP L"\\UI"
#define REG_KEY_PROXY_PROFILES		REG_KEY_APP L"\\Proxy Profiles"
#define REG_KEY_SUBSCRIPTIONS		REG_KEY_APP L"\\Subscriptions"
#define REG_KEY_KEYWORDS			REG_KEY_APP L"\\Keywords"
#define REG_KEY_GROUPS				REG_KEY_APP L"\\Groups"
#define REG_KEY_TODAY				REG_KEY_APP L"\\Today"
#define REG_KEY_SOCIAL_BOOKMARKING_SITES	REG_KEY_APP L"\\BookmarkSites"
#define REG_KEY_REWRITE_RULES      REG_KEY_APP L"\\RewriteRules"

#define REG_TODAYROOT				L"Software\\Microsoft\\Today\\Items\\pRSSreader"
//#define REG_SERVICE_ROOT			L"Services\\pRSSreader"
#define REG_NOTIFICATION_ROOT		L"ControlPanel\\Notifications\\{5056DB7C-77D7-4DEE-A7B5-8B9578370A8F}"
//#define REG_ITEMS					L"Items"
//#define REG_TODAY					L"Today"
//#define REG_TEMP					L"Temp"
//#define REG_SITE_LIST_CACHE		L"SiteListCache"

/*
HKEY RegOpenConfigKey();
HKEY RegOpenItemsKey();
HKEY RegOpenSiteListCacheKey();
*/

void RegWriteDword(HKEY hKey, LPCTSTR valueName, DWORD value);
void RegWriteString(HKEY hKey, LPCTSTR valueName, LPCTSTR value);
DWORD RegReadDword(HKEY hKey, LPCTSTR valueName, DWORD defaultValue);
CString RegReadString(HKEY hKey, LPCTSTR valueName, LPCTSTR defaultValue);

// CReg ///////////////

class CRegistry {
public:
	CRegistry(HKEY hRoot, const CString &regPath);
	virtual ~CRegistry();

	static void DeleteKey(HKEY hRoot, const CString &regPath);

	void Write(LPCTSTR valueName, BOOL value);
	void Write(LPCTSTR valueName, DWORD value);
	void Write(LPCTSTR valueName, UINT value);
	void Write(LPCTSTR valueName, LPCTSTR value);
	void Write(LPCTSTR valueName, void *value, DWORD size);
	BOOL Read(LPCTSTR valueName, BOOL defaultValue);
	DWORD Read(LPCTSTR valueName, DWORD defaultValue);
	UINT Read(LPCTSTR valueName, UINT defaultValue);
	CString Read(LPCTSTR valueName, const CString &defaultValue);
	BOOL Read(LPCTSTR valueName, void *value, DWORD size);

	void QuerySubKeyNumber(LPDWORD lpcSubKeys);
	void QueryValueNumber(LPDWORD lpcValues);

	void DeleteValue(LPCTSTR valueName);

	void Close();

	operator HKEY() { return Key; }

protected:
//	CString RegPath;
//	HKEY RootKey;
	HKEY Key;
};


#endif
