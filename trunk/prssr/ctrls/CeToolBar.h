/**
 *  CeToolBar.h
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

#if !defined(AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_)
#define AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define IDC_ACT_SITE					0x0900
#define ID_TOOLBAR_SEPARATOR			0x0901
#define IDC_SORT_BUTTON					0x0902

/////////////////////////////////////////////////////////////////////////////
// CCeToolBar window

class CCeToolBar : public CToolBar
{
// Construction
public:
	CCeToolBar();

// Attributes
public:

// Operations
public:
	void SetText(const CString &strText) { m_strText = strText; }
	void SetImageIdx(int nImageIdx) { m_nImageIdx = nImageIdx; }
	void EnableSortButton(BOOL enable = TRUE) { m_bSortButton = enable; }
	void SetSortButton(int imageIdx) { m_nImageSortIdx = imageIdx; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeToolBar();

	// Generated message map functions
protected:
	int m_nImageIdx;
	CString m_strText;
	CRect m_rcText;
	BOOL m_bSortButton;
	int m_nImageSortIdx;

	//{{AFX_MSG(CCeToolBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CETOOLBAR_H__E5AC3314_DCFF_44BC_8440_D50430A6A5D6__INCLUDED_)
