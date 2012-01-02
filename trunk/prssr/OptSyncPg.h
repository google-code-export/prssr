/**
 *  OptSyncPg.h
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

#if !defined(_OPTSYNCPG_H__INCLUDED_)
#define _OPTSYNCPG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// COptSyncPg dialog

class COptSyncPg : public CPropertyPage {
	DECLARE_DYNCREATE(COptSyncPg)

// Construction
public:
	COptSyncPg();
	~COptSyncPg();

// Dialog Data
	//{{AFX_DATA(COptSyncPg)
	enum { IDD = IDD_OPT_SYNC };
	CComboBox m_ctlSyncSite;
	CStatic	m_lblUserName;
	CCeEdit	m_ctlUserName;
	CStatic	m_lblPassword;
	CCeEdit	m_ctlPassword;
	CString m_strUserName;
	CString m_strPassword;
	BOOL m_bTranslate;
	CButton	m_ctlTranslate;
	CComboBox m_ctlTranslateLanguage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptSyncPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(COptSyncPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokSyncSite();
	afx_msg void OnTranslate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OPTSYNCPG_H__INCLUDED_)
