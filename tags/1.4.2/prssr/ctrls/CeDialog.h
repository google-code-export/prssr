/**
 *  CeDialog.h
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

#if !defined(AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_)
#define AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCeDialog window

class CCeDialog : public CDialog
{
// Construction
public:
	CCeDialog();
	CCeDialog(UINT nTemplateID, CWnd *pParent = NULL);

// Attributes
public:

// Operations
public:
	void SetMenu(UINT menuId) { m_nMenuID = menuId; }
	void SetCaption(const CString &caption) { m_strCaption = caption; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeDialog)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeDialog();

//	void SessionRestore();

	// Generated message map functions
protected:
	UINT m_nMenuID;
	CString m_strCaption;
	HWND m_hwndCmdBar;

	int Mode;

	void DrawCaption(CDC &dc);

	//{{AFX_MSG(CCeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void ResizeControls();

	static const int CAPTION_PANEL_HEIGHT;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDIALOG_H__96E58831_D1CA_4017_AA0F_27043AB58132__INCLUDED_)
