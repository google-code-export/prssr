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

#include "StdAfx.h"
#include "prssr.h"

#include "misc.h"
#include "Config.h"

#include "../share/fs.h"
#include "../share/reg.h"
#include "../prssr/sha1/sha1.h"

#include "net/Connection.h"
#include "www/url.h"

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


///////////////////////////////////////////////////////////////////////////////////////////////////

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

// cache related ////

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

CString GetCacheFile(EFileType type, const CString &strPath, const CString &url) {
	CString filePath;
	switch (type) {
		case FILE_TYPE_FAVICON:	  filePath.Format(_T("%s\\%s.ico"), GetCachePath(type, strPath), url); break;
		case FILE_TYPE_ENCLOSURE: filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, FALSE)); break;
		case FILE_TYPE_HTML:      filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, TRUE)); break;
		case FILE_TYPE_IMAGE:     filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), UrlToFileName(url, FALSE)); break;
		default:				  filePath.Format(_T("%s\\%s"), GetCachePath(type, strPath), url); break;
	}
	return filePath;
}

BOOL IsHTMLCached(const CString &link, BOOL addHTMLExt) {
	CString url = link;
	ReplaceHTMLEntities(url);

	CString path = GetCachePath(FILE_TYPE_HTML, Config.CacheLocation);
	CString fileName = UrlToFileName(MakeAbsoluteUrl(url, _T(""), _T("")), addHTMLExt);
	CString strFileName = path + _T("\\") + fileName;

	// if exists -> return
	if (FileExists(strFileName)) return TRUE;
	else return FALSE;
}

BOOL IsEnclosureCached(const CString &link) {
	CString path = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
	CString fileName = UrlToFileName(MakeAbsoluteUrl(link, _T(""), _T("")), FALSE);
	CString strFileName = path + _T("\\") + fileName;

	// if exists -> return
	if (FileExists(strFileName)) return TRUE;
	else return FALSE;
}

void ClearCacheFile(EFileType type, const CString &cacheLocation, const CString &fileName) {
	CString rootDir = GetCachePath(type, cacheLocation);

	CString strFileName = fileName;
	DeleteFile(strFileName);
	RemoveEmptyDirs(strFileName, rootDir);
}

// DDX/DDV stuff ////

void DDV_NotEmpty(CDataExchange *pDX, CString &str, UINT nID) {
	if (pDX->m_bSaveAndValidate) {
		if (str.IsEmpty()) {
			AfxMessageBox(nID, MB_OK | MB_ICONSTOP);
			pDX->Fail();
		}
	}
}

// GUI stuff ////

// Load menu from resource 'srcID' and appends it to the 'menu'
BOOL AppendMenuFromResource(CMenu *menu, UINT srcID) {
	CMenu mnuAppend;
	mnuAppend.LoadMenu(srcID);
	MergeMenu(menu, &mnuAppend);
	return TRUE;
}

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

void SendByEmail(const CString &body) {
	CString pars;
	pars.Format(_T("-service \"%s\" -body \"%s\""), Config.EmailAccount, body);
	CreateProcess(_T("tmail.exe"), pars, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
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

// hot keys ////

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

// Open ////

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

void OpenEnclosure(const CString &url) {
	CString path = GetCachePath(FILE_TYPE_ENCLOSURE, Config.CacheLocation);
	CString fileName = path + _T("\\") + UrlToFileName(MakeAbsoluteUrl(url, _T(""), _T("")), FALSE);

	if (FileExists(fileName)) ShellOpenFile(fileName);
	else OpenUrlExt(url);
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

// ////

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

//

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

// Hash ////

CString GetSha1Hash(const CString &str) {
	SHA1Context sha = { 0 };
	int err;
    uint8_t digest[20];

	err = SHA1Reset(&sha);
	err = SHA1Input(&sha, (LPCTSTR) str, str.GetLength());
	err = SHA1Result(&sha, digest);

	// convert to string
	CString hash;
	LPTSTR lpHash = hash.GetBufferSetLength(40);
	TCHAR hexTab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	for (int i = 0; i < 20; i++) {
		lpHash[2 * i] = hexTab[digest[i] & 0x0F];
		lpHash[(2 * i) + 1] = hexTab[(digest[i] >> 4) & 0x0F];
	}

	return hash;
}
