/**
 *  HtmlCtrl.cpp
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

#include "..\stdafx.h"
#include "..\prssr.h"
#include "HtmlCtrl.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE CHTMLCtrl::HHtmlDLL = 0;

/////////////////////////////////////////////////////////////////////////////
// CHTMLCtrl

CHTMLCtrl::CHTMLCtrl() {
	HWnd = NULL;
}

/*BEGIN_MESSAGE_MAP(CHTMLCtrl, CWnd)
	//{{AFX_MSG_MAP(CHTMLCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

BOOL CHTMLCtrl::Create(DWORD dwStyle, CRect &rc, HWND parent, UINT id) {
	HHtmlDLL = LoadLibrary(_T("htmlview.dll"));

	HWnd = ::CreateWindow(_T("DISPLAYCLASS"), NULL, dwStyle, 
		rc.left, rc.top, rc.Width(), rc.Height(),
		parent, (HMENU) id, HHtmlDLL, NULL);

	return HWnd != NULL;
}

void CHTMLCtrl::Clear() {
	::SendMessage(HWnd, DTM_CLEAR, 0, 0);
}

void CHTMLCtrl::EnableContextMenu(BOOL enable/* = TRUE*/) {
	::SendMessage(HWnd, DTM_ENABLECONTEXTMENU, 0, enable);
}

void CHTMLCtrl::EnableClearType(BOOL enable/* = TRUE*/) {
	::SendMessage(HWnd, DTM_ENABLECLEARTYPE, 0, (LPARAM) enable);
}

void CHTMLCtrl::EnableScripting(BOOL enable/* = TRUE*/) {
	::SendMessage(HWnd, DTM_ENABLESCRIPTING, 0, (LPARAM) enable);
}

void CHTMLCtrl::ZoomLevel(int level) {
	::SendMessage(HWnd, DTM_ZOOMLEVEL, 0, (LPARAM) level);
}

void CHTMLCtrl::AddText(LPWSTR str) {
	::SendMessage(HWnd, DTM_ADDTEXTW, FALSE, (LPARAM) str);
}

void CHTMLCtrl::AddText(const CString &str) {
	AddText((LPWSTR) (LPCTSTR) str);
}

void CHTMLCtrl::EndOfSource() {
	::SendMessage(HWnd, DTM_ENDOFSOURCE, 0, 0);
}

BOOL CHTMLCtrl::IsSelection() {
	return (BOOL) ::SendMessage(HWnd, DTM_ISSELECTION, 0, 0);
}

void CHTMLCtrl::CopySelectionToNewIStream(DWORD *rsd, LPSTREAM *stream) {
	::SendMessage(HWnd, DTM_COPYSELECTIONTONEWISTREAM, (WPARAM) rsd, (LPARAM) stream);
}