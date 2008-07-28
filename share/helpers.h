/**
 *  helpers.h : Miscelanous helping functions
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

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <winsock.h>
#include "file.h"
#include "../prssr/Feed.h"

// class for storing sort info

class CSortInfo {
public:
	enum ESortType {
		Ascending = 1,
		Descending = 2
	};

	enum ESortItem {
		Date = 1,
		Read = 2
	};

	ESortType Type;				// ascending/descending
	ESortItem Item;				// date/read

	CSortInfo() {
		Type = Descending;
		Item = Date;
	}
};


BOOL CreatePath(const CString &fileNamePath);

CString FormatFileSize(DWORD fileSize);

enum EFileType {
	FILE_TYPE_FEED,
	FILE_TYPE_ENCLOSURE,
	FILE_TYPE_FAVICON,
	FILE_TYPE_HTML,
	FILE_TYPE_IMAGE,
	FILE_TYPE_OPML
};

long int MakeLinearDate(SYSTEMTIME &st);
BOOL FormatDateTime(CString &strDateTime, SYSTEMTIME date, BOOL relDates = FALSE);
BOOL FileExists(const CString &strFileName);
BOOL DirectoryExists(const CString &strDirName);
//CString FormatFilePath(EFileType type, const CString &strPath, const CString &fileName);
CString GetCachePath(EFileType type, const CString &strPath);
CString GetCacheFile(EFileType type, const CString &strPath, const CString &url);
SYSTEMTIME TimeToTimeZone(SYSTEMTIME *time);
SYSTEMTIME TimeToGMT(SYSTEMTIME *time, int tzhour, int tzmin);
void SystemTimeToLinearDate(SYSTEMTIME *st, FILETIME *ft);

// cache

class CCache {
public:
	CCache();
	virtual ~CCache();

	BOOL InCache(const CString &item);
	BOOL AddItem(const CString &item);
	void Empty();

protected:
	CMapStringToPtr Items;
	CCriticalSection CS;
};


#if defined PRSSR_APP

CString FormatSysError(DWORD errCode);
BOOL SysError(int resID, DWORD err, BOOL quiet, ...);

BOOL UpdateIntervalElapsed(SYSTEMTIME *now, SYSTEMTIME *t, int intervalIdx);
int UpdateIntervalToMinutes(int updateInterval);

// Cache related functions
class CFileData {
public:
	CFileData(const CString &name, DWORD size) {
		Name = name;
		Size = size;
	}

	CString Name;
	DWORD Size;
};

BOOL RemoveLastPathPart(CString &strPath);
void ScanDirForFiles(const CString &dir, const CString &mask, CList<CFileData *, CFileData *> &list);
void RemoveDirectories(const CString &dir);
void ClearCacheFiles(EFileType type, const CString &cacheLocation, CStringList &deleteList);
void ClearCacheFile(EFileType type, const CString &cacheLocation, const CString &fileName);

BOOL IsHTMLCached(const CString &link, BOOL addHTMLExt);
BOOL IsEnclosureCached(const CString &link);

// menu
BOOL AppendMenuFromResource(CMenu *menu, UINT srcID);
BOOL MergeMenu(CMenu *pMenuDestination, const CMenu *pMenuAdd);

#endif


#ifdef PRSSR_APP
	// column for report ListViews
	class CColumn {
	public:
		CString Heading;
		int Format;
		int Width;

		CColumn(const CString &heading, int format = LVCFMT_LEFT, int width = -1) {
			Heading = heading;
			Format = format;
			Width = width;
		}

		CColumn(UINT nID, int format = LVCFMT_LEFT, int width = -1) {
			Heading.LoadString(nID);
			Format = format;
			Width = width;
		}
	};

#endif

#define countof(a) (sizeof(a) / sizeof(a[0]))

// draw
int DrawTextEndEllipsis(CDC &dc, const CString &strText, CRect &rc, UINT uFormat);

#ifdef PRSSR_APP
CString FormatNumberItemText(const CString &strText, int newCount);
CString GetNumberItemText(CDC *pDC, const CString &strTitle, int newCount, int nWidth);
#endif

// cradling

// get the device IP address (form: 127.0.0.1 is represented by 0x0100007F)
u_long GetDeviceIP();
// returns TRUE if device is cradle, otherwise it returns FALSE
BOOL IsDeviceCradled();

// returns FALSE, if there is no internet connection
BOOL CheckConnection(BOOL autoConnect, BOOL &disconnect);

//
char *WCharToChar(const CString &str, UINT codePage = CP_ACP);
CString CharToWChar(const char *str, UINT codePage);
CString CharToWChar(const char *str);
//CString Utf8ToWChar(const char *str);
char *DuplicateString(const char *s);

#if defined PRSSR_APP
//BOOL WriteFileString(HANDLE hFile, LPCTSTR lpStr, UINT codePage);
//BOOL WriteFileString(HANDLE hFile, LPCTSTR lpStr);

BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr, UINT codePage);
BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr);
BOOL WriteFileString(CBufferedFile &file, const CString &s);
#endif

#ifdef PRSSR_APP
extern UINT DPadActionStr[2];
#endif


#ifdef PRSSR_APP
void OpenUrlExt(const CString &url);
void OpenEnclosure(CEnclosureItem *ei);
void ShellOpenFile(const CString &strTransUrl);

BOOL CopyTextToClipboard(HWND hWnd, const CString &strText);

void SendByEmail(const CString &body);
#endif

BOOL GetFileTime(LPCTSTR fileName, FILETIME *ft);

BOOL InWideMode();

BOOL IsFileChanged(LPCTSTR fileName, FILETIME *ft);

#ifdef PRSSR_APP
void UnregisterHwKeys(HWND hWnd);
void RegisterHwKeys(HWND hWnd);
void DDV_NotEmpty(CDataExchange *pDX, CString &str, UINT nID);
#endif

#endif // _HELPERS_H_
