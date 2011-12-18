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

#include "../StdAfx.h"
#include "../prssr.h"
#include "HtmlCtrl.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE CHTMLCtrl::HHtmlDLL = LoadLibrary(_T("htmlview.dll"));

/////////////////////////////////////////////////////////////////////////////
// CHTMLCtrl

CHTMLCtrl::CHTMLCtrl() {
}

BEGIN_MESSAGE_MAP(CHTMLCtrl, CWnd)
	//{{AFX_MSG_MAP(CHTMLCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//BOOL CHTMLCtrl::Create(DWORD dwStyle, CRect &rc, HWND parent, UINT id) {
BOOL CHTMLCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext/* = NULL*/) {
	// allow modification of several common create parameters
	CREATESTRUCT cs;
	cs.dwExStyle = 0;//dwExStyle;
	cs.lpszClass = _T("DISPLAYCLASS");//lpszClassName;
	cs.lpszName = NULL; //lpszWindowName;
	cs.style = dwStyle;
	cs.x = rect.left;
	cs.y = rect.top;
	cs.cx = rect.right - rect.left;
	cs.cy = rect.bottom - rect.top;
	cs.hwndParent = pParentWnd->GetSafeHwnd();
	cs.hMenu = (HMENU) nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.lpCreateParams = NULL;//pContext;

	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass,
			cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
			cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

#ifdef _DEBUG
	if (hWnd == NULL) {
		TRACE1("Warning: Window creation failed: GetLastError returns 0x%8.8X\n",
			GetLastError());
	}
#endif

	if (hWnd == NULL)
		return FALSE;

	m_hWnd = hWnd;
	::SetWindowLong(hWnd, GWL_ID, 12321);
	::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) _T(""));

	return TRUE;
}

void CHTMLCtrl::Clear() {
	::SendMessage(m_hWnd, DTM_CLEAR, 0, 0);
}

void CHTMLCtrl::EnableContextMenu(BOOL enable/* = TRUE*/) {
	::SendMessage(m_hWnd, DTM_ENABLECONTEXTMENU, 0, enable);
}

void CHTMLCtrl::EnableClearType(BOOL enable/* = TRUE*/) {
	::SendMessage(m_hWnd, DTM_ENABLECLEARTYPE, 0, (LPARAM) enable);
}

void CHTMLCtrl::EnableScripting(BOOL enable/* = TRUE*/) {
	::SendMessage(m_hWnd, DTM_ENABLESCRIPTING, 0, (LPARAM) enable);
}

void CHTMLCtrl::ZoomLevel(int level) {
	::SendMessage(m_hWnd, DTM_ZOOMLEVEL, 0, (LPARAM) level);
}

void CHTMLCtrl::AddText(LPWSTR str) {
	::SendMessage(m_hWnd, DTM_ADDTEXTW, FALSE, (LPARAM) str);
}

void CHTMLCtrl::AddText(const CString &str) {
	AddText((LPWSTR) (LPCTSTR) str);
}

void CHTMLCtrl::EndOfSource() {
	::SendMessage(m_hWnd, DTM_ENDOFSOURCE, 0, 0);
}

BOOL CHTMLCtrl::IsSelection() {
	return (BOOL) ::SendMessage(m_hWnd, DTM_ISSELECTION, 0, 0);
}

void CHTMLCtrl::CopySelectionToNewIStream(DWORD *rsd, LPSTREAM *stream) {
	::SendMessage(m_hWnd, DTM_COPYSELECTIONTONEWISTREAM, (WPARAM) rsd, (LPARAM) stream);
}

HWND CHTMLCtrl::SetParent(HWND hNewParent) {
	return ::SetParent(m_hWnd, hNewParent);
}

void CHTMLCtrl::SelectAll() {
	::SendMessage(m_hWnd, DTM_SELECTALL, 0, 0);
}