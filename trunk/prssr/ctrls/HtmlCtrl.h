/**
 *  HtmlCtrl.h
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

#if !defined(_HTMLCTRL_H_)
#define _HTMLCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHTMLCtrl

class CHTMLCtrl : public CWnd {
public:
	CHTMLCtrl();

//	HWND GetHwnd() { return HWnd; }

//	BOOL Create(DWORD dwStyle, CRect &rc, HWND parent, UINT id);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	void Clear();
	void EnableContextMenu(BOOL enable = TRUE);
	void EnableClearType(BOOL enable = TRUE);
	void EnableScripting(BOOL enable = TRUE);
	void ZoomLevel(int level);
	void AddText(LPWSTR str);
	void AddText(const CString &str);
	void EndOfSource();
	BOOL IsSelection();
	void CopySelectionToNewIStream(DWORD *rsd, LPSTREAM *stream);
	HWND SetParent(HWND hNewParent);
	void SelectAll();

protected:
	static HINSTANCE HHtmlDLL;
//	HWND HWnd;

	// Generated message map functions
	//{{AFX_MSG(CHTMLCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_HTMLCTRL_H_)
