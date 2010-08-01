/**
 *  MessageDlg.h
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

#if !defined(_MESSAGEBOXDLG_H__INCLUDED_)
#define _MESSAGEBOXDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

class CMessageDlg : public CCeDialog {
// Construction
public:
	CMessageDlg(UINT nCaption, UINT nText, DWORD style, UINT btnID);   // standard constructor
	~CMessageDlg();

// Dialog Data
	//{{AFX_DATA(CMessageDlg)
	enum { IDD = IDD_MESSAGEBOX };
	enum { IDD_WIDE = IDD_MESSAGEBOX_WIDE };
	//}}AFX_DATA
	CWnd m_ctlIcon;
	CStatic m_ctlText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnOk();
	virtual void OnCancel();
	virtual void OnYes();
	virtual void OnNo();

// Implementation
protected:
	CString m_strText;
	CString m_strSKLeft;
	DWORD m_nStyle;
	HICON m_hIcon;

	virtual void ResizeControls();

	// Generated message map functions
	//{{AFX_MSG(CMessageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_MESSAGEBOXDLG_H__INCLUDED_)
