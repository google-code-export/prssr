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

#include "../share/file.h"


enum EFileType {
	FILE_TYPE_FEED,
	FILE_TYPE_ENCLOSURE,
	FILE_TYPE_FAVICON,
	FILE_TYPE_HTML,
	FILE_TYPE_IMAGE,
	FILE_TYPE_OPML
};


BOOL CheckConnection(BOOL autoConnect, BOOL &disconnect);

BOOL IsEnclosureCached(const CString &link);
BOOL IsHTMLCached(const CString &link, BOOL addHTMLExt);

// cache ////

CString GetCachePath(EFileType type, const CString &strPath);
CString GetCacheFile(EFileType type, const CString &strPath, const CString &url);
void ClearCacheFile(EFileType type, const CString &cacheLocation, const CString &fileName);

void DDV_NotEmpty(CDataExchange *pDX, CString &str, UINT nID);

// GUI stuff ////

BOOL AppendMenuFromResource(CMenu *menu, UINT srcID);
BOOL MergeMenu(CMenu *pMenuDestination, const CMenu *pMenuAdd);

// ////

void SendByEmail(const CString &body);
BOOL CopyTextToClipboard(HWND hWnd, const CString &strText);

// hot keys ////

void RegisterHwKeys(HWND hWnd);
void UnregisterHwKeys(HWND hWnd);

// open files ////

void OpenUrlExt(const CString &url);
void OpenEnclosure(const CString &url);
void ShellOpenFile(const CString &strTransUrl);

//
CString FormatFileSize(DWORD fileSize);
CString FormatNumberItemText(const CString &strText, int newCount);
CString GetNumberItemText(CDC *pDC, const CString &strTitle, int newCount, int nWidth);

//

BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr, UINT codePage);
BOOL WriteFileString(CBufferedFile &file, LPCTSTR lpStr);
BOOL WriteFileString(CBufferedFile &file, const CString &s);

//

CString GetSha1Hash(const CString &str);


#endif // _HELPERS_H_
