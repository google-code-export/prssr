/**
 *  InfoDescriptionPg.h
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

#if !defined(AFX_INFODESCRIPTIONPG_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_)
#define AFX_INFODESCRIPTIONPG_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Hrule.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoDescriptionPg dialog

class CInfoDescriptionPg : public CPropertyPage {
	DECLARE_DYNCREATE(CInfoDescriptionPg)

// Construction
public:
	CInfoDescriptionPg();
	~CInfoDescriptionPg();

// Dialog Data
	//{{AFX_DATA(CInfoDescriptionPg)
	enum { IDD = IDD_INFO_DESCRIPTION };
	CHrule	m_ctlHrule;
	CString	m_strCopyright;
	CString	m_strDescription;
	CString	m_strLanguage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoDescriptionPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInfoDescriptionPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_InfoDescriptionPg_H__80352AD3_8952_40D5_9ABC_EC185756E414__INCLUDED_)
