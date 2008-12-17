/**
 *  AcceptCertificateDlg.h
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

#if !defined(_ACCEPT_CERTIFICATE_H_)
#define _ACCEPT_CERTIFICATE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"
#include "ctrls/CeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CAcceptCertificateDlg dialog

class CAcceptCertificateDlg : public CCeDialog
{
// Construction
public:
	CAcceptCertificateDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAcceptCertificateDlg)
	enum { IDD = IDD_ACCEPT_CERTIFICATE };
	CStatic m_ctlAlertIcon;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcceptCertificateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	BOOL Trusted;
	BOOL ValidDate;
	BOOL ValidName;

// Implementation
protected:
	HWND m_hwndCmdBar;

	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CAcceptCertificateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_ACCEPT_CERTIFICATE_H_)
