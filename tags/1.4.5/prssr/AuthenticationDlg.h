/**
 *  AuthenticationDlg.h
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

#if !defined(AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_)
#define AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeDialog.h"
#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAuthenticationPg dialog

class CAuthenticationPg : public CPropertyPage
{
// Construction
public:
	CAuthenticationPg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAuthenticationPg)
	enum { IDD = IDD_AUTHENTICATION };
	CString	m_strSite;
	CString	m_strRealm;
	CCeEdit	m_ctlUserName;
	CCeEdit	m_ctlPassword;
	CString	m_strPassword;
	CString	m_strUserName;
	BOOL m_bSavePassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthenticationPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CAuthenticationPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHENTICATIONDLG_H__E0D7AD51_DB89_4DFE_AB9C_5646EF132EDC__INCLUDED_)
