/**
 *  helpers.cpp : Miscelaneous helping functions 
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

#if defined PRSSR_APP
	#include "../prssr/stdafx.h"
	#include "../prssr/prssr.h"
#elif defined  PRSSR_TODAY
	#include "../prssrtoday/stdafx.h"
#endif

#include "helpers.h"
#include "winsock.h"

#include "defs.h"
#include "reg.h"
#include "../prssr/net/HttpConnection.h"
#include "../prssr/www/url.h"

#ifdef PRSSR_APP
	#include "../prssr/config.h"
	#include "../prssr/net/Connection.h"
#elif defined PRSSR_TODAY
	#include "../prssrtoday/Config.h"
#endif

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


#if defined PRSSR_APP

//
// Creates path for file 'fileNamePath'
//
BOOL CreatePath(const CString &fileNamePath) {
	// remove file name
	int slashPos = fileNamePath.ReverseFind('\\');
	CString strPath = fileNamePath.Left(slashPos + 1);

	// create path
	int start = 0;
	int pos = strPath.Find('\\', start);
	while (pos != -1) {
		if (start != -1) {
			CString strDirName = strPath.Left(pos);
			CreateDirectory(strDirName, NULL);
		}

		start = pos + 1;
		pos = strPath.Find('\\', start);
	}

	return TRUE;
}


CString FormatFileSize(DWORD fileSize) {
	CString strSize;
	if (fileSize > 0x40000000)
		strSize.Format(_T("%0.1lf GB"), (double) fileSize / 0x40000000);
	if (fileSize > 0x100000)
		strSize.Format(_T("%0.1lf MB"), (double) fileSize / 0x100000);
	else if (fileSize > 0x400)
		strSize.Format(_T("%0.1lf KB"), (double) fileSize / 0x400);
	else
		strSize.Format(_T("%d B"), fileSize);

	return strSize;
}

#endif

#if defined PRSSR_APP || defined PRSSR_TODAY

// structure to hold date
struct date {
	int month;
	int day;
	int year;
};

// part of # of days calc.
long int funct1 (int y, int m) {
	long int result;
	if ( m <= 2 )
		y -= 1;
	result = y;
	return (result);
}

long int funct2 (int m) {
	long int result;
	if ( m <= 2 )
		result = m + 13;
	else
		result = m + 1;
	return(result);
}

/* Function to calculate the number of days in dates */
long int day_count (int m, int d, int y) {
	long int number;
	number = 1461 * funct1(y, m) / 4 + 153 * funct2(m) / 5 + d;
	return (number);
}

long int MakeLinearDate(SYSTEMTIME &st) {
	return day_count(st.wMonth, st.wDay, st.wYear);
}

int SystemTimeToLong(SYSTEMTIME *today) {
	return (today->wHour * 60) + today->wMinute;
}

BOOL FormatDateTime(CString &strDateTime, SYSTEMTIME date, BOOL relDates/* = FALSE*/) {
	if (date.wYear != 0) {
		int len;
		LPTSTR buffer;

		SYSTEMTIME today;
		GetLocalTime(&today);

		long int diff = MakeLinearDate(today) - MakeLinearDate(date);
		CString strDate;
		if (relDates && diff < 5) {
			if (diff == 0)
				strDate.LoadString(IDS_TODAY);
			else if (diff == 1)
				strDate.LoadString(IDS_YESTERDAY);
			else 
				strDate.Format(IDS_DAYS_AGO, diff);
		}
		else {
			// insert date
			if (Config.DateFormat.IsEmpty()) {
				len = GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, NULL, NULL, 0);
				buffer = strDate.GetBufferSetLength(len);
				GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, NULL, buffer, len);
			}
			else {
				len = GetDateFormat(LOCALE_USER_DEFAULT, 0, &date, Config.DateFormat, NULL, 0);
				buffer = strDate.GetBufferSetLength(len);
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &date, Config.DateFormat, buffer, len);
			}
		}

		CString strTime;
/*		if (relTimes && (diff == 0)) {
			int lDiff = SystemTimeToLong(&today) - SystemTimeToLong(&date);

			int hours = lDiff / 60;
			int minutes = lDiff % 60;

			if (hours == 0) {
				strTime.Format(IDS_MINUTES_AGO, minutes);
			}
			else {
				strTime.Format(IDS_HOURS_AGO, hours);
			}
		}
		else {
*/			// time
			LPCTSTR timeFormat = _T("H:mm");
			len = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &date, timeFormat, NULL, 0);
			buffer = strTime.GetBufferSetLength(len);
			GetTimeFormat(LOCALE_USER_DEFAULT, 0, &date, timeFormat, buffer, len);
//		}

		// put it together
		if (strTime.GetLength() > 0)
			strDateTime.Format(_T("%s, %s"), strDate, strTime);
		else
			strDateTime.Format(_T("%s"), strDate);

		return TRUE;
	}
	else {
		return FALSE;
	}
}

SYSTEMTIME TimeToTimeZone(SYSTEMTIME *time) {
	SYSTEMTIME st = *time;
	FILETIME ft, lFt;
	SystemTimeToFileTime(&st, &ft);
	FileTimeToLocalFileTime(&ft, &lFt);
	FileTimeToSystemTime(&lFt, &st);

	return st;
}

void SystemTimeToLinearDate(SYSTEMTIME *st, FILETIME *ft) {
	DWORD hi = 0;
	hi = (st->wYear - 2000);
	hi = (hi << 4) | (st->wMonth - 1);
	hi = (hi << 5) | (st->wDay - 1);
	hi = (hi << 5) | (st->wHour);
	hi = (hi << 6) | (st->wMinute);
	hi = (hi << 6) | (st->wSecond);

	ft->dwHighDateTime = hi;
	ft->dwLowDateTime = st->wMilliseconds;
}

#endif

SYSTEMTIME TimeToGMT(SYSTEMTIME *time, int tzhour, int tzmin) {
	CTime tm(*time);

	if (tzhour < 0)
		tm -= CTimeSpan(0, abs(tzhour), abs(tzmin), 0);
	else
		tm += CTimeSpan(0, abs(tzhour), abs(tzmin), 0);

	SYSTEMTIME st;
	if (tm.GetAsSystemTime(st))
		return st;
	else
		return *time;
}

BOOL FileExists(const CString &strFileName) {
	LOG1(7, "FileExists(%S)", strFileName);

	HANDLE file = CreateFile(strFileName, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	else {
		CloseHandle(file);
		return TRUE;
	}
}

BOOL DirectoryExists(const CString &strDirName) {
	LOG1(7, "DirectoryExists(%S)", strDirName);

	DWORD attrs = GetFileAttributes(strDirName);
	if (attrs == 0xFFFFFFFF)
		return FALSE;
	else if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return TRUE;
	else
		return FALSE;
}

CString GetCachePath(EFileType type, const CString &strPath) {
	CString path = strPath;
	if (path.Right(1) == _T("\\"))
		path = path.Left(path.GetLength() - 1);
	CString cPath;
	switch (type) {
		case FILE_TYPE_FEED:      cPath.Format(_T("%s\\feeds"), path); break;
		case FILE_TYPE_ENCLOSURE: cPath.Format(_T("%s\\enclosures"), path); break;
		case FILE_TYPE_FAVICON:   cPath.Format(_T("%s\\favicons"), path); break;
		case FILE_TYPE_IMAGE:
		case FILE_TYPE_HTML:      cPath.Format(_T("%s\\html"), path); break;
		case FILE_TYPE_OPML:
		default:                  cPath.Format(_T("%s"), path); break;
	}
	return cPath;
}

/*
CString FormatFilePath(EFileType type, const CString &strPath, const CString &fileName) {
	CString filePath;
	switch (type) {
		case FILE_TYPE_FAVICON:	filePath.Format(_T("%s\\%s.ico"), GetCachePath(type, strPath), fileName); break;
		default:				filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), fileName); break;
	}
	return filePath;
}
*/

CString GetCacheFile(EFileType type, const CString &strPath, const CString &url) {
	CString filePath;
#if defined PRSSR_APP
	switch (type) {
		case FILE_TYPE_FAVICON:	  filePath.Format(_T("%s\\%s.ico"), GetCachePath(type, strPath), url); break;
		case FILE_TYPE_ENCLOSURE: filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, FALSE)); break;
		case FILE_TYPE_HTML:      filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, TRUE)); break;
		case FILE_TYPE_IMAGE:     filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, FALSE)); break;
		default:				  filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), url); break;
	}
#else
	filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), url);
#endif
	return filePath;
}

#if defined PRSSR_APP

CString FormatSysError(DWORD errCode) {
	TCHAR errMsg[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errMsg, 1024, NULL);

	CString strMessage;
	strMessage.Format(_T("(%d) %s"), errCode, errMsg);

	return strMessage;
}

BOOL SysError(int resID, DWORD err, BOOL quiet, ...) {
	if (!quiet) {
		TCHAR msg[1024];
		msg[0] = 0;
		va_list arglist;
		va_start(arglist, quiet);

		CString strFmt;
		strFmt.LoadString(resID);
		vswprintf(msg, strFmt, arglist);
		va_end(arglist);

		CString strMessage;
		if (err != ERROR_SUCCESS)
			strMessage.Format(_T("%s\n\n%s"), msg, FormatSysError(err));
		else
			strMessage.Format(_T("%s"), msg);
		AfxMessageBox(strMessage);
	}

	return FALSE;
}


#define UPDATE_INTERVAL_15MINS					0
#define UPDATE_INTERVAL_30MINS					1
#define UPDATE_INTERVAL_1HOUR					2
#define UPDATE_INTERVAL_2HOURS					3
#define UPDATE_INTERVAL_3HOURS					4
#define UPDATE_INTERVAL_4HOURS					5
#define UPDATE_INTERVAL_8HOURS					6
#define UPDATE_INTERVAL_24HOURS					7
#define UPDATE_INTERVAL_NEVER					8

//
// This function has migration purpose
//
// @param	updateInterval	- update interval index
// @return					- number of minutes that corresponds to update interval
//
int UpdateIntervalToMinutes(int updateInterval) {
	int mins = UPDATE_INTERVAL_GLOBAL;
	switch (updateInterval) {
		case UPDATE_INTERVAL_15MINS:	mins = 15; break;
		case UPDATE_INTERVAL_30MINS:	mins = 30; break;
		case UPDATE_INTERVAL_1HOUR:		mins = 60; break;
		case UPDATE_INTERVAL_2HOURS:	mins = 120; break;
		case UPDATE_INTERVAL_3HOURS:	mins = 180; break;
		case UPDATE_INTERVAL_4HOURS:	mins = 240; break;
		case UPDATE_INTERVAL_8HOURS:	mins = 480; break;
		case UPDATE_INTERVAL_24HOURS:	mins = 1440; break;
		case UPDATE_INTERVAL_NEVER:		mins = UPDATE_INTERVAL_NO_AUTO; break;
	}

	return mins;
}

//
// @param	now			- current time
// @param	t			- last update time of a feed
// @param	interval	- update interval in minutes
// @return				- TRUE if feed update interval elapsed, i.e. feed has to be update
//
BOOL UpdateIntervalElapsed(SYSTEMTIME *now, SYSTEMTIME *t, int interval) {
	CTime tmNow(*now);
	CTime tmT(*t);
	CTimeSpan span;

	if (interval > 0) {
		int days, hours, mins;
		int rem;
		
		days = interval / 1440;
		rem = interval % 1440;
		hours = rem / 60;
		mins = rem % 60;

		span = CTimeSpan(days, hours, mins, 0);

		return (tmT + span) <= tmNow;
	}
	else
		// never update the feed
		return FALSE;
}

//
// Cache related functions
//

void ScanDirForFiles(const CString &dir, const CString &mask, CList<CFileData *, CFileData *> &list) {
	CString path;
	path.Format(_T("%s\\*.*"), dir);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CString fileNamePath;
			fileNamePath.Format(_T("%s\\%s"), dir, CString(fd.cFileName));

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				ScanDirForFiles(fileNamePath, mask, list);
			}
			else {
				list.AddTail(new CFileData(fileNamePath, fd.nFileSizeLow));
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

BOOL RemoveLastPathPart(CString &strPath) {
	int slashPos = strPath.ReverseFind('\\');
	if (slashPos != -1) {
		strPath = strPath.Left(slashPos);
		return TRUE;
	}
	else
		return FALSE;
}

void ClearCacheFiles(EFileType type, const CString &cacheLocation, CStringList &deleteList) {
	LOG0(5, "ClearCacheFiles()");

	CString rootDir = GetCachePath(type, cacheLocation);

	while (!deleteList.IsEmpty()) {
		CString strPath = deleteList.RemoveHead();
		DeleteFile(strPath);
		
		// remove empty dirs, if there are some
		RemoveLastPathPart(strPath);
		while (strPath.GetLength() > 0 && strPath.CompareNoCase(rootDir) != 0) {
			if (RemoveDirectory(strPath))
				RemoveLastPathPart(strPath);
			else
				break;
		}
	}
}

void ClearCacheFile(EFileType type, const CString &cacheLocation, const CString &fileName) {
	CString rootDir = GetCachePath(type, cacheLocation);

	CString strFileName = fileName;
	DeleteFile(strFileName);
	
	// remove empty dirs, if there are some
	RemoveLastPathPart(strFileName);
	while (strFileName.GetLength() > 0 && strFileName.CompareNoCase(rootDir) != 0) {
		if (RemoveDirectory(strFileName))
			RemoveLastPathPart(strFileName);
		else
			break;
	}
}

//
// Removes empty directories traversing from 'rootPath' deep in the structure
// 'rootPath' is not deleted
//
void RemoveDirectories(const CString &rootPath) {
	CString path;
	path.Format(_T("%s\\*.*"), rootPath);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString fileNamePath;
				fileNamePath.Format(_T("%s\\%s"), rootPath, CString(fd.cFileName));
				// remove the content of directory
				RemoveDirectories(fileNamePath);
				// remove the directory itself
				RemoveDirectory(fileNamePath);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}

#endif

CCache::CCache() : Items(100) {
	LOG0(5, "CCache::CCache()");
}

CCache::~CCache() {
	LOG0(5, "CCache::~CCache()");
}

BOOL CCache::InCache(const CString &item) {
	LOG0(5, "CCache::InCache()");

	void *ptr;
	CS.Lock();
	BOOL res = Items.Lookup(item, ptr);
	CS.Unlock();

	return res;
}

BOOL CCache::AddItem(const CString &item) {
	LOG0(5, "CCache::AddItem()");

	if (!InCache(item)) {
		CS.Lock();
		Items.SetAt(item, NULL);
		CS.Unlock();
		return TRUE;
	}
	else
		return FALSE;
}

void CCache::Empty() {
	CS.Lock();
	Items.RemoveAll();
	CS.Unlock();
}

#ifdef PRSSR_APP

// Abstract:
//      Merges two menus.
//
// Parameters:
//      pMenuDestination    - [in, retval] destination menu handle
//      pMenuAdd            - [in] menu to merge
//
// Return value:
//      <false> in case of error.
//
// Comments:
//      This function calles itself recursivley.
BOOL MergeMenu(CMenu *pMenuDestination, const CMenu *pMenuAdd) {
	// get the number menu items in the menus
	int iMenuAddItemCount = pMenuAdd->GetMenuItemCount();
	int iMenuDestItemCount = pMenuDestination->GetMenuItemCount();

	// if there are no items return
	if (iMenuAddItemCount == 0)
		return TRUE;

	// iterate through the top level of 
	for (int iLoop = 0; iLoop < iMenuAddItemCount; iLoop++) {
		// get the menu string from the add menu
		CString sMenuAddString;
		pMenuAdd->GetMenuString(iLoop, sMenuAddString, MF_BYPOSITION);

		// try to get the submenu of the current menu item
		CMenu *pSubMenu = pMenuAdd->GetSubMenu(iLoop);

		// check if we have a sub menu
		if (!pSubMenu) {
			// normal menu item
			// read the source and append at the destination
			UINT nState = pMenuAdd->GetMenuState(iLoop, MF_BYPOSITION);
			UINT nItemID = pMenuAdd->GetMenuItemID(iLoop);

			if (nItemID == 0) {
				pMenuDestination->AppendMenu(MF_SEPARATOR);
				iMenuDestItemCount++;
			}
			else if (pMenuDestination->AppendMenu(nState, nItemID, sMenuAddString)) {
				// menu item added, don't forget to correct the item count
				iMenuDestItemCount++;
			}
			else {
				TRACE0( "MergeMenu: AppendMenu failed!\n" );
				return FALSE;
			}
		}
		else {
			// create or insert a new popup menu item

			// default insert pos is like ap
			int iInsertPosDefault = -1;

			// if the top level search did not find a position append the menu
			if (iInsertPosDefault == -1)
				iInsertPosDefault = pMenuDestination->GetMenuItemCount();

			// create a new popup and insert before <Window> or <Help>
			CMenu newPopupMenu;
			if (!newPopupMenu.CreatePopupMenu()) {
				TRACE0("MergeMenu: CreatePopupMenu failed!\n");
				return FALSE;
			}

			// merge the new popup recursivly
			if (!MergeMenu(&newPopupMenu, pSubMenu))
				return FALSE;

			// insert the new popup menu into the destination menu
			HMENU hNewMenu = newPopupMenu.GetSafeHmenu();

			if (pMenuDestination->InsertMenu(iInsertPosDefault, MF_BYPOSITION | MF_POPUP | MF_ENABLED, (UINT) hNewMenu, sMenuAddString)) {
				// don't forget to correct the item count
				iMenuDestItemCount++;
			}
			else {
				TRACE0("MergeMenu: InsertMenu failed!\n");
				return FALSE;
			}

			// don't destroy the new menu       
			newPopupMenu.Detach();
		} 
	} 

    return TRUE;
}


//
// Loads menu from resource 'srcID' and appends it to the 'menu'
//
BOOL AppendMenuFromResource(CMenu *menu, UINT srcID) {
	LOG0(5, "AppendMenuFromResource(, )");

	CMenu mnuAppend;
	mnuAppend.LoadMenu(srcID);
	MergeMenu(menu, &mnuAppend);
	return TRUE;
}

#endif

#if defined PRSSR_APP

//
//
//
BOOL IsHTMLCached(const CString &link, BOOL addHTMLExt) {
	LOG1(5, "IsHTMLCached('%S')", link);

//	CString strTransUrl = UrlToFileName(link, Config.CacheLocation, TRUE);
//	CString strTransUrl = MakeOfflineUrl(rootPath, UrlToFileName(link, TRUE));
	CString path = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
	CString fileName = UrlToFileName(MakeAbsoluteUrl(link, _T(""), _T("")), addHTMLExt);
	CString strFileName = path + _T("\\") + fileName;

/*	// dont include #part in the file name
	CString strFileName;
	int hashPos = strTransUrl.ReverseFind('#');
	if (hashPos != -1)
		strFileName = strTransUrl.Left(hashPos);
	else
		strFileName = strTransUrl;
*/
	// if exists -> return
	if (FileExists(strFileName))
		return TRUE;
	else
		return FALSE;
}

BOOL IsEnclosureCached(const CString &link) {
	LOG1(5, "IsEnclosureCached('%S')", link);

	CString path = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
	CString fileName = UrlToFileName(MakeAbsoluteUrl(link, _T(""), _T("")), FALSE);
	CString strFileName = path + _T("\\") + fileName;

	// if exists -> return
	if (FileExists(strFileName))
		return TRUE;
	else
		return FALSE;
}

#endif


int DrawTextEndEllipsis(CDC &dc, const CString &strText, CRect &rc, UINT uFormat) {
	LOG1(5, "DrawTextEndEllipsis(, '%S', , )", strText);

	int nWidth = rc.Width();

	CRect rcTemp = rc;
	dc.DrawText(strText, rcTemp, uFormat | DT_CALCRECT);
	if (rcTemp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		CString strTemp;

		for (int i = strText.GetLength(); i >= 0; i--) {
			strTemp.Format(_T("%s..."), strText.Left(i));

			rcTemp = *rc;
			dc.DrawText(strTemp, rcTemp, uFormat | DT_CALCRECT);
			if (rcTemp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
		return dc.DrawText(strTemp, rc, uFormat);
	}

	return dc.DrawText(strText, rc, uFormat);
}

CString FormatNumberItemText(const CString &strText, int newCount) {
	LOG2(5, "FormatNumberItemText('%S', %d)", strText, newCount);

	CString strItemText;

	if (newCount > 0)
		strItemText.Format(_T("%s (%d)"), strText, newCount);
	else
		strItemText.Format(_T("%s"), strText);

	return strItemText;
}

#define NUMBERITEM_FORMAT			DT_VCENTER | DT_SINGLELINE | DT_LEFT

CString GetNumberItemText(CDC *pDC, const CString &strTitle, int newCount, int nWidth) {
	LOG3(5, "GetNumberItemText(%S, %d, %d)", strTitle, newCount, nWidth);

	CString strTemp = FormatNumberItemText(strTitle, newCount);
	CRect rcTmp;
	pDC->DrawText(strTemp, &rcTmp, NUMBERITEM_FORMAT | DT_CALCRECT);
	if (rcTmp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		for (int i = strTitle.GetLength(); i >= 0; i--) {
			strTemp = FormatNumberItemText(strTitle.Left(i) + CString(_T("...")), newCount);
			pDC->DrawText(strTemp, &rcTmp, NUMBERITEM_FORMAT | DT_CALCRECT);
			if (rcTmp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
	}

	return strTemp;
}

#if defined PRSSR_APP

u_long GetDeviceIP() {
	LOG0(5, "GetDeviceIP()");

	char hostName[256] = { 0 };
	if (gethostname(hostName, 255) == SOCKET_ERROR)
		return 0x00000000;

	hostent *lpHostEntry = gethostbyname(hostName);
	if (lpHostEntry == NULL)
		return 0x00000000;

	u_long ipAddr = *((u_long *) lpHostEntry->h_addr_list[0]);
	return ipAddr;
}

BOOL IsDeviceCradled() {
	LOG0(5, "IsDeviceCradled()");

	u_long ipAddr = GetDeviceIP();
	if (ipAddr == 0x00000000)
		return FALSE;		// error

	// compare with "127.0.0.1" (if true -> not cradled, otherwise cradled)
	if (ipAddr == 0x0100007F)
		return FALSE;
	else
		return TRUE;
}


BOOL CheckConnection(BOOL autoConnect, BOOL &disconnect) {
	LOG0(5, "CheckConnection()");

	BOOL ret = TRUE;
	disconnect = FALSE;

	// Automatically connect
	if (autoConnect) {
		// already connected?
		if (Connection.IsAvailable() != S_OK) {
			if (Connection.AttemptConnect() == S_OK) {
				disconnect = TRUE;
			}
			else {
				ret = FALSE;
			}
		}
	}
	else {
		ret = Connection.IsAvailable() == S_OK;
	}

	return ret;
}

#endif

char *WCharToChar(const CString &str, UINT codePage/* = CP_ACP*/) {
	int len = WideCharToMultiByte(codePage, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	char *conv = new char [len + 1];
	WideCharToMultiByte(codePage, 0, str, str.GetLength(), conv, len, NULL, NULL);
	conv[len] = '\0';
	return conv;
}

/*CString Utf8ToWChar(const char *str) {
	int len = strlen(str);
	int wLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	TCHAR *wStr = new TCHAR [wLen];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wStr, wLen);
	wStr[wLen - 1] = '\0';
	CString s;
	s.Format(_T("%s"), wStr);
	delete [] wStr;
	return s;
}
*/

CString CharToWChar(const char *str, UINT codePage) {
	int wLen = MultiByteToWideChar(codePage, 0, str, -1, NULL, 0);
	CString s;
	LPTSTR wStr = s.GetBufferSetLength(wLen - 1);
	MultiByteToWideChar(codePage, 0, str, -1, wStr, wLen - 1);
	return s;
}

CString CharToWChar(const char *str) {
	int len = strlen(str);
	CString s;
	LPTSTR sStr = s.GetBufferSetLength(len);
	for (int i = 0; i < len; i++)
		s.SetAt(i, (TCHAR) str[i]);

	return s;
}

char *DuplicateString(const char *s) {
	int len = strlen(s);
	char *dup = new char [len + 1];
	strncpy(dup, s, len);
	dup[len] = '\0';
	return dup;
}


//////////////////////////////////////////////////////////////////////

#if defined PRSSR_APP

BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr, UINT codePage) {
	LOG2(5, "WriteFileString('%S', %d)", lpStr, codePage);

	int mbLen = WideCharToMultiByte(codePage, 0, lpStr, -1, NULL, 0, NULL, NULL);
	char *mbStr = new char [mbLen];
	WideCharToMultiByte(codePage, 0, lpStr, -1, mbStr, mbLen, NULL, NULL);
	mbStr[mbLen - 1] = '\0';

	BOOL ret = file.Write(mbStr, mbLen - 1);
	delete [] mbStr;

	return ret;
}

BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr) {
	LOG1(5, "WriteFileString('%S')", lpStr);

	int mbLen = lstrlen(lpStr);
	char *mbStr = new char [mbLen + 1];
	for (int i = 0; i < mbLen; i++)
		mbStr[i] = (char) lpStr[i];
	mbStr[mbLen] = '\0';

	BOOL ret = file.Write(mbStr, mbLen);
	delete [] mbStr;

	return ret;
}

BOOL WriteFileString(CBufferedFile &file, const CString &s) {
	int len = s.GetLength();
	char *str = new char[len];

	for (int i = 0; i < len; i++)
		str[i] = (char) s.GetAt(i);
	BOOL ret = file.Write(str, len);

	delete [] str;

	return ret;
}

#endif

//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP

void OpenUrlExt(const CString &url) {
	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = _T("Open");
//	sei.lpFile = _T("\\windows\\iexplore.exe");
//	sei.lpParameters = url;
	sei.lpFile = url;
	sei.nShow = SW_SHOW;
	ShellExecuteEx(&sei);
}

void OpenEnclosure(CEnclosureItem *ei) {
	CString path = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
	CString fileName = path + _T("\\") + UrlToFileName(MakeAbsoluteUrl(ei->URL, _T(""), _T("")), FALSE);

	if (FileExists(fileName)) {
		ShellOpenFile(fileName);
	}
	else {
		OpenUrlExt(ei->URL);
	}
}

void ShellOpenFile(const CString &strTransUrl) {
	LOG0(3, "ShellOpenFile()");

	// get extension
	int nPos = strTransUrl.ReverseFind('.');
	CString strExt = strTransUrl.Mid(nPos);

	HKEY hExt;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, strExt, 0, 0, &hExt) == ERROR_SUCCESS) {
		// read file type
		CString strFileType = RegReadString(hExt, NULL, _T(""));
		if (!strFileType.IsEmpty()) {
			// read open info for the file
			HKEY hFileType;
			if (RegOpenKeyEx(HKEY_CLASSES_ROOT, strFileType, 0, 0, &hFileType) == ERROR_SUCCESS) {
				HKEY hOpenCmd;
				// get open command
				if (RegOpenKeyEx(hFileType, _T("Shell\\Open\\Command"), 0, 0, &hOpenCmd) == ERROR_SUCCESS) {
					CString strOpenCommand = RegReadString(hOpenCmd, NULL, _T(""));
					if (!strOpenCommand.IsEmpty()) {
						// parse command line (split program name and args)
						CString strProgram;
						CString strArgs;

						// find "
						int nSQuote = strOpenCommand.Find('"');
						if (nSQuote == -1) {
							// no quotes -> the space splits prog and args
							int nSpace = strOpenCommand.Find(' ');
							strProgram = strOpenCommand.Left(nSpace);
							strArgs = strOpenCommand.Mid(nSpace + 1);
						}
						else {
							// is there a space before " ?
							if (nSQuote > 1 && strOpenCommand.GetAt(nSQuote - 1) == ' ') {
								// args quoted
								strProgram = strOpenCommand.Left(nSQuote - 1);
								strArgs = strOpenCommand.Mid(nSQuote);
							}
							else {
								// the program name is quoted
								int nEQuote = strOpenCommand.Find('"', nSQuote + 1);
								strProgram = strOpenCommand.Left(nEQuote);
								strArgs = strOpenCommand.Mid(nEQuote + 1);
							}
						}


						strArgs.Replace(_T("%1"), strTransUrl);

						SHELLEXECUTEINFO ei = { 0 };
						ei.cbSize = sizeof(ei);
						ei.fMask = SEE_MASK_NOCLOSEPROCESS;
						ei.lpVerb = _T("Open");
						ei.lpFile = strProgram;
						ei.lpParameters = strArgs;
						ei.nShow = SW_SHOW;
						ShellExecuteEx(&ei);
					}
					
					RegCloseKey(hOpenCmd);
				}
				RegCloseKey(hFileType);
			}
		}
		RegCloseKey(hExt);
	}
}

BOOL CopyTextToClipboard(HWND hWnd, const CString &strText) {
	int ret = TRUE;

	if (hWnd != NULL && ::OpenClipboard(hWnd)) {
		::EmptyClipboard();

		HGLOBAL hClipboardData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(TCHAR) * (strText.GetLength() + 1));
		if (hClipboardData != NULL) {
			TCHAR *pchData = (TCHAR *) GlobalLock(hClipboardData);
			lstrcpy(pchData, (LPCTSTR) strText);
			GlobalUnlock(hClipboardData);
			
			::SetClipboardData(CF_UNICODETEXT, hClipboardData);
		}
		else {
			ret = FALSE;
		}

		::CloseClipboard();
	}
	else {
		// clipboard opening failed
		ret = FALSE;
	}

	return ret;
}

void SendByEmail(const CString &body) {
	CString pars;
	pars.Format(_T("-service \"%s\" -body \"%s\""), Config.EmailService, body);
	CreateProcess(_T("tmail.exe"), pars, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
}

#endif

BOOL GetFileTime(LPCTSTR fileName, FILETIME *ft) {
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		BOOL ret = GetFileTime(hFile, NULL, NULL, ft);
		CloseHandle(hFile);

		return ret;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION: InWideMode()
//
BOOL InWideMode() {
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	return (width > height && height < SCALEX(320)) ? TRUE : FALSE;
}

BOOL IsFileChanged(LPCTSTR fileName, FILETIME *ft) {
	LOG2(5, "IsFileChanged('%S', %p)", fileName, ft);

	if (ft == NULL)
		return FALSE;

	BOOL change = FALSE;

	FILETIME lastWrite;
	if (GetFileTime(fileName, &lastWrite)) {
		if (CompareFileTime(ft, &lastWrite) != 0) {
			*ft = lastWrite;
			change = TRUE;
		}
	}
	else {
		if (ft->dwLowDateTime == 0 && ft->dwHighDateTime == 0)
			change = FALSE;
		else {
			change = TRUE;
			memset(ft, 0, sizeof(FILETIME));
		}
	}

	return change;
}

#ifdef PRSSR_APP

void UnregisterHwKeys(HWND hWnd) {
	for (int i = 0xc1; i <= 0xcf; i++)
		UnregisterHotKey(hWnd, i);
}

void RegisterHwKeys(HWND hWnd) {
	POSITION pos = Config.HwKeysCmd.GetStartPosition();
	while (pos != NULL) {
		int key;
		UINT cmd;
		Config.HwKeysCmd.GetNextAssoc(pos, key, cmd);

		UnregisterHotKey(hWnd, key);
		RegisterHotKey(hWnd, key, MOD_WIN, key);
	}
}


void DDV_NotEmpty(CDataExchange *pDX, CString &str, UINT nID) {
	if (pDX->m_bSaveAndValidate) {
		if (str.IsEmpty()) {
			AfxMessageBox(nID);
			pDX->Fail();
		}
	}
}

#endif