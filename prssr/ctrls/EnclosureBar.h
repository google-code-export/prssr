/**
 *  EnclosureBar.h
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

#if !defined(_ENCLOSUREBAR_H_)
#define _ENCLOSUREBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEnclosureBar window

class CEnclosureBar : public CStatic {
// Construction
public:
	CEnclosureBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnclosureBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEnclosureBar();

	void SetSize(DWORD size) { m_nSize = size; }
	void SetName(const CString &name) { m_strName = name; }
//	void SetItems(int unread, int total = -1) { m_nUnread = unread; if (total != -1) m_nTotal = total; }
	void SetIcon(HICON icon) { m_hIcon = icon; }
	void SetCached(BOOL cached = TRUE) { Cached = cached; }

	// Generated message map functions
protected:
	CFont m_fntBold;
	
	DWORD m_nSize;
	CString m_strName;
	HICON m_hIcon;
	BOOL Cached;

	CPoint LastCursorPos;

	//{{AFX_MSG(CEnclosureBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTION_H__B0F01B36_3F07_4A52_85E8_EC4A6C18B76A__INCLUDED_)
