/**
 *  PropAuthenticationPg.h
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

#if !defined(AFX_PROPAUTHENTICATIONPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
#define AFX_PROPAUTHENTICATIONPG_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPropAuthenticationPg dialog

class CPropAuthenticationPg : public CPropertyPage {
	DECLARE_DYNCREATE(CPropAuthenticationPg)

// Construction
public:
	CPropAuthenticationPg();
	~CPropAuthenticationPg();

// Dialog Data
	//{{AFX_DATA(CPropAuthenticationPg)
	enum { IDD = IDD_PROP_AUTHENTICATION };
	CStatic	m_ctlUserNameLbl;
	CStatic	m_ctlPasswordLbl;
	CCeEdit	m_ctlUserName;
	CCeEdit	m_ctlPassword;
	CButton	m_ctlAuthRequired;
	CString	m_strPassword;
	CString	m_strUserName;
	BOOL	m_bAuthRequired;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropAuthenticationPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CPropAuthenticationPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAuthRequired();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropAuthenticationPg_H__4B2FEDB2_9A3D_465D_81A4_770F16C159E0__INCLUDED_)
